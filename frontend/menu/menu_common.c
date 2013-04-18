/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2013 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2013 - Daniel De Matteis
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

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "menu_common.h"

#include "../../file.h"
#ifdef HAVE_FILEBROWSER
#include "utils/file_browser.h"
#else
#include "utils/file_list.h"
#endif

#include "../../compat/posix_string.h"

rgui_handle_t *rgui;

#ifdef HAVE_SHADER_MANAGER
void shader_manager_init(rgui_handle_t *rgui)
{
   config_file_t *conf = NULL;
   char cgp_path[PATH_MAX];

   const char *ext = path_get_extension(g_settings.video.shader_path);
   if (strcmp(ext, "glslp") == 0 || strcmp(ext, "cgp") == 0)
   {
      conf = config_file_new(g_settings.video.shader_path);
      if (conf)
      {
         if (gfx_shader_read_conf_cgp(conf, &rgui->shader))
            gfx_shader_resolve_relative(&rgui->shader, g_settings.video.shader_path);
         config_file_free(conf);
      }
   }
   else if (strcmp(ext, "glsl") == 0 || strcmp(ext, "cg") == 0)
   {
      strlcpy(rgui->shader.pass[0].source.cg, g_settings.video.shader_path,
            sizeof(rgui->shader.pass[0].source.cg));
      rgui->shader.passes = 1;
   }
   else
   {
      const char *shader_dir = *g_settings.video.shader_dir ?
         g_settings.video.shader_dir : g_settings.system_directory;

      fill_pathname_join(cgp_path, shader_dir, "rgui.glslp", sizeof(cgp_path));
      conf = config_file_new(cgp_path);

      if (!conf)
      {
         fill_pathname_join(cgp_path, shader_dir, "rgui.cgp", sizeof(cgp_path));
         conf = config_file_new(cgp_path);
      }

      if (conf)
      {
         if (gfx_shader_read_conf_cgp(conf, &rgui->shader))
            gfx_shader_resolve_relative(&rgui->shader, cgp_path);
         config_file_free(conf);
      }
   }
}

void shader_manager_get_str(struct gfx_shader *shader,
      char *type_str, size_t type_str_size, unsigned type)
{
   if (type == RGUI_SETTINGS_SHADER_APPLY)
      *type_str = '\0';
   else if (type == RGUI_SETTINGS_SHADER_PASSES)
      snprintf(type_str, type_str_size, "%u", shader->passes);
   else
   {
      unsigned pass = (type - RGUI_SETTINGS_SHADER_0) / 3;
      switch ((type - RGUI_SETTINGS_SHADER_0) % 3)
      {
         case 0:
            if (*shader->pass[pass].source.cg)
               fill_pathname_base(type_str,
                     shader->pass[pass].source.cg, type_str_size);
            else
               strlcpy(type_str, "N/A", type_str_size);
            break;

         case 1:
            switch (shader->pass[pass].filter)
            {
               case RARCH_FILTER_LINEAR:
                  strlcpy(type_str, "Linear", type_str_size);
                  break;

               case RARCH_FILTER_NEAREST:
                  strlcpy(type_str, "Nearest", type_str_size);
                  break;

               case RARCH_FILTER_UNSPEC:
                  strlcpy(type_str, "Don't care", type_str_size);
                  break;
            }
            break;

         case 2:
         {
            unsigned scale = shader->pass[pass].fbo.scale_x;
            if (!scale)
               strlcpy(type_str, "Don't care", type_str_size);
            else
               snprintf(type_str, type_str_size, "%ux", scale);
            break;
         }
      }
   }
}
#endif

#ifdef HAVE_FILEBROWSER

static bool directory_parse(void *data, const char *path)
{
   filebrowser_t *filebrowser = (filebrowser_t*)data;

   struct string_list *list = dir_list_new(path,
         filebrowser->current_dir.extensions, true);
   if(!list)
      return false;
   
   dir_list_sort(list, true);

   filebrowser->current_dir.ptr   = 0;
   strlcpy(filebrowser->current_dir.directory_path,
         path, sizeof(filebrowser->current_dir.directory_path));

   if(filebrowser->list)
      dir_list_free(filebrowser->list);

   filebrowser->list = list;

   return true;

}

void filebrowser_free(void *data)
{
   filebrowser_t *filebrowser = (filebrowser_t*)data;

   dir_list_free(filebrowser->list);
   filebrowser->list = NULL;
   filebrowser->current_dir.ptr   = 0;
   free(filebrowser);
}

void filebrowser_set_root_and_ext(void *data, const char *ext, const char *root_dir)
{
   filebrowser_t *filebrowser = (filebrowser_t*)data;
   
   if (ext)
      strlcpy(filebrowser->current_dir.extensions, ext,
            sizeof(filebrowser->current_dir.extensions));

   strlcpy(filebrowser->current_dir.root_dir,
         root_dir, sizeof(filebrowser->current_dir.root_dir));
   filebrowser_iterate(filebrowser, FILEBROWSER_ACTION_RESET);
}

