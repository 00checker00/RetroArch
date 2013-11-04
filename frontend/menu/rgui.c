/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2013 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2013 - Daniel De Matteis
 *  Copyright (C) 2012-2013 - Michael Lelli
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

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "rgui.h"
#include "menu_context.h"
#include "file_list.h"
#include "../../general.h"
#include "../../gfx/gfx_common.h"
#include "../../config.def.h"
#include "../../file.h"
#include "../../dynamic.h"
#include "../../compat/posix_string.h"
#include "../../gfx/shader_parse.h"
#include "../../performance.h"
#include "../../input/input_common.h"

#ifdef HAVE_OPENGL
#include "../../gfx/gl_common.h"
#endif

#include "../../screenshot.h"

#if defined(HAVE_CG) || defined(HAVE_GLSL) || defined(HAVE_HLSL)
#define HAVE_SHADER_MANAGER
#endif

unsigned RGUI_WIDTH = 320;
unsigned RGUI_HEIGHT = 240;
uint16_t menu_framebuf[400 * 240];

#ifdef HAVE_SHADER_MANAGER
static int shader_manager_toggle_setting(rgui_handle_t *rgui, unsigned setting, rgui_action_t action);
#endif

static void rgui_flush_menu_stack_type(rgui_handle_t *rgui, unsigned final_type)
{
   rgui->need_refresh = true;
   unsigned type = 0;
   rgui_list_get_last(rgui->menu_stack, NULL, &type);
   while (type != final_type)
   {
      rgui_list_pop(rgui->menu_stack, &rgui->selection_ptr);
      rgui_list_get_last(rgui->menu_stack, NULL, &type);
   }
}

#include "rguidisp.c"

static void *rgui_init(void)
{
   uint16_t *framebuf = menu_framebuf;
   size_t framebuf_pitch = RGUI_WIDTH * sizeof(uint16_t);

   rgui_handle_t *rgui = (rgui_handle_t*)calloc(1, sizeof(*rgui));

   rgui->frame_buf = framebuf;
   rgui->frame_buf_pitch = framebuf_pitch;

   bool ret = rguidisp_init_font(rgui);

   if (!ret)
   {
      RARCH_ERR("No font bitmap or binary, abort");
      /* TODO - should be refactored - perhaps don't do rarch_fail but instead
       * exit program */
      g_extern.lifecycle_mode_state &= ~((1ULL << MODE_MENU) | (1ULL << MODE_GAME));
      return NULL;
   }

   strlcpy(rgui->base_path, g_settings.rgui_browser_directory, sizeof(rgui->base_path));

   rgui->menu_stack = (rgui_list_t*)calloc(1, sizeof(rgui_list_t));
   rgui->selection_buf = (rgui_list_t*)calloc(1, sizeof(rgui_list_t));
   rgui_list_push(rgui->menu_stack, "", RGUI_SETTINGS, 0);
   rgui->selection_ptr = 0;
   rgui->push_start_screen = g_settings.rgui_show_start_screen;
   g_settings.rgui_show_start_screen = false;
   menu_set_settings_populate_entries(rgui, RGUI_SETTINGS);

   return rgui;
}

static void rgui_free(void *data)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;
   if (rgui->alloc_font)
      free((uint8_t*)rgui->font);

#ifdef HAVE_DYNAMIC
   libretro_free_system_info(&rgui->info);
#endif

   rgui_list_free(rgui->menu_stack);
   rgui_list_free(rgui->selection_buf);
}

static int rgui_core_setting_toggle(unsigned setting, rgui_action_t action)
{
   unsigned index = setting - RGUI_SETTINGS_CORE_OPTION_START;
   switch (action)
   {
      case RGUI_ACTION_LEFT:
         core_option_prev(g_extern.system.core_options, index);
         break;

      case RGUI_ACTION_RIGHT:
      case RGUI_ACTION_OK:
         core_option_next(g_extern.system.core_options, index);
         break;

      case RGUI_ACTION_START:
         core_option_set_default(g_extern.system.core_options, index);
         break;

      default:
         break;
   }

   return 0;
}

static int rgui_settings_toggle_setting(rgui_handle_t *rgui, unsigned setting, rgui_action_t action, unsigned menu_type)
{
#ifdef HAVE_SHADER_MANAGER
   if (setting >= RGUI_SETTINGS_SHADER_FILTER && setting <= RGUI_SETTINGS_SHADER_LAST)
      return shader_manager_toggle_setting(rgui, setting, action);
#endif
   if (setting >= RGUI_SETTINGS_CORE_OPTION_START)
      return rgui_core_setting_toggle(setting, action);

   return menu_set_settings(setting, action);
}

