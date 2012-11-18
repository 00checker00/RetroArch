/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2012 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2012 - Daniel De Matteis
 *  Copyright (C) 2012 - Michael Lelli
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

#include "../driver.h"
#include "../general.h"
#include "../console/rarch_console_video.h"
#include "../console/font.h"
#include "../console/rgui/rgui.h"
#include "../gfx/gfx_common.h"

#ifdef HW_RVL
#include "../wii/mem2_manager.h"
#endif

#include "gx_video.h"
#include <gccore.h>
#include <ogcsys.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define SYSMEM1_SIZE 0x01800000

void *g_framebuf[2];
unsigned g_current_framebuf;

bool g_vsync;
lwpq_t g_video_cond;
volatile bool g_draw_done;
uint32_t g_orientation;

struct
{
   uint32_t data[512 * 256];
   GXTexObj obj;
} g_tex ATTRIBUTE_ALIGN(32);

struct
{
   uint32_t data[240 * 200];
   GXTexObj obj;
} menu_tex ATTRIBUTE_ALIGN(32);

uint8_t gx_fifo[256 * 1024] ATTRIBUTE_ALIGN(32);
uint8_t display_list[1024] ATTRIBUTE_ALIGN(32);
uint16_t gx_width, gx_height;
size_t display_list_size;
GXRModeObj gx_mode;
unsigned gx_old_width, gx_old_height;

float verts[16] ATTRIBUTE_ALIGN(32) = {
   -1,  1, -0.5,
   -1, -1, -0.5,
    1, -1, -0.5,
    1,  1, -0.5,
};

float vertex_ptr[8] ATTRIBUTE_ALIGN(32) = {
   0, 0,
   0, 1,
   1, 1,
   1, 0,
};

static void retrace_callback(u32 retrace_count)
{
   (void)retrace_count;
   g_draw_done = true;
   LWP_ThreadSignal(g_video_cond);
}

