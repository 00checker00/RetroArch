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

#include <pspkernel.h>
#include <pspdebug.h>

#include <stdint.h>
#include "../boolean.h"
#include <stddef.h>
#include <string.h>

#undef main
#include "../psp/sdk_defines.h"

PSP_MODULE_INFO("RetroArch PSP", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
PSP_HEAP_SIZE_MAX();

int rarch_main(int argc, char *argv[]);

static int exit_callback(int arg1, int arg2, void *common)
{
   g_extern.verbose = false;

#ifdef HAVE_FILE_LOGGER
   if (g_extern.log_file)
      fclose(g_extern.log_file);
   g_extern.log_file = NULL;
#endif

   sceKernelExitGame();
   return 0;
}

static void get_environment_settings(int argc, char *argv[])
{
   fill_pathname_basedir(default_paths.port_dir, argv[0], sizeof(default_paths.port_dir));
   RARCH_LOG("port dir: [%s]\n", default_paths.port_dir);

   snprintf(default_paths.core_dir, sizeof(default_paths.core_dir), "%s/cores", default_paths.port_dir);
   snprintf(default_paths.executable_extension, sizeof(default_paths.executable_extension), ".SELF");
   snprintf(default_paths.savestate_dir, sizeof(default_paths.savestate_dir), "%s/savestates", default_paths.core_dir);
   snprintf(default_paths.filesystem_root_dir, sizeof(default_paths.filesystem_root_dir), "/");
   snprintf(default_paths.filebrowser_startup_dir, sizeof(default_paths.filebrowser_startup_dir), default_paths.filesystem_root_dir);
   snprintf(default_paths.sram_dir, sizeof(default_paths.sram_dir), "%s/sram", default_paths.core_dir);

   snprintf(default_paths.system_dir, sizeof(default_paths.system_dir), "%s/system", default_paths.core_dir);

   /* now we fill in all the variables */
   snprintf(default_paths.border_file, sizeof(default_paths.border_file), "%s/borders/Centered-1080p/mega-man-2.png", default_paths.core_dir);
   snprintf(default_paths.menu_border_file, sizeof(default_paths.menu_border_file), "%s/borders/Menu/main-menu.png", default_paths.core_dir);
   snprintf(default_paths.cgp_dir, sizeof(default_paths.cgp_dir), "%s/presets", default_paths.core_dir);
   snprintf(default_paths.input_presets_dir, sizeof(default_paths.input_presets_dir), "%s/input", default_paths.cgp_dir);
   snprintf(default_paths.border_dir, sizeof(default_paths.border_dir), "%s/borders", default_paths.core_dir);
   snprintf(g_extern.config_path, sizeof(g_extern.config_path), "%s/retroarch.cfg", default_paths.port_dir);
   snprintf(default_paths.salamander_file, sizeof(default_paths.salamander_file), "EBOOT.BIN");
}

int callback_thread(SceSize args, void *argp)
{
   int cbid = sceKernelCreateCallback("Exit callback", exit_callback, NULL);
   sceKernelRegisterExitCallback(cbid);
   sceKernelSleepThreadCB();

   return 0;
}

static int setup_callback(void)
{
   int thread_id = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);

   if (thread_id >= 0)
      sceKernelStartThread(thread_id, 0, 0);

   return thread_id;
}

void menu_init (void)
{
   g_extern.console.rmenu.mode = MODE_MENU;
}

bool rmenu_iterate(void)
{
   char game_rom[256];
   snprintf(game_rom, sizeof(game_rom), "%s%s", default_paths.port_dir, "dkc.sfc");
   RARCH_LOG("game ROM: %s\n", game_rom);
   rarch_console_load_game_wrap(game_rom, 0, 0);
   g_extern.console.rmenu.mode = MODE_EMULATION;

   return false;
}

void menu_free (void)
{
}

int main(int argc, char *argv[])
{
   //initialize debug screen
   pspDebugScreenInit();
   pspDebugScreenClear();

   setup_callback();

   rarch_main_clear_state();

   g_extern.verbose = true;

#ifdef HAVE_FILE_LOGGER
   g_extern.log_file = fopen("ms0:/retroarch-log.txt", "w");
#endif

   get_environment_settings(argc, argv);

   config_set_defaults();

   init_drivers_pre();
   driver.input->init();

   char tmp_path[PATH_MAX];
   snprintf(tmp_path, sizeof(tmp_path), "%s/", default_paths.core_dir);
   const char *path_prefix = tmp_path; 
   const char *extension = default_paths.executable_extension;

   char core_exe_path[1024];
   snprintf(core_exe_path, sizeof(core_exe_path), "%sCORE%s", path_prefix, extension);

   rarch_settings_set_default();
   rarch_input_set_controls_default(input);
   rarch_config_load();

#ifdef HAVE_LIBRETRO_MANAGEMENT
   if (path_file_exists(core_exe_path))
   {
      if (rarch_libretro_core_install(core_exe_path, path_prefix, path_prefix, 
               g_extern.config_path, extension))
      {
         RARCH_LOG("New default libretro core saved to config file: %s.\n", g_settings.libretro);
         config_save_file(g_extern.config_path);
      }
   }
#endif

   init_libretro_sym();

   driver.input->post_init();

   driver.video->start();
   
   menu_init();

begin_loop:
   if (g_extern.console.rmenu.mode == MODE_EMULATION)
   {
      driver.input->poll(NULL);
      driver.video->set_aspect_ratio(driver.video_data, g_settings.video.aspect_ratio_idx);

      while(rarch_main_iterate());
   }
   else if (g_extern.console.rmenu.mode == MODE_INIT)
   {
      if(g_extern.main_is_init)
         rarch_main_deinit();

      struct rarch_main_wrap args = {0};

      args.verbose = g_extern.verbose;
      args.config_path = g_extern.config_path;
      args.sram_path = g_extern.console.main_wrap.state.default_sram_dir.enable ? g_extern.console.main_wrap.paths.default_sram_dir : NULL,
         args.state_path = g_extern.console.main_wrap.state.default_savestate_dir.enable ? g_extern.console.main_wrap.paths.default_savestate_dir : NULL,
         args.rom_path = g_extern.file_state.rom_path;
      args.libretro_path = g_settings.libretro;

      int init_ret = rarch_main_init_wrap(&args);

      if (init_ret == 0)
         RARCH_LOG("rarch_main_init succeeded.\n");
      else
         RARCH_ERR("rarch_main_init failed.\n");
   }
   else if(g_extern.console.rmenu.mode == MODE_MENU)
      while(rmenu_iterate());
   else
      goto begin_shutdown;

   goto begin_loop;

begin_shutdown:
   config_save_file(g_extern.config_path);

   if(g_extern.main_is_init)
      rarch_main_deinit();

   driver.input->free(NULL);
   driver.video->stop();
   menu_free();

   g_extern.verbose = false;

#ifdef HAVE_FILE_LOGGER
   if (g_extern.log_file)
      fclose(g_extern.log_file);
   g_extern.log_file = NULL;
#endif

   sceKernelExitGame();
   return 1;
}
