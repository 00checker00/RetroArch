/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2011-2015 - Daniel De Matteis
 *  Copyright (C) 2014-2015 - Jean-André Santoni
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
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <compat/posix_string.h>
#include <file/file_path.h>

#include "../menu.h"
#include "../menu_driver.h"
#include "../menu_entry.h"
#include "../menu_display.h"
#include "../../runloop_data.h"

#include "../../gfx/video_thread_wrapper.h"
#include "../../gfx/font_driver.h"
#include "../../gfx/video_texture.h"
#include "../../retroarch_logger.h"

#include "shared.h"

typedef struct glui_handle
{
   unsigned line_height;
   unsigned margin;
   unsigned ticker_limit;
   char box_message[PATH_MAX_LENGTH];

   struct
   {
      struct
      {
         GLuint id;
         char path[PATH_MAX_LENGTH];
      } bg;
      GLuint white;
   } textures;

   gl_font_raster_block_t list_block;
} glui_handle_t;

static void glui_blit_line(float x, float y,
      const char *message, uint32_t color, enum text_alignment text_align)
{
   unsigned width, height;
   glui_handle_t *glui = NULL;
   struct font_params params = {0};
   menu_handle_t *menu = menu_driver_get_ptr();

   if (!menu)
      return;

   video_driver_get_size(&width, &height);

   glui = (glui_handle_t*)menu->userdata;

   params.x           = x / width;
   params.y           = 1.0f - (y + glui->line_height/2 + menu->font.size/3) 
                        / height;
   params.scale       = 1.0;
   params.color       = color;
   params.full_screen = true;
   params.text_align  = text_align;

   video_driver_set_osd_msg(message, &params, menu->font.buf);
}

static void glui_render_quad(gl_t *gl, int x, int y, int w, int h,
      float r, float g, float b, float a)
{
   unsigned width, height;
   struct gl_coords coords;
   menu_handle_t *menu = menu_driver_get_ptr();
   glui_handle_t *glui = (glui_handle_t*)menu->userdata;
   static const GLfloat vertex[] = {
      0, 0,
      1, 0,
      0, 1,
      1, 1,
   };

   static const GLfloat tex_coord[] = {
      0, 1,
      1, 1,
      0, 0,
      1, 0,
   };

   GLfloat color[] = {
      r, g, b, a,
      r, g, b, a,
      r, g, b, a,
      r, g, b, a,
   };

   video_driver_get_size(&width, &height);

   glViewport(x, height - y - h, w, h);

   coords.vertices      = 4;
   coords.vertex        = vertex;
   coords.tex_coord     = tex_coord;
   coords.lut_tex_coord = tex_coord;

   coords.color = color;

   menu_gl_draw_frame(gl->shader, &coords, &gl->mvp_no_rot, true, glui->textures.white);

   gl->coords.color = gl->white_color_ptr;
}

static void glui_draw_cursor(gl_t *gl, float x, float y)
{
   glui_render_quad(gl, x-5, y-5, 10, 10, 1, 1, 1, 1);
}

static void glui_draw_scrollbar(gl_t *gl)
{
   unsigned width, height;
   float content_height, total_height, scrollbar_height, y;
   int scrollbar_width = 4;
   glui_handle_t *glui = NULL;
   menu_handle_t *menu = menu_driver_get_ptr();

   if (!menu)
      return;

   video_driver_get_size(&width, &height);

   glui                 = (glui_handle_t*)menu->userdata;
   content_height       = menu_entries_get_end() * glui->line_height;
   total_height         = height - menu->header_height * 2;
   scrollbar_height     = total_height / (content_height / total_height);
   y                    = total_height * menu->scroll_y / content_height;

   if (content_height < total_height)
      return;

   glui_render_quad(gl,
         width - scrollbar_width,
         menu->header_height + y,
         scrollbar_width,
         scrollbar_height,
         1, 1, 1, 1);
}

