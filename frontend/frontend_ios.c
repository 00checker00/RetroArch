/* RetroArch - A frontend for libretro.
 * Copyright (C) 2010-2013 - Hans-Kristian Arntzen
 * Copyright (C) 2011-2013 - Daniel De Matteis
 *
 * RetroArch is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Found-
 * ation, either version 3 of the License, or (at your option) any later version.
 *
 * RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with RetroArch.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <dispatch/dispatch.h>
#include <pthread.h>

#include "../general.h"
#include "../conf/config_file.h"
#include "../file.h"

#include "../ios/RetroArch/rarch_wrapper.h"

#ifdef HAVE_RGUI
#include "../frontend/menu/rgui.h"
#endif

static pthread_mutex_t ios_event_queue_lock = PTHREAD_MUTEX_INITIALIZER;

static struct
{
   void (*function)(void*);
   void* userdata;
} ios_event_queue[16];
static uint32_t ios_event_queue_size;

void ios_frontend_post_event(void (*fn)(void*), void* userdata)
{
   pthread_mutex_lock(&ios_event_queue_lock);

   if (ios_event_queue_size < 16)
   {
      ios_event_queue[ios_event_queue_size].function = fn;
      ios_event_queue[ios_event_queue_size].userdata = userdata;
      ios_event_queue_size ++;
   }

   pthread_mutex_unlock(&ios_event_queue_lock);
}

static void process_events()
{
   pthread_mutex_lock(&ios_event_queue_lock);

   for (int i = 0; i < ios_event_queue_size; i ++)
      ios_event_queue[i].function(ios_event_queue[i].userdata);

   ios_event_queue_size = 0;

   pthread_mutex_unlock(&ios_event_queue_lock);
}


static void ios_free_main_wrap(struct rarch_main_wrap* wrap)
{
   if (wrap)
   {
      free((char*)wrap->libretro_path);
      free((char*)wrap->rom_path);
      free((char*)wrap->sram_path);
      free((char*)wrap->state_path);
      free((char*)wrap->config_path);
   }

   free(wrap);
}

void* rarch_main_ios(void* args)
{
   struct rarch_main_wrap* argdata = (struct rarch_main_wrap*)args;
   int init_ret = rarch_main_init_wrap(argdata);
   ios_free_main_wrap(argdata);

   if (init_ret)
   {
      rarch_main_clear_state();
      dispatch_async_f(dispatch_get_main_queue(), (void*)1, ios_rarch_exited);
      return 0;
   }

#ifdef HAVE_RGUI
   char* system_directory = ios_get_rarch_system_directory();

   menu_init();
   g_extern.lifecycle_mode_state |= 1ULL << MODE_GAME;

   for (;;)
   {
      if (g_extern.system.shutdown)
         break;
      else if (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME))
      {
         if (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW))
         {
            char tmp[PATH_MAX];
            char str[PATH_MAX];

            fill_pathname_base(tmp, g_extern.fullpath, sizeof(tmp));
            snprintf(str, sizeof(str), "INFO - Loading %s...", tmp);
            msg_queue_push(g_extern.msg_queue, str, 1, 1);
         }

#if defined(HAVE_RGUI) || defined(HAVE_RMENU) || defined(HAVE_RMENU_XUI)
         if (rgui->history)
         {
            rom_history_push(rgui->history,
                  g_extern.fullpath,
                  g_settings.libretro,
                  rgui->info.library_name);
         }

         // draw frame for loading message
         if (driver.video_poke && driver.video_poke->set_texture_enable)
            driver.video_poke->set_texture_enable(driver.video_data, rgui->frame_buf_show, MENU_TEXTURE_FULLSCREEN);

         rarch_render_cached_frame();

         if (driver.video_poke && driver.video_poke->set_texture_enable)
            driver.video_poke->set_texture_enable(driver.video_data, false,
                  MENU_TEXTURE_FULLSCREEN);
#endif

         g_extern.lifecycle_mode_state &= ~(1ULL << MODE_LOAD_GAME);
         g_extern.lifecycle_mode_state |= (1ULL << MODE_INIT);
      }
      else if (g_extern.lifecycle_mode_state & (1ULL << MODE_GAME))
      {
         while ((g_extern.is_paused && !g_extern.is_oneshot) ? rarch_main_idle_iterate() : rarch_main_iterate())
            process_events();

         g_extern.lifecycle_mode_state &= ~(1ULL << MODE_GAME);
      }
      else if (g_extern.lifecycle_mode_state & (1ULL << MODE_INIT))
      {
         if (g_extern.main_is_init)
            rarch_main_deinit();

         struct rarch_main_wrap args = {0};

         args.verbose       = g_extern.verbose;
         args.config_path   = *g_extern.config_path ? g_extern.config_path : NULL;
         args.sram_path     = system_directory;
         args.state_path    = system_directory;
         args.rom_path      = g_extern.fullpath;
         args.libretro_path = g_settings.libretro;

         int init_ret = rarch_main_init_wrap(&args);
         if (init_ret == 0)
         {
            RARCH_LOG("rarch_main_init() succeeded.\n");
            g_extern.lifecycle_mode_state |= (1ULL << MODE_GAME);
         }
         else
         {
            RARCH_ERR("rarch_main_init() failed.\n");
            g_extern.lifecycle_mode_state |= (1ULL << MODE_MENU);
            msg_queue_push(g_extern.msg_queue, "ERROR - An error occurred during ROM loading.", 1, 180);
         }

         g_extern.lifecycle_mode_state &= ~(1ULL << MODE_INIT);
      }
      else if (g_extern.lifecycle_mode_state & (1ULL << MODE_MENU))
      {
         g_extern.lifecycle_mode_state |= 1ULL << MODE_MENU_PREINIT;
         while (!g_extern.system.shutdown && menu_iterate())
            process_events();
         g_extern.lifecycle_mode_state &= ~(1ULL << MODE_MENU);
      }
      else
         break;
   }

   g_extern.system.shutdown = false;

   menu_free();
   if (g_extern.main_is_init)
      rarch_main_deinit();

   free(system_directory);
#else
   while ((g_extern.is_paused && !g_extern.is_oneshot) ? rarch_main_idle_iterate() : rarch_main_iterate());
   rarch_main_deinit();
#endif
   
   rarch_deinit_msg_queue();

#ifdef PERF_TEST
   rarch_perf_log();
#endif

   rarch_main_clear_state();

   dispatch_async_f(dispatch_get_main_queue(), 0, ios_rarch_exited);
   return 0;
}