#ifdef HAVE_SHADER_MANAGER
static int shader_manager_toggle_setting(rgui_handle_t *rgui, unsigned setting, rgui_action_t action)
{
   unsigned dist_shader = setting - RGUI_SETTINGS_SHADER_0;
   unsigned dist_filter = setting - RGUI_SETTINGS_SHADER_0_FILTER;
   unsigned dist_scale  = setting - RGUI_SETTINGS_SHADER_0_SCALE;

   if (setting == RGUI_SETTINGS_SHADER_FILTER)
   {
      switch (action)
      {
         case RGUI_ACTION_START:
            g_settings.video.smooth = true;
            break;

         case RGUI_ACTION_LEFT:
         case RGUI_ACTION_RIGHT:
         case RGUI_ACTION_OK:
            g_settings.video.smooth = !g_settings.video.smooth;
            break;

         default:
            break;
      }
   }
   else if (setting == RGUI_SETTINGS_SHADER_APPLY || setting == RGUI_SETTINGS_SHADER_PASSES)
      return menu_set_settings(setting, action);
   else if ((dist_shader % 3) == 0 || setting == RGUI_SETTINGS_SHADER_PRESET)
   {
      dist_shader /= 3;
      struct gfx_shader_pass *pass = setting == RGUI_SETTINGS_SHADER_PRESET ? 
         &rgui->shader.pass[dist_shader] : NULL;
      switch (action)
      {
         case RGUI_ACTION_OK:
            rgui_list_push(rgui->menu_stack, g_settings.video.shader_dir, setting, rgui->selection_ptr);
            rgui->selection_ptr = 0;
            rgui->need_refresh = true;
            break;

         case RGUI_ACTION_START:
            if (pass)
               *pass->source.cg = '\0';
            break;

         default:
            break;
      }
   }
   else if ((dist_filter % 3) == 0)
   {
      dist_filter /= 3;
      struct gfx_shader_pass *pass = &rgui->shader.pass[dist_filter];
      switch (action)
      {
         case RGUI_ACTION_START:
            rgui->shader.pass[dist_filter].filter = RARCH_FILTER_UNSPEC;
            break;

         case RGUI_ACTION_LEFT:
         case RGUI_ACTION_RIGHT:
         case RGUI_ACTION_OK:
         {
            unsigned delta = action == RGUI_ACTION_LEFT ? 2 : 1;
            pass->filter = (enum gfx_filter_type)((pass->filter + delta) % 3);
            break;
         }

         default:
         break;
      }
   }
   else if ((dist_scale % 3) == 0)
   {
      dist_scale /= 3;
      struct gfx_shader_pass *pass = &rgui->shader.pass[dist_scale];
      switch (action)
      {
         case RGUI_ACTION_START:
            pass->fbo.scale_x = pass->fbo.scale_y = 0;
            pass->fbo.valid = false;
            break;

         case RGUI_ACTION_LEFT:
         case RGUI_ACTION_RIGHT:
         case RGUI_ACTION_OK:
         {
            unsigned current_scale = pass->fbo.scale_x;
            unsigned delta = action == RGUI_ACTION_LEFT ? 5 : 1;
            current_scale = (current_scale + delta) % 6;
            pass->fbo.valid = current_scale;
            pass->fbo.scale_x = pass->fbo.scale_y = current_scale;
            break;
         }

         default:
         break;
      }
   }

   return 0;
}
#endif

// This only makes sense for PC so far.
// Consoles use set_keybind callbacks instead.
static int rgui_custom_bind_iterate(rgui_handle_t *rgui, rgui_action_t action)
{
   (void)action; // Have to ignore action here. Only bind that should work here is Quit RetroArch or something like that.

   render_text(rgui);

   char msg[256];
   snprintf(msg, sizeof(msg), "[%s]\npress joypad\n(RETURN to skip)", input_config_bind_map[rgui->binds.begin - RGUI_SETTINGS_BIND_BEGIN].desc);
   render_messagebox(rgui, msg);

   struct rgui_bind_state binds = rgui->binds;
   menu_poll_bind_state(&binds);

   if ((binds.skip && !rgui->binds.skip) || menu_poll_find_trigger(&rgui->binds, &binds))
   {
      binds.begin++;
      if (binds.begin <= binds.last)
         binds.target++;
      else
         rgui_list_pop(rgui->menu_stack, &rgui->selection_ptr);

      // Avoid new binds triggering things right away.
      rgui->trigger_state = 0;
      rgui->old_input_state = -1ULL;
   }
   rgui->binds = binds;
   return 0;
}