void gx_set_video_mode(unsigned fbWidth, unsigned lines)
{
   VIDEO_SetBlack(true);
   VIDEO_Flush();
   gx_video_t *gx = (gx_video_t*)driver.video_data;
   unsigned modetype;
   unsigned viHeightMultiplier = 1;
   unsigned viWidth = 640;
#if defined(HW_RVL)
   bool progressive = CONF_GetProgressiveScan() > 0 && VIDEO_HaveComponentCable();
   unsigned tvmode;
   switch (CONF_GetVideo())
   {
      case CONF_VIDEO_PAL:
         if (CONF_GetEuRGB60() > 0)
            tvmode = VI_PAL;
         else
            tvmode = VI_EURGB60;
         break;
      case CONF_VIDEO_MPAL:
         tvmode = VI_MPAL;
         break;
      default:
         tvmode = VI_NTSC;
         break;
   }
#else
   bool progressive = VIDEO_HaveComponentCable();
   unsigned tvmode = VIDEO_GetCurrentTvMode();
#endif
   unsigned max_width, max_height;
   switch (tvmode)
   {
      case VI_PAL:
         max_width = VI_MAX_WIDTH_PAL;
         max_height = VI_MAX_HEIGHT_PAL;
         break;
      case VI_MPAL:
         max_width = VI_MAX_WIDTH_MPAL;
         max_height = VI_MAX_HEIGHT_MPAL;
         break;
      case VI_EURGB60:
         max_width = VI_MAX_WIDTH_NTSC;
         max_height = VI_MAX_HEIGHT_NTSC;
         break;
      default:
         tvmode = VI_NTSC;
         max_width = VI_MAX_WIDTH_EURGB60;
         max_height = VI_MAX_HEIGHT_EURGB60;
         break;
   }

   if (lines <= max_height / 2)
   {
      modetype = VI_NON_INTERLACE;
      viHeightMultiplier = 2;
   }
   else
   {
      modetype = (progressive) ? VI_PROGRESSIVE : VI_INTERLACE;
   }

   if (lines == 0 || fbWidth == 0)
   {
      VIDEO_GetPreferredMode(&gx_mode);
      goto config;
   }

   if (lines > max_height)
      lines = max_height;
   if (fbWidth > max_width)
   {
      fbWidth = viWidth = max_width;
   }

   gx_mode.viTVMode = VI_TVMODE(tvmode, modetype);
   gx_mode.fbWidth = fbWidth;
   gx_mode.efbHeight = lines;
   gx_mode.xfbHeight = lines;
   gx_mode.viWidth = viWidth;
   gx_mode.viHeight = lines * viHeightMultiplier;
   gx_mode.viXOrigin = (max_width - gx_mode.viWidth) / 2;
   if (viHeightMultiplier == 2)
      gx_mode.viYOrigin = (max_height / 2 - gx_mode.viHeight / 2) / 2;
   else
      gx_mode.viYOrigin = (max_height - gx_mode.viHeight) / 2;
   gx_mode.xfbMode = modetype == VI_INTERLACE ? VI_XFBMODE_DF : VI_XFBMODE_SF;
   gx_mode.field_rendering = GX_FALSE;
   gx_mode.aa = GX_FALSE;

   for (unsigned i = 0; i < 12; i++)
      gx_mode.sample_pattern[i][0] = gx_mode.sample_pattern[i][1] = 6;

   if (modetype == VI_INTERLACE)
   {
      gx_mode.vfilter[0] = 8;
      gx_mode.vfilter[1] = 8;
      gx_mode.vfilter[2] = 10;
      gx_mode.vfilter[3] = 12;
      gx_mode.vfilter[4] = 10;
      gx_mode.vfilter[5] = 8;
      gx_mode.vfilter[6] = 8;
   }
   else
   {
      gx_mode.vfilter[0] = 0;
      gx_mode.vfilter[1] = 0;
      gx_mode.vfilter[2] = 21;
      gx_mode.vfilter[3] = 22;
      gx_mode.vfilter[4] = 21;
      gx_mode.vfilter[5] = 0;
      gx_mode.vfilter[6] = 0;
   }

config:
   RARCH_LOG("GX Resolution: %dx%d (%s)\n", gx_mode.fbWidth, gx_mode.efbHeight, (gx_mode.viTVMode & 3) == VI_INTERLACE ? "interlaced" : "progressive");

   gx->win_width = gx_mode.fbWidth;
   gx->win_height = gx_mode.efbHeight;
   gx->double_strike = modetype == VI_NON_INTERLACE;
   gx->should_resize = true;

   RGUI_HEIGHT = gx_mode.efbHeight / (gx->double_strike ? 1 : 2);
   RGUI_HEIGHT &= ~3;
   if (RGUI_HEIGHT > 240)
      RGUI_HEIGHT = 240;
   RGUI_WIDTH = gx_mode.fbWidth / (gx_mode.fbWidth < 400 ? 1 : 2);
   RGUI_WIDTH &= ~3;
   if (RGUI_WIDTH > 400)
      RGUI_WIDTH = 400;

   VIDEO_Configure(&gx_mode);
   VIDEO_ClearFrameBuffer(&gx_mode, g_framebuf[0], COLOR_BLACK);
   VIDEO_ClearFrameBuffer(&gx_mode, g_framebuf[1], COLOR_BLACK);
   VIDEO_SetNextFramebuffer(g_framebuf[0]);
   VIDEO_SetPostRetraceCallback(retrace_callback);
   VIDEO_SetBlack(false);
   VIDEO_Flush();
   //VIDEO_WaitVSync();
   //if (gx_mode.viTVMode & VI_NON_INTERLACE)
   //   VIDEO_WaitVSync();

   GX_SetViewport(0, 0, gx_mode.fbWidth, gx_mode.efbHeight, 0, 1);
   GX_SetDispCopyYScale(GX_GetYScaleFactor(gx_mode.efbHeight, gx_mode.xfbHeight));
   //GX_SetScissor(0, 0, gx_mode.fbWidth, gx_mode.efbHeight);
   GX_SetDispCopySrc(0, 0, gx_mode.fbWidth, gx_mode.efbHeight);
   GX_SetDispCopyDst(gx_mode.fbWidth, gx_mode.xfbHeight);
   GX_SetCopyFilter(gx_mode.aa, gx_mode.sample_pattern, (gx_mode.xfbMode == VI_XFBMODE_SF) ? GX_FALSE : GX_TRUE,
         gx_mode.vfilter);
   GX_SetCopyClear((GXColor) { 0, 0, 0, 0xff }, GX_MAX_Z24);
   GX_SetFieldMode(gx_mode.field_rendering, (gx_mode.viHeight == 2 * gx_mode.xfbHeight) ? GX_ENABLE : GX_DISABLE);
   GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
   GX_InvalidateTexAll();
   GX_Flush();

   g_current_framebuf = 0;
}

