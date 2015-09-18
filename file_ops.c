/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2015 - Daniel De Matteis
 *  Copyright (C) 2014-2015 - Timo Strunk
 * 
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <boolean.h>

#include <compat/strl.h>
#include <compat/posix_string.h>
#include <retro_assert.h>
#include <retro_miscellaneous.h>
#include <file/file_path.h>
#include <retro_file.h>
#include <string/string_list.h>
#ifdef HAVE_COMPRESSION
#include <file/file_extract.h>
#endif

#include "file_ops.h"

#ifdef HAVE_COMPRESSION
#ifdef HAVE_7ZIP
#include "deps/7zip/7z.h"
#include "deps/7zip/7zAlloc.h"
#include "deps/7zip/7zCrc.h"
#include "deps/7zip/7zFile.h"
#include "deps/7zip/7zVersion.h"

static ISzAlloc g_Alloc = { SzAlloc, SzFree };

static int Buf_EnsureSize(CBuf *dest, size_t size)
{
   if (dest->size >= size)
      return 1;
   Buf_Free(dest, &g_Alloc);
   return Buf_Create(dest, size, &g_Alloc);
}

#ifndef _WIN32

static uint8_t kUtf8Limits[5] = { 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

static Bool Utf16_To_Utf8(uint8_t *dest, size_t *destLen,
      const uint16_t *src, size_t srcLen)
{
   size_t destPos = 0;
   size_t srcPos  = 0;

   for (;;)
   {
      unsigned numAdds;
      uint32_t value;

      if (srcPos == srcLen)
      {
         *destLen = destPos;
         return True;
      }
      value = src[srcPos++];
      if (value < 0x80)
      {
         if (dest)
            dest[destPos] = (char)value;
         destPos++;
         continue;
      }
      if (value >= 0xD800 && value < 0xE000)
      {
         uint32_t c2;

         if (value >= 0xDC00 || srcPos == srcLen)
            break;
         c2 = src[srcPos++];
         if (c2 < 0xDC00 || c2 >= 0xE000)
            break;
         value = (((value - 0xD800) << 10) | (c2 - 0xDC00)) + 0x10000;
      }
      for (numAdds = 1; numAdds < 5; numAdds++)
         if (value < (((uint32_t)1) << (numAdds * 5 + 6)))
            break;
      if (dest)
         dest[destPos] = (char)(kUtf8Limits[numAdds - 1] 
               + (value >> (6 * numAdds)));
      destPos++;
      do
      {
         numAdds--;
         if (dest)
            dest[destPos] = (char)(0x80 
                  + ((value >> (6 * numAdds)) & 0x3F));
         destPos++;
      }while (numAdds != 0);
   }
   *destLen = destPos;
   return False;
}

static SRes Utf16_To_Utf8Buf(CBuf *dest,
      const uint16_t *src, size_t srcLen)
{
   Bool res;
   size_t destLen = 0;

   Utf16_To_Utf8(NULL, &destLen, src, srcLen);
   destLen += 1;

   if (!Buf_EnsureSize(dest, destLen))
      return SZ_ERROR_MEM;

   res = Utf16_To_Utf8(dest->data, &destLen, src, srcLen);
   dest->data[destLen] = 0;

   return res ? SZ_OK : SZ_ERROR_FAIL;
}
#endif

static SRes Utf16_To_Char(CBuf *buf, const uint16_t *s, int fileMode)
{
   int len = 0;

   for (len = 0; s[len] != '\0'; len++);

#ifdef _WIN32
   {
      int size = len * 3 + 100;
      if (!Buf_EnsureSize(buf, size))
         return SZ_ERROR_MEM;
      {
         char defaultChar = '_';
         BOOL defUsed;
         int numChars = WideCharToMultiByte(fileMode ?
               (
#ifdef UNDER_CE
                     CP_ACP
#else
                     AreFileApisANSI() ? CP_ACP : CP_OEMCP
#endif
               ) : CP_OEMCP,
               0, (LPCWSTR)s, len, (char *)buf->data,
               size, &defaultChar, &defUsed);
         if (numChars == 0 || numChars >= size)
            return SZ_ERROR_FAIL;
         buf->data[numChars] = 0;
         return SZ_OK;
      }
   }
#else
   (void)fileMode;
   return Utf16_To_Utf8Buf(buf, s, len);
#endif
}


static SRes ConvertUtf16toCharString(const uint16_t *s, char *outstring)
{
   CBuf buf;
   SRes res;

   Buf_Init(&buf);
   res = Utf16_To_Char(&buf, s, 0);

   if (res == SZ_OK)
      strncpy(outstring, (const char*)buf.data, PATH_MAX_LENGTH);

   Buf_Free(&buf, &g_Alloc);
   return res;
}

/* Extract the relative path relative_path from a 7z archive 
 * archive_path and allocate a buf for it to write it in.
 * If optional_outfile is set, extract to that instead and don't alloc buffer.
 */
static int read_7zip_file(
      const char *archive_path,
      const char *relative_path, void **buf,
      const char *optional_outfile)
{
   CFileInStream archiveStream;
   CLookToRead lookStream;
   CSzArEx db;
   SRes res;
   ISzAlloc allocImp;
   ISzAlloc allocTempImp;
   uint16_t *temp  = NULL;
   size_t tempSize = 0;
   long outsize    = -1;
   bool file_found = false;

   /*These are the allocation routines.
    * Currently using the non-standard 7zip choices. */
   allocImp.Alloc     = SzAlloc;
   allocImp.Free      = SzFree;
   allocTempImp.Alloc = SzAllocTemp;
   allocTempImp.Free  = SzFreeTemp;

   if (InFile_Open(&archiveStream.file, archive_path))
   {
      RARCH_ERR("Could not open %s as 7z archive\n.",archive_path);
      return -1;
   }
   else
   {
      RARCH_LOG_OUTPUT("Openend archive %s. Now trying to extract %s\n",
            archive_path,relative_path);
   }

   FileInStream_CreateVTable(&archiveStream);
   LookToRead_CreateVTable(&lookStream, False);
   lookStream.realStream = &archiveStream.s;
   LookToRead_Init(&lookStream);
   CrcGenerateTable();
   SzArEx_Init(&db);
   res = SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp);

   if (res == SZ_OK)
   {
      uint32_t i;
      uint32_t blockIndex  = 0xFFFFFFFF;
      uint8_t *outBuffer   = 0;
      size_t outBufferSize = 0;

      for (i = 0; i < db.db.NumFiles; i++)
      {
         size_t len;
         char infile[PATH_MAX_LENGTH] = {0};
         size_t offset           = 0;
         size_t outSizeProcessed = 0;
         const CSzFileItem    *f = db.db.Files + i;

         if (f->IsDir)
         {
            /* We skip over everything which is not a directory. 
             * FIXME: Why continue then if f->IsDir is true?*/
            continue;
         }

         len = SzArEx_GetFileNameUtf16(&db, i, NULL);
         if (len > tempSize)
         {
            free(temp);
            tempSize = len;
            temp = (uint16_t *)malloc(tempSize * sizeof(temp[0]));
            if (temp == 0)
            {
               res = SZ_ERROR_MEM;
               break;
            }
         }
         SzArEx_GetFileNameUtf16(&db, i, temp);
         res = ConvertUtf16toCharString(temp,infile);

         if (!strcmp(infile, relative_path))
         {
            /* C LZMA SDK does not support chunked extraction - see here:
             * sourceforge.net/p/sevenzip/discussion/45798/thread/6fb59aaf/
             * */
            file_found = true;
            res = SzArEx_Extract(&db, &lookStream.s, i,&blockIndex,
                  &outBuffer, &outBufferSize,&offset, &outSizeProcessed,
                  &allocImp, &allocTempImp);
            if (res != SZ_OK)
            {
               break; /* This goes to the error section. */
            }
            outsize = outSizeProcessed;
            if (optional_outfile != NULL)
            {
               FILE* outsink = fopen(optional_outfile,"wb");
               if (outsink == NULL)
               {
                  RARCH_ERR("Could not open outfilepath %s.\n",
                        optional_outfile);
                  IAlloc_Free(&allocImp, outBuffer);
                  SzArEx_Free(&db, &allocImp);
                  free(temp);
                  File_Close(&archiveStream.file);
                  return -1;
               }
               fwrite(outBuffer+offset,1,outsize,outsink);
               fclose(outsink);
            }
            else
            {
               /*We could either use the 7Zip allocated buffer,
                * or create our own and use it.
                * We would however need to realloc anyways, because RetroArch
                * expects a \0 at the end, therefore we allocate new,
                * copy and free the old one. */
               *buf = malloc(outsize + 1);
               ((char*)(*buf))[outsize] = '\0';
               memcpy(*buf,outBuffer+offset,outsize);
            }
            IAlloc_Free(&allocImp, outBuffer);
            break;
         }
      }
   }
   SzArEx_Free(&db, &allocImp);
   free(temp);

   File_Close(&archiveStream.file);

   if (res == SZ_OK && file_found == true)
      return outsize;

   /* Error handling */
   if (!file_found)
      RARCH_ERR("File %s not found in %s\n",relative_path,archive_path);
   else if (res == SZ_ERROR_UNSUPPORTED)
      RARCH_ERR("7Zip decoder doesn't support this archive\n");
   else if (res == SZ_ERROR_MEM)
      RARCH_ERR("7Zip decoder could not allocate memory\n");
   else if (res == SZ_ERROR_CRC)
      RARCH_ERR("7Zip decoder encountered a CRC error in the archive\n");
   else
      RARCH_ERR("\nUnspecified error in 7-ZIP archive, error number was: #%d\n", res);
   return -1;
}

