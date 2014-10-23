/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
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

#include "../general.h"
#include "gfx_context.h"
#include "../general.h"
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

static const gfx_ctx_driver_t *gfx_ctx_drivers[] = {
#if defined(__CELLOS_LV2__)
   &gfx_ctx_ps3,
#endif
#if defined(HAVE_WIN32_D3D9) || defined(_XBOX)
   &gfx_ctx_d3d,
#endif
#if defined(HAVE_VIDEOCORE)
   &gfx_ctx_videocore,
#endif
#if defined(HAVE_MALI_FBDEV)
   &gfx_ctx_mali_fbdev,
#endif
#if defined(HAVE_VIVANTE_FBDEV)
   &gfx_ctx_vivante_fbdev,
#endif
#if defined(_WIN32) && defined(HAVE_OPENGL)
   &gfx_ctx_wgl,
#endif
#if defined(HAVE_X11) && defined(HAVE_OPENGL) && !defined(HAVE_OPENGLES)
   &gfx_ctx_glx,
#endif
#if defined(HAVE_WAYLAND) && defined(HAVE_OPENGL) && defined(HAVE_EGL)
   &gfx_ctx_wayland,
#endif
#if defined(HAVE_X11) && defined(HAVE_OPENGL) && defined(HAVE_EGL)
   &gfx_ctx_x_egl,
#endif
#if defined(HAVE_KMS)
   &gfx_ctx_drm_egl,
#endif
#if defined(ANDROID)
   &gfx_ctx_android,
#endif
#if defined(__QNX__)
   &gfx_ctx_bbqnx,
#endif
#if defined(IOS) || defined(OSX)
   /* < Don't use __APPLE__ as it breaks basic SDL builds */
   &gfx_ctx_apple,
#endif
#if (defined(HAVE_SDL) || defined(HAVE_SDL2)) && defined(HAVE_OPENGL)
   &gfx_ctx_sdl_gl,
#endif
#ifdef EMSCRIPTEN
   &gfx_ctx_emscripten,
#endif
   &gfx_ctx_null,
   NULL
};

const gfx_ctx_driver_t *gfx_ctx_find_driver(const char *ident)
{
   unsigned i;
   for (i = 0; i < ARRAY_SIZE(gfx_ctx_drivers); i++)
   {
      if (gfx_ctx_drivers[i] && strcmp(gfx_ctx_drivers[i]->ident, ident) == 0)
         return gfx_ctx_drivers[i];
   }

   return NULL;
}

static const gfx_ctx_driver_t *ctx_init(void *data,
      const gfx_ctx_driver_t *ctx,
      const char *driver,
      enum gfx_ctx_api api, unsigned major,
      unsigned minor, bool hw_render_ctx)
{
   const gfx_ctx_driver_t *tmp = gfx_ctx_find_driver(driver);

   if (tmp)
      ctx = (const gfx_ctx_driver_t*)tmp;

   if (ctx->bind_api(data, api, major, minor))
   {
      if (ctx->bind_hw_render)
         ctx->bind_hw_render(data,
               g_settings.video.shared_context && hw_render_ctx);

      if (ctx->init(data))
         return ctx;
   }

   return NULL;
}

const gfx_ctx_driver_t *gfx_ctx_init_first(void *data,
      const char *driver,
      enum gfx_ctx_api api, unsigned major,
      unsigned minor, bool hw_render_ctx)
{
   unsigned i;
   const gfx_ctx_driver_t *ctx = NULL;

   for (i = 0; gfx_ctx_drivers[i]; i++)
   {
      ctx = ctx_init(data, gfx_ctx_drivers[i], driver,
            api, major, minor, hw_render_ctx);

      if (ctx)
         return ctx;
   }

   return NULL;
}