const char *gx_get_video_mode()
{
   static char format[16];
   snprintf(format, sizeof(format), "%.3ux%.3u%c", gx_mode.fbWidth, gx_mode.efbHeight, (gx_mode.viTVMode & 3) == VI_INTERLACE ? 'i' : 'p');
   return format;
}

void gx_set_aspect_ratio(void *data, unsigned aspectratio_idx)
{
   gx_video_t *gx = (gx_video_t*)driver.video_data;

   if (g_settings.video.aspect_ratio_idx == ASPECT_RATIO_AUTO)
      rarch_set_auto_viewport(g_extern.frame_cache.width, g_extern.frame_cache.height);
   else if(g_settings.video.aspect_ratio_idx == ASPECT_RATIO_CORE)
      rarch_set_core_viewport();

   g_settings.video.aspect_ratio = aspectratio_lut[g_settings.video.aspect_ratio_idx].value;
   g_settings.video.force_aspect = false;
   gx->keep_aspect = true;
   gx->should_resize = true;
}

static void setup_video_mode()
{
   for (unsigned i = 0; i < 2; i++)
      g_framebuf[i] = MEM_K0_TO_K1(memalign(32, 640 * 576 * VI_DISPLAY_PIX_SZ));

   g_current_framebuf = 0;
   g_draw_done = true;
   g_orientation = ORIENTATION_NORMAL;
   LWP_InitQueue(&g_video_cond);

   VIDEO_GetPreferredMode(&gx_mode);
   gx_set_video_mode(0, 0);
}

static void init_texture(unsigned width, unsigned height)
{
   unsigned g_filter = g_settings.video.smooth ? GX_LINEAR : GX_NEAR;

   GX_InitTexObj(&g_tex.obj, g_tex.data, width, height, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
   GX_InitTexObjLOD(&g_tex.obj, g_filter, g_filter, 0, 0, 0, GX_TRUE, GX_FALSE, GX_ANISO_1);
   GX_InitTexObj(&menu_tex.obj, menu_tex.data, RGUI_WIDTH, RGUI_HEIGHT, GX_TF_RGB5A3, GX_CLAMP, GX_CLAMP, GX_FALSE);
   GX_InitTexObjLOD(&menu_tex.obj, g_filter, g_filter, 0, 0, 0, GX_TRUE, GX_FALSE, GX_ANISO_1);
   GX_InvalidateTexAll();
}

static void init_vtx()
{
   GX_SetCullMode(GX_CULL_NONE);
   GX_SetClipMode(GX_CLIP_DISABLE);
   GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
   GX_SetZMode(GX_ENABLE, GX_ALWAYS, GX_ENABLE);
   GX_SetColorUpdate(GX_TRUE);
   GX_SetAlphaUpdate(GX_FALSE);

   Mtx44 m;
   guOrtho(m, 1, -1, -1, 1, 0.4, 0.6);
   GX_LoadProjectionMtx(m, GX_ORTHOGRAPHIC);

   GX_ClearVtxDesc();
   GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
   GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX8);

   GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
   GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
   GX_SetArray(GX_VA_POS, verts, 3 * sizeof(float));
   GX_SetArray(GX_VA_TEX0, vertex_ptr, 2 * sizeof(float));

   GX_SetNumTexGens(1);
   GX_SetNumChans(0);
   GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
   GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
   GX_InvVtxCache();

   GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_INVSRCALPHA, 0);

   memset(g_tex.data, 0, sizeof(g_tex.data));
   DCFlushRange(g_tex.data, sizeof(g_tex.data));
   init_texture(4, 4); // for menu texture
   GX_Flush();
}