static struct string_list *compressed_7zip_file_list_new(
      const char *path, const char* ext)
{
   CFileInStream archiveStream;
   CLookToRead lookStream;
   CSzArEx db;
   SRes res;
   ISzAlloc allocImp;
   ISzAlloc allocTempImp;
   uint16_t *temp               = NULL;
   size_t tempSize              = 0;
   long outsize                 = -1;

   struct string_list *ext_list = NULL;
   struct string_list     *list = string_list_new();

   if (!list)
      return NULL;

   if (ext)
      ext_list = string_split(ext, "|");

   (void)outsize;

   /* These are the allocation routines - currently using 
    * the non-standard 7zip choices. */
   allocImp.Alloc     = SzAlloc;
   allocImp.Free      = SzFree;
   allocTempImp.Alloc = SzAllocTemp;
   allocTempImp.Free  = SzFreeTemp;

   if (InFile_Open(&archiveStream.file, path))
   {
      RARCH_ERR("Could not open %s as 7z archive.\n",path);
      goto error;
   }

   FileInStream_CreateVTable(&archiveStream);
   LookToRead_CreateVTable(&lookStream, False);
   lookStream.realStream = &archiveStream.s;
   LookToRead_Init(&lookStream);
   CrcGenerateTable();
   SzArEx_Init(&db);
   res = SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp);

   if (res == SZ_OK)
   {
      uint32_t i;
      uint32_t blockIndex  = 0xFFFFFFFF;
      uint8_t *outBuffer   = 0;
      size_t outBufferSize = 0;

      (void)blockIndex;
      (void)outBufferSize;
      (void)outBuffer;

      for (i = 0; i < db.db.NumFiles; i++)
      {
         union string_list_elem_attr attr;
         const char *file_ext         = NULL;
         char infile[PATH_MAX_LENGTH] = {0};
         size_t offset                = 0;
         size_t outSizeProcessed      = 0;
         size_t                   len = 0;
         bool supported_by_core       = false;
         const CSzFileItem         *f = db.db.Files + i;

         (void)offset;
         (void)outSizeProcessed;

         /* we skip over everything, which is a directory. */
         if (f->IsDir)
            continue;

         len = SzArEx_GetFileNameUtf16(&db, i, NULL);

         if (len > tempSize)
         {
            free(temp);
            tempSize = len;
            temp     = (uint16_t *)malloc(tempSize * sizeof(temp[0]));

            if (temp == 0)
            {
               res = SZ_ERROR_MEM;
               break;
            }
         }
         SzArEx_GetFileNameUtf16(&db, i, temp);
         res      = ConvertUtf16toCharString(temp, infile);
         file_ext = path_get_extension(infile);

         if (string_list_find_elem_prefix(ext_list, ".", file_ext))
            supported_by_core = true;

         /*
          * Currently we only support files without subdirs in the archives.
          * Folders are not supported (differences between win and lin.
          * Archives within archives should imho never be supported.
          */

         if (!supported_by_core)
            continue;

         attr.i = RARCH_COMPRESSED_FILE_IN_ARCHIVE;

         if (!string_list_append(list, infile, attr))
            goto error;

      }
   }
   SzArEx_Free(&db, &allocImp);
   free(temp);
   File_Close(&archiveStream.file);

   if (res != SZ_OK)
   {
      /* Error handling */
      if (res == SZ_ERROR_UNSUPPORTED)
         RARCH_ERR("7Zip decoder doesn't support this archive. \n");
      else if (res == SZ_ERROR_MEM)
         RARCH_ERR("7Zip decoder could not allocate memory. \n");
      else if (res == SZ_ERROR_CRC)
         RARCH_ERR("7Zip decoder encountered a CRC error in the archive. \n");
      else
         RARCH_ERR(
               "\nUnspecified error in 7-ZIP archive, error number was: #%d. \n",
               res);
      goto error;
   }

   string_list_free(ext_list);
   return list;