static int rgui_start_screen_iterate(rgui_handle_t *rgui, rgui_action_t action)
{
   render_text(rgui);
   unsigned i;
   char msg[1024];

   char desc[6][64];
   static const unsigned binds[] = {
      RETRO_DEVICE_ID_JOYPAD_UP,
      RETRO_DEVICE_ID_JOYPAD_DOWN,
      RETRO_DEVICE_ID_JOYPAD_A,
      RETRO_DEVICE_ID_JOYPAD_B,
      RARCH_MENU_TOGGLE,
      RARCH_QUIT_KEY,
   };

   for (i = 0; i < ARRAY_SIZE(binds); i++)
   {
      if (driver.input && driver.input->set_keybinds)
      {
         struct platform_bind key_label;
         strlcpy(key_label.desc, "Unknown", sizeof(key_label.desc));
         key_label.joykey = g_settings.input.binds[0][binds[i]].joykey;
         driver.input->set_keybinds(&key_label, 0, 0, 0, 1ULL << KEYBINDS_ACTION_GET_BIND_LABEL);
         strlcpy(desc[i], key_label.desc, sizeof(desc[i]));
      }
      else
      {
         const struct retro_keybind *bind = &g_settings.input.binds[0][binds[i]];
         input_get_bind_string(desc[i], bind, sizeof(desc[i]));
      }
   }

   snprintf(msg, sizeof(msg),
         "-- Welcome to RetroArch / RGUI --\n"
         " \n" // strtok_r doesn't split empty strings.

         "Basic RGUI controls:\n"
         "    Scroll (Up): %-20s\n"
         "  Scroll (Down): %-20s\n"
         "      Accept/OK: %-20s\n"
         "           Back: %-20s\n"
         "Enter/Exit RGUI: %-20s\n"
         " Exit RetroArch: %-20s\n"
         " \n"

         "To play a game:\n"
         "Load a libretro core (Core).\n"
         "Load a ROM (Load Game).     \n"
         " \n"

         "See Path Options to set directories\n"
         "for faster access to files.\n"
         " \n"

         "Press Accept/OK to continue.",
         desc[0], desc[1], desc[2], desc[3], desc[4], desc[5]);

   render_messagebox(rgui, msg);

   if (action == RGUI_ACTION_OK)
      rgui_list_pop(rgui->menu_stack, &rgui->selection_ptr);
   return 0;
}

static int rgui_viewport_iterate(rgui_handle_t *rgui, rgui_action_t action)
{
   rarch_viewport_t *custom = &g_extern.console.screen.viewports.custom_vp;

   unsigned menu_type = 0;
   rgui_list_get_last(rgui->menu_stack, NULL, &menu_type);

   struct retro_game_geometry *geom = &g_extern.system.av_info.geometry;
   int stride_x = g_settings.video.scale_integer ?
      geom->base_width : 1;
   int stride_y = g_settings.video.scale_integer ?
      geom->base_height : 1;

   switch (action)
   {
      case RGUI_ACTION_UP:
         if (menu_type == RGUI_SETTINGS_CUSTOM_VIEWPORT)
         {
            custom->y -= stride_y;
            custom->height += stride_y;
         }
         else if (custom->height >= (unsigned)stride_y)
            custom->height -= stride_y;

         if (driver.video_poke->apply_state_changes)
            driver.video_poke->apply_state_changes(driver.video_data);
         break;

      case RGUI_ACTION_DOWN:
         if (menu_type == RGUI_SETTINGS_CUSTOM_VIEWPORT)
         {
            custom->y += stride_y;
            if (custom->height >= (unsigned)stride_y)
               custom->height -= stride_y;
         }
         else
            custom->height += stride_y;

         if (driver.video_poke->apply_state_changes)
            driver.video_poke->apply_state_changes(driver.video_data);
         break;

      case RGUI_ACTION_LEFT:
         if (menu_type == RGUI_SETTINGS_CUSTOM_VIEWPORT)
         {
            custom->x -= stride_x;
            custom->width += stride_x;
         }
         else if (custom->width >= (unsigned)stride_x)
            custom->width -= stride_x;

         if (driver.video_poke->apply_state_changes)
            driver.video_poke->apply_state_changes(driver.video_data);
         break;

      case RGUI_ACTION_RIGHT:
         if (menu_type == RGUI_SETTINGS_CUSTOM_VIEWPORT)
         {
            custom->x += stride_x;
            if (custom->width >= (unsigned)stride_x)
               custom->width -= stride_x;
         }
         else
            custom->width += stride_x;

         if (driver.video_poke->apply_state_changes)
            driver.video_poke->apply_state_changes(driver.video_data);
         break;

      case RGUI_ACTION_CANCEL:
         rgui_list_pop(rgui->menu_stack, &rgui->selection_ptr);
         if (menu_type == RGUI_SETTINGS_CUSTOM_VIEWPORT_2)
         {
            rgui_list_push(rgui->menu_stack, "",
                  RGUI_SETTINGS_CUSTOM_VIEWPORT,
                  rgui->selection_ptr);
         }
         break;

      case RGUI_ACTION_OK:
         rgui_list_pop(rgui->menu_stack, &rgui->selection_ptr);
         if (menu_type == RGUI_SETTINGS_CUSTOM_VIEWPORT
               && !g_settings.video.scale_integer)
         {
            rgui_list_push(rgui->menu_stack, "",
                  RGUI_SETTINGS_CUSTOM_VIEWPORT_2,
                  rgui->selection_ptr);
         }
         break;

      case RGUI_ACTION_START:
         if (!g_settings.video.scale_integer)
         {
            rarch_viewport_t vp;
            driver.video->viewport_info(driver.video_data, &vp);

            if (menu_type == RGUI_SETTINGS_CUSTOM_VIEWPORT)
            {
               custom->width += custom->x;
               custom->height += custom->y;
               custom->x = 0;
               custom->y = 0;
            }
            else
            {
               custom->width = vp.full_width - custom->x;
               custom->height = vp.full_height - custom->y;
            }

            if (driver.video_poke->apply_state_changes)
               driver.video_poke->apply_state_changes(driver.video_data);
         }
         break;

      case RGUI_ACTION_MESSAGE:
         rgui->msg_force = true;
         break;

      default:
         break;
   }

   rgui_list_get_last(rgui->menu_stack, NULL, &menu_type);

   render_text(rgui);

   const char *base_msg = NULL;
   char msg[64];

   if (g_settings.video.scale_integer)
   {
      custom->x = 0;
      custom->y = 0;
      custom->width = ((custom->width + geom->base_width - 1) / geom->base_width) * geom->base_width;
      custom->height = ((custom->height + geom->base_height - 1) / geom->base_height) * geom->base_height;

      base_msg = "Set scale";
      snprintf(msg, sizeof(msg), "%s (%4ux%4u, %u x %u scale)",
            base_msg,
            custom->width, custom->height,
            custom->width / geom->base_width,
            custom->height / geom->base_height); 
   }
   else
   {
      if (menu_type == RGUI_SETTINGS_CUSTOM_VIEWPORT)
         base_msg = "Set Upper-Left Corner";
      else if (menu_type == RGUI_SETTINGS_CUSTOM_VIEWPORT_2)
         base_msg = "Set Bottom-Right Corner";

      snprintf(msg, sizeof(msg), "%s (%d, %d : %4ux%4u)",
            base_msg, custom->x, custom->y, custom->width, custom->height); 
   }
   render_messagebox(rgui, msg);

   if (!custom->width)
      custom->width = stride_x;
   if (!custom->height)
      custom->height = stride_y;

   aspectratio_lut[ASPECT_RATIO_CUSTOM].value =
      (float)custom->width / custom->height;

   if (driver.video_poke->apply_state_changes)
      driver.video_poke->apply_state_changes(driver.video_data);

   return 0;
}