static void build_disp_list(void)
{
   DCInvalidateRange(display_list, sizeof(display_list));
   GX_BeginDispList(display_list, sizeof(display_list));
   GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
   for (unsigned i = 0; i < 4; i++)
   {
      GX_Position1x8(i);
      GX_TexCoord1x8(i);
   }
   GX_End();
   display_list_size = GX_EndDispList();
}

//#define TAKE_EFB_SCREENSHOT_ON_EXIT

#ifdef TAKE_EFB_SCREENSHOT_ON_EXIT

// Adapted from code by Crayon for GRRLIB (http://code.google.com/p/grrlib)
static void gx_efb_screenshot()
{
   int x, y;

   uint8_t tga_header[] = {0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x02, 0xE0, 0x01, 0x18, 0x00};
   FILE *out = fopen("/screenshot.tga", "wb");

   if (!out)
      return;

   fwrite(tga_header, 1, sizeof(tga_header), out);

   for (y = 479; y >= 0; --y)
   {
      uint8_t line[640 * 3];
      unsigned i = 0;
      for (x = 0; x < 640; x++)
      {
         GXColor color;
         GX_PeekARGB(x, y, &color);
         line[i++] = color.b;
         line[i++] = color.g;
         line[i++] = color.r;
      }
      fwrite(line, 1, sizeof(line), out);
   }

   fclose(out);
}

#endif

static void gx_stop(void)
{
#ifdef TAKE_EFB_SCREENSHOT_ON_EXIT
   gx_efb_screenshot();
#endif
   GX_DrawDone();
   GX_AbortFrame();
   GX_Flush();
   VIDEO_SetBlack(true);
   VIDEO_Flush();
   VIDEO_WaitVSync();

   for (unsigned i = 0; i < 2; i++)
      free(MEM_K1_TO_K0(g_framebuf[i]));
}

static void gx_restart(void)
{
}

static void *gx_init(const video_info_t *video,
      const input_driver_t **input, void **input_data)
{
   g_vsync = video->vsync;

   if (driver.video_data)
      return driver.video_data;

   gx_video_t *gx = (gx_video_t*)calloc(1, sizeof(gx_video_t));
   if (!gx)
      return NULL;
   return gx;
}

static void gx_start(void)
{
   video_info_t video_info = {0};

   video_info.vsync = g_settings.video.vsync;
   video_info.force_aspect = false;
   video_info.fullscreen = true;
   video_info.smooth = g_settings.video.smooth;
   video_info.input_scale = 2;

   driver.video_data = gx_init(&video_info, NULL, NULL);

   VIDEO_Init();
   GX_Init(gx_fifo, sizeof(gx_fifo));

   setup_video_mode();
   init_vtx();
   build_disp_list();

   gx_video_t *gx = (gx_video_t*)driver.video_data;
   gx->win_width = gx_mode.fbWidth;
   gx->win_height = gx_mode.efbHeight;
   gx->should_resize = true;
   gx_old_width = gx_old_height = 0;
}

#define ASM_BLITTER

#ifdef ASM_BLITTER