error:
   RARCH_ERR("Failed to open compressed_file: \"%s\"\n", path);
   SzArEx_Free(&db, &allocImp);
   free(temp);
   File_Close(&archiveStream.file);
   string_list_free(list);
   string_list_free(ext_list);
   return NULL;
}
#endif

#ifdef HAVE_ZLIB
#include "deps/zlib/unzip.h"

#define RARCH_ZIP_SUPPORT_BUFFER_SIZE_MAX 16384

/* Extract the relative path relative_path from a 
 * zip archive archive_path and allocate a buf for it to write it in. */
/* This code is inspired by:
 * stackoverflow.com/questions/10440113/simple-way-to-unzip-a-zip-file-using-zlib
 *
 * optional_outfile if not NULL will be used to extract the file. buf will be 0
 * then.
 */

static int read_zip_file(const char *archive_path,
      const char *relative_path, void **buf,
      const char* optional_outfile)
{
   uLong i;
   unz_global_info global_info;
   ssize_t    bytes_read = -1;
   bool finished_reading = false;
   unzFile      *zipfile = (unzFile*)unzOpen(archive_path);

   if (!zipfile)
      return -1;

   /* Get info about the zip file */
   if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK)
      goto error;

   for ( i = 0; i < global_info.number_entry; ++i )
   {
      /* Get info about current file. */
      unz_file_info file_info;
      char filename[PATH_MAX_LENGTH] = {0};
      char last_char = ' ';

      if (unzGetCurrentFileInfo(
               zipfile,
               &file_info,
               filename,
               PATH_MAX_LENGTH,
               NULL, 0, NULL, 0 ) != UNZ_OK )
         goto error;

      /* Check if this entry is a directory or file. */
      last_char = filename[strlen(filename)-1];

      /* We skip directories */
      if ( last_char == '/' || last_char == '\\' ) { }
      else if (!strcmp(filename, relative_path))
      {
         /* We found the correct file in the zip, 
          * now extract it to *buf. */
         if (unzOpenCurrentFile(zipfile) != UNZ_OK )
            goto error;

         if (optional_outfile == 0)
         {
            /* Allocate outbuffer */
            *buf       = malloc(file_info.uncompressed_size + 1 );
            bytes_read = unzReadCurrentFile(zipfile, *buf, file_info.uncompressed_size);

            if (bytes_read != (ssize_t)file_info.uncompressed_size)
            {
               RARCH_ERR(
                     "Tried to read %d bytes, but only got %d of file %s in ZIP %s.\n",
                     (unsigned int) file_info.uncompressed_size, (int)bytes_read,
                     relative_path, archive_path);
               free(*buf);
               goto close;
            }
            ((char*)(*buf))[file_info.uncompressed_size] = '\0';
         }
         else
         {
            char read_buffer[RARCH_ZIP_SUPPORT_BUFFER_SIZE_MAX] = {0};
            FILE* outsink = fopen(optional_outfile,"wb");

            if (outsink == NULL)
               goto close;

            bytes_read = 0;

            do
            {
               ssize_t fwrite_bytes;

               bytes_read = unzReadCurrentFile(zipfile, read_buffer,
                     RARCH_ZIP_SUPPORT_BUFFER_SIZE_MAX );
               fwrite_bytes = fwrite(read_buffer, 1, bytes_read,outsink);

               if (fwrite_bytes == bytes_read)
                  continue;

               /* couldn't write all bytes */
               RARCH_ERR("Error writing to %s.\n",optional_outfile);
               fclose(outsink);
               goto close;
            } while(bytes_read > 0);

            fclose(outsink);
         }
         finished_reading = true;
      }

      unzCloseCurrentFile(zipfile);

      if (finished_reading)
         break;

      if ((i + 1) < global_info.number_entry)
      {
         if (unzGoToNextFile(zipfile) == UNZ_OK)
            continue;

         goto error;
      }
   }

   unzClose(zipfile);

   if(!finished_reading)
      return -1;

   return bytes_read;