static int rgui_settings_iterate(rgui_handle_t *rgui, rgui_action_t action)
{
   rgui->frame_buf_pitch = RGUI_WIDTH * 2;
   unsigned type = 0;
   const char *label = NULL;
   if (action != RGUI_ACTION_REFRESH)
      rgui_list_get_at_offset(rgui->selection_buf, rgui->selection_ptr, &label, &type);

   if (type == RGUI_SETTINGS_CORE)
   {
#if defined(HAVE_DYNAMIC)
      label = rgui->libretro_dir;
#elif defined(HAVE_LIBRETRO_MANAGEMENT)
      label = default_paths.core_dir;
#else
      label = ""; // Shouldn't happen ...
#endif
   }
   else if (type == RGUI_SETTINGS_CONFIG)
      label = g_settings.rgui_config_directory;
   else if (type == RGUI_SETTINGS_DISK_APPEND)
      label = rgui->base_path;

   const char *dir = NULL;
   unsigned menu_type = 0;
   rgui_list_get_last(rgui->menu_stack, &dir, &menu_type);

   if (rgui->need_refresh)
      action = RGUI_ACTION_NOOP;

   switch (action)
   {
      case RGUI_ACTION_UP:
         if (rgui->selection_ptr > 0)
            rgui->selection_ptr--;
         else
            rgui->selection_ptr = rgui->selection_buf->size - 1;
         break;

      case RGUI_ACTION_DOWN:
         if (rgui->selection_ptr + 1 < rgui->selection_buf->size)
            rgui->selection_ptr++;
         else
            rgui->selection_ptr = 0;
         break;

      case RGUI_ACTION_CANCEL:
         if (rgui->menu_stack->size > 1)
         {
            rgui_list_pop(rgui->menu_stack, &rgui->selection_ptr);
            rgui->need_refresh = true;
         }
         break;

      case RGUI_ACTION_LEFT:
      case RGUI_ACTION_RIGHT:
      case RGUI_ACTION_OK:
      case RGUI_ACTION_START:
         if ((type == RGUI_SETTINGS_OPEN_FILEBROWSER || type == RGUI_SETTINGS_OPEN_FILEBROWSER_DEFERRED_CORE)
               && action == RGUI_ACTION_OK)
         {
            rgui->defer_core = type == RGUI_SETTINGS_OPEN_FILEBROWSER_DEFERRED_CORE;
            rgui_list_push(rgui->menu_stack, rgui->base_path, RGUI_FILE_DIRECTORY, rgui->selection_ptr);
            rgui->selection_ptr = 0;
            rgui->need_refresh = true;
         }
         else if ((type == RGUI_SETTINGS_OPEN_HISTORY || menu_type_is(type) == RGUI_FILE_DIRECTORY) && action == RGUI_ACTION_OK)
         {
            rgui_list_push(rgui->menu_stack, "", type, rgui->selection_ptr);
            rgui->selection_ptr = 0;
            rgui->need_refresh = true;
         }
         else if ((menu_type_is(type) == RGUI_SETTINGS || type == RGUI_SETTINGS_CORE || type == RGUI_SETTINGS_CONFIG || type == RGUI_SETTINGS_DISK_APPEND) && action == RGUI_ACTION_OK)
         {
            rgui_list_push(rgui->menu_stack, label, type, rgui->selection_ptr);
            rgui->selection_ptr = 0;
            rgui->need_refresh = true;
         }
         else if (type == RGUI_SETTINGS_CUSTOM_VIEWPORT && action == RGUI_ACTION_OK)
         {
            rgui_list_push(rgui->menu_stack, "", type, rgui->selection_ptr);

            // Start with something sane.
            rarch_viewport_t *custom = &g_extern.console.screen.viewports.custom_vp;
            driver.video->viewport_info(driver.video_data, custom);
            aspectratio_lut[ASPECT_RATIO_CUSTOM].value =
               (float)custom->width / custom->height;

            g_settings.video.aspect_ratio_idx = ASPECT_RATIO_CUSTOM;
            if (driver.video_poke->set_aspect_ratio)
               driver.video_poke->set_aspect_ratio(driver.video_data,
                     g_settings.video.aspect_ratio_idx);
         }
         else
         {
            int ret = rgui_settings_toggle_setting(rgui, type, action, menu_type);
            if (ret)
               return ret;
         }
         break;

      case RGUI_ACTION_REFRESH:
         rgui->selection_ptr = 0;
         rgui->need_refresh = true;
         break;

      case RGUI_ACTION_MESSAGE:
         rgui->msg_force = true;
         break;

      default:
         break;
   }

   rgui_list_get_last(rgui->menu_stack, &dir, &menu_type);

   if (rgui->need_refresh && !(menu_type == RGUI_FILE_DIRECTORY ||
            menu_type_is(menu_type) == RGUI_SETTINGS_SHADER_OPTIONS||
            menu_type_is(menu_type) == RGUI_FILE_DIRECTORY ||
            menu_type == RGUI_SETTINGS_OVERLAY_PRESET ||
            menu_type == RGUI_SETTINGS_CORE ||
            menu_type == RGUI_SETTINGS_CONFIG ||
            menu_type == RGUI_SETTINGS_DISK_APPEND ||
            menu_type == RGUI_SETTINGS_OPEN_HISTORY))
   {
      rgui->need_refresh = false;
      if (
               menu_type == RGUI_SETTINGS_INPUT_OPTIONS
            || menu_type == RGUI_SETTINGS_PATH_OPTIONS
            || menu_type == RGUI_SETTINGS_OPTIONS
            || menu_type == RGUI_SETTINGS_DRIVERS
            || menu_type == RGUI_SETTINGS_CORE_OPTIONS
            || menu_type == RGUI_SETTINGS_AUDIO_OPTIONS
            || menu_type == RGUI_SETTINGS_DISK_OPTIONS
            || menu_type == RGUI_SETTINGS_VIDEO_OPTIONS 
            || menu_type == RGUI_SETTINGS_SHADER_OPTIONS
            )
         menu_set_settings_populate_entries(rgui, menu_type);
      else
         menu_set_settings_populate_entries(rgui, RGUI_SETTINGS);
   }

   render_text(rgui);

   // Have to defer it so we let settings refresh.
   if (rgui->push_start_screen)
   {
      rgui->push_start_screen = false;
      rgui_list_push(rgui->menu_stack, "", RGUI_START_SCREEN, 0);
   }

   return 0;
}