static __attribute__ ((noinline)) void update_texture_asm(const uint32_t *src, const uint32_t *dst,
      unsigned width, unsigned height, unsigned pitch)
{
   register uint32_t tmp0, tmp1, tmp2, tmp3, line2, line2b, line3, line3b, line4, line4b, line5;

   __asm__ __volatile__ (
      "     srwi     %[width],   %[width],   2           \n"
      "     srwi     %[height],  %[height],  2           \n"
      "     subi     %[tmp3],    %[dst],     4           \n"
      "     mr       %[dst],     %[tmp3]                 \n"
      "     subi     %[dst],     %[dst],     4           \n"
      "     mr       %[line2],   %[pitch]                \n"
      "     addi     %[line2b],  %[line2],   4           \n"
      "     mulli    %[line3],   %[pitch],   2           \n"
      "     addi     %[line3b],  %[line3],   4           \n"
      "     mulli    %[line4],   %[pitch],   3           \n"
      "     addi     %[line4b],  %[line4],   4           \n"
      "     mulli    %[line5],   %[pitch],   4           \n"

      "2:   mtctr    %[width]                            \n"
      "     mr       %[tmp0],    %[src]                  \n"

      "1:   lwz      %[tmp1],    0(%[src])               \n"
      "     stwu     %[tmp1],    8(%[dst])               \n"
      "     lwz      %[tmp2],    4(%[src])               \n"
      "     stwu     %[tmp2],    8(%[tmp3])              \n"

      "     lwzx     %[tmp1],    %[line2],   %[src]      \n"
      "     stwu     %[tmp1],    8(%[dst])               \n"
      "     lwzx     %[tmp2],    %[line2b],  %[src]      \n"
      "     stwu     %[tmp2],    8(%[tmp3])              \n"

      "     lwzx     %[tmp1],    %[line3],   %[src]      \n"
      "     stwu     %[tmp1],    8(%[dst])               \n"
      "     lwzx     %[tmp2],    %[line3b],  %[src]      \n"
      "     stwu     %[tmp2],    8(%[tmp3])              \n"

      "     lwzx     %[tmp1],    %[line4],   %[src]      \n"
      "     stwu     %[tmp1],    8(%[dst])               \n"
      "     lwzx     %[tmp2],    %[line4b],  %[src]      \n"
      "     stwu     %[tmp2],    8(%[tmp3])              \n"

      "     addi     %[src],     %[src],     8           \n"
      "     bdnz     1b                                  \n"

      "     add      %[src],     %[tmp0],    %[line5]    \n"
      "     subic.   %[height],  %[height],  1           \n"
      "     bne      2b                                  \n"
      :  [tmp0]   "=&b" (tmp0),
         [tmp1]   "=&b" (tmp1),
         [tmp2]   "=&b" (tmp2),
         [tmp3]   "=&b" (tmp3),
         [line2]  "=&b" (line2),
         [line2b] "=&b" (line2b),
         [line3]  "=&b" (line3),
         [line3b] "=&b" (line3b),
         [line4]  "=&b" (line4),
         [line4b] "=&b" (line4b),
         [line5]  "=&b" (line5),
         [dst]    "+b"  (dst)
      :  [src]    "b"   (src),
         [width]  "b"   (width),
         [height] "b"   (height),
         [pitch]  "b"   (pitch)
   );
}

#endif

#define BLIT_LINE(off) \
{ \
   const uint32_t *tmp_src = src2; \
   uint32_t *tmp_dst = dst; \
   for (unsigned x = 0; x < width2; x += 8, tmp_src += 8, tmp_dst += 32) \
   { \
      tmp_dst[ 0 + off] = tmp_src[0]; \
      tmp_dst[ 1 + off] = tmp_src[1]; \
      tmp_dst[ 8 + off] = tmp_src[2]; \
      tmp_dst[ 9 + off] = tmp_src[3]; \
      tmp_dst[16 + off] = tmp_src[4]; \
      tmp_dst[17 + off] = tmp_src[5]; \
      tmp_dst[24 + off] = tmp_src[6]; \
      tmp_dst[25 + off] = tmp_src[7]; \
   } \
   src2 += tmp_pitch; \
}

