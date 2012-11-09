/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2012 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2012 - Daniel De Matteis
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

#include "../../driver.h"
#include "../gfx_common.h"
#include "../gl_common.h"

#include <EGL/egl.h> /* Requires NDK r5 or newer */
#include <GLES/gl.h>

#include "../../android/native/jni/android_general.h"

#include <stdint.h>

#ifdef HAVE_GLSL
#include "../shader_glsl.h"
#endif

static EGLContext g_egl_ctx;
static EGLSurface g_egl_surf;
static EGLDisplay g_egl_dpy;
static EGLConfig g_config;

GLfloat _angle;

static enum gfx_ctx_api g_api;

static float gfx_ctx_get_aspect_ratio(void)
{
   return 4.0f / 3.0f;
}

static void gfx_ctx_set_swap_interval(unsigned interval)
{
   RARCH_LOG("gfx_ctx_set_swap_interval(%d).\n", interval);
   eglSwapInterval(g_egl_dpy, interval);
}

static void gfx_ctx_destroy(void)
{
   RARCH_LOG("gfx_ctx_destroy().\n");
   eglMakeCurrent(g_egl_dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
   eglDestroyContext(g_egl_dpy, g_egl_ctx);
   eglDestroySurface(g_egl_dpy, g_egl_surf);
   eglTerminate(g_egl_dpy);

   g_egl_dpy = EGL_NO_DISPLAY;
   g_egl_surf = EGL_NO_SURFACE;
   g_egl_ctx = EGL_NO_CONTEXT;
   g_config   = 0;
}

static void gfx_ctx_get_video_size(unsigned *width, unsigned *height)
{
   (void)width;
   (void)height;

   if (g_egl_dpy)
   {
      EGLint gl_width, gl_height;
      eglQuerySurface(g_egl_dpy, g_egl_surf, EGL_WIDTH, &gl_width);
      eglQuerySurface(g_egl_dpy, g_egl_surf, EGL_HEIGHT, &gl_height);
      *width = gl_width;
      *height = gl_height;
   }
}

static bool gfx_ctx_init(void)
{
   RARCH_LOG("gfx_ctx_init().\n");
   const EGLint attribs[] = {
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_BLUE_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_RED_SIZE, 8,
      EGL_NONE
   };
   EGLint num_config;
   EGLint egl_version_major, egl_version_minor;
   EGLint format;
   EGLint width;
   EGLint height;
   GLfloat ratio;

   EGLint context_attributes[] = {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
   };

   RARCH_LOG("Initializing context\n");

   if ((g_egl_dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY)
   {
      RARCH_ERR("eglGetDisplay failed.\n");
      goto error;
   }

   if (!eglInitialize(g_egl_dpy, &egl_version_major, &egl_version_minor))
   {
      RARCH_ERR("eglInitialize failed.\n");
      goto error;
   }

   RARCH_LOG("[ANDROID/EGL]: EGL version: %d.%d\n", egl_version_major, egl_version_minor);

   if (!eglChooseConfig(g_egl_dpy, attribs, &g_config, 1, &num_config))
   {
      RARCH_ERR("eglChooseConfig failed.\n");
      goto error;
   }

   int var = eglGetConfigAttrib(g_egl_dpy, g_config, EGL_NATIVE_VISUAL_ID, &format);

   if (!var)
   {
      RARCH_ERR("eglGetConfigAttrib failed: %d.\n", var);
      goto error;
   }

   ANativeWindow_setBuffersGeometry(g_android.app->window, 0, 0, format);

   if (!(g_egl_surf = eglCreateWindowSurface(g_egl_dpy, g_config, g_android.app->window, 0)))
   {
      RARCH_ERR("eglCreateWindowSurface failed.\n");
      goto error;
   }

   if (!(g_egl_ctx = eglCreateContext(g_egl_dpy, g_config, 0, context_attributes)))
   {
      RARCH_ERR("eglCreateContext failed.\n");
      goto error;
   }

   if (!eglMakeCurrent(g_egl_dpy, g_egl_surf, g_egl_surf, g_egl_ctx))
   {
      RARCH_ERR("eglMakeCurrent failed.\n");
      goto error;
   }

   if (!eglQuerySurface(g_egl_dpy, g_egl_surf, EGL_WIDTH, &width) ||
         !eglQuerySurface(g_egl_dpy, g_egl_surf, EGL_HEIGHT, &height))
   {
      RARCH_ERR("eglQuerySurface failed.\n");
      goto error;
   }

   if (g_android.input_state & (1ULL << RARCH_REENTRANT))
   {
      RARCH_LOG("[ANDROID/EGL]: Setting up reentrant state.\n");

      gl_t *gl = (gl_t*)driver.video_data;

      // Get real known video size, which might have been altered by context.
      gfx_ctx_get_video_size(&gl->win_width, &gl->win_height);
      RARCH_LOG("GL: Using resolution %ux%u\n", gl->win_width, gl->win_height);

      if (gl->full_x || gl->full_y) // We got bogus from gfx_ctx_get_video_size. Replace.
      {
         gl->full_x = gl->win_width;
         gl->full_y = gl->win_height;
      }

#ifdef HAVE_GLSL
      gl_glsl_use(0);
#endif
      gl_set_viewport(gl, gl->win_width, gl->win_height, false, true);
#ifdef HAVE_GLSL
      gl_glsl_use(1);
#endif
      gl_set_viewport(gl, gl->win_width, gl->win_height, false, true);
   }

   return true;

error:
   RARCH_ERR("EGL error: %d.\n", eglGetError());
   gfx_ctx_destroy();
   return false;
}

static void gfx_ctx_swap_buffers(void)
{
   eglSwapBuffers(g_egl_dpy, g_egl_surf);
}

static void gfx_ctx_check_window(bool *quit,
      bool *resize, unsigned *width, unsigned *height, unsigned frame_count)
{
   (void)width;
   (void)height;
   (void)frame_count;

   *quit = false;
   *resize = false;

   // Check if we are exiting.
   if (g_android.app->destroyRequested != 0 || (g_android.input_state & (1ULL << RARCH_KILL)))
      *quit = true;
}

static void gfx_ctx_clear(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
}

static void gfx_ctx_set_blend(bool enable)
{
   if(enable)
   {
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      glEnable(GL_BLEND);
   }
   else
      glDisable(GL_BLEND);
}

static void gfx_ctx_set_resize(unsigned width, unsigned height)
{
   (void)width;
   (void)height;
}

static void gfx_ctx_update_window_title(bool reset)
{
   (void)reset;
   if (reset)
      gfx_window_title_reset();

   char buf[128];

   if (gfx_window_title(buf, sizeof(buf)))
      RARCH_LOG("%s.\n", buf);
}



static bool gfx_ctx_set_video_mode(
      unsigned width, unsigned height,
      unsigned bits, bool fullscreen)
{
   (void)width;
   (void)height;
   (void)bits;
   (void)fullscreen;
   return true;
}


static void gfx_ctx_input_driver(const input_driver_t **input, void **input_data)
{
   *input = NULL;
   *input_data = NULL;
}

static gfx_ctx_proc_t gfx_ctx_get_proc_address(const char *symbol)
{
   rarch_assert(sizeof(void*) == sizeof(void (*)(void)));
   gfx_ctx_proc_t ret;

   void *sym__ = eglGetProcAddress(symbol);
   memcpy(&ret, &sym__, sizeof(void*));

   return ret;
}

static bool gfx_ctx_bind_api(enum gfx_ctx_api api)
{
   g_api = api;
   return api == GFX_CTX_OPENGL_ES_API;
}

static bool gfx_ctx_has_focus(void)
{
   return true;
}

static bool gfx_ctx_init_egl_image_buffer(const video_info_t *video)
{
   return false;
}

static bool gfx_ctx_write_egl_image(const void *frame, unsigned width, unsigned height, unsigned pitch, bool rgb32, unsigned index, void **image_handle)
{
   return false;
}

const gfx_ctx_driver_t gfx_ctx_android = {
   gfx_ctx_init,
   gfx_ctx_destroy,
   gfx_ctx_bind_api,
   gfx_ctx_set_swap_interval,
   gfx_ctx_set_video_mode,
   gfx_ctx_get_video_size,
   NULL,
   gfx_ctx_update_window_title,
   gfx_ctx_check_window,
   gfx_ctx_set_resize,
   gfx_ctx_has_focus,
   gfx_ctx_swap_buffers,
   gfx_ctx_input_driver,
   NULL,
   gfx_ctx_init_egl_image_buffer,
   gfx_ctx_write_egl_image,
   "android",
};