static void glui_get_message(const char *message)
{
   glui_handle_t *glui = NULL;
   menu_handle_t *menu = menu_driver_get_ptr();

   if (!menu)
      return;

   if (!message || !*message)
      return;

   glui = (glui_handle_t*)menu->userdata;

   if (glui)
      strlcpy(glui->box_message, message, sizeof(glui->box_message));
}

static void glui_render_messagebox(const char *message)
{
   unsigned i;
   unsigned width, height;
   uint32_t normal_color;
   int x, y;
   struct string_list *list = NULL;
   menu_handle_t *menu      = menu_driver_get_ptr();
   settings_t *settings     = config_get_ptr();

   if (!menu || !menu->userdata)
      return;

   list = (struct string_list*)string_split(message, "\n");

   if (!list)
      return;

   if (list->elems == 0)
      goto end;

   video_driver_get_size(&width, &height);

   x = width  / 2;
   y = height / 2 - list->size * menu->font.size / 2;

   normal_color = FONT_COLOR_ARGB_TO_RGBA(settings->menu.entry_normal_color);

   for (i = 0; i < list->size; i++)
   {
      const char *msg = list->elems[i].data;
      if (msg)
         glui_blit_line(x, y + i * menu->font.size, msg, normal_color, TEXT_ALIGN_CENTER);
   }

end:
   string_list_free(list);
}

static void glui_render(void)
{
   int bottom;
   unsigned width, height;
   glui_handle_t *glui = NULL;
   menu_handle_t *menu  = menu_driver_get_ptr();
   settings_t *settings = config_get_ptr();

   if (!menu || !menu->userdata)
      return;

   video_driver_get_size(&width, &height);

   glui = (glui_handle_t*)menu->userdata;

   menu_animation_update(menu->animation, menu->dt / IDEAL_DT);

   menu->frame_buf.width  = width;
   menu->frame_buf.height = height;

   if (settings->menu.pointer.enable)
   {
      menu->pointer.ptr =
            (menu->pointer.y - glui->line_height + menu->scroll_y - 16)
            / glui->line_height;

      if (menu->pointer.dragging)
         menu->scroll_y -= menu->pointer.dy;
   }

   if (settings->menu.mouse.enable)
   {
      if (menu->mouse.scrolldown)
         menu->scroll_y += 10;

      if (menu->mouse.scrollup)
         menu->scroll_y -= 10;

      menu->mouse.ptr =
            (menu->mouse.y - glui->line_height + menu->scroll_y - 16)
            / glui->line_height;
   }

   if (menu->scroll_y < 0)
      menu->scroll_y = 0;

   bottom = menu_entries_get_end() * glui->line_height
         - height + menu->header_height * 2;
   if (menu->scroll_y > bottom)
      menu->scroll_y = bottom;

   if (menu_entries_get_end() * glui->line_height
      < height - menu->header_height*2)
      menu->scroll_y = 0;
}

static void glui_render_menu_list(glui_handle_t *glui,
      menu_handle_t *menu,
      uint32_t normal_color,
      uint32_t hover_color)
{
   unsigned width, height;
   size_t i = 0;
   uint64_t frame_count = video_driver_get_frame_count();
   size_t          end  = menu_entries_get_end();

   if (!menu_display_update_pending())
      return;

   video_driver_get_size(&width, &height);

   glui->list_block.carr.coords.vertices = 0;

   for (i = 0; i < end; i++)
   {
      char entry_path[PATH_MAX_LENGTH], entry_value[PATH_MAX_LENGTH];
      char message[PATH_MAX_LENGTH],
           entry_title_buf[PATH_MAX_LENGTH], type_str_buf[PATH_MAX_LENGTH];
      bool entry_selected;
      unsigned y = menu->header_height - menu->scroll_y + (glui->line_height * i);
      int max_visible_entries = ((y + glui->line_height));

      if (y > height || (max_visible_entries < 0))
         continue;

      entry_selected = menu_entry_is_currently_selected(i);
      menu_entry_get_value(i, entry_value, sizeof(entry_value));
      menu_entry_get_path(i, entry_path, sizeof(entry_path));

      menu_animation_ticker_line(entry_title_buf, glui->ticker_limit,
            frame_count / 100, entry_path, entry_selected);
      menu_animation_ticker_line(type_str_buf, glui->ticker_limit,
            frame_count / 100, entry_value, entry_selected);

      strlcpy(message, entry_title_buf, sizeof(message));

      glui_blit_line(glui->margin, y, message,
            entry_selected ? hover_color : normal_color, TEXT_ALIGN_LEFT);

      glui_blit_line(width - glui->margin, y, type_str_buf,
            entry_selected ? hover_color : normal_color, TEXT_ALIGN_RIGHT);
   }
}