static void update_texture(const uint32_t *src,
      unsigned width, unsigned height, unsigned pitch)
{
   gx_video_t *gx = (gx_video_t*)driver.video_data;
   if (src)
   {
#ifdef ASM_BLITTER
      if (width && height && !(width & 3) && !(height & 3))
      {
         update_texture_asm(src, g_tex.data, width, height, pitch);
      }
      else
#endif
      {
         width &= ~15;
         height &= ~3;
         unsigned tmp_pitch = pitch >> 2;
         unsigned width2 = width >> 1;

         // Texture data is 4x4 tiled @ 15bpp.
         // Use 32-bit to transfer more data per cycle.
         const uint32_t *src2 = src;
         uint32_t *dst = g_tex.data;
         for (unsigned i = 0; i < height; i += 4, dst += 4 * width2)
         {
            BLIT_LINE(0)
            BLIT_LINE(2)
            BLIT_LINE(4)
            BLIT_LINE(6)
         }
      }
   }

   if(gx->menu_render)
   {
#ifdef ASM_BLITTER
      update_texture_asm(gx->menu_data, menu_tex.data, RGUI_WIDTH, RGUI_HEIGHT, RGUI_WIDTH * 2);
#else
      unsigned tmp_pitch = (RGUI_WIDTH * 2) >> 2;
      unsigned width2 = RGUI_WIDTH >> 1;

      const uint32_t *src2 = gx->menu_data;
      uint32_t *dst = menu_tex.data;
      for (unsigned i = 0; i < RGUI_HEIGHT; i += 4, dst += 4 * width2)
      {
         BLIT_LINE(0)
         BLIT_LINE(2)
         BLIT_LINE(4)
         BLIT_LINE(6)
      }
#endif
   }

   DCFlushRange(g_tex.data, sizeof(g_tex.data));
   DCFlushRange(menu_tex.data, sizeof(menu_tex.data));
   GX_InvalidateTexAll();
}

static void gx_resize(gx_video_t *gx)
{
   int x = 0, y = 0;
   unsigned width = gx->win_width, height = gx->win_height;

#ifdef HW_RVL
   VIDEO_SetTrapFilter(g_extern.console.screen.state.soft_filter.enable);
#endif
   GX_SetDispCopyGamma(g_extern.console.screen.gamma_correction);

   if (gx->keep_aspect && gx_mode.efbHeight >= 480) // ingore this for custom resolutions
   {
      float desired_aspect = g_settings.video.aspect_ratio;
      if (desired_aspect == 0.0)
         desired_aspect = 1.0;
#ifdef HW_RVL
      float device_aspect = CONF_GetAspectRatio() == CONF_ASPECT_4_3 ? 4.0 / 3.0 : 16.0 / 9.0;
#else
      float device_aspect = 4.0 / 3.0;
#endif
      if (g_orientation == ORIENTATION_VERTICAL || g_orientation == ORIENTATION_FLIPPED_ROTATED)
         desired_aspect = 1.0 / desired_aspect;
      float delta;

#ifdef RARCH_CONSOLE
      if (g_settings.video.aspect_ratio_idx == ASPECT_RATIO_CUSTOM)
      {
         if (!g_extern.console.screen.viewports.custom_vp.width || !g_extern.console.screen.viewports.custom_vp.height)
         {
            g_extern.console.screen.viewports.custom_vp.x = 0;
            g_extern.console.screen.viewports.custom_vp.y = 0;
            g_extern.console.screen.viewports.custom_vp.width = gx->win_width;
            g_extern.console.screen.viewports.custom_vp.height = gx->win_height;
         }

         x      = g_extern.console.screen.viewports.custom_vp.x;
         y      = g_extern.console.screen.viewports.custom_vp.y;
         width  = g_extern.console.screen.viewports.custom_vp.width;
         height = g_extern.console.screen.viewports.custom_vp.height;
      }
      else
#endif
      {
         if (fabs(device_aspect - desired_aspect) < 0.0001)
         {
            // If the aspect ratios of screen and desired aspect ratio are sufficiently equal (floating point stuff), 
            // assume they are actually equal.
         }
         else if (device_aspect > desired_aspect)
         {
            delta = (desired_aspect / device_aspect - 1.0) / 2.0 + 0.5;
            x     = (unsigned)(width * (0.5 - delta));
            width = (unsigned)(2.0 * width * delta);
         }
         else
         {
            delta  = (device_aspect / desired_aspect - 1.0) / 2.0 + 0.5;
            y      = (unsigned)(height * (0.5 - delta));
            height = (unsigned)(2.0 * height * delta);
         }
      }
   }

   GX_SetViewport(x, y, width, height, 0, 1);

   Mtx44 m1, m2;
   float top = 1, bottom = -1, left = -1, right = 1;
   if (g_extern.console.screen.state.overscan.enable)
   {
      top    -= g_extern.console.screen.overscan_amount / 2;
      left   += g_extern.console.screen.overscan_amount / 2;
      right  -= g_extern.console.screen.overscan_amount / 2;
      bottom += g_extern.console.screen.overscan_amount / 2;
   }
   guOrtho(m1, top, bottom, left, right, 0, 1);
   unsigned degrees;
   switch(g_orientation)
   {
      case ORIENTATION_VERTICAL:
         degrees = 90;
         break;
      case ORIENTATION_FLIPPED:
         degrees = 180;
         break;
      case ORIENTATION_FLIPPED_ROTATED:
         degrees = 270;
         break;
      default:
         degrees = 0;
         break;
   }
   guMtxIdentity(m2);
   guMtxRotDeg(m2, 'Z', degrees);
   guMtxConcat(m1, m2, m1);
   GX_LoadPosMtxImm(m1, GX_PNMTX0);

   init_texture(4, 4);
   gx_old_width = gx_old_height = 0;
   gx->should_resize = false;
}