static inline void rgui_descend_alphabet(rgui_handle_t *rgui, size_t *ptr_out)
{
   if (!rgui->scroll_indices_size)
      return;
   size_t ptr = *ptr_out;
   if (ptr == 0)
      return;
   size_t i = rgui->scroll_indices_size - 1;
   while (i && rgui->scroll_indices[i - 1] >= ptr)
      i--;
   *ptr_out = rgui->scroll_indices[i - 1];
}

static inline void rgui_ascend_alphabet(rgui_handle_t *rgui, size_t *ptr_out)
{
   if (!rgui->scroll_indices_size)
      return;
   size_t ptr = *ptr_out;
   if (ptr == rgui->scroll_indices[rgui->scroll_indices_size - 1])
      return;
   size_t i = 0;
   while (i < rgui->scroll_indices_size - 1 && rgui->scroll_indices[i + 1] <= ptr)
      i++;
   *ptr_out = rgui->scroll_indices[i + 1];
}


static int rgui_iterate(void *data, unsigned action)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;

   const char *dir = 0;
   unsigned menu_type = 0;
   rgui_list_get_last(rgui->menu_stack, &dir, &menu_type);
   int ret = 0;

   if (driver.video_poke && driver.video_poke->set_texture_enable)
      driver.video_poke->set_texture_frame(driver.video_data, menu_framebuf,
            false, RGUI_WIDTH, RGUI_HEIGHT, 1.0f);

   if (menu_type == RGUI_START_SCREEN)
      return rgui_start_screen_iterate(rgui, action);
   else if (menu_type_is(menu_type) == RGUI_SETTINGS)
      return rgui_settings_iterate(rgui, action);
   else if (menu_type == RGUI_SETTINGS_CUSTOM_VIEWPORT || menu_type == RGUI_SETTINGS_CUSTOM_VIEWPORT_2)
      return rgui_viewport_iterate(rgui, action);
   else if (menu_type == RGUI_SETTINGS_CUSTOM_BIND)
      return rgui_custom_bind_iterate(rgui, action);

   if (rgui->need_refresh && action != RGUI_ACTION_MESSAGE)
      action = RGUI_ACTION_NOOP;

   unsigned scroll_speed = (max(rgui->scroll_accel, 2) - 2) / 4 + 1;
   unsigned fast_scroll_speed = 4 + 4 * scroll_speed;

   switch (action)
   {
      case RGUI_ACTION_UP:
         if (rgui->selection_ptr >= scroll_speed)
            rgui->selection_ptr -= scroll_speed;
         else
            rgui->selection_ptr = rgui->selection_buf->size - 1;
         break;

      case RGUI_ACTION_DOWN:
         if (rgui->selection_ptr + scroll_speed < rgui->selection_buf->size)
            rgui->selection_ptr += scroll_speed;
         else
            rgui->selection_ptr = 0;
         break;

      case RGUI_ACTION_LEFT:
         if (rgui->selection_ptr > fast_scroll_speed)
            rgui->selection_ptr -= fast_scroll_speed;
         else
            rgui->selection_ptr = 0;
         break;

      case RGUI_ACTION_RIGHT:
         if (rgui->selection_ptr + fast_scroll_speed < rgui->selection_buf->size)
            rgui->selection_ptr += fast_scroll_speed;
         else
            rgui->selection_ptr = rgui->selection_buf->size - 1;
         break;

      case RGUI_ACTION_SCROLL_UP:
         rgui_descend_alphabet(rgui, &rgui->selection_ptr);
         break;
      case RGUI_ACTION_SCROLL_DOWN:
         rgui_ascend_alphabet(rgui, &rgui->selection_ptr);
         break;
      
      case RGUI_ACTION_CANCEL:
         if (rgui->menu_stack->size > 1)
         {
            rgui->need_refresh = true;
            rgui_list_pop(rgui->menu_stack, &rgui->selection_ptr);
         }
         break;

      case RGUI_ACTION_OK:
      {
         if (rgui->selection_buf->size == 0)
            return 0;

         const char *path = 0;
         unsigned type = 0;
         rgui_list_get_at_offset(rgui->selection_buf, rgui->selection_ptr, &path, &type);

         if (
               menu_type_is(type) == RGUI_SETTINGS_SHADER_OPTIONS ||
               menu_type_is(type) == RGUI_FILE_DIRECTORY ||
               type == RGUI_SETTINGS_OVERLAY_PRESET ||
               type == RGUI_SETTINGS_CORE ||
               type == RGUI_SETTINGS_CONFIG ||
               type == RGUI_SETTINGS_DISK_APPEND ||
               type == RGUI_FILE_DIRECTORY)
         {
            char cat_path[PATH_MAX];
            fill_pathname_join(cat_path, dir, path, sizeof(cat_path));

            rgui_list_push(rgui->menu_stack, cat_path, type, rgui->selection_ptr);
            rgui->selection_ptr = 0;
            rgui->need_refresh = true;
         }
         else
         {
#ifdef HAVE_SHADER_MANAGER
            if (menu_type_is(menu_type) == RGUI_SETTINGS_SHADER_OPTIONS)
            {
               if (menu_type == RGUI_SETTINGS_SHADER_PRESET)
               {
                  char shader_path[PATH_MAX];
                  fill_pathname_join(shader_path, dir, path, sizeof(shader_path));
                  shader_manager_set_preset(&rgui->shader, gfx_shader_parse_type(shader_path, RARCH_SHADER_NONE),
                        shader_path);
               }
               else
               {
                  unsigned pass = (menu_type - RGUI_SETTINGS_SHADER_0) / 3;
                  fill_pathname_join(rgui->shader.pass[pass].source.cg,
                        dir, path, sizeof(rgui->shader.pass[pass].source.cg));
               }

               // Pop stack until we hit shader manager again.
               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS_SHADER_OPTIONS);
            }
            else
#endif
            if (menu_type == RGUI_SETTINGS_DEFERRED_CORE)
            {
               // FIXME: Add for consoles.
               strlcpy(g_settings.libretro, path, sizeof(g_settings.libretro));
               strlcpy(g_extern.fullpath, rgui->deferred_path, sizeof(g_extern.fullpath));
#ifdef HAVE_DYNAMIC
               libretro_free_system_info(&rgui->info);
               libretro_get_system_info(g_settings.libretro, &rgui->info,
                     &rgui->load_no_rom);

               g_extern.lifecycle_mode_state |= (1ULL << MODE_LOAD_GAME);
#else
               rarch_environment_cb(RETRO_ENVIRONMENT_SET_LIBRETRO_PATH, (void*)g_settings.libretro);
               rarch_environment_cb(RETRO_ENVIRONMENT_EXEC, (void*)g_extern.fullpath);
#endif
               rgui->msg_force = true;
               ret = -1;
               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS);
            }
            else if (menu_type == RGUI_SETTINGS_CORE)
            {
#if defined(HAVE_DYNAMIC)
               fill_pathname_join(g_settings.libretro, dir, path, sizeof(g_settings.libretro));
               libretro_free_system_info(&rgui->info);
               libretro_get_system_info(g_settings.libretro, &rgui->info,
                     &rgui->load_no_rom);

               // No ROM needed for this core, load game immediately.
               if (rgui->load_no_rom)
               {
                  g_extern.lifecycle_mode_state |= (1ULL << MODE_LOAD_GAME);
                  *g_extern.fullpath = '\0';
                  rgui->msg_force = true;
                  ret = -1;
               }

               // Core selection on non-console just updates directory listing.
               // Will take affect on new ROM load.
#elif defined(GEKKO) && defined(HW_RVL)
               rarch_environment_cb(RETRO_ENVIRONMENT_SET_LIBRETRO_PATH, (void*)path);

               fill_pathname_join(g_extern.fullpath, default_paths.core_dir,
                     SALAMANDER_FILE, sizeof(g_extern.fullpath));
               g_extern.lifecycle_mode_state &= ~(1ULL << MODE_GAME);
               g_extern.lifecycle_mode_state |= (1ULL << MODE_EXITSPAWN);
               ret = -1;
#endif

               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS);
            }
            else if (menu_type == RGUI_SETTINGS_CONFIG)
            {
               char config[PATH_MAX];
               fill_pathname_join(config, dir, path, sizeof(config));
               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS);
               rgui->msg_force = true;
               if (menu_replace_config(config))
               {
                  rgui->selection_ptr = 0; // Menu can shrink.
                  ret = -1;
               }
            }