static void glui_frame(void)
{
   unsigned width, height;
   char title[PATH_MAX_LENGTH],     title_buf[PATH_MAX_LENGTH], 
        title_msg[PATH_MAX_LENGTH], timedate[PATH_MAX_LENGTH];
   gl_t *gl                     = NULL;
   glui_handle_t *glui          = NULL;
   const struct font_renderer *font_driver = NULL;
   driver_t *driver             = driver_get_ptr();
   menu_handle_t *menu          = menu_driver_get_ptr();
   settings_t *settings         = config_get_ptr();
   const uint32_t normal_color  = FONT_COLOR_ARGB_TO_RGBA(
         settings->menu.entry_normal_color);
   const uint32_t hover_color   = FONT_COLOR_ARGB_TO_RGBA(
         settings->menu.entry_hover_color);
   const uint32_t title_color   = FONT_COLOR_ARGB_TO_RGBA(
         settings->menu.title_color);
   uint64_t frame_count         = video_driver_get_frame_count();

   if (!menu || !menu->userdata)
      return;

   gl = (gl_t*)video_driver_get_ptr(NULL);

   if (!gl)
      return;

   glui = (glui_handle_t*)menu->userdata;

   if (menu_needs_refresh()
         && menu_driver_alive() 
         && !menu->msg_force
         && !glui->box_message[0])
      return;

   video_driver_get_size(&width, &height);

   menu_display_set_viewport();

   gl_menu_frame_background(menu, settings, gl, glui->textures.bg.id, 0.75f, 0.75f, false);

   menu_entries_get_title(title, sizeof(title));

   font_driver = driver->font_osd_driver;

   menu_display_font_bind_block(menu, font_driver, &glui->list_block);


   glui_render_menu_list(glui, menu, normal_color, hover_color);

   menu_display_font_flush_block(menu, font_driver);

   glui_render_quad(gl, 0,
         menu->header_height - menu->scroll_y + glui->line_height *
         menu->navigation.selection_ptr,
         width, glui->line_height, 1, 1, 1, 0.1);

   menu->animation_is_active = true;
   menu->label.is_updated    = false;

   glui_render_quad(gl, 0, 0, width,
         menu->header_height, 0.2, 0.2, 0.2, 1);

   menu_animation_ticker_line(title_buf, glui->ticker_limit,
         frame_count / 100, title, true);
   glui_blit_line(width / 2, 0, title_buf,
         title_color, TEXT_ALIGN_CENTER);

   if (menu_entries_show_back())
      glui_blit_line(glui->margin, 0, "BACK",
            title_color, TEXT_ALIGN_LEFT);

   glui_render_quad(gl, 0,
         height - menu->header_height,
         width, menu->header_height,
         0.2, 0.2, 0.2, 1);

   glui_draw_scrollbar(gl);

   if (settings->menu.core_enable)
   {
      menu_entries_get_core_title(title_msg, sizeof(title_msg));

      glui_blit_line(glui->margin,
            height - glui->line_height, title_msg,
            title_color, TEXT_ALIGN_LEFT);
   }

   if (settings->menu.timedate_enable)
   {
      disp_timedate_set_label(timedate, sizeof(timedate), 0);
      glui_blit_line(width - glui->margin,
            height - glui->line_height, timedate, hover_color,
            TEXT_ALIGN_RIGHT);
   }

   if (menu->keyboard.display)
   {
      char msg[PATH_MAX_LENGTH];
      const char *str = *menu->keyboard.buffer;
      if (!str)
         str = "";
      glui_render_quad(gl, 0, 0, width, height, 0, 0, 0, 0.75);
      snprintf(msg, sizeof(msg), "%s\n%s", menu->keyboard.label, str);
      glui_render_messagebox(msg);
   }

   if (glui->box_message[0] != '\0')
   {
      glui_render_quad(gl, 0, 0, width, height, 0, 0, 0, 0.75);
      glui_render_messagebox(glui->box_message);
      glui->box_message[0] = '\0';
   }

   if (settings->menu.mouse.enable)
      glui_draw_cursor(gl, menu->mouse.x, menu->mouse.y);

   gl->shader->use(gl, GL_SHADER_STOCK_BLEND);

   menu_display_unset_viewport();
}