static void gx_blit_line(unsigned x, unsigned y, const char *message)
{
   gx_video_t *gx = (gx_video_t*)driver.video_data;

   const GXColor b = {
      .r = 0x00,
      .g = 0x00,
      .b = 0x00,
      .a = 0xff
   };
   const GXColor w = {
      .r = 0xff,
      .g = 0xff,
      .b = 0xff,
      .a = 0xff
   };

   unsigned h;

   if (!*message)
      return;

   bool double_width = gx_mode.fbWidth > 400;
   unsigned width = (double_width ? 2 : 1);
   unsigned height = FONT_HEIGHT * (gx->double_strike ? 1 : 2);
   for (h = 0; h < height; h++)
   {
      GX_PokeARGB(x, y + h, b);
      if (double_width)
         GX_PokeARGB(x + 1, y + h, b);
   }

   x += (double_width ? 2 : 1);

   while (*message)
   {
      for (unsigned j = 0; j < FONT_HEIGHT; j++)
      {
         for (unsigned i = 0; i < FONT_WIDTH; i++)
         {
            GXColor c;
            uint8_t rem = 1 << ((i + j * FONT_WIDTH) & 7);
            unsigned offset = (i + j * FONT_WIDTH) >> 3;
            bool col = (_binary_console_font_bin_start[FONT_OFFSET((unsigned char) *message) + offset] & rem);

            if (col)
               c = w;
            else
               c = b;

            if (!gx->double_strike)
            {
               GX_PokeARGB(x + (i * width),     y + (j * 2),     c);
               if (double_width)
               {
                  GX_PokeARGB(x + (i * width) + 1, y + (j * 2),     c);
                  GX_PokeARGB(x + (i * width) + 1, y + (j * 2) + 1, c);
               }
               GX_PokeARGB(x + (i * width),     y + (j * 2) + 1, c);
            }
            else
            {
               GX_PokeARGB(x + (i * width),     y + j, c);
               if (double_width)
                  GX_PokeARGB(x + (i * width) + 1, y + j, c);
            }
         }
      }

      for (unsigned h = 0; h < height; h++)
      {
         GX_PokeARGB(x + (FONT_WIDTH * width), y + h, b);
         if (double_width)
            GX_PokeARGB(x + (FONT_WIDTH * width) + 1, y + h, b);
      }

      x += FONT_WIDTH_STRIDE * (double_width ? 2 : 1);
      message++;
   }
}