close:
   unzCloseCurrentFile(zipfile);
error:
   unzClose(zipfile);
   return -1;
}
#endif

#endif

/**
 * write_file:
 * @path             : path to file.
 * @data             : contents to write to the file.
 * @size             : size of the contents.
 *
 * Writes data to a file.
 *
 * Returns: true (1) on success, false (0) otherwise.
 */
bool write_file(const char *path, const void *data, ssize_t size)
{
   ssize_t ret   = 0;
   RFILE *file   = retro_fopen(path, RFILE_MODE_WRITE, -1);
   if (!file)
      return false;

   ret = retro_fwrite(file, data, size);
   retro_fclose(file);

   return (ret == size);
}

/**
 * read_generic_file:
 * @path             : path to file.
 * @buf              : buffer to allocate and read the contents of the
 *                     file into. Needs to be freed manually.
 *
 * Read the contents of a file into @buf.
 *
 * Returns: number of items read, -1 on error.
 */
static int read_generic_file(const char *path, void **buf, ssize_t *len)
{
   ssize_t ret;
   ssize_t bytes_read       = 0;
   ssize_t content_buf_size = 0;
   void *content_buf        = NULL;
   RFILE *file              = retro_fopen(path, RFILE_MODE_READ, -1);

   if (!file)
      goto error;

   ret = retro_fseek(file, 0, SEEK_END);

   if (ret != 0)
      goto error;

   content_buf_size = retro_ftell(file);
   if (content_buf_size == -1)
      goto error;

   retro_frewind(file);

   content_buf = malloc(content_buf_size + 1);

   if (!content_buf)
      goto error;

   if ((bytes_read = retro_fread(file, content_buf, content_buf_size)) < content_buf_size)
      RARCH_WARN("Didn't read whole file.\n");

   *buf    = content_buf;

   /* Allow for easy reading of strings to be safe.
    * Will only work with sane character formatting (Unix). */
   ((char*)content_buf)[content_buf_size] = '\0';

   retro_fclose(file);

   if (len)
      *len = bytes_read;

   return 1;

error:
   retro_fclose(file);
   if (content_buf)
      free(content_buf);
   if (len)
      *len = -1;
   *buf = NULL;
   return 0;
}