static void glui_allocate_white_texture(glui_handle_t *glui)
{
   static const uint8_t white_data[] = { 0xff, 0xff, 0xff, 0xff };
   struct texture_image ti;

   ti.width  = 1;
   ti.height = 1;
   ti.pixels = (uint32_t*)&white_data;

   glui->textures.white   = video_texture_load(&ti,
         TEXTURE_BACKEND_OPENGL, TEXTURE_FILTER_NEAREST);
}

static void *glui_init(void)
{
   float dpi;
   glui_handle_t *glui                     = NULL;
   const video_driver_t *video_driver      = NULL;
   menu_handle_t                     *menu = NULL;
   settings_t *settings                    = config_get_ptr();
   gl_t *gl                                = (gl_t*)
      video_driver_get_ptr(&video_driver);

   if (video_driver != &video_gl || !gl)
   {
      RARCH_ERR("Cannot initialize GLUI menu driver: gl video driver is not active.\n");
      return NULL;
   }

   menu                 = (menu_handle_t*)calloc(1, sizeof(*menu));

   if (!menu)
      goto error;

   menu->userdata       = (glui_handle_t*)calloc(1, sizeof(glui_handle_t));

   if (!menu->userdata)
      goto error;

   glui                 = (glui_handle_t*)menu->userdata;
   dpi                  = menu_display_get_dpi();

   glui->line_height    = dpi / 3;
   glui->margin         = dpi / 6;
   glui->ticker_limit   = dpi / 3;
   menu->header_height  = dpi / 3;
   menu->font.size      = dpi / 8;
   glui->textures.bg.id = 0;

   glui_allocate_white_texture(glui);

   if (settings->menu.wallpaper[0] != '\0')
      rarch_main_data_msg_queue_push(DATA_TYPE_IMAGE,
            settings->menu.wallpaper, "cb_menu_wallpaper", 0, 1, true);

   return menu;
error:
   if (menu)
      free(menu);
   return NULL;
}

static void glui_free(void *data)
{
   gl_t *gl                                = NULL;
   const struct font_renderer *font_driver = NULL;
   menu_handle_t *menu                     = (menu_handle_t*)data;
   driver_t      *driver                   = driver_get_ptr();
   glui_handle_t *glui                     = (glui_handle_t*)menu->userdata;

   if (!glui || !menu)
      return;

   gl_coord_array_free(&glui->list_block.carr);

   gl = (gl_t*)video_driver_get_ptr(NULL);

   font_driver = gl ? (const struct font_renderer*)driver->font_osd_driver : NULL;

   if (font_driver && font_driver->bind_block)
      font_driver->bind_block(driver->font_osd_data, NULL);

   if (menu->userdata)
      free(menu->userdata);
}

static void glui_context_bg_destroy(glui_handle_t *glui)
{
   if (glui)
   {
      if (glui->textures.bg.id)
         glDeleteTextures(1, &glui->textures.bg.id);
      if (glui->textures.white)
         glDeleteTextures(1, &glui->textures.white);

      glui->textures.bg.id = 0;
      glui->textures.white = 0;
   }
}