static bool gx_frame(void *data, const void *frame,
      unsigned width, unsigned height, unsigned pitch,
      const char *msg)
{
   gx_video_t *gx = (gx_video_t*)driver.video_data;
   bool menu_render = gx->menu_render;
   bool should_resize = gx->should_resize;
   u8 clear_efb = GX_FALSE;

   (void)data;

   if (msg)
      snprintf(gx->msg, sizeof(gx->msg), "%s", msg);
   else
      gx->msg[0] = 0;

   if(!frame && !menu_render)
      return true;

   if (!frame)
   {
      width = height = 4; // draw a black square in the background
   }

   gx->frame_count++;

   if(should_resize)
   {
      gx_resize(gx);
      clear_efb = GX_TRUE;
   }

   while ((g_vsync || menu_render) && !g_draw_done)
      LWP_ThreadSleep(g_video_cond);

   if (width != gx_old_width || height != gx_old_height)
   {
      init_texture(width, height);
      gx_old_width = width;
      gx_old_height = height;
   }

   g_draw_done = false;
   g_current_framebuf ^= 1;
   update_texture(frame, width, height, pitch);

   //if (frame)
   {
      GX_LoadTexObj(&g_tex.obj, GX_TEXMAP0);
      GX_CallDispList(display_list, display_list_size);
      GX_DrawDone();
   }

   if(menu_render)
   {
      GX_LoadTexObj(&menu_tex.obj, GX_TEXMAP0);
      GX_CallDispList(display_list, display_list_size);
      GX_DrawDone();
   }

   if (g_extern.console.rmenu.state.msg_fps.enable)
   {
      char fps_txt[128];
      char mem1_txt[128];
      unsigned x = 15;
      unsigned y = 15;

      gfx_fps_title(fps_txt, sizeof(fps_txt));
      gx_blit_line(x, y, fps_txt);
      y += FONT_HEIGHT * (gx->double_strike ? 1 : 2);
      snprintf(mem1_txt, sizeof(mem1_txt), "MEM1: %8d / %8d", SYSMEM1_SIZE - SYS_GetArena1Size(), SYSMEM1_SIZE);
      gx_blit_line(x, y, mem1_txt);
#ifdef HW_RVL
      y += FONT_HEIGHT * (gx->double_strike ? 1 : 2);
      char mem2_txt[128];
      snprintf(mem2_txt, sizeof(mem2_txt), "MEM2: %8d / %8d", gx_mem2_used(), gx_mem2_total());
      gx_blit_line(x, y, mem2_txt);
#endif
   }

   GX_CopyDisp(g_framebuf[g_current_framebuf], clear_efb);
   GX_Flush();
   VIDEO_SetNextFramebuffer(g_framebuf[g_current_framebuf]);
   VIDEO_Flush();

   return true;
}

static void gx_set_nonblock_state(void *data, bool state)
{
   (void)data;
   g_vsync = !state;
}

static bool gx_alive(void *data)
{
   (void)data;
   return true;
}

static bool gx_focus(void *data)
{
   (void)data;
   return true;
}

static void gx_free(void *data)
{
   (void)data;
}

static void gx_set_rotation(void * data, unsigned orientation)
{
   (void)data;
   gx_video_t *gx = (gx_video_t*)driver.video_data;
   g_orientation = orientation;
   gx->should_resize = true;
}

static void gx_apply_state_changes(void)
{
   gx_video_t *gx = (gx_video_t*)driver.video_data;
   gx->should_resize = true;
}

const video_driver_t video_gx = {
   .init = gx_init,
   .frame = gx_frame,
   .alive = gx_alive,
   .set_nonblock_state = gx_set_nonblock_state,
   .focus = gx_focus,
   .free = gx_free,
   .ident = "gx",
   .set_rotation = gx_set_rotation,
   .start = gx_start,
   .stop = gx_stop,
   .restart = gx_restart,
   .apply_state_changes = gx_apply_state_changes,
};