#ifdef HAVE_COMPRESSION
/* Generic compressed file loader.
 * Extracts to buf, unless optional_filename != 0
 * Then extracts to optional_filename and leaves buf alone.
 */
int read_compressed_file(const char * path, void **buf,
      const char* optional_filename, ssize_t *length)
{
   const char* file_ext               = NULL;
   char *archive_found                = NULL;
   char archive_path[PATH_MAX_LENGTH] = {0};

   if (optional_filename)
   {
      /* Safety check.  * If optional_filename and optional_filename 
       * exists, we simply return 0,
       * hoping that optional_filename is the 
       * same as requested.
       */
      if(path_file_exists(optional_filename))
      {
         *length = 0;
         return 1;
      }
   }

   /* We split carchive path and relative path: */
   strlcpy(archive_path, path, sizeof(archive_path));

   archive_found = (char*)strchr(archive_path,'#');

   rarch_assert(archive_found != NULL);

   /* We assure that there is something after the '#' symbol. */
   if (strlen(archive_found) <= 1)
   {
      /*
       * This error condition happens for example, when
       * path = /path/to/file.7z, or
       * path = /path/to/file.7z#
       */
      RARCH_ERR("Could not extract image path and carchive path from "
            "path: %s.\n", path);
      *length = 0;
      return 0;
   }

   /* We split the string in two, by putting a \0, where the hash was: */
   *archive_found  = '\0';
   archive_found  += 1;
   file_ext        = path_get_extension(archive_path);

#ifdef HAVE_7ZIP
   if (strcasecmp(file_ext,"7z") == 0)
   {
      *length = read_7zip_file(archive_path,archive_found,buf,optional_filename);
      if (*length != -1)
         return 1;
   }
#endif
#ifdef HAVE_ZLIB
   if (strcasecmp(file_ext,"zip") == 0)
   {
      *length = read_zip_file(archive_path,archive_found,buf,optional_filename);
      if (*length != -1)
         return 1;
   }
#endif
   return 0;
}
#endif

/**
 * read_file:
 * @path             : path to file.
 * @buf              : buffer to allocate and read the contents of the
 *                     file into. Needs to be freed manually.
 * @length           : Number of items read, -1 on error.
 *
 * Read the contents of a file into @buf. Will call read_compressed_file
 * if path contains a compressed file, otherwise will call read_generic_file.
 *
 * Returns: 1 if file read, 0 on error.
 */
int read_file(const char *path, void **buf, ssize_t *length)
{
#ifdef HAVE_COMPRESSION
   if (path_contains_compressed_file(path))
   {
      if (read_compressed_file(path, buf, NULL, length))
         return 1;
   }
#endif
   return read_generic_file(path, buf, length);
}

struct string_list *compressed_file_list_new(const char *path,
      const char* ext)
{
#ifdef HAVE_COMPRESSION
   const char* file_ext = path_get_extension(path);
#ifdef HAVE_7ZIP
   if (strcasecmp(file_ext,"7z") == 0)
      return compressed_7zip_file_list_new(path,ext);
#endif
#ifdef HAVE_ZLIB
   if (strcasecmp(file_ext,"zip") == 0)
      return zlib_get_file_list(path, ext);
#endif

#endif
   return NULL;
}