static void glui_context_destroy(void)
{
   gl_t          *gl     = (gl_t*)video_driver_get_ptr(NULL);
   glui_handle_t *glui   = NULL;
   menu_handle_t *menu   = menu_driver_get_ptr();
   driver_t      *driver = driver_get_ptr();
    
   if (!menu || !menu->userdata || !gl || !driver)
      return;

   glui = (glui_handle_t*)menu->userdata;

   menu_display_free_main_font(menu);

   glui_context_bg_destroy(glui);
}

static bool glui_load_image(void *data, menu_image_type_t type)
{
   glui_handle_t *glui = NULL;
   menu_handle_t *menu = menu_driver_get_ptr();

   if (!menu || !menu->userdata)
      return false;
   
   glui = (glui_handle_t*)menu->userdata;

   switch (type)
   {
      case MENU_IMAGE_NONE:
         break;
      case MENU_IMAGE_WALLPAPER:
         glui_context_bg_destroy(glui);

         glui->textures.bg.id   = video_texture_load(data,
               TEXTURE_BACKEND_OPENGL, TEXTURE_FILTER_MIPMAP_LINEAR);
         glui_allocate_white_texture(glui);
         break;
      case MENU_IMAGE_BOXART:
         break;
   }

   return true;
}

static float glui_get_scroll(void)
{
   int half;
   unsigned width, height;
   glui_handle_t *glui = NULL;
   menu_handle_t *menu = menu_driver_get_ptr();

   if (!menu || !menu->userdata)
      return 0;

   video_driver_get_size(&width, &height);

   glui = (glui_handle_t*)menu->userdata;
   half = (height / glui->line_height) / 2;

   if (menu->navigation.selection_ptr < half)
      return 0;
   return ((menu->navigation.selection_ptr + 2 - half) * glui->line_height);
}

static void glui_navigation_set(bool scroll)
{
   menu_handle_t *menu = menu_driver_get_ptr();

   if (!menu || !scroll)
      return;

   menu_animation_push(menu->animation, 10, glui_get_scroll(),
         &menu->scroll_y, EASING_IN_OUT_QUAD, NULL);
}

static void glui_navigation_clear(bool pending_push)
{
   menu_handle_t *menu = menu_driver_get_ptr();
   if (!menu)
      return;

   menu->begin         = 0;
   menu->scroll_y      = 0;
}

static void glui_navigation_set_last(void)
{
   glui_navigation_set(true);
}

static void glui_navigation_descend_alphabet(size_t *unused)
{
   glui_navigation_set(true);
}

static void glui_navigation_ascend_alphabet(size_t *unused)
{
   glui_navigation_set(true);
}

static void glui_populate_entries(const char *path,
      const char *label, unsigned i)
{
   menu_handle_t *menu = menu_driver_get_ptr();
   if (!menu)
      return;

   menu->scroll_y      = glui_get_scroll();
}

static void glui_context_reset(void)
{
   glui_handle_t *glui   = NULL;
   menu_handle_t *menu   = menu_driver_get_ptr();
   settings_t *settings  = config_get_ptr();
   const char *font_path = NULL;

   if (!menu || !menu->userdata || !settings)
      return;

   glui      = (glui_handle_t*)menu->userdata;
   font_path = settings->video.font_enable ? settings->video.font_path : NULL;

   if (!menu_display_init_main_font(menu, font_path, menu->font.size))
      RARCH_WARN("Failed to load font.");

   glui_context_bg_destroy(glui);
   glui_allocate_white_texture(glui);

   rarch_main_data_msg_queue_push(DATA_TYPE_IMAGE,
         settings->menu.wallpaper, "cb_menu_wallpaper", 0, 1, true);
}

menu_ctx_driver_t menu_ctx_glui = {
   NULL,
   glui_get_message,
   glui_render,
   glui_frame,
   glui_init,
   glui_free,
   glui_context_reset,
   glui_context_destroy,
   glui_populate_entries,
   NULL,
   glui_navigation_clear,
   NULL,
   NULL,
   glui_navigation_set,
   glui_navigation_set_last,
   glui_navigation_descend_alphabet,
   glui_navigation_ascend_alphabet,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   glui_load_image,
   "glui",
   NULL,
};