#define GET_CURRENT_PATH(browser) (browser->list->elems[browser->current_dir.ptr].data)

bool filebrowser_iterate(void *data, unsigned action)
{
   filebrowser_t *filebrowser = (filebrowser_t*)data;
   bool ret = true;
   unsigned entries_to_scroll = 19;

   switch(action)
   {
      case FILEBROWSER_ACTION_UP:
         filebrowser->current_dir.ptr--;
         if (filebrowser->current_dir.ptr >= filebrowser->list->size)
            filebrowser->current_dir.ptr = filebrowser->list->size - 1;
         break;
      case FILEBROWSER_ACTION_DOWN:
         filebrowser->current_dir.ptr++;
         if (filebrowser->current_dir.ptr >= filebrowser->list->size)
            filebrowser->current_dir.ptr = 0;
         break;
      case FILEBROWSER_ACTION_LEFT:
         if (filebrowser->current_dir.ptr <= 5)
            filebrowser->current_dir.ptr = 0;
         else
            filebrowser->current_dir.ptr -= 5;
         break;
      case FILEBROWSER_ACTION_RIGHT:
         filebrowser->current_dir.ptr = (min(filebrowser->current_dir.ptr + 5, 
         filebrowser->list->size-1));
         break;
      case FILEBROWSER_ACTION_SCROLL_UP:
         if (filebrowser->current_dir.ptr <= entries_to_scroll)
            filebrowser->current_dir.ptr= 0;
         else
            filebrowser->current_dir.ptr -= entries_to_scroll;
         break;
      case FILEBROWSER_ACTION_SCROLL_DOWN:
         filebrowser->current_dir.ptr = (min(filebrowser->current_dir.ptr + 
         entries_to_scroll, filebrowser->list->size-1));
         break;
      case FILEBROWSER_ACTION_OK:
         ret = directory_parse(filebrowser, GET_CURRENT_PATH(filebrowser));
         break;
      case FILEBROWSER_ACTION_CANCEL:
         fill_pathname_parent_dir(filebrowser->current_dir.directory_path,
               filebrowser->current_dir.directory_path,
               sizeof(filebrowser->current_dir.directory_path));

         ret = directory_parse(filebrowser, filebrowser->current_dir.directory_path);
         break;
      case FILEBROWSER_ACTION_RESET:
         ret = directory_parse(filebrowser, filebrowser->current_dir.root_dir);
         break;
      case FILEBROWSER_ACTION_RESET_CURRENT_DIR:
         ret = directory_parse(filebrowser, filebrowser->current_dir.directory_path);
         break;
      case FILEBROWSER_ACTION_PATH_ISDIR:
         ret = filebrowser->list->elems[filebrowser->current_dir.ptr].attr.b;
         break;
      case FILEBROWSER_ACTION_NOOP:
      default:
         break;
   }

   strlcpy(filebrowser->current_dir.path, GET_CURRENT_PATH(filebrowser),
         sizeof(filebrowser->current_dir.path));

   return ret;
}

#else

struct rgui_file
{
   char *path;
   unsigned type;
   size_t directory_ptr;
};

void rgui_list_push(void *userdata,
      const char *path, unsigned type, size_t directory_ptr)
{
   rgui_list_t *list = (rgui_list_t*)userdata;

   if (!list)
      return;

   if (list->size >= list->capacity)
   {
      list->capacity++;
      list->capacity *= 2;
      list->list = (struct rgui_file*)realloc(list->list, list->capacity * sizeof(struct rgui_file));
   }

   list->list[list->size].path = strdup(path);
   list->list[list->size].type = type;
   list->list[list->size].directory_ptr = directory_ptr;
   list->size++;
}

void rgui_list_pop(rgui_list_t *list, size_t *directory_ptr)
{
   if (!(list->size == 0))
      free(list->list[--list->size].path);

   if (directory_ptr)
      *directory_ptr = list->list[list->size].directory_ptr;
}

void rgui_list_free(rgui_list_t *list)
{
   for (size_t i = 0; i < list->size; i++)
      free(list->list[i].path);
   free(list->list);
   free(list);
}

void rgui_list_clear(rgui_list_t *list)
{
   for (size_t i = 0; i < list->size; i++)
      free(list->list[i].path);
   list->size = 0;
}

void rgui_list_get_at_offset(const rgui_list_t *list, size_t index,
      const char **path, unsigned *file_type)
{
   if (path)
      *path = list->list[index].path;
   if (file_type)
      *file_type = list->list[index].type;
}

void rgui_list_get_last(const rgui_list_t *list,
      const char **path, unsigned *file_type)
{
   if (list->size)
      rgui_list_get_at_offset(list, list->size - 1, path, file_type);
}

#endif

void menu_init(void)
{
   rgui = rgui_init();

   if (rgui == NULL)
   {
      RARCH_ERR("Could not initialize menu.\n");
      rarch_fail(1, "menu_init()");
   }
}

void menu_free(void)
{
   rgui_free(rgui);
}
