/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2015 - Daniel De Matteis
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

#ifndef __VIDEO_DRIVER__H
#define __VIDEO_DRIVER__H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <boolean.h>
#include <gfx/scaler/scaler.h>
#include "video_filter.h"
#include "video_shader_parse.h"

#include "../libretro.h"
#include "../input/input_driver.h"


#ifdef __cplusplus
extern "C" {
#endif

struct rarch_viewport;

typedef struct video_info
{
   unsigned width;
   unsigned height;
   bool fullscreen;
   bool vsync;
   bool force_aspect;
#ifdef GEKKO
   /* TODO - we can't really have driver system-specific
    * variables in here. There should be some
    * kind of publicly accessible driver implementation
    * video struct for specific things like this.
    */
   unsigned viwidth;
#endif
   bool vfilter;
   bool smooth;
   /* Maximum input size: RARCH_SCALE_BASE * input_scale */
   unsigned input_scale;
   /* Use 32bit RGBA rather than native XBGR1555. */
   bool rgb32;
} video_info_t;

struct font_params
{
   float x;
   float y;
   float scale;
   /* Drop shadow color multiplier. */
   float drop_mod;
   /* Drop shadow offset.
    * If both are 0, no drop shadow will be rendered. */
   int drop_x, drop_y;
   /* ABGR. Use the macros. */
   uint32_t color;
   bool full_screen;
   bool align_right;
};


#define FONT_COLOR_RGBA(r, g, b, a) (((r) << 0) | ((g) << 8) | ((b) << 16) | ((a) << 24))
#define FONT_COLOR_GET_RED(col)   (((col) >>  0) & 0xff)
#define FONT_COLOR_GET_GREEN(col) (((col) >>  8) & 0xff)
#define FONT_COLOR_GET_BLUE(col)  (((col) >> 16) & 0xff)
#define FONT_COLOR_GET_ALPHA(col) (((col) >> 24) & 0xff)

/* Optionally implemented interface to poke more
 * deeply into video driver. */

typedef struct video_poke_interface
{
   void (*set_filtering)(void *data, unsigned index, bool smooth);
#ifdef HAVE_FBO
   uintptr_t (*get_current_framebuffer)(void *data);
#endif
   retro_proc_address_t (*get_proc_address)(void *data, const char *sym);
   void (*make_current_context)(void *data);
   void (*set_aspect_ratio)(void *data, unsigned aspectratio_index);
   void (*apply_state_changes)(void *data);

#ifdef HAVE_MENU
   /* Update texture. */
   void (*set_texture_frame)(void *data, const void *frame, bool rgb32,
         unsigned width, unsigned height, float alpha);

   /* Enable or disable rendering. */
   void (*set_texture_enable)(void *data, bool enable, bool full_screen);
#endif
   void (*set_osd_msg)(void *data, const char *msg,
         const struct font_params *params, void *font);

   void (*show_mouse)(void *data, bool state);
   void (*grab_mouse_toggle)(void *data);

   struct video_shader *(*get_current_shader)(void *data);
} video_poke_interface_t;

typedef struct video_driver
{
   /* Should the video driver act as an input driver as well?
    * The video initialization might preinitialize an input driver 
    * to override the settings in case the video driver relies on 
    * input driver for event handling. */
   void *(*init)(const video_info_t *video, const input_driver_t **input,
         void **input_data); 

   /* msg is for showing a message on the screen along with the video frame. */
   bool (*frame)(void *data, const void *frame, unsigned width,
         unsigned height, unsigned pitch, const char *msg);

   /* Should we care about syncing to vblank? Fast forwarding. */
   void (*set_nonblock_state)(void *data, bool toggle);

   /* Is the window still active? */
   bool (*alive)(void *data);

   /* Does the window have focus? */
   bool (*focus)(void *data);

   /* Should the screensaver be suppressed? */
   bool (*suppress_screensaver)(void *data, bool enable);

   /* Does the graphics context support windowed mode? */
   bool (*has_windowed)(void *data);

   /* Sets shader. Might not be implemented. Will be moved to
    * poke_interface later. */
   bool (*set_shader)(void *data, enum rarch_shader_type type,
         const char *path);

   /* Frees driver. */
   void (*free)(void *data);

   /* Human-readable identifier. */
   const char *ident;

   void (*set_rotation)(void *data, unsigned rotation);
   void (*viewport_info)(void *data, struct rarch_viewport *vp);

   /* Reads out in BGR byte order (24bpp). */
   bool (*read_viewport)(void *data, uint8_t *buffer);

#ifdef HAVE_OVERLAY
   void (*overlay_interface)(void *data, const video_overlay_interface_t **iface);
#endif
   void (*poke_interface)(void *data, const video_poke_interface_t **iface);
   unsigned (*wrap_type_to_enum)(enum gfx_wrap_type type);
} video_driver_t;

extern video_driver_t video_gl;
extern video_driver_t video_psp1;
extern video_driver_t video_vita;
extern video_driver_t video_d3d;
extern video_driver_t video_gx;
extern video_driver_t video_xenon360;
extern video_driver_t video_xvideo;
extern video_driver_t video_xdk_d3d;
extern video_driver_t video_sdl;
extern video_driver_t video_sdl2;
extern video_driver_t video_vg;
extern video_driver_t video_omap;
extern video_driver_t video_exynos;
extern video_driver_t video_null;

enum rarch_display_type
{
   /* Non-bindable types like consoles, KMS, VideoCore, etc. */
   RARCH_DISPLAY_NONE = 0,
   /* video_display => Display*, video_window => Window */
   RARCH_DISPLAY_X11,
   /* video_display => N/A, video_window => HWND */
   RARCH_DISPLAY_WIN32,
   RARCH_DISPLAY_OSX
};

/**
 * video_driver_find_handle:
 * @index              : index of driver to get handle to.
 *
 * Returns: handle to video driver at index. Can be NULL
 * if nothing found.
 **/
const void *video_driver_find_handle(int index);

/**
 * video_driver_find_ident:
 * @index              : index of driver to get handle to.
 *
 * Returns: Human-readable identifier of video driver at index. Can be NULL
 * if nothing found.
 **/
const char *video_driver_find_ident(int index);

/**
 * config_get_video_driver_options:
 *
 * Get an enumerated list of all video driver names, separated by '|'.
 *
 * Returns: string listing of all video driver names, separated by '|'.
 **/
const char* config_get_video_driver_options(void);

void find_video_driver(void);

/**
 * video_driver_resolve:
 * @drv                : real video driver will be set to this.
 *
 * Use this if you need the real video driver 
 * and driver data pointers.
 *
 * Returns: video driver's userdata.
 **/
void *video_driver_resolve(const video_driver_t **drv);

/**
 * video_driver_get_current_framebuffer:
 *
 * Gets pointer to current hardware renderer framebuffer object.
 * Used by RETRO_ENVIRONMENT_SET_HW_RENDER.
 *
 * Returns: pointer to hardware framebuffer object, otherwise 0.
 **/
uintptr_t video_driver_get_current_framebuffer(void);

retro_proc_address_t video_driver_get_proc_address(const char *sym);

void video_driver_make_current_context(void);

bool video_driver_is_alive(void);

bool video_driver_has_focus(void);

bool video_driver_set_shader(enum rarch_shader_type type,
      const char *shader);

void uninit_video_input(void);

void init_video(void);

#ifdef __cplusplus
}
#endif

#endif