#ifdef HAVE_OVERLAY
            else if (menu_type == RGUI_SETTINGS_OVERLAY_PRESET)
            {
               fill_pathname_join(g_settings.input.overlay, dir, path, sizeof(g_settings.input.overlay));

               if (driver.overlay)
                  input_overlay_free(driver.overlay);
               driver.overlay = input_overlay_new(g_settings.input.overlay);
               if (!driver.overlay)
                  RARCH_ERR("Failed to load overlay.\n");

               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS_INPUT_OPTIONS);
            }
#endif
            else if (menu_type == RGUI_SETTINGS_DISK_APPEND)
            {
               char image[PATH_MAX];
               fill_pathname_join(image, dir, path, sizeof(image));
               rarch_disk_control_append_image(image);

               g_extern.lifecycle_mode_state |= 1ULL << MODE_GAME;

               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS);
               ret = -1;
            }
            else if (menu_type == RGUI_SETTINGS_OPEN_HISTORY)
            {
               load_menu_game_history(rgui->selection_ptr);
               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS);
               ret = -1;
            }
            else if (menu_type == RGUI_BROWSER_DIR_PATH)
            {
               strlcpy(g_settings.rgui_browser_directory, dir, sizeof(g_settings.rgui_browser_directory));
               strlcpy(rgui->base_path, dir, sizeof(rgui->base_path));
               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
#ifdef HAVE_SCREENSHOTS
            else if (menu_type == RGUI_SCREENSHOT_DIR_PATH)
            {
               strlcpy(g_settings.screenshot_directory, dir, sizeof(g_settings.screenshot_directory));
               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
#endif
            else if (menu_type == RGUI_SAVEFILE_DIR_PATH)
            {
               strlcpy(g_extern.savefile_dir, dir, sizeof(g_extern.savefile_dir));
               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
#ifdef HAVE_OVERLAY
            else if (menu_type == RGUI_OVERLAY_DIR_PATH)
            {
               strlcpy(g_extern.overlay_dir, dir, sizeof(g_extern.overlay_dir));
               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
#endif
            else if (menu_type == RGUI_SAVESTATE_DIR_PATH)
            {
               strlcpy(g_extern.savestate_dir, dir, sizeof(g_extern.savestate_dir));
               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
#ifdef HAVE_DYNAMIC
            else if (menu_type == RGUI_LIBRETRO_DIR_PATH)
            {
               strlcpy(rgui->libretro_dir, dir, sizeof(rgui->libretro_dir));
               menu_init_core_info(rgui);
               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
            else if (menu_type == RGUI_CONFIG_DIR_PATH)
            {
               strlcpy(g_settings.rgui_config_directory, dir, sizeof(g_settings.rgui_config_directory));
               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
#endif
            else if (menu_type == RGUI_LIBRETRO_INFO_DIR_PATH)
            {
               strlcpy(g_settings.libretro_info_path, dir, sizeof(g_settings.libretro_info_path));
               menu_init_core_info(rgui);
               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
            else if (menu_type == RGUI_SHADER_DIR_PATH)
            {
               strlcpy(g_settings.video.shader_dir, dir, sizeof(g_settings.video.shader_dir));
               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
            else if (menu_type == RGUI_SYSTEM_DIR_PATH)
            {
               strlcpy(g_settings.system_directory, dir, sizeof(g_settings.system_directory));
               rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
            else
            {
               if (rgui->defer_core)
               {
                  fill_pathname_join(rgui->deferred_path, dir, path, sizeof(rgui->deferred_path));

                  const core_info_t *info = NULL;
                  size_t supported = 0;
                  if (rgui->core_info)
                     core_info_list_get_supported_cores(rgui->core_info, rgui->deferred_path, &info, &supported);

                  if (supported == 1) // Can make a decision right now.
                  {
                     strlcpy(g_extern.fullpath, rgui->deferred_path, sizeof(g_extern.fullpath));
                     strlcpy(g_settings.libretro, info->path, sizeof(g_settings.libretro));

#ifdef HAVE_DYNAMIC
                     libretro_free_system_info(&rgui->info);
                     libretro_get_system_info(g_settings.libretro, &rgui->info,
                           &rgui->load_no_rom);
#else
                     rarch_environment_cb(RETRO_ENVIRONMENT_SET_LIBRETRO_PATH, (void*)g_settings.libretro);
                     rarch_environment_cb(RETRO_ENVIRONMENT_EXEC, (void*)g_extern.fullpath);
#endif

                     g_extern.lifecycle_mode_state |= (1ULL << MODE_LOAD_GAME);
                     rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS);
                     rgui->msg_force = true;
                     ret = -1;
                  }
                  else // Present a selection.
                  {
                     rgui_list_push(rgui->menu_stack, rgui->libretro_dir, RGUI_SETTINGS_DEFERRED_CORE, rgui->selection_ptr);
                     rgui->selection_ptr = 0;
                     rgui->need_refresh = true;
                  }
               }
               else
               {
                  fill_pathname_join(g_extern.fullpath, dir, path, sizeof(g_extern.fullpath));
                  g_extern.lifecycle_mode_state |= (1ULL << MODE_LOAD_GAME);

                  rgui_flush_menu_stack_type(rgui, RGUI_SETTINGS);
                  rgui->msg_force = true;
                  ret = -1;
               }
            }
         }
         break;
      }

      case RGUI_ACTION_REFRESH:
         rgui->selection_ptr = 0;
         rgui->need_refresh = true;
         break;

      case RGUI_ACTION_MESSAGE:
         rgui->msg_force = true;
         break;

      default:
         break;
   }

   // refresh values in case the stack changed
   rgui_list_get_last(rgui->menu_stack, &dir, &menu_type);

   if (rgui->need_refresh && (menu_type == RGUI_FILE_DIRECTORY ||
            menu_type_is(menu_type) == RGUI_SETTINGS_SHADER_OPTIONS ||
            menu_type_is(menu_type) == RGUI_FILE_DIRECTORY || 
            menu_type == RGUI_SETTINGS_OVERLAY_PRESET ||
            menu_type == RGUI_SETTINGS_DEFERRED_CORE ||
            menu_type == RGUI_SETTINGS_CORE ||
            menu_type == RGUI_SETTINGS_CONFIG ||
            menu_type == RGUI_SETTINGS_OPEN_HISTORY ||
            menu_type == RGUI_SETTINGS_DISK_APPEND))
   {
      rgui->need_refresh = false;
      menu_parse_and_resolve(rgui, menu_type);
   }

   render_text(rgui);

   return ret;
}

int rgui_input_postprocess(void *data, uint64_t old_state)
{
   (void)data;

   int ret = 0;

   if ((rgui->trigger_state & (1ULL << RARCH_MENU_TOGGLE)) &&
         g_extern.main_is_init &&
         !g_extern.libretro_dummy)
   {
      g_extern.lifecycle_mode_state |= (1ULL << MODE_GAME);
      ret = -1;
   }

   if (ret < 0)
   {
      unsigned type = 0;
      rgui_list_get_last(rgui->menu_stack, NULL, &type);
      while (type != RGUI_SETTINGS)
      {
         rgui_list_pop(rgui->menu_stack, &rgui->selection_ptr);
         rgui_list_get_last(rgui->menu_stack, NULL, &type);
      }
   }

   return ret;
}

const menu_ctx_driver_t menu_ctx_rgui = {
   rgui_iterate,
   rgui_init,
   rgui_free,
   "rgui",
};
