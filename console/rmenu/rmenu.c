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

#if defined(__CELLOS_LV2__)
#include <sdk_version.h>

#if(CELL_SDK_VERSION > 0x340000)
#include <sysutil/sysutil_bgmplayback.h>
#endif

#endif

#include "../../console/fileio/file_browser.h"

#include "../../console/rarch_console.h"
#include "../../console/rarch_console_rom_ext.h"
#include "../../console/rarch_console_input.h"
#include "../../console/rarch_console_config.h"
#include "../../console/rarch_console_settings.h"

#include "../../gfx/image.h"

#ifdef HAVE_RSOUND
#include "../../console/rarch_console_rsound.h"
#endif

#include "../../console/rarch_console_video.h"

#ifdef HAVE_ZLIB
#include "../../console/rarch_console_rzlib.h"
#endif

#include "../../gfx/gfx_context.h"

#include "../../file.h"
#include "../../general.h"

#include "rmenu.h"

rmenu_state_t rmenu_state;

static bool set_libretro_core_as_launch;

filebrowser_t browser;
filebrowser_t tmpBrowser;
unsigned set_shader = 0;
unsigned currently_selected_controller_menu = 0;
char m_title[256];

static const struct retro_keybind _rmenu_nav_binds[] = {
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_UP) | (1ULL << RARCH_ANALOG_LEFT_Y_DPAD_UP), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_DOWN) | (1ULL << RARCH_ANALOG_LEFT_Y_DPAD_DOWN), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_LEFT) | (1ULL << RARCH_ANALOG_LEFT_X_DPAD_LEFT), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_RIGHT) | (1ULL << RARCH_ANALOG_LEFT_X_DPAD_RIGHT), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RARCH_ANALOG_LEFT_Y_DPAD_UP), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RARCH_ANALOG_LEFT_Y_DPAD_DOWN), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RARCH_ANALOG_LEFT_X_DPAD_LEFT), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RARCH_ANALOG_LEFT_X_DPAD_RIGHT), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RARCH_ANALOG_RIGHT_Y_DPAD_UP), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RARCH_ANALOG_RIGHT_Y_DPAD_DOWN), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RARCH_ANALOG_RIGHT_X_DPAD_LEFT), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RARCH_ANALOG_RIGHT_X_DPAD_RIGHT), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_B), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_A), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_X), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_Y), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_START), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_SELECT), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_L), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_R), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_L2), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_R2), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_L3), 0 },
   { 0, 0, (enum retro_key)0, (1ULL << RETRO_DEVICE_ID_JOYPAD_R3), 0 },
};

static const struct retro_keybind *rmenu_nav_binds[] = {
   _rmenu_nav_binds
};

enum
{
   RMENU_DEVICE_NAV_UP = 0,
   RMENU_DEVICE_NAV_DOWN,
   RMENU_DEVICE_NAV_LEFT,
   RMENU_DEVICE_NAV_RIGHT,
   RMENU_DEVICE_NAV_UP_ANALOG_L,
   RMENU_DEVICE_NAV_DOWN_ANALOG_L,
   RMENU_DEVICE_NAV_LEFT_ANALOG_L,
   RMENU_DEVICE_NAV_RIGHT_ANALOG_L,
   RMENU_DEVICE_NAV_UP_ANALOG_R,
   RMENU_DEVICE_NAV_DOWN_ANALOG_R,
   RMENU_DEVICE_NAV_LEFT_ANALOG_R,
   RMENU_DEVICE_NAV_RIGHT_ANALOG_R,
   RMENU_DEVICE_NAV_B,
   RMENU_DEVICE_NAV_A,
   RMENU_DEVICE_NAV_X,
   RMENU_DEVICE_NAV_Y,
   RMENU_DEVICE_NAV_START,
   RMENU_DEVICE_NAV_SELECT,
   RMENU_DEVICE_NAV_L1,
   RMENU_DEVICE_NAV_R1,
   RMENU_DEVICE_NAV_L2,
   RMENU_DEVICE_NAV_R2,
   RMENU_DEVICE_NAV_L3,
   RMENU_DEVICE_NAV_R3,
   RMENU_DEVICE_NAV_LAST
};

/*============================================================
EVENT CALLBACKS (AND RELATED)
============================================================ */

static void populate_setting_item(void *data, unsigned input)
{
   item *current_item = (item*)data;
   char fname[PATH_MAX];
   (void)fname;

   unsigned currentsetting = input;
   current_item->enum_id = input;

   switch(currentsetting)
   {
#ifdef __CELLOS_LV2__
      case SETTING_CHANGE_RESOLUTION:
         {
            unsigned width = gfx_ctx_get_resolution_width(g_extern.console.screen.resolutions.list[g_extern.console.screen.resolutions.current.idx]);
            unsigned height = gfx_ctx_get_resolution_height(g_extern.console.screen.resolutions.list[g_extern.console.screen.resolutions.current.idx]);
            snprintf(current_item->text, sizeof(current_item->text), "Resolution");
            snprintf(current_item->setting_text, sizeof(current_item->setting_text), "%dx%d", width, height);
            snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Change the display resolution.");
         }
         break;
#endif
#if defined(HAVE_CG) || defined(HAVE_HLSL) || defined(HAVE_GLSL)
      case SETTING_SHADER_PRESETS:
         snprintf(current_item->text, sizeof(current_item->text), "Shader Presets (CGP)");
         fill_pathname_base(fname, g_extern.file_state.cgp_path, sizeof(fname));
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), fname);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Select a [CG Preset] script.");
         break;
      case SETTING_SHADER:
         fill_pathname_base(fname, g_settings.video.cg_shader_path, sizeof(fname));
         snprintf(current_item->text, sizeof(current_item->text), "Shader #1");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "%s", fname);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Select a shader as [Shader #1]. NOTE: Some shaders might be\ntoo slow at 1080p. If you experience any slowdown, try another shader.");
         break;
      case SETTING_SHADER_2:
         fill_pathname_base(fname, g_settings.video.second_pass_shader, sizeof(fname));
         snprintf(current_item->text, sizeof(current_item->text), "Shader #2");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "%s", fname);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Select a shader as [Shader #2]. NOTE: Some shaders might be\ntoo slow at 1080p. If you experience any slowdown, try another shader.");
         break;
#endif
      case SETTING_FONT_SIZE:
         snprintf(current_item->text, sizeof(current_item->text), "Font Size");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "%f", g_extern.console.rmenu.font_size);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Increase or decrease the [Font Size].");
         break;
      case SETTING_KEEP_ASPECT_RATIO:
         snprintf(current_item->text, sizeof(current_item->text), "Aspect Ratio");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), aspectratio_lut[g_settings.video.aspect_ratio_idx].name);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Select an [Aspect Ratio].");
         break;
      case SETTING_HW_TEXTURE_FILTER:
         snprintf(current_item->text, sizeof(current_item->text), "Hardware filtering #1");
         if(g_settings.video.smooth)
            snprintf(current_item->setting_text, sizeof(current_item->setting_text), "Bilinear");
         else
            snprintf(current_item->setting_text, sizeof(current_item->setting_text), "Point");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Hardware filtering #1 is set to [%s].", current_item->setting_text);
         break;
#ifdef HAVE_FBO
      case SETTING_HW_TEXTURE_FILTER_2:
         snprintf(current_item->text, sizeof(current_item->text), "Hardware filtering #2");
         if(g_settings.video.second_pass_smooth)
            snprintf(current_item->setting_text, sizeof(current_item->setting_text), "Bilinear");
         else
            snprintf(current_item->setting_text, sizeof(current_item->setting_text), "Point");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Hardware filtering #2 is set to [%s].", current_item->setting_text);
         break;
      case SETTING_SCALE_ENABLED:
         snprintf(current_item->text, sizeof(current_item->text), "Custom Scaling/Dual Shaders");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_settings.video.render_to_texture ? "ON" : "OFF");
         snprintf(current_item->comment, sizeof(current_item->comment), g_settings.video.render_to_texture ? "INFO - [Custom Scaling] is set to 'ON' - 2x shaders will look much\nbetter, and you can select a shader for [Shader #2]." : "INFO - [Custom Scaling] is set to 'OFF'.");
         break;
      case SETTING_SCALE_FACTOR:
         snprintf(current_item->text, sizeof(current_item->text), "Custom Scaling Factor");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "%fx (X) / %fx (Y)", g_settings.video.fbo.scale_x, g_settings.video.fbo.scale_y);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - [Custom Scaling Factor] is set to: '%fx (X) / %fx (Y)'.", g_settings.video.fbo.scale_x, g_settings.video.fbo.scale_y);
         break;
#endif
#ifdef _XBOX1
      case SETTING_FLICKER_FILTER:
         snprintf(current_item->text, sizeof(current_item->text), "Flicker Filter");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "%d", g_extern.console.screen.state.flicker_filter.enable);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Toggle the [Flicker Filter].");
         break;
      case SETTING_SOFT_DISPLAY_FILTER:
         snprintf(current_item->text, sizeof(current_item->text), "Soft Display Filter");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.console.screen.state.soft_filter.enable ? "ON" : "OFF");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Toggle the [Soft Display Filter].");
         break;
#endif
      case SETTING_HW_OVERSCAN_AMOUNT:
         snprintf(current_item->text, sizeof(current_item->text), "Overscan");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "%f", g_extern.console.screen.overscan_amount);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Adjust or decrease [Overscan]. Set this to higher than 0.000\nif the screen doesn't fit on your TV/monitor.");
         break;
      case SETTING_THROTTLE_MODE:
         snprintf(current_item->text, sizeof(current_item->text), "Throttle Mode");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.console.screen.state.throttle.enable ? "ON" : "OFF");
         snprintf(current_item->comment, sizeof(current_item->comment), g_extern.console.screen.state.throttle.enable ? "INFO - [Throttle Mode] is 'ON' - Vsync is enabled." : "INFO - [Throttle Mode] is 'OFF' - Vsync is disabled.");
         break;
      case SETTING_TRIPLE_BUFFERING:
         snprintf(current_item->text, sizeof(current_item->text), "Triple Buffering");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.console.screen.state.triple_buffering.enable ? "ON" : "OFF");
         snprintf(current_item->comment, sizeof(current_item->comment), g_extern.console.screen.state.triple_buffering.enable ? "INFO - [Triple Buffering] is set to 'ON'." : "INFO - [Triple Buffering] is set to 'OFF'.");
         break;
      case SETTING_ENABLE_SCREENSHOTS:
         snprintf(current_item->text, sizeof(current_item->text), "Screenshot Option");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.console.screen.state.screenshots.enable ? "ON" : "OFF");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Screenshots feature is set to '%s'.", g_extern.console.screen.state.screenshots.enable ? "ON" : "OFF");
         break;
#if defined(HAVE_CG) || defined(HAVE_HLSL) || defined(HAVE_GLSL)
      case SETTING_APPLY_SHADER_PRESET_ON_STARTUP:
         snprintf(current_item->text, sizeof(current_item->text), "APPLY SHADER PRESET ON STARTUP");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Automatically load the currently selected [CG Preset] file on startup.");
         break;
#endif
      case SETTING_DEFAULT_VIDEO_ALL:
         snprintf(current_item->text, sizeof(current_item->text), "DEFAULTS");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Set all [General Video Settings] back to their 'DEFAULT' values.");
         break;
      case SETTING_SOUND_MODE:
         snprintf(current_item->text, sizeof(current_item->text), "Sound Output");
         switch(g_extern.console.sound.mode)
         {
            case SOUND_MODE_NORMAL:
               snprintf(current_item->comment, sizeof(current_item->comment), "INFO - [Sound Output] is set to 'Normal'.");
               snprintf(current_item->setting_text, sizeof(current_item->setting_text), "Normal");
               break;
#ifdef HAVE_RSOUND
            case SOUND_MODE_RSOUND:
               snprintf(current_item->comment, sizeof(current_item->comment), "INFO - [Sound Output] is set to 'RSound'." );
               snprintf(current_item->setting_text, sizeof(current_item->setting_text), "RSound");
               break;
#endif
#ifdef HAVE_HEADSET
            case SOUND_MODE_HEADSET:
               snprintf(current_item->comment, sizeof(current_item->comment), "INFO - [Sound Output] is set to 'USB/Bluetooth Headset'.");
               snprintf(current_item->setting_text, sizeof(current_item->setting_text), "USB/Bluetooth Headset");
               break;
#endif
            default:
               break;
         }
         break;
#ifdef HAVE_RSOUND
      case SETTING_RSOUND_SERVER_IP_ADDRESS:
         snprintf(current_item->text, sizeof(current_item->text), "RSound Server IP Address");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_settings.audio.device);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Enter the IP Address of the [RSound Audio Server]. IP address\nmust be an IPv4 32-bits address, eg: '192.168.1.7'.");
         break;
#endif
      case SETTING_DEFAULT_AUDIO_ALL:
         snprintf(current_item->text, sizeof(current_item->text), "DEFAULTS");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Set all [General Audio Settings] back to their 'DEFAULT' values.");
         break;
      case SETTING_EMU_CURRENT_SAVE_STATE_SLOT:
         snprintf(current_item->text, sizeof(current_item->text), "Current save state slot");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "%d", g_extern.state_slot);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Set the currently selected savestate slot.");
         break;
         /* emu-specific */
      case SETTING_EMU_SHOW_DEBUG_INFO_MSG:
         snprintf(current_item->text, sizeof(current_item->text), "Debug info messages");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.console.rmenu.state.msg_fps.enable ? "ON" : "OFF");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Show onscreen debug messages.");
         break;
      case SETTING_EMU_SHOW_INFO_MSG:
         snprintf(current_item->text, sizeof(current_item->text), "Info messages");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.console.rmenu.state.msg_info.enable ? "ON" : "OFF");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Show onscreen info messages in the menu.");
         break;
      case SETTING_EMU_REWIND_ENABLED:
         snprintf(current_item->text, sizeof(current_item->text), "Rewind option");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_settings.rewind_enable ? "ON" : "OFF");
         if(g_settings.rewind_enable)
            snprintf(current_item->comment, sizeof(current_item->comment), "INFO - [Rewind] feature is set to 'ON'.");
         else
            snprintf(current_item->comment, sizeof(current_item->comment), "INFO - [Rewind] feature is set to 'OFF'.");
         break;
#ifdef HAVE_ZLIB
      case SETTING_ZIP_EXTRACT:
         snprintf(current_item->text, sizeof(current_item->text), "ZIP Extract Option");
         switch(g_extern.file_state.zip_extract_mode)
         {
            case ZIP_EXTRACT_TO_CURRENT_DIR:
               snprintf(current_item->setting_text, sizeof(current_item->setting_text), "Current dir");
               snprintf(current_item->comment, sizeof(current_item->comment), "INFO - ZIP files are extracted to the current dir.");
               break;
            case ZIP_EXTRACT_TO_CURRENT_DIR_AND_LOAD_FIRST_FILE:
               snprintf(current_item->setting_text, sizeof(current_item->setting_text), "Current dir and load first file");
               snprintf(current_item->comment, sizeof(current_item->comment), "INFO - ZIP files are extracted to current dir, and auto-loaded.");
               break;
            case ZIP_EXTRACT_TO_CACHE_DIR:
               snprintf(current_item->setting_text, sizeof(current_item->setting_text), "Cache dir");
               snprintf(current_item->comment, sizeof(current_item->comment), "INFO - ZIP files are extracted to the cache dir.");
               break;
         }
         break;
#endif
      case SETTING_RARCH_DEFAULT_EMU:
         snprintf(current_item->text, sizeof(current_item->text), "Default libretro core");
         fill_pathname_base(fname, g_settings.libretro, sizeof(fname));
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "%s", fname);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Select a default libretro core to launch at start-up.");
         break;
      case SETTING_QUIT_RARCH:
         snprintf(current_item->text, sizeof(current_item->text), "Quit RetroArch and save settings ");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Quits RetroArch and saves the settings.");
         break;
      case SETTING_EMU_AUDIO_MUTE:
         snprintf(current_item->text, sizeof(current_item->text), "Mute Audio");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.audio_data.mute ? "ON" : "OFF");
         if(g_extern.audio_data.mute)
            snprintf(current_item->comment, sizeof(current_item->comment), "INFO - [Audio Mute] is set to 'ON'. The game audio will be muted.");
         else
            snprintf(current_item->comment, sizeof(current_item->comment), "INFO - [Audio Mute] is set to 'OFF'.");
         break;
#ifdef _XBOX1
      case SETTING_EMU_AUDIO_SOUND_VOLUME_LEVEL:
         snprintf(current_item->text, sizeof(current_item->text), "Volume Level");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.console.sound.volume_level ? "Loud" : "Normal");
         if(g_extern.audio_data.mute)
            snprintf(current_item->comment, sizeof(current_item->comment), "INFO - [Volume Level] is set to 'Loud'");
         else
            snprintf(current_item->comment, sizeof(current_item->comment), "INFO - [Volume Level' is set to 'Normal'.");
         break;
#endif
      case SETTING_ENABLE_CUSTOM_BGM:
         snprintf(current_item->text, sizeof(current_item->text), "Custom BGM Option");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.console.sound.custom_bgm.enable ? "ON" : "OFF");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - [Custom BGM] is set to '%s'.", g_extern.console.sound.custom_bgm.enable ? "ON" : "OFF");
         break;
      case SETTING_PATH_DEFAULT_ROM_DIRECTORY:
         snprintf(current_item->text, sizeof(current_item->text), "Startup ROM Directory");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.console.main_wrap.paths.default_rom_startup_dir);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Set the default [Startup ROM directory]. NOTE: You will have to\nrestart the emulator for this change to have any effect.");
         break;
      case SETTING_PATH_SAVESTATES_DIRECTORY:
         snprintf(current_item->text, sizeof(current_item->text), "Savestate Directory");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.console.main_wrap.paths.default_savestate_dir);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Set the default path where all the savestate files will be saved to.");
         break;
      case SETTING_PATH_SRAM_DIRECTORY:
         snprintf(current_item->text, sizeof(current_item->text), "SRAM Directory");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.console.main_wrap.paths.default_sram_dir);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Set the default SRAM (SaveRAM) directory path. All the\nbattery backup saves will be stored in this directory.");
         break;
#ifdef HAVE_XML
      case SETTING_PATH_CHEATS:
         snprintf(current_item->text, sizeof(current_item->text), "Cheatfile Directory");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_settings.cheat_database);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Set the default [Cheatfile directory] path. All CHT (cheat) files\nwill be stored here.");
         break;
#endif
      case SETTING_PATH_SYSTEM:
         snprintf(current_item->text, sizeof(current_item->text), "System Directory");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_settings.system_directory);
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Set the default [System directory] path. System files like\nBIOS files, etc. will be stored here.");
         break;
      case SETTING_ENABLE_SRAM_PATH:
         snprintf(current_item->text, sizeof(current_item->text), "Custom SRAM Dir Enable");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.console.main_wrap.state.default_sram_dir.enable ? "ON" : "OFF");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - [Custom SRAM Dir Path] is set to '%s'.", g_extern.console.main_wrap.state.default_sram_dir.enable ? "ON" : "OFF");
         break;
      case SETTING_ENABLE_STATE_PATH:
         snprintf(current_item->text, sizeof(current_item->text), "Custom Savestate Dir Enable");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.console.main_wrap.state.default_savestate_dir.enable ? "ON" : "OFF");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - [Custom Savestate Dir Path] is set to '%s'.", g_extern.console.main_wrap.state.default_savestate_dir.enable ? "ON" : "OFF");
         break;
      case SETTING_CONTROLS_SCHEME:
         snprintf(current_item->text, sizeof(current_item->text), "Control Scheme Preset");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Input scheme preset [%s] is selected.", g_extern.file_state.input_cfg_path);
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), g_extern.file_state.input_cfg_path);
         break;
      case SETTING_CONTROLS_NUMBER:
         snprintf(current_item->text, sizeof(current_item->text), "Controller No");
         snprintf(current_item->comment, sizeof(current_item->comment), "Controller %d is currently selected.", currently_selected_controller_menu+1);
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "%d", currently_selected_controller_menu+1);
         break;
      case SETTING_DPAD_EMULATION:
         snprintf(current_item->text, sizeof(current_item->text), "D-Pad Emulation");
         snprintf(current_item->comment, sizeof(current_item->comment), "[%s] from Controller %d is mapped to D-pad.", rarch_dpad_emulation_name_lut[g_settings.input.dpad_emulation[currently_selected_controller_menu]], currently_selected_controller_menu+1);
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "%s", rarch_dpad_emulation_name_lut[g_settings.input.dpad_emulation[currently_selected_controller_menu]]);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_B:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_Y:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_SELECT:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_START:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_UP:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_DOWN:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_LEFT:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_RIGHT:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_A:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_X:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_L:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_R:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_L2:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_R2:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_L3:
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_R3:
         {
            const char * value = rarch_input_find_platform_key_label(g_settings.input.binds[currently_selected_controller_menu][currentsetting-(FIRST_CONTROL_BIND)].joykey);
            unsigned id = currentsetting - FIRST_CONTROL_BIND;
            snprintf(current_item->text, sizeof(current_item->text), rarch_input_get_default_keybind_name(id));
            snprintf(current_item->comment, sizeof(current_item->comment), "INFO - [%s] is mapped to action:\n[%s].", current_item->text, value);
            snprintf(current_item->setting_text, sizeof(current_item->setting_text), value);
         }
         break;
      case SETTING_CONTROLS_SAVE_CUSTOM_CONTROLS:
         snprintf(current_item->text, sizeof(current_item->text), "SAVE CUSTOM CONTROLS");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Save the [Custom Controls] settings to file.");
         break;
      case SETTING_CONTROLS_DEFAULT_ALL:
         snprintf(current_item->text, sizeof(current_item->text), "DEFAULTS");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "");
         snprintf(current_item->comment, sizeof(current_item->comment), "IFNO - Set all [Controls] back to their 'DEFAULT' values.");
         break;
      case SETTING_EMU_VIDEO_DEFAULT_ALL:
         snprintf(current_item->text, sizeof(current_item->text), "DEFAULTS");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Set [all RetroArch Video settings] back to their 'DEFAULT' values.");
         break;
      case SETTING_EMU_AUDIO_DEFAULT_ALL:
         snprintf(current_item->text, sizeof(current_item->text), "DEFAULTS");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Set all [RetroArch Audio settings] back to their 'DEFAULT' values.");
         break;
      case SETTING_PATH_DEFAULT_ALL:
         snprintf(current_item->text, sizeof(current_item->text), "DEFAULTS");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Set all [Path settings] back to their 'DEFAULT' values.");
         break;
      case SETTING_EMU_DEFAULT_ALL:
         snprintf(current_item->text, sizeof(current_item->text), "DEFAULTS");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Set [all RetroArch settings] back to their 'DEFAULT' values.");
         break;
#if defined(HAVE_CG) || defined(HAVE_HLSL) || defined(HAVE_GLSL)
      case SETTING_SAVE_SHADER_PRESET:
         snprintf(current_item->text, sizeof(current_item->text), "SAVE SETTINGS AS CGP PRESET");
         snprintf(current_item->setting_text, sizeof(current_item->setting_text), "");
         snprintf(current_item->comment, sizeof(current_item->comment), "INFO - Save the current video settings to a [CG Preset] (CGP) file.");
         break;
#endif
      default:
         break;
   }
}

static void display_menubar(void *data)
{
   menu *current_menu = (menu*)data;
   DEVICE_CAST device_ptr = (DEVICE_CAST)driver.video_data;
   filebrowser_t *fb = &browser;
   char current_path[256], rarch_version[128], msg[128];

   rmenu_default_positions_t default_pos;
   device_ptr->ctx_driver->rmenu_set_default_pos(&default_pos);

   snprintf(rarch_version, sizeof(rarch_version), "v%s", PACKAGE_VERSION);

   switch(current_menu->enum_id)
   {
      case GENERAL_VIDEO_MENU:
         snprintf(msg, sizeof(msg), "NEXT -> [%s]", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_R));
         device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.current_path_y_position, default_pos.current_path_font_size, WHITE, msg);
         break;
      case GENERAL_AUDIO_MENU:
      case EMU_GENERAL_MENU:
      case EMU_VIDEO_MENU:
      case EMU_AUDIO_MENU:
      case PATH_MENU:
         snprintf(msg, sizeof(msg), "[%s] <- PREV | NEXT -> [%s]", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_L), rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_R));
         device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.current_path_y_position, default_pos.current_path_font_size, WHITE, msg);
         break;
      case CONTROLS_MENU:
      case INGAME_MENU_RESIZE:
         snprintf(msg, sizeof(msg), "[%s] <- PREV", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_L));
         device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.current_path_y_position, default_pos.current_path_font_size, WHITE, msg);
         break;
      default:
         break;
   }

   switch(current_menu->enum_id)
   {
      case SHADER_CHOICE:
      case PRESET_CHOICE:
      case BORDER_CHOICE:
      case LIBRETRO_CHOICE:
      case INPUT_PRESET_CHOICE:
      case PATH_SAVESTATES_DIR_CHOICE:
      case PATH_DEFAULT_ROM_DIR_CHOICE:
#ifdef HAVE_XML
      case PATH_CHEATS_DIR_CHOICE:
#endif
      case PATH_SRAM_DIR_CHOICE:
      case PATH_SYSTEM_DIR_CHOICE:
         fb = &tmpBrowser;
      case FILE_BROWSER_MENU:
         snprintf(current_path, sizeof(current_path), "PATH: %s", filebrowser_get_current_dir(fb));
         device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.current_path_y_position, default_pos.current_path_font_size, WHITE, current_path);
         break;
      default:
         break;
   }

   rarch_position_t position = {0};
   device_ptr->ctx_driver->rmenu_draw_bg(&position);

   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.core_msg_x_position, default_pos.core_msg_y_position, default_pos.core_msg_font_size, WHITE, m_title);
#ifdef __CELLOS_LV2__
   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, 0.05f, 1.4f, WHITE, current_menu->title);
   device_ptr->font_ctx->render_msg_place(device_ptr,0.80f, 0.015f, 0.82f, WHITE, rarch_version);
#endif
}

static void browser_update(void *data, uint64_t input, const char *extensions)
{
   filebrowser_t *b = (filebrowser_t*)data;
   filebrowser_action_t action = FILEBROWSER_ACTION_NOOP;
   bool ret = true;

   if (input & (1ULL << RMENU_DEVICE_NAV_DOWN))
      action = FILEBROWSER_ACTION_DOWN;
   else if (input & (1ULL << RMENU_DEVICE_NAV_UP))
      action = FILEBROWSER_ACTION_UP;
   else if (input & (1ULL << RMENU_DEVICE_NAV_RIGHT))
      action = FILEBROWSER_ACTION_RIGHT;
   else if (input & (1ULL << RMENU_DEVICE_NAV_LEFT))
      action = FILEBROWSER_ACTION_LEFT;
   else if (input & (1ULL << RMENU_DEVICE_NAV_R2))
      action = FILEBROWSER_ACTION_SCROLL_DOWN;
   else if (input & (1ULL << RMENU_DEVICE_NAV_L2))
      action = FILEBROWSER_ACTION_SCROLL_UP;
   else if (input & (1ULL << RMENU_DEVICE_NAV_A))
      action = FILEBROWSER_ACTION_CANCEL;
   else if (input & (1ULL << RMENU_DEVICE_NAV_START))
   {
      action = FILEBROWSER_ACTION_RESET;
      filebrowser_set_root(b, default_paths.filesystem_root_dir);
      strlcpy(b->extensions, extensions, sizeof(b->extensions));
   }

   if(action != FILEBROWSER_ACTION_NOOP)
      ret = filebrowser_iterate(b, action);

   if(!ret)
      rarch_settings_msg(S_MSG_DIR_LOADING_ERROR, S_DELAY_180);
}

void browser_render(void *data)
{
   filebrowser_t *b = (filebrowser_t*)data;
   DEVICE_CAST device_ptr = (DEVICE_CAST)driver.video_data;
   unsigned file_count = b->current_dir.list->size;
   unsigned int current_index, page_number, page_base, i;

   rmenu_default_positions_t default_pos;
   device_ptr->ctx_driver->rmenu_set_default_pos(&default_pos);

   current_index = b->current_dir.ptr;
   page_number = current_index / default_pos.entries_per_page;
   page_base = page_number * default_pos.entries_per_page;

   for (i = page_base; i < file_count && i < page_base + default_pos.entries_per_page; ++i)
   {
      char fname_tmp[256];
      fill_pathname_base(fname_tmp, b->current_dir.list->elems[i].data, sizeof(fname_tmp));
      default_pos.starting_y_position += default_pos.y_position_increment;

      //check if this is the currently selected file
      const char *current_pathname = filebrowser_get_current_path(b);
      if(strcmp(current_pathname, b->current_dir.list->elems[i].data) == 0)
      {
         rarch_position_t position = {0};
         position.x = default_pos.x_position;
         position.y = default_pos.starting_y_position;
         device_ptr->ctx_driver->rmenu_draw_panel(&position);
      }

      device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.starting_y_position, default_pos.variable_font_size, i == current_index ? RED : b->current_dir.list->elems[i].attr.b ? GREEN : WHITE, fname_tmp);
   }
}

int select_file(void *data, void *state)
{
   menu *current_menu = (menu*)data;
   rmenu_state_t *rstate = (rmenu_state_t*)state;

   uint64_t input = rstate->input;

   DEVICE_CAST device_ptr = (DEVICE_CAST)driver.video_data;
   char extensions[256], comment[256], path[PATH_MAX];
   bool ret = true;

   filebrowser_t *filebrowser = &tmpBrowser;
   rmenu_default_positions_t default_pos;
   device_ptr->ctx_driver->rmenu_set_default_pos(&default_pos);

   switch(current_menu->enum_id)
   {
      case SHADER_CHOICE:
         strlcpy(extensions, EXT_SHADERS, sizeof(extensions));
         snprintf(comment, sizeof(comment), "INFO - Select a shader by pressing [%s].", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
         break;
      case PRESET_CHOICE:
         strlcpy(extensions, EXT_CGP_PRESETS, sizeof(extensions));
         snprintf(comment, sizeof(comment), "INFO - Select a shader preset by pressing [%s].", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
         break;
      case INPUT_PRESET_CHOICE:
         strlcpy(extensions, EXT_INPUT_PRESETS, sizeof(extensions));
         snprintf(comment, sizeof(comment), "INFO - Select an input preset by pressing [%s].", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
         break;
      case BORDER_CHOICE:
         strlcpy(extensions, EXT_IMAGES, sizeof(extensions));
         snprintf(comment, sizeof(comment), "INFO - Select a border image file by pressing [%s].", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
         break;
      case LIBRETRO_CHOICE:
         strlcpy(extensions, EXT_EXECUTABLES, sizeof(extensions));
         snprintf(comment, sizeof(comment), "INFO - Select a Libretro core by pressing [%s].", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
         break;
   }

   browser_update(filebrowser, input, extensions);

   if (input & (1ULL << RMENU_DEVICE_NAV_B))
   {
      bool is_dir = filebrowser_get_current_path_isdir(filebrowser);
      if(is_dir)
         ret = filebrowser_iterate(filebrowser, FILEBROWSER_ACTION_OK);
      else
      {
         snprintf(path, sizeof(path), filebrowser_get_current_path(filebrowser));

         switch(current_menu->enum_id)
         {
#if defined(HAVE_CG) || defined(HAVE_HLSL) || defined(HAVE_GLSL)
            case SHADER_CHOICE:
               rarch_load_shader(set_shader+1, path);
               switch(set_shader+1)
               {
                  case 1:
                     strlcpy(g_settings.video.cg_shader_path, path, sizeof(g_settings.video.cg_shader_path));
                     break;
                  case 2:
                     strlcpy(g_settings.video.second_pass_shader, path, sizeof(g_settings.video.second_pass_shader));
                     break;
               }
               break;
            case PRESET_CHOICE:
               strlcpy(g_extern.file_state.cgp_path, path, sizeof(g_extern.file_state.cgp_path));
               device_ptr->ctx_driver->set_fbo(FBO_DEINIT);
#ifdef HAVE_OPENGL
               gl_cg_reinit(path);
#endif
               device_ptr->ctx_driver->set_fbo(FBO_INIT);
               break;
#endif
            case INPUT_PRESET_CHOICE:
               strlcpy(g_extern.file_state.input_cfg_path, path, sizeof(g_extern.file_state.input_cfg_path));
               config_read_keybinds(path);
               break;
            case BORDER_CHOICE:
               break;
            case LIBRETRO_CHOICE:
               strlcpy(g_settings.libretro, path, sizeof(g_settings.libretro));

               if(set_libretro_core_as_launch)
               {
                  strlcpy(g_extern.console.external_launch.launch_app, path, sizeof(g_extern.console.external_launch.launch_app));
                  set_libretro_core_as_launch = false;
                  rarch_settings_change(S_RETURN_TO_LAUNCHER);
               }
               else
               {
                  if(g_extern.console.rmenu.state.msg_info.enable)
                     rarch_settings_msg(S_MSG_RESTART_RARCH, S_DELAY_180);
               }
               break;
         }

         menu_stack_pop();
      }

      if(!ret)
         rarch_settings_msg(S_MSG_DIR_LOADING_ERROR, S_DELAY_180);
   }
   else if (input & (1ULL << RMENU_DEVICE_NAV_X))
      menu_stack_pop();

   display_menubar(current_menu);

   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.comment_y_position, default_pos.font_size, WHITE, comment);
   snprintf(comment, sizeof(comment), "[%s] - return to settings [%s] - Reset Startdir", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_X), rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_START));
   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.comment_two_y_position, default_pos.font_size, YELLOW, comment);

   if(current_menu->browser_draw)
      current_menu->browser_draw(filebrowser);

   return 1;
}

int select_directory(void *data, void *state)
{
   menu *current_menu = (menu*)data;
   rmenu_state_t *rstate = (rmenu_state_t*)state;

   uint64_t input = rstate->input;

   DEVICE_CAST device_ptr = (DEVICE_CAST)driver.video_data;
   char path[PATH_MAX], msg[256];
   bool ret = true;

   filebrowser_t *filebrowser = &tmpBrowser;
   rmenu_default_positions_t default_pos;

   device_ptr->ctx_driver->rmenu_set_default_pos(&default_pos);

   bool is_dir = filebrowser_get_current_path_isdir(filebrowser);
   browser_update(filebrowser, input, "empty");

   if (input & (1ULL << RMENU_DEVICE_NAV_Y))
   {
      if(is_dir)
      {
         snprintf(path, sizeof(path), filebrowser_get_current_path(filebrowser));

         switch(current_menu->enum_id)
         {
            case PATH_SAVESTATES_DIR_CHOICE:
               strlcpy(g_extern.console.main_wrap.paths.default_savestate_dir, path, sizeof(g_extern.console.main_wrap.paths.default_savestate_dir));
               break;
            case PATH_SRAM_DIR_CHOICE:
               strlcpy(g_extern.console.main_wrap.paths.default_sram_dir, path, sizeof(g_extern.console.main_wrap.paths.default_sram_dir));
               break;
            case PATH_DEFAULT_ROM_DIR_CHOICE:
               strlcpy(g_extern.console.main_wrap.paths.default_rom_startup_dir, path, sizeof(g_extern.console.main_wrap.paths.default_rom_startup_dir));
               break;
#ifdef HAVE_XML
            case PATH_CHEATS_DIR_CHOICE:
               strlcpy(g_settings.cheat_database, path, sizeof(g_settings.cheat_database));
               break;
#endif
            case PATH_SYSTEM_DIR_CHOICE:
               strlcpy(g_settings.system_directory, path, sizeof(g_settings.system_directory));
               break;
         }
         menu_stack_pop();
      }
   }
   else if (input & (1ULL << RMENU_DEVICE_NAV_X))
   {
      strlcpy(path, default_paths.port_dir, sizeof(path));
      switch(current_menu->enum_id)
      {
         case PATH_SAVESTATES_DIR_CHOICE:
            strlcpy(g_extern.console.main_wrap.paths.default_savestate_dir, path, sizeof(g_extern.console.main_wrap.paths.default_savestate_dir));
            break;
         case PATH_SRAM_DIR_CHOICE:
            strlcpy(g_extern.console.main_wrap.paths.default_sram_dir, path, sizeof(g_extern.console.main_wrap.paths.default_sram_dir));
            break;
         case PATH_DEFAULT_ROM_DIR_CHOICE:
            strlcpy(g_extern.console.main_wrap.paths.default_rom_startup_dir, path, sizeof(g_extern.console.main_wrap.paths.default_rom_startup_dir));
            break;
#ifdef HAVE_XML
         case PATH_CHEATS_DIR_CHOICE:
            strlcpy(g_settings.cheat_database, path, sizeof(g_settings.cheat_database));
            break;
#endif
         case PATH_SYSTEM_DIR_CHOICE:
            strlcpy(g_settings.system_directory, path, sizeof(g_settings.system_directory));
            break;
      }

      menu_stack_pop();
   }
   else if (input & (1ULL << RMENU_DEVICE_NAV_B))
   {
      if(is_dir)
         ret = filebrowser_iterate(filebrowser, FILEBROWSER_ACTION_OK);
   }

   if(!ret)
      rarch_settings_msg(S_MSG_DIR_LOADING_ERROR, S_DELAY_180);

   display_menubar(current_menu);

   snprintf(msg, sizeof(msg), "[%s] - Enter dir | [%s] - Go back", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B), rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_X));
   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.comment_two_y_position, default_pos.font_size, YELLOW, msg);

   snprintf(msg, sizeof(msg), "[%s] - Reset to startdir", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_START));
   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.comment_two_y_position + (default_pos.y_position_increment * 1), default_pos.font_size, YELLOW, msg);

   snprintf(msg, sizeof(msg), "INFO - Browse to a directory and assign it as the path by\npressing [%s].", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_Y));
   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.comment_y_position, default_pos.font_size, WHITE, msg);

   if(current_menu->browser_draw)
      current_menu->browser_draw(filebrowser);

   return 1;
}

static void set_keybind_digital(uint64_t default_retro_joypad_id, uint64_t input)
{
   unsigned keybind_action = KEYBIND_NOACTION;

   if(input & (1ULL << RMENU_DEVICE_NAV_LEFT))
      keybind_action = KEYBIND_DECREMENT;

   if((input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
      keybind_action = KEYBIND_INCREMENT;

   if(input & (1ULL << RMENU_DEVICE_NAV_START))
      keybind_action = KEYBIND_DEFAULT;

   rarch_input_set_keybind(currently_selected_controller_menu, keybind_action, default_retro_joypad_id);
}

#if defined(HAVE_OSKUTIL)
#ifdef __CELLOS_LV2__
static char filepath[PATH_MAX];

static bool osk_callback_enter_rsound(void *data)
{
   rmenu_state_t *rstate = (rmenu_state_t*)state;

   if(!(OSK_IS_RUNNING(g_extern.console.misc.oskutil_handle)) && g_extern.console.misc.oskutil_handle.text_can_be_fetched)
   {
      strlcpy(g_settings.audio.device, OUTPUT_TEXT_STRING(g_extern.console.misc.oskutil_handle), sizeof(g_settings.audio.device));

      if(!rstate->osk_unbind_after_finish)
         rstate->osk_callback = NULL;

      return true;
   }
   return false;
}

static bool osk_callback_enter_rsound_init(void *data)
{
   rmenu_state_t *rstate = (rmenu_state_t*)state;

   oskutil_write_initial_message(&g_extern.console.misc.oskutil_handle, L"192.168.1.1");
   oskutil_write_message(&g_extern.console.misc.oskutil_handle, L"Enter IP address for the RSound Server.");
   oskutil_start(&g_extern.console.misc.oskutil_handle);

   rstate->osk_unbind_after_finish = true;
   rstate->osk_callback = osk_callback_enter_rsound;

   return true;
}

static bool osk_callback_enter_title_init(void *data);

static bool osk_callback_enter_filename(void *data)
{
   rmenu_state_t *rstate = (rmenu_state_t*)state;
   char filename_tmp[256];

   if(!(OSK_IS_RUNNING(g_extern.console.misc.oskutil_handle)) && g_extern.console.misc.oskutil_handle.text_can_be_fetched)
   {
      strlcpy(filename_tmp, OUTPUT_TEXT_STRING(g_extern.console.misc.oskutil_handle), sizeof(filename_tmp));

      switch(rstate->osk_param)
      {
         case CONFIG_FILE:
            break;
         case SHADER_PRESET_FILE:
            snprintf(filepath, sizeof(filepath), "%s/%s.cgp", default_paths.cgp_dir, filename_tmp);
            break;
         case INPUT_PRESET_FILE:
            snprintf(filepath, sizeof(filepath), "%s/%s.cfg", default_paths.input_presets_dir, filename_tmp);
            break;
      }

      if(!rstate->osk_unbind_after_finish)
         rstate->osk_callback = NULL;

      rstate->osk_init  = osk_callback_enter_title_init;

      return true;
   }

   return false;
}

static bool osk_callback_enter_filename_init(void *data)
{
   rmenu_state_t *rstate = (rmenu_state_t*)state;

   oskutil_write_initial_message(&g_extern.console.misc.oskutil_handle, L"example");
   oskutil_write_message(&g_extern.console.misc.oskutil_handle, 
         L"Enter filename for preset (with no file extension)");
   oskutil_start(&g_extern.console.misc.oskutil_handle);

   rstate->osk_unbind_after_finish = false;

   if(!rstate->osk_unbind_after_finish)
      rstate->osk_init = NULL;

   rstate->osk_callback = osk_callback_enter_filename;

   return true;
}

static bool osk_callback_enter_title(void *data)
{
   rmenu_state_t *rstate = (rmenu_state_t*)state;
   char filetitle_tmp[256];

   if(!(OSK_IS_RUNNING(g_extern.console.misc.oskutil_handle)) && g_extern.console.misc.oskutil_handle.text_can_be_fetched)
   {
      if(g_extern.console.misc.oskutil_handle.text_can_be_fetched)
         snprintf(filetitle_tmp, sizeof(filetitle_tmp), "%s", OUTPUT_TEXT_STRING(g_extern.console.misc.oskutil_handle));
      else
         snprintf(filetitle_tmp, sizeof(filetitle_tmp), "%s", "Custom");

      switch(rstate->osk_param)
      {
         case CONFIG_FILE:
            break;
         case SHADER_PRESET_FILE:
            {
               struct gl_cg_cgp_info current_settings;
               current_settings.shader[0] = g_settings.video.cg_shader_path;
               current_settings.shader[1] = g_settings.video.second_pass_shader;
               current_settings.filter_linear[0] = g_settings.video.smooth;
               current_settings.filter_linear[1] = g_settings.video.second_pass_smooth;
               current_settings.render_to_texture = true;
               current_settings.fbo_scale = g_settings.video.fbo.scale_x; //fbo.scale_x and y are the same anyway
               gl_cg_save_cgp(filepath, &current_settings);
            }
            break;
         case INPUT_PRESET_FILE:
            config_save_keybinds(filepath);
            break;
      }

      if(!rstate->osk_unbind_after_finish)
         rstate->osk_callback = NULL;

      return true;
   }

   return false;
}

static bool osk_callback_enter_title_init(void *data)
{
   rmenu_state_t *rstate = (rmenu_state_t*)state;

   oskutil_write_initial_message(&g_extern.console.misc.oskutil_handle, L"Example file title");
   oskutil_write_message(&g_extern.console.misc.oskutil_handle, L"Enter title for preset");
   oskutil_start(&g_extern.console.misc.oskutil_handle);

   rstate->osk_unbind_after_finish = true;
   rstate->osk_callback = osk_callback_enter_title;

   return true;
}

#endif
#endif

static void set_setting_action(void *data, unsigned switchvalue, uint64_t input)
{
   (void)data;
   DEVICE_CAST device_ptr = (DEVICE_CAST)driver.video_data;
   filebrowser_t *filebrowser = &tmpBrowser;
   
   switch(switchvalue)
   {
#ifdef __CELLOS_LV2__
      case SETTING_CHANGE_RESOLUTION:
         if(input & (1ULL << RMENU_DEVICE_NAV_RIGHT))
            rarch_settings_change(S_RESOLUTION_NEXT);
         if(input & (1ULL << RMENU_DEVICE_NAV_LEFT))
            rarch_settings_change(S_RESOLUTION_PREVIOUS);
         if(input & (1ULL << RMENU_DEVICE_NAV_B))
         {
            if (g_extern.console.screen.resolutions.list[g_extern.console.screen.resolutions.current.idx] == CELL_VIDEO_OUT_RESOLUTION_576)
            {
               if(gfx_ctx_check_resolution(CELL_VIDEO_OUT_RESOLUTION_576))
               {
                  //ps3graphics_set_pal60hz(Settings.PS3PALTemporalMode60Hz);
                  video_ptr.restart();
               }
            }
            else
            {
               //ps3graphics_set_pal60hz(0);
               video_ptr.restart();
            }
         }
         break;
         /*
            case SETTING_PAL60_MODE:
            if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
            {
            if (Graphics->GetCurrentResolution() == CELL_VIDEO_OUT_RESOLUTION_576)
            {
            if(Graphics->CheckResolution(CELL_VIDEO_OUT_RESOLUTION_576))
            {
            Settings.PS3PALTemporalMode60Hz = !Settings.PS3PALTemporalMode60Hz;
            Graphics->SetPAL60Hz(Settings.PS3PALTemporalMode60Hz);
            Graphics->SwitchResolution(Graphics->GetCurrentResolution(), Settings.PS3PALTemporalMode60Hz, Settings.TripleBuffering);
            }
            }

            }
            break;
            */
#endif
#if defined(HAVE_CG) || defined(HAVE_HLSL) || defined(HAVE_GLSL)
      case SETTING_SHADER_PRESETS:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            if(g_extern.console.emulator_initialized)
            {
               menu_stack_push(PRESET_CHOICE);
               filebrowser_set_root_and_ext(filebrowser, EXT_CGP_PRESETS, default_paths.cgp_dir);
            }
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            strlcpy(g_extern.file_state.cgp_path, "", sizeof(g_extern.file_state.cgp_path));
         break;
      case SETTING_SHADER:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            menu_stack_push(SHADER_CHOICE);
            filebrowser_set_root_and_ext(filebrowser, EXT_SHADERS, default_paths.shader_dir);
            set_shader = 0;
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
            rarch_load_shader(1, NULL);
            strlcpy(g_settings.video.cg_shader_path, default_paths.shader_file, sizeof(g_settings.video.cg_shader_path));
         }
         break;
      case SETTING_SHADER_2:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            menu_stack_push(SHADER_CHOICE);
            filebrowser_set_root_and_ext(filebrowser, EXT_SHADERS, default_paths.shader_dir);
            set_shader = 1;
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
            rarch_load_shader(2, NULL);
            strlcpy(g_settings.video.second_pass_shader, default_paths.shader_file, sizeof(g_settings.video.second_pass_shader));
         }
         break;
#endif
      case SETTING_FONT_SIZE:
         if(input & (1ULL << RMENU_DEVICE_NAV_LEFT))
         {
            if(g_extern.console.rmenu.font_size > 0) 
               g_extern.console.rmenu.font_size -= 0.01f;
         }
         if((input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            if((g_extern.console.rmenu.font_size < 2.0f))
               g_extern.console.rmenu.font_size += 0.01f;
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            g_extern.console.rmenu.font_size = 1.0f;
         break;
      case SETTING_KEEP_ASPECT_RATIO:
         if(input & (1ULL << RMENU_DEVICE_NAV_LEFT))
         {
            rarch_settings_change(S_ASPECT_RATIO_DECREMENT);
            driver.video->set_aspect_ratio(driver.video_data, g_settings.video.aspect_ratio_idx);
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_RIGHT))
         {
            rarch_settings_change(S_ASPECT_RATIO_INCREMENT);
            driver.video->set_aspect_ratio(driver.video_data, g_settings.video.aspect_ratio_idx);
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
            rarch_settings_default(S_DEF_ASPECT_RATIO);
            driver.video->set_aspect_ratio(driver.video_data, g_settings.video.aspect_ratio_idx);
         }
         break;
      case SETTING_HW_TEXTURE_FILTER:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            rarch_settings_change(S_HW_TEXTURE_FILTER);
            device_ptr->ctx_driver->set_filtering(1, g_settings.video.smooth);
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
            rarch_settings_change(S_DEF_HW_TEXTURE_FILTER);
            device_ptr->ctx_driver->set_filtering(1, g_settings.video.smooth);
         }
         break;
#ifdef HAVE_FBO
      case SETTING_HW_TEXTURE_FILTER_2:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            rarch_settings_change(S_HW_TEXTURE_FILTER_2);
            device_ptr->ctx_driver->set_filtering(2, g_settings.video.second_pass_smooth);
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
            rarch_settings_change(S_DEF_HW_TEXTURE_FILTER_2);
            device_ptr->ctx_driver->set_filtering(2, g_settings.video.second_pass_smooth);
         }
         break;
      case SETTING_SCALE_ENABLED:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            rarch_settings_change(S_SCALE_ENABLED);

            if(g_settings.video.render_to_texture)
               device_ptr->ctx_driver->set_fbo(FBO_INIT);
            else
               device_ptr->ctx_driver->set_fbo(FBO_DEINIT);
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
            rarch_settings_default(S_DEF_SCALE_ENABLED);
            device_ptr->ctx_driver->set_fbo(FBO_REINIT);
         }
         break;
      case SETTING_SCALE_FACTOR:
         if(input & (1ULL << RMENU_DEVICE_NAV_LEFT))
         {
            if(g_settings.video.render_to_texture)
            {
               bool should_decrement = g_settings.video.fbo.scale_x > MIN_SCALING_FACTOR;

               if(should_decrement)
               {
                  rarch_settings_change(S_SCALE_FACTOR_DECREMENT);
                  device_ptr->ctx_driver->set_fbo(FBO_REINIT);
               }
            }
         }
         if((input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            if(g_settings.video.render_to_texture)
            {
               bool should_increment = g_settings.video.fbo.scale_x < MAX_SCALING_FACTOR;
               if(should_increment)
               {
                  rarch_settings_change(S_SCALE_FACTOR_INCREMENT);
                  device_ptr->ctx_driver->set_fbo(FBO_REINIT);
               }
            }
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
            rarch_settings_default(S_DEF_SCALE_FACTOR);
            device_ptr->ctx_driver->set_fbo(FBO_REINIT);
         }
         break;
#endif
#ifdef _XBOX1
      case SETTING_FLICKER_FILTER:
         if(input & (1ULL << RMENU_DEVICE_NAV_LEFT))
         {
            if(g_extern.console.screen.state.flicker_filter.value > 0)
               g_extern.console.screen.state.flicker_filter.value--;
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_RIGHT))
         {
            if(g_extern.console.screen.state.flicker_filter.value < 5)
               g_extern.console.screen.state.flicker_filter.value++;
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            g_extern.console.screen.state.flicker_filter.value = 0;
         break;
      case SETTING_SOFT_DISPLAY_FILTER:
         if(input & (1ULL << RMENU_DEVICE_NAV_LEFT) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
            g_extern.console.screen.state.soft_filter.enable = !g_extern.console.screen.state.soft_filter.enable;
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            g_extern.console.screen.state.soft_filter.enable = true;
         break;
#endif
      case SETTING_HW_OVERSCAN_AMOUNT:
         if(input & (1ULL << RMENU_DEVICE_NAV_LEFT))
         {
            rarch_settings_change(S_OVERSCAN_DECREMENT);
            gfx_ctx_set_overscan();
         }
         if((input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            rarch_settings_change(S_OVERSCAN_INCREMENT);
            gfx_ctx_set_overscan();
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
            rarch_settings_default(S_DEF_OVERSCAN);
            gfx_ctx_set_overscan();
         }
         break;
      case SETTING_THROTTLE_MODE:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            rarch_settings_change(S_THROTTLE);
            device_ptr->ctx_driver->swap_interval(g_extern.console.screen.state.throttle.enable);
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
            rarch_settings_default(S_DEF_THROTTLE);
            device_ptr->ctx_driver->swap_interval(g_extern.console.screen.state.throttle.enable);
         }
         break;
      case SETTING_TRIPLE_BUFFERING:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            rarch_settings_change(S_TRIPLE_BUFFERING);
            video_ptr.restart();
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
            bool old_buffer_input = g_extern.console.screen.state.triple_buffering.enable;
            rarch_settings_default(S_DEF_TRIPLE_BUFFERING);

            if(!old_buffer_input)
               video_ptr.restart();
         }
         break;
      case SETTING_ENABLE_SCREENSHOTS:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            g_extern.console.screen.state.screenshots.enable = !g_extern.console.screen.state.screenshots.enable;
            device_ptr->ctx_driver->rmenu_screenshot_enable(g_extern.console.screen.state.screenshots.enable);
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
            g_extern.console.screen.state.screenshots.enable = true;
            device_ptr->ctx_driver->rmenu_screenshot_enable(g_extern.console.screen.state.screenshots.enable);
         }
         break;
#if defined(HAVE_CG) || defined(HAVE_HLSL) || defined(HAVE_GLSL)
      case SETTING_SAVE_SHADER_PRESET:
#ifdef HAVE_OSKUTIL
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            rmenu_state.osk_param = SHADER_PRESET_FILE;
            rmenu_state.osk_init = osk_callback_enter_filename_init;
         }
#endif
         break;
      case SETTING_APPLY_SHADER_PRESET_ON_STARTUP:
         break;
#endif
      case SETTING_DEFAULT_VIDEO_ALL:
         break;
      case SETTING_SOUND_MODE:
         if(input & (1ULL << RMENU_DEVICE_NAV_LEFT))
         {
            if(g_extern.console.sound.mode != SOUND_MODE_NORMAL)
               g_extern.console.sound.mode--;
         }
         if((input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            if(g_extern.console.sound.mode < (SOUND_MODE_LAST-1))
               g_extern.console.sound.mode++;
         }
         if((input & (1ULL << RMENU_DEVICE_NAV_UP)) || (input & (1ULL << RMENU_DEVICE_NAV_DOWN)))
         {
#ifdef HAVE_RSOUND
            if(g_extern.console.sound.mode != SOUND_MODE_RSOUND)
               rarch_console_rsound_stop();
            else
               rarch_console_rsound_start(g_settings.audio.device);
#endif
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
            g_extern.console.sound.mode = SOUND_MODE_NORMAL;
#ifdef HAVE_RSOUND
            rarch_console_rsound_stop();
#endif
         }
         break;
#ifdef HAVE_RSOUND
      case SETTING_RSOUND_SERVER_IP_ADDRESS:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
#ifdef HAVE_OSKUTIL
            rmenu_state.osk_init = osk_callback_enter_rsound_init;
#endif
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            strlcpy(g_settings.audio.device, "0.0.0.0", sizeof(g_settings.audio.device));
         break;
#endif
      case SETTING_DEFAULT_AUDIO_ALL:
         break;
      case SETTING_EMU_CURRENT_SAVE_STATE_SLOT:
         if(input & (1ULL << RMENU_DEVICE_NAV_LEFT))
            rarch_settings_change(S_SAVESTATE_DECREMENT);
         if((input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
            rarch_settings_change(S_SAVESTATE_INCREMENT);
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            rarch_settings_default(S_DEF_SAVE_STATE);
         break;
      case SETTING_EMU_SHOW_DEBUG_INFO_MSG:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
            g_extern.console.rmenu.state.msg_fps.enable = !g_extern.console.rmenu.state.msg_fps.enable;
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            g_extern.console.rmenu.state.msg_fps.enable = false;
         break;
      case SETTING_EMU_SHOW_INFO_MSG:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
            g_extern.console.rmenu.state.msg_info.enable = !g_extern.console.rmenu.state.msg_info.enable;
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            g_extern.console.rmenu.state.msg_info.enable = true;
         break;
      case SETTING_EMU_REWIND_ENABLED:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            rarch_settings_change(S_REWIND);

            if(g_extern.console.rmenu.state.msg_info.enable)
               rarch_settings_msg(S_MSG_RESTART_RARCH, S_DELAY_180);
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            g_settings.rewind_enable = false;
         break;
#ifdef HAVE_ZLIB
      case SETTING_ZIP_EXTRACT:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)))
         {
            if(g_extern.file_state.zip_extract_mode > 0)
               g_extern.file_state.zip_extract_mode--;
         }
         if((input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            if(g_extern.file_state.zip_extract_mode < ZIP_EXTRACT_TO_CACHE_DIR)
               g_extern.file_state.zip_extract_mode++;
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            g_extern.file_state.zip_extract_mode = ZIP_EXTRACT_TO_CURRENT_DIR;
         break;
#endif
      case SETTING_RARCH_DEFAULT_EMU:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            menu_stack_push(LIBRETRO_CHOICE);
            filebrowser_set_root_and_ext(filebrowser, EXT_EXECUTABLES, default_paths.core_dir);
            set_libretro_core_as_launch = false;
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
         }
         break;
      case SETTING_QUIT_RARCH:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            rarch_settings_change(S_QUIT_RARCH);
         }
         break;
      case SETTING_EMU_AUDIO_MUTE:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
            rarch_settings_change(S_AUDIO_MUTE);

         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            rarch_settings_default(S_DEF_AUDIO_MUTE);
         break;
#ifdef _XBOX1
      case SETTING_EMU_AUDIO_SOUND_VOLUME_LEVEL:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            g_extern.console.sound.volume_level = !g_extern.console.sound.volume_level;
            if(g_extern.console.rmenu.state.msg_info.enable)
               rarch_settings_msg(S_MSG_RESTART_RARCH, S_DELAY_180);
         }

         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
            g_extern.console.sound.volume_level = 0;
            if(g_extern.console.rmenu.state.msg_info.enable)
               rarch_settings_msg(S_MSG_RESTART_RARCH, S_DELAY_180);
         }
         break;
#endif
      case SETTING_ENABLE_CUSTOM_BGM:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
#if(CELL_SDK_VERSION > 0x340000)
            g_extern.console.sound.custom_bgm.enable = !g_extern.console.sound.custom_bgm.enable;
            if(g_extern.console.sound.custom_bgm.enable)
               cellSysutilEnableBgmPlayback();
            else
               cellSysutilDisableBgmPlayback();

#endif
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
#if(CELL_SDK_VERSION > 0x340000)
            g_extern.console.sound.custom_bgm.enable = true;
#endif
         }
         break;
      case SETTING_EMU_VIDEO_DEFAULT_ALL:
         break;
      case SETTING_EMU_AUDIO_DEFAULT_ALL:
         break;
      case SETTING_PATH_DEFAULT_ROM_DIRECTORY:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            menu_stack_push(PATH_DEFAULT_ROM_DIR_CHOICE);
            filebrowser_set_root_and_ext(filebrowser, "empty", default_paths.filesystem_root_dir);
         }

         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            strlcpy(g_extern.console.main_wrap.paths.default_rom_startup_dir, default_paths.filesystem_root_dir, sizeof(g_extern.console.main_wrap.paths.default_rom_startup_dir));
         break;
      case SETTING_PATH_SAVESTATES_DIRECTORY:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            menu_stack_push(PATH_SAVESTATES_DIR_CHOICE);
            filebrowser_set_root_and_ext(filebrowser, "empty", default_paths.filesystem_root_dir);
         }

         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            strlcpy(g_extern.console.main_wrap.paths.default_savestate_dir, default_paths.savestate_dir, sizeof(g_extern.console.main_wrap.paths.default_savestate_dir));

         break;
      case SETTING_PATH_SRAM_DIRECTORY:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            menu_stack_push(PATH_SRAM_DIR_CHOICE);
            filebrowser_set_root_and_ext(filebrowser, "empty", default_paths.filesystem_root_dir);
         }

         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            strlcpy(g_extern.console.main_wrap.paths.default_sram_dir, default_paths.sram_dir, sizeof(g_extern.console.main_wrap.paths.default_sram_dir));
         break;
#ifdef HAVE_XML
      case SETTING_PATH_CHEATS:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            menu_stack_push(PATH_CHEATS_DIR_CHOICE);
            filebrowser_set_root_and_ext(filebrowser, "empty", default_paths.filesystem_root_dir);
         }

         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            strlcpy(g_settings.cheat_database, default_paths.port_dir, sizeof(g_settings.cheat_database));
         break;
#endif
      case SETTING_PATH_SYSTEM:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            menu_stack_push(PATH_SYSTEM_DIR_CHOICE);
            filebrowser_set_root_and_ext(filebrowser, "empty", default_paths.system_dir);
         }

         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            strlcpy(g_settings.system_directory, default_paths.system_dir, sizeof(g_settings.system_directory));
         break;
      case SETTING_ENABLE_SRAM_PATH:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)))
            g_extern.console.main_wrap.state.default_sram_dir.enable = !g_extern.console.main_wrap.state.default_sram_dir.enable;
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            g_extern.console.main_wrap.state.default_sram_dir.enable = true;
         break;
      case SETTING_ENABLE_STATE_PATH:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)))
            g_extern.console.main_wrap.state.default_savestate_dir.enable = !g_extern.console.main_wrap.state.default_savestate_dir.enable;
         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            g_extern.console.main_wrap.state.default_savestate_dir.enable = true;
         break;
      case SETTING_PATH_DEFAULT_ALL:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)) || (input & (1ULL << RMENU_DEVICE_NAV_START)))
         {
            strlcpy(g_extern.console.main_wrap.paths.default_rom_startup_dir, "/", sizeof(g_extern.console.main_wrap.paths.default_rom_startup_dir));
            strlcpy(g_extern.console.main_wrap.paths.default_savestate_dir, default_paths.port_dir, sizeof(g_extern.console.main_wrap.paths.default_savestate_dir));
#ifdef HAVE_XML
            strlcpy(g_settings.cheat_database, default_paths.port_dir, sizeof(g_settings.cheat_database));
#endif
            strlcpy(g_extern.console.main_wrap.paths.default_sram_dir, "", sizeof(g_extern.console.main_wrap.paths.default_sram_dir));
         }
         break;
      case SETTING_CONTROLS_SCHEME:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)) || (input & (1ULL << RMENU_DEVICE_NAV_START)))
         {
            menu_stack_push(INPUT_PRESET_CHOICE);
            filebrowser_set_root_and_ext(filebrowser, EXT_INPUT_PRESETS, default_paths.input_presets_dir);
         }
         break;
      case SETTING_CONTROLS_NUMBER:
         if(input & (1ULL << RMENU_DEVICE_NAV_LEFT))
         {
            if(currently_selected_controller_menu != 0)
               currently_selected_controller_menu--;
         }

         if((input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            if (currently_selected_controller_menu < 6)
               currently_selected_controller_menu++;
         }

         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            currently_selected_controller_menu = 0;
         break; 
      case SETTING_DPAD_EMULATION:
         if(input & (1ULL << RMENU_DEVICE_NAV_LEFT))
         {
            switch(g_settings.input.dpad_emulation[currently_selected_controller_menu])
            {
               case DPAD_EMULATION_NONE:
                  break;
               case DPAD_EMULATION_LSTICK:
                  input_ptr.set_analog_dpad_mapping(0, DPAD_EMULATION_NONE, currently_selected_controller_menu);
                  break;
               case DPAD_EMULATION_RSTICK:
                  input_ptr.set_analog_dpad_mapping(0, DPAD_EMULATION_LSTICK, currently_selected_controller_menu);
                  break;
            }
         }

         if((input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            switch(g_settings.input.dpad_emulation[currently_selected_controller_menu])
            {
               case DPAD_EMULATION_NONE:
                  input_ptr.set_analog_dpad_mapping(0, DPAD_EMULATION_LSTICK, currently_selected_controller_menu);
                  break;
               case DPAD_EMULATION_LSTICK:
                  input_ptr.set_analog_dpad_mapping(0, DPAD_EMULATION_RSTICK, currently_selected_controller_menu);
                  break;
               case DPAD_EMULATION_RSTICK:
                  break;
            }
         }

         if(input & (1ULL << RMENU_DEVICE_NAV_START))
            input_ptr.set_analog_dpad_mapping(0, DPAD_EMULATION_LSTICK, currently_selected_controller_menu);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_UP:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_UP, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_DOWN:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_DOWN, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_LEFT:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_LEFT, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_RIGHT:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_RIGHT, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_A:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_A, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_B:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_B, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_X:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_X, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_Y:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_Y, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_SELECT:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_SELECT, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_START:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_START, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_L:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_L, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_R:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_R, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_L2:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_L2, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_R2:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_R2, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_L3:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_L3, input);
         break;
      case SETTING_CONTROLS_RETRO_DEVICE_ID_JOYPAD_R3:
         set_keybind_digital(RETRO_DEVICE_ID_JOYPAD_R3, input);
         break;
#ifdef HAVE_OSKUTIL
      case SETTING_CONTROLS_SAVE_CUSTOM_CONTROLS:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)) || (input & (1ULL << RMENU_DEVICE_NAV_START)))
         {
            rmenu_state_t *rstate = (rmenu_state_t*)&rmenu_state;
            rstate->osk_param = INPUT_PRESET_FILE;
            rstate->osk_init = osk_callback_enter_filename_init;
         }
         break;
#endif
      case SETTING_CONTROLS_DEFAULT_ALL:
         if((input & (1ULL << RMENU_DEVICE_NAV_LEFT)) || (input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)) || (input & (1ULL << RMENU_DEVICE_NAV_START)))
            rarch_input_set_default_keybinds(currently_selected_controller_menu);
         break;
   }
}

static int select_setting(void *data, void *state)
{
   menu *current_menu = (menu*)data;
   rmenu_state_t *rstate = (rmenu_state_t*)state;

   uint64_t input = rstate->input;

   DEVICE_CAST device_ptr = (DEVICE_CAST)driver.video_data;
   item *items = (item*)malloc(current_menu->max_settings * sizeof(*items));
   unsigned i;
   char msg[256];

   filebrowser_t *filebrowser = NULL;
   rmenu_default_positions_t default_pos;

   device_ptr->ctx_driver->rmenu_set_default_pos(&default_pos);

   unsigned j = 0;
   int page = 0;
   for(i = current_menu->first_setting; i < current_menu->max_settings; i++)
   {
      populate_setting_item(&items[i], i);

      if(!(j < default_pos.entries_per_page))
      {
         j = 0;
         page++;
      }

      items[i].page = page;
      j++;
   }

   /* back to ROM menu if CIRCLE is pressed */
   if ((input & (1ULL << RMENU_DEVICE_NAV_L1)) || (input & (1ULL << RMENU_DEVICE_NAV_A)))
      menu_stack_pop();
   else if (input & (1ULL << RMENU_DEVICE_NAV_R1))
   {
      switch(current_menu->enum_id)
      {
         case GENERAL_VIDEO_MENU:
         case GENERAL_AUDIO_MENU:
         case EMU_GENERAL_MENU:
         case EMU_VIDEO_MENU:
         case EMU_AUDIO_MENU:
         case PATH_MENU:
            menu_stack_push(current_menu->enum_id + 1);
            break;
         case CONTROLS_MENU:
         default:
            break;
      }
   }
   else if (input & (1ULL << RMENU_DEVICE_NAV_DOWN))
   {
      current_menu->selected++;

      if (current_menu->selected >= current_menu->max_settings)
         current_menu->selected = current_menu->first_setting; 
      if (items[current_menu->selected].page != current_menu->page)
         current_menu->page = items[current_menu->selected].page;
   }
   else if (input & (1ULL << RMENU_DEVICE_NAV_UP))
   {
      if (current_menu->selected == current_menu->first_setting)
         current_menu->selected = current_menu->max_settings-1;
      else
         current_menu->selected--;

      if (items[current_menu->selected].page != current_menu->page)
         current_menu->page = items[current_menu->selected].page;
   }

   set_setting_action(current_menu, current_menu->selected, input);

   display_menubar(current_menu);


   for(i = current_menu->first_setting; i < current_menu->max_settings; i++)
   {
      if(items[i].page == current_menu->page)
      {
         default_pos.starting_y_position += default_pos.y_position_increment;
         device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.starting_y_position, default_pos.variable_font_size, current_menu->selected == items[i].enum_id ? YELLOW : WHITE, items[i].text);
         device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position_center, default_pos.starting_y_position, default_pos.variable_font_size, WHITE, items[i].setting_text);

         if(current_menu->selected == items[i].enum_id)
         {
            rarch_position_t position = {0};
            position.x = default_pos.x_position;
            position.y = default_pos.starting_y_position;

            device_ptr->ctx_driver->rmenu_draw_panel(&position);
            device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.comment_y_position, default_pos.font_size, WHITE, items[i].comment);
         }
      }
   }

   free(items);

   snprintf(msg, sizeof(msg), "[%s] + [%s] - Resume game", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_L3), rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_R3));
   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.comment_two_y_position, default_pos.font_size, YELLOW, msg);
   snprintf(msg, sizeof(msg), "[%s] - Reset to default", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_START));
   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.comment_two_y_position + (default_pos.y_position_increment * 1), default_pos.font_size, YELLOW, msg);

   if(current_menu->browser_draw)
      current_menu->browser_draw(filebrowser);

   return 1;
}

int select_rom(void *data, void *state)
{
   menu *current_menu = (menu*)data;
   rmenu_state_t *rstate = (rmenu_state_t*)state;

   uint64_t input = rstate->input;

   DEVICE_CAST device_ptr = (DEVICE_CAST)driver.video_data;
   char msg[128];
   rmenu_default_positions_t default_pos;
   filebrowser_t *filebrowser = &browser;

   device_ptr->ctx_driver->rmenu_set_default_pos(&default_pos);

   browser_update(filebrowser, input, rarch_console_get_rom_ext());

   if (input & (1ULL << RMENU_DEVICE_NAV_SELECT))
      menu_stack_push(GENERAL_VIDEO_MENU);
   else if (input & (1ULL << RMENU_DEVICE_NAV_B))
   {
      if(filebrowser_get_current_path_isdir(filebrowser))
      {
         bool ret = filebrowser_iterate(filebrowser, FILEBROWSER_ACTION_OK);

         if(!ret)
            rarch_settings_msg(S_MSG_DIR_LOADING_ERROR, S_DELAY_180);
      }
      else
         rarch_console_load_game_wrap(filebrowser_get_current_path(filebrowser), g_extern.file_state.zip_extract_mode, S_DELAY_45);
   }
   else if (input & (1ULL << RMENU_DEVICE_NAV_L1))
   {
      const char * drive_map = device_ptr->ctx_driver->drive_mapping_previous();
      if(drive_map != NULL)
      {
         filebrowser_set_root_and_ext(filebrowser, rarch_console_get_rom_ext(), drive_map);
         browser_update(filebrowser, 1ULL << RMENU_DEVICE_NAV_B, rarch_console_get_rom_ext());
      }
   }
   else if (input & (1ULL << RMENU_DEVICE_NAV_R1))
   {
      const char * drive_map = device_ptr->ctx_driver->drive_mapping_next();
      if(drive_map != NULL)
      {
         filebrowser_set_root_and_ext(filebrowser, rarch_console_get_rom_ext(), drive_map);
         browser_update(filebrowser, 1ULL << RMENU_DEVICE_NAV_B, rarch_console_get_rom_ext());
      }
   }

   bool is_dir = filebrowser_get_current_path_isdir(filebrowser);

   if (is_dir)
   {
      const char *current_path = filebrowser_get_current_path(filebrowser);
      snprintf(msg, sizeof(msg), "INFO - Press [%s] to enter the directory.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
   }
   else
      snprintf(msg, sizeof(msg), "INFO - Press [%s] to load the game.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));

   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.comment_y_position, default_pos.font_size, WHITE, msg);

   display_menubar(current_menu);

   snprintf(msg, sizeof(msg), "[%s] + [%s] - resume game", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_L3), rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_R3));
   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.comment_two_y_position, default_pos.font_size, YELLOW, msg);

   snprintf(msg, sizeof(msg), "[%s] - Settings", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_SELECT));
   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.comment_two_y_position + (default_pos.y_position_increment * 1), default_pos.font_size, YELLOW, msg);

   if(current_menu->browser_draw)
      current_menu->browser_draw(filebrowser);

   return 1;
}

int ingame_menu_resize(void *data, void *state)
{
   menu *current_menu = (menu*)data;
   rmenu_state_t *rstate = (rmenu_state_t*)state;

   uint64_t input = rstate->input;
   DEVICE_CAST device_ptr = (DEVICE_CAST)driver.video_data;

   filebrowser_t *filebrowser = NULL;
   rmenu_default_positions_t default_pos;
   device_ptr->ctx_driver->rmenu_set_default_pos(&default_pos);

   g_settings.video.aspect_ratio_idx = ASPECT_RATIO_CUSTOM;
   driver.video->set_aspect_ratio(driver.video_data, g_settings.video.aspect_ratio_idx);

   if(input & (1ULL << RMENU_DEVICE_NAV_LEFT_ANALOG_L))
   {
#ifdef _XBOX
      if(g_extern.console.screen.viewports.custom_vp.x >= 4)
#endif
         g_extern.console.screen.viewports.custom_vp.x -= 4;
   }
   else if(input & (1ULL << RMENU_DEVICE_NAV_LEFT) && (input & ~(1ULL << RMENU_DEVICE_NAV_LEFT_ANALOG_L)))
   {
#ifdef _XBOX
      if(g_extern.console.screen.viewports.custom_vp.x > 0)
#endif
         g_extern.console.screen.viewports.custom_vp.x -= 1;
   }

   if(input & (1ULL << RMENU_DEVICE_NAV_RIGHT_ANALOG_L))
      g_extern.console.screen.viewports.custom_vp.x += 4;
   else if(input & (1ULL << RMENU_DEVICE_NAV_RIGHT) && (input & ~(1ULL << RMENU_DEVICE_NAV_RIGHT_ANALOG_L)))
      g_extern.console.screen.viewports.custom_vp.x += 1;

   if(input & (1ULL << RMENU_DEVICE_NAV_UP_ANALOG_L))
      g_extern.console.screen.viewports.custom_vp.y += 4;
   else if(input & (1ULL << RMENU_DEVICE_NAV_UP) && (input & ~(1ULL << RMENU_DEVICE_NAV_UP_ANALOG_L)))
      g_extern.console.screen.viewports.custom_vp.y += 1;

   if(input & (1ULL << RMENU_DEVICE_NAV_DOWN_ANALOG_L))
   {
#ifdef _XBOX
      if(g_extern.console.screen.viewports.custom_vp.y >= 4)
#endif
         g_extern.console.screen.viewports.custom_vp.y -= 4;
   }
   else if(input & (1ULL << RMENU_DEVICE_NAV_DOWN) && (input & ~(1ULL << RMENU_DEVICE_NAV_DOWN_ANALOG_L)))
   {
#ifdef _XBOX
      if(g_extern.console.screen.viewports.custom_vp.y > 0)
#endif
         g_extern.console.screen.viewports.custom_vp.y -= 1;
   }

   if(input & (1ULL << RMENU_DEVICE_NAV_LEFT_ANALOG_R))
      g_extern.console.screen.viewports.custom_vp.width -= 4;
   else if(input & (1ULL << RMENU_DEVICE_NAV_L1) && (input && ~(1ULL << RMENU_DEVICE_NAV_LEFT_ANALOG_R)))
      g_extern.console.screen.viewports.custom_vp.width -= 1;

   if (input & (1ULL << RMENU_DEVICE_NAV_RIGHT_ANALOG_R))
      g_extern.console.screen.viewports.custom_vp.width += 4;
   else if(input & (1ULL << RMENU_DEVICE_NAV_R1) && (input & ~(1ULL << RMENU_DEVICE_NAV_RIGHT_ANALOG_R)))
      g_extern.console.screen.viewports.custom_vp.width += 1;

   if(input & (1ULL << RMENU_DEVICE_NAV_UP_ANALOG_R))
      g_extern.console.screen.viewports.custom_vp.height += 4;
   else if(input & (1ULL << RMENU_DEVICE_NAV_L2) && (input & ~(1ULL << RMENU_DEVICE_NAV_UP_ANALOG_R)))
      g_extern.console.screen.viewports.custom_vp.height += 1;

   if(input & (1ULL << RMENU_DEVICE_NAV_DOWN_ANALOG_R))
      g_extern.console.screen.viewports.custom_vp.height -= 4;
   else if (input & (1ULL << RMENU_DEVICE_NAV_R2) && (input & ~(1ULL << RMENU_DEVICE_NAV_DOWN_ANALOG_R)))
      g_extern.console.screen.viewports.custom_vp.height -= 1;

   if (input & (1ULL << RMENU_DEVICE_NAV_X))
   {
      g_extern.console.screen.viewports.custom_vp.x = 0;
      g_extern.console.screen.viewports.custom_vp.y = 0;
      g_extern.console.screen.viewports.custom_vp.width = device_ptr->win_width;
      g_extern.console.screen.viewports.custom_vp.height = device_ptr->win_height;
   }

   if (input & (1ULL << RMENU_DEVICE_NAV_A))
   {
      menu_stack_pop();
      g_extern.draw_menu = true;
   }

   if((input & (1ULL << RMENU_DEVICE_NAV_Y)))
      g_extern.draw_menu = !g_extern.draw_menu;

   if(g_extern.draw_menu)
   {
      char viewport_x[32], viewport_y[32], viewport_w[32], viewport_h[32];
      char msg[256];
      display_menubar(current_menu);

      snprintf(viewport_x, sizeof(viewport_x), "Viewport X: #%d", g_extern.console.screen.viewports.custom_vp.x);
      snprintf(viewport_y, sizeof(viewport_y), "Viewport Y: #%d", g_extern.console.screen.viewports.custom_vp.y);
      snprintf(viewport_w, sizeof(viewport_w), "Viewport W: #%d", g_extern.console.screen.viewports.custom_vp.width);
      snprintf(viewport_h, sizeof(viewport_h), "Viewport H: #%d", g_extern.console.screen.viewports.custom_vp.height);

      device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.y_position, default_pos.font_size, GREEN, viewport_x);
      device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*1), default_pos.font_size, GREEN, viewport_y);
      device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*2), default_pos.font_size, GREEN, viewport_w);
      device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*3), default_pos.font_size, GREEN, viewport_h);

      device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*4), default_pos.font_size, WHITE, "CONTROLS:");

      snprintf(msg, sizeof(msg), "[%s] or [%s]", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_LEFT), rarch_input_find_platform_key_label(1ULL << RARCH_ANALOG_LEFT_X_DPAD_LEFT));
      device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*5), default_pos.font_size,  WHITE, msg);
      device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position_center, default_pos.y_position+(default_pos.y_position_increment*5), default_pos.font_size, WHITE, "- Viewport X --");

      snprintf(msg, sizeof(msg), "[%s] or [%s]", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_RIGHT), rarch_input_find_platform_key_label(1ULL << RARCH_ANALOG_LEFT_X_DPAD_RIGHT));
      device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*6), default_pos.font_size, WHITE, msg);
      device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position_center, default_pos.y_position+(default_pos.y_position_increment*6), default_pos.font_size, WHITE, "- Viewport X ++");

      snprintf(msg, sizeof(msg), "[%s] or [%s]", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_UP), rarch_input_find_platform_key_label(1ULL << RARCH_ANALOG_LEFT_Y_DPAD_UP));
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*7), default_pos.font_size, WHITE, msg);
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position_center, default_pos.y_position+(default_pos.y_position_increment*7), default_pos.font_size, WHITE, "- Viewport Y ++");

      snprintf(msg, sizeof(msg), "[%s] or [%s]", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_DOWN), rarch_input_find_platform_key_label(1ULL << RARCH_ANALOG_LEFT_Y_DPAD_DOWN));
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*8), default_pos.font_size, WHITE, msg);
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position_center, default_pos.y_position+(default_pos.y_position_increment*8), default_pos.font_size, WHITE, "- Viewport Y --");

      snprintf(msg, sizeof(msg), "[%s] or [%s]", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_L), rarch_input_find_platform_key_label(1ULL << RARCH_ANALOG_RIGHT_X_DPAD_LEFT));
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*9), default_pos.font_size, WHITE, msg);
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position_center, default_pos.y_position+(default_pos.y_position_increment*9), default_pos.font_size, WHITE, "- Viewport W --");

      snprintf(msg, sizeof(msg), "[%s] or [%s]", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_R), rarch_input_find_platform_key_label(1ULL << RARCH_ANALOG_RIGHT_X_DPAD_RIGHT));
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*10), default_pos.font_size, WHITE, msg);
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position_center, default_pos.y_position+(default_pos.y_position_increment*10), default_pos.font_size, WHITE, "- Viewport W ++");

      snprintf(msg, sizeof(msg), "[%s] or [%s]", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_L2), rarch_input_find_platform_key_label(1ULL << RARCH_ANALOG_RIGHT_Y_DPAD_UP));
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*11), default_pos.font_size, WHITE, msg);
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position_center, default_pos.y_position+(default_pos.y_position_increment*11), default_pos.font_size, WHITE, "- Viewport H ++");


      snprintf(msg, sizeof(msg), "[%s] or [%s]", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_R2), rarch_input_find_platform_key_label(1ULL << RARCH_ANALOG_RIGHT_Y_DPAD_DOWN));
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*12), default_pos.font_size, WHITE, msg);
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position_center, default_pos.y_position+(default_pos.y_position_increment*12), default_pos.font_size, WHITE, "- Viewport H --");

      snprintf(msg, sizeof(msg), "[%s]", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_X));
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*13), default_pos.font_size, WHITE, msg);
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position_center, default_pos.y_position+(default_pos.y_position_increment*13), default_pos.font_size, WHITE, "- Reset To Defaults");

      snprintf(msg, sizeof(msg), "[%s]", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_Y));
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*14), default_pos.font_size, WHITE, msg);
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position_center, default_pos.y_position+(default_pos.y_position_increment*14), default_pos.font_size, WHITE, "- Show Game");

      snprintf(msg, sizeof(msg), "[%s]", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_A));
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*15), default_pos.font_size, WHITE, msg);
      device_ptr->font_ctx->render_msg_place (device_ptr, default_pos.x_position_center, default_pos.y_position+(default_pos.y_position_increment*15), default_pos.font_size, WHITE, "- Go back");

      snprintf(msg, sizeof(msg), "Press [%s] to reset to defaults.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_X));
      device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.comment_y_position, default_pos.font_size, WHITE, msg);
   }

   if(current_menu->browser_draw)
      current_menu->browser_draw(filebrowser);

   return 1;
}

int ingame_menu_screenshot(void *data, void *state)
{
   menu *current_menu = (menu*)data;
   rmenu_state_t *rstate = (rmenu_state_t*)state;

   uint64_t input = rstate->input;
   
   DEVICE_CAST device_ptr = (DEVICE_CAST)driver.video_data;
   filebrowser_t *filebrowser = NULL;

   g_extern.draw_menu = false;

   if(g_extern.console.rmenu.state.ingame_menu.enable)
   {
      if(input & (1ULL << RMENU_DEVICE_NAV_A))
      {
         menu_stack_pop();
         g_extern.draw_menu = true;
      }

      if(input & (1ULL << RMENU_DEVICE_NAV_B))
         if(device_ptr->ctx_driver->rmenu_screenshot_dump)
            device_ptr->ctx_driver->rmenu_screenshot_dump(NULL);
   }

   if(current_menu->browser_draw)
      current_menu->browser_draw(filebrowser);

   return 1;
}

#define MENU_ITEM_SELECTED(index) (menuitem_colors[index])

int ingame_menu(void *data, void *state)
{
   //if(!g_extern.console.rmenu.state.ingame_menu.enable)
      //return false;

   menu *current_menu    = (menu*)data;
   rmenu_state_t *rstate = (rmenu_state_t*)state;

   uint64_t input = rstate->input;
   DEVICE_CAST device_ptr = (DEVICE_CAST)driver.video_data;
   char strw_buffer[256];
   unsigned menuitem_colors[MENU_ITEM_LAST];

   filebrowser_t *filebrowser = &tmpBrowser;
   rmenu_default_positions_t default_pos;
   device_ptr->ctx_driver->rmenu_set_default_pos(&default_pos);

   for(int i = 0; i < MENU_ITEM_LAST; i++)
      menuitem_colors[i] = WHITE;

   menuitem_colors[g_extern.console.rmenu.ingame_menu.idx] = RED;

   if(input & (1ULL << RMENU_DEVICE_NAV_A))
      rarch_settings_change(S_RETURN_TO_GAME);

   switch(g_extern.console.rmenu.ingame_menu.idx)
   {
      case MENU_ITEM_LOAD_STATE:
         if(input & (1ULL << RMENU_DEVICE_NAV_B))
         {
            rarch_load_state();
            rarch_settings_change(S_RETURN_TO_GAME);
         }
         if(input & (1ULL << RMENU_DEVICE_NAV_LEFT))
            rarch_state_slot_decrease();
         if(input & (1ULL << RMENU_DEVICE_NAV_RIGHT))
            rarch_state_slot_increase();

         snprintf(strw_buffer, sizeof(strw_buffer), "Press [%s] to load the current state.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
         break;
      case MENU_ITEM_SAVE_STATE:
         if(input & (1ULL << RMENU_DEVICE_NAV_B))
         {
            rarch_save_state();
            rarch_settings_change(S_RETURN_TO_GAME);
         }

         if(input & (1ULL << RMENU_DEVICE_NAV_LEFT))
            rarch_state_slot_decrease();
         if(input & (1ULL << RMENU_DEVICE_NAV_RIGHT))
            rarch_state_slot_increase();

         snprintf(strw_buffer, sizeof(strw_buffer), "Press [%s] to save the current state.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
         break;
      case MENU_ITEM_KEEP_ASPECT_RATIO:
         set_setting_action(current_menu, SETTING_KEEP_ASPECT_RATIO, input);
         snprintf(strw_buffer, sizeof(strw_buffer), "Press [%s] to reset back to default values.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_START));
         break;
      case MENU_ITEM_OVERSCAN_AMOUNT:
         set_setting_action(current_menu, SETTING_HW_OVERSCAN_AMOUNT, input);
         snprintf(strw_buffer, sizeof(strw_buffer), "Press [%s] to reset back to default values.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_START));
         break;
      case MENU_ITEM_ORIENTATION:
         if(input & (1ULL << RMENU_DEVICE_NAV_LEFT))
         {
            rarch_settings_change(S_ROTATION_DECREMENT);
            video_ptr.set_rotation(NULL, g_extern.console.screen.orientation);
         }

         if((input & (1ULL << RMENU_DEVICE_NAV_RIGHT)) || (input & (1ULL << RMENU_DEVICE_NAV_B)))
         {
            rarch_settings_change(S_ROTATION_INCREMENT);
            video_ptr.set_rotation(NULL, g_extern.console.screen.orientation);
         }

         if(input & (1ULL << RMENU_DEVICE_NAV_START))
         {
            rarch_settings_default(S_DEF_ROTATION);
            video_ptr.set_rotation(NULL, g_extern.console.screen.orientation);
         }
         snprintf(strw_buffer, sizeof(strw_buffer), "Press [%s] to reset back to default values.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_START));
         break;
#ifdef HAVE_FBO
      case MENU_ITEM_SCALE_FACTOR:
         set_setting_action(current_menu, SETTING_SCALE_FACTOR, input);
         snprintf(strw_buffer, sizeof(strw_buffer), "Press [%s] to reset back to default values.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_START));
         break;
#endif
      case MENU_ITEM_FRAME_ADVANCE:
         if((input & (1ULL << RMENU_DEVICE_NAV_B)) || (input & (1ULL << RMENU_DEVICE_NAV_R2)) || (input & (1ULL << RMENU_DEVICE_NAV_L2)))
         {
            g_extern.lifecycle_state |= (1ULL << RARCH_FRAMEADVANCE);
            rarch_settings_change(S_FRAME_ADVANCE);
            g_extern.console.rmenu.ingame_menu.idx = MENU_ITEM_FRAME_ADVANCE;
         }
         snprintf(strw_buffer, sizeof(strw_buffer), "Press [%s] to step one frame.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
         break;
      case MENU_ITEM_RESIZE_MODE:
         if(input & (1ULL << RMENU_DEVICE_NAV_B))
            menu_stack_push(INGAME_MENU_RESIZE);
         snprintf(strw_buffer, sizeof(strw_buffer), "Allows you to resize the screen.");
         break;
      case MENU_ITEM_SCREENSHOT_MODE:
         if(input & (1ULL << RMENU_DEVICE_NAV_B))
            menu_stack_push(INGAME_MENU_SCREENSHOT);
         snprintf(strw_buffer, sizeof(strw_buffer), "Press [%s] to go back to the in-game menu.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_A));
         break;
      case MENU_ITEM_RETURN_TO_GAME:
         if(input & (1ULL << RMENU_DEVICE_NAV_B))
            rarch_settings_change(S_RETURN_TO_GAME);

         snprintf(strw_buffer, sizeof(strw_buffer), "Press [%s] to return to the game.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
         break;
      case MENU_ITEM_RESET:
         if(input & (1ULL << RMENU_DEVICE_NAV_B))
         {
            rarch_settings_change(S_RETURN_TO_GAME);
            rarch_game_reset();
         }
         snprintf(strw_buffer, sizeof(strw_buffer), "Press [%s] to reset the game.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
         break;
      case MENU_ITEM_RETURN_TO_MENU:
         if(input & (1ULL << RMENU_DEVICE_NAV_B))
         {
            rarch_settings_change(S_RETURN_TO_MENU);
         }
         snprintf(strw_buffer, sizeof(strw_buffer), "Press [%s] to return to the ROM Browser.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
         break;
      case MENU_ITEM_CHANGE_LIBRETRO:
         if(input & (1ULL << RMENU_DEVICE_NAV_B))
         {
            menu_stack_push(LIBRETRO_CHOICE);
            filebrowser_set_root_and_ext(filebrowser, EXT_EXECUTABLES, default_paths.core_dir);
            set_libretro_core_as_launch = true;
         }
         snprintf(strw_buffer, sizeof(strw_buffer), "Press [%s] to choose another core.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
         break;
#ifdef HAVE_MULTIMAN
      case MENU_ITEM_RETURN_TO_MULTIMAN:
         if(input & (1ULL << RMENU_DEVICE_NAV_B))
         {
            RARCH_LOG("Boot Multiman: %s.\n", default_paths.multiman_self_file);
            strlcpy(g_extern.console.external_launch.launch_app, default_paths.multiman_self_file,
                  sizeof(g_extern.console.external_launch.launch_app));
            rarch_settings_change(S_RETURN_TO_LAUNCHER);
         }
         snprintf(strw_buffer, sizeof(strw_buffer), "Press [%s] to quit RetroArch and return to multiMAN.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
         break;
#endif
      case MENU_ITEM_QUIT_RARCH:
         if(input & (1ULL << RMENU_DEVICE_NAV_B))
            rarch_settings_change(S_QUIT_RARCH);

         snprintf(strw_buffer, sizeof(strw_buffer), "Press [%s] to quit RetroArch.", rarch_input_find_platform_key_label(1ULL << RETRO_DEVICE_ID_JOYPAD_B));
         break;
   }

   if(input & (1ULL << RMENU_DEVICE_NAV_UP))
   {
      if(g_extern.console.rmenu.ingame_menu.idx > 0)
         g_extern.console.rmenu.ingame_menu.idx--;
      else
         g_extern.console.rmenu.ingame_menu.idx = MENU_ITEM_LAST - 1;
   }

   if(input & (1ULL << RMENU_DEVICE_NAV_DOWN))
   {
      if(g_extern.console.rmenu.ingame_menu.idx < (MENU_ITEM_LAST-1))
         g_extern.console.rmenu.ingame_menu.idx++;
      else
         g_extern.console.rmenu.ingame_menu.idx = 0;
   }

   if((input & (1ULL << RMENU_DEVICE_NAV_L3)) && (input & (1ULL << RMENU_DEVICE_NAV_R3)))
      rarch_settings_change(S_RETURN_TO_GAME);

   display_menubar(current_menu);

   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, default_pos.comment_y_position, default_pos.font_size, WHITE, strw_buffer);

   rarch_settings_create_menu_item_label(strw_buffer, S_LBL_LOAD_STATE_SLOT, sizeof(strw_buffer));
   device_ptr->font_ctx->render_msg_place(device_ptr, default_pos.x_position, default_pos.y_position, default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_LOAD_STATE), strw_buffer);

   rarch_settings_create_menu_item_label(strw_buffer, S_LBL_SAVE_STATE_SLOT, sizeof(strw_buffer));
   device_ptr->font_ctx->render_msg_place(device_ptr, default_pos.x_position, default_pos.y_position+(default_pos.y_position_increment*MENU_ITEM_SAVE_STATE), default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_SAVE_STATE), strw_buffer);

   rarch_settings_create_menu_item_label(strw_buffer, S_LBL_ASPECT_RATIO, sizeof(strw_buffer));
   device_ptr->font_ctx->render_msg_place(device_ptr, default_pos.x_position, (default_pos.y_position+(default_pos.y_position_increment*MENU_ITEM_KEEP_ASPECT_RATIO)), default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_KEEP_ASPECT_RATIO), strw_buffer);

   snprintf(strw_buffer, sizeof(strw_buffer), "Overscan: %f", g_extern.console.screen.overscan_amount);
   device_ptr->font_ctx->render_msg_place(device_ptr, default_pos.x_position, (default_pos.y_position+(default_pos.y_position_increment*MENU_ITEM_OVERSCAN_AMOUNT)), default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_OVERSCAN_AMOUNT), strw_buffer);

   rarch_settings_create_menu_item_label(strw_buffer, S_LBL_ROTATION, sizeof(strw_buffer));
   device_ptr->font_ctx->render_msg_place(device_ptr, default_pos.x_position, (default_pos.y_position+(default_pos.y_position_increment*MENU_ITEM_ORIENTATION)), default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_ORIENTATION), strw_buffer);

#ifdef HAVE_FBO
   rarch_settings_create_menu_item_label(strw_buffer, S_LBL_SCALE_FACTOR, sizeof(strw_buffer));
   device_ptr->font_ctx->render_msg_place(device_ptr, default_pos.x_position, (default_pos.y_position+(default_pos.y_position_increment*MENU_ITEM_SCALE_FACTOR)), default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_SCALE_FACTOR), strw_buffer);
#endif

   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, (default_pos.y_position+(default_pos.y_position_increment*MENU_ITEM_RESIZE_MODE)), default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_RESIZE_MODE), "Resize Mode");

   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, (default_pos.y_position+(default_pos.y_position_increment*MENU_ITEM_FRAME_ADVANCE)), default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_FRAME_ADVANCE), "Frame Advance");

   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, (default_pos.y_position+(default_pos.y_position_increment*MENU_ITEM_SCREENSHOT_MODE)), default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_SCREENSHOT_MODE), "Screenshot Mode");

   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, (default_pos.y_position+(default_pos.y_position_increment*MENU_ITEM_RESET)), default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_RESET), "Reset");

   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, (default_pos.y_position+(default_pos.y_position_increment*MENU_ITEM_RETURN_TO_GAME)), default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_RETURN_TO_GAME), "Return to Game");

   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, (default_pos.y_position+(default_pos.y_position_increment*MENU_ITEM_RETURN_TO_MENU)), default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_RETURN_TO_MENU), "Return to Menu");

   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, (default_pos.y_position+(default_pos.y_position_increment*MENU_ITEM_CHANGE_LIBRETRO)), default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_CHANGE_LIBRETRO), "Change libretro core");

#ifdef HAVE_MULTIMAN
   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, (default_pos.y_position+(default_pos.y_position_increment*MENU_ITEM_RETURN_TO_MULTIMAN)), default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_RETURN_TO_MULTIMAN), "Return to multiMAN");
#endif

   device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.x_position, (default_pos.y_position+(default_pos.y_position_increment*MENU_ITEM_QUIT_RARCH)), default_pos.font_size, MENU_ITEM_SELECTED(MENU_ITEM_QUIT_RARCH), "Quit RetroArch");

   rarch_position_t position = {0};
   position.x = default_pos.x_position;
   position.y = (default_pos.y_position+(default_pos.y_position_increment * g_extern.console.rmenu.ingame_menu.idx));
   device_ptr->ctx_driver->rmenu_draw_panel(&position);

   if(current_menu->browser_draw)
      current_menu->browser_draw(filebrowser);

   return 1;
}

/*============================================================
INPUT POLL CALLBACK
============================================================ */

void rmenu_input_poll(void *data, void *state)
{
   menu *current_menu    = (menu*)data;
   rmenu_state_t *rstate = (rmenu_state_t*)state;

   //first button input frame
   uint64_t input_state_first_frame = 0;
   uint64_t input_state = 0;
   static bool first_held = false;

   input_ptr.poll(NULL);

   for (unsigned i = 0; i < RMENU_DEVICE_NAV_LAST; i++)
      input_state |= input_ptr.input_state(NULL, rmenu_nav_binds, 0,
            RETRO_DEVICE_JOYPAD, 0, i) ? (1ULL << i) : 0;

   //set first button input frame as trigger
   rstate->input = input_state & ~(rstate->old_state);
   //hold onto first button input frame
   input_state_first_frame = input_state;          

   //second button input frame
   input_state = 0;
   input_ptr.poll(NULL);

   for (unsigned i = 0; i < RMENU_DEVICE_NAV_LAST; i++)
   {
      input_state |= input_ptr.input_state(NULL, rmenu_nav_binds, 0,
            RETRO_DEVICE_JOYPAD, 0, i) ? (1ULL << i) : 0;
   }

   bool analog_sticks_pressed = (input_state & (1ULL << RMENU_DEVICE_NAV_LEFT_ANALOG_L)) || (input_state & (1ULL << RMENU_DEVICE_NAV_RIGHT_ANALOG_L)) || (input_state & (1ULL << RMENU_DEVICE_NAV_UP_ANALOG_L)) || (input_state & (1ULL << RMENU_DEVICE_NAV_DOWN_ANALOG_L)) || (input_state & (1ULL << RMENU_DEVICE_NAV_LEFT_ANALOG_R)) || (input_state & (1ULL << RMENU_DEVICE_NAV_RIGHT_ANALOG_R)) || (input_state & (1ULL << RMENU_DEVICE_NAV_UP_ANALOG_R)) || (input_state & (1ULL << RMENU_DEVICE_NAV_DOWN_ANALOG_R));
   bool shoulder_buttons_pressed = ((input_state & (1ULL << RMENU_DEVICE_NAV_L2)) || (input_state & (1ULL << RMENU_DEVICE_NAV_R2))) && current_menu->category_id != CATEGORY_SETTINGS;
   bool do_held = analog_sticks_pressed || shoulder_buttons_pressed;

   if(do_held)
   {
      if(!first_held)
      {
         first_held = true;
         SET_TIMER_EXPIRATION(1, 7);
      }

      if(IS_TIMER_EXPIRED(1))
      {
         first_held = false;
         rstate->input = input_state; //second input frame set as current frame
      }
   }

   rstate->old_state = input_state_first_frame;
}

/*============================================================
INPUT PROCESS CALLBACK
============================================================ */

void rmenu_input_process(void *data, void *state)
{
   (void)data;
   rmenu_state_t *rstate = (rmenu_state_t*)state;

   if(IS_TIMER_EXPIRED(0))
   {
      // if we want to force goto the emulation loop, skip this
      if(g_extern.console.rmenu.mode != MODE_EMULATION)
      {
         if(g_extern.console.rmenu.mode == MODE_EXIT)
         {
         }
         // for ingame menu, we need a different precondition because menu_enable
         // can be set to false when going back from ingame menu to menu
         else if(g_extern.console.rmenu.state.ingame_menu.enable == true)
         {
            //we want to force exit when g_extern.console.mode is set to MODE_EXIT
            if(g_extern.console.rmenu.mode != MODE_EXIT)
               g_extern.console.rmenu.mode = (((rstate->old_state & (1ULL << RMENU_DEVICE_NAV_L3)) && (rstate->old_state & (1ULL << RMENU_DEVICE_NAV_R3)) && g_extern.console.emulator_initialized)) ? MODE_EMULATION : MODE_MENU;
         }
         else
         {
            bool rmenu_enable = !(((rstate->old_state & (1ULL << RMENU_DEVICE_NAV_L3)) && (rstate->old_state & (1ULL << RMENU_DEVICE_NAV_R3)) && g_extern.console.emulator_initialized));
            g_extern.console.rmenu.mode = rmenu_enable ? MODE_MENU : MODE_EMULATION;
         }
      }
   }
}

/*============================================================
RESOURCE CALLBACKS
============================================================ */

void init_filebrowser(void *data)
{
   (void)data;

   menu_stack_push(FILE_BROWSER_MENU);
   filebrowser_set_root_and_ext(&browser, rarch_console_get_rom_ext(), default_paths.filebrowser_startup_dir);
   filebrowser_set_root(&tmpBrowser, default_paths.filesystem_root_dir);
}

void free_filebrowser(void *data)
{
   (void)data;

   filebrowser_free(&browser);
   filebrowser_free(&tmpBrowser);
}

/*============================================================
RMENU API
============================================================ */


void menu_init(void)
{
   DEVICE_CAST device_ptr = (DEVICE_CAST)driver.video_data;

   //Set libretro filename and version to variable
   struct retro_system_info info;
   retro_get_system_info(&info);
   const char *id = info.library_name ? info.library_name : "Unknown";
   snprintf(m_title, sizeof(m_title), "%s %s", id, info.library_version);

   rmenu_state.init_resources = init_filebrowser;
   rmenu_state.free_resources = free_filebrowser;
   rmenu_state.input          = 0;
   rmenu_state.old_state      = 0;

   if(rmenu_state.init_resources)
      rmenu_state.init_resources(&rmenu_state);

   device_ptr->ctx_driver->rmenu_init();
}

void menu_free(void)
{
   if(rmenu_state.free_resources)
      rmenu_state.free_resources(&rmenu_state);
}

bool rmenu_iterate(void)
{
   static bool preinit = true;
   const char *msg;

   DEVICE_CAST device_ptr = (DEVICE_CAST)driver.video_data;
   menu current_menu;

   if(preinit)
   {
      if(g_extern.console.rmenu.state.ingame_menu.enable)
         menu_stack_push(INGAME_MENU);

      menu_stack_force_refresh();
      g_extern.draw_menu = true;

      device_ptr->ctx_driver->rmenu_init();

      preinit = false;
   }

   g_extern.frame_count++;

   menu_stack_get_current_ptr(&current_menu);

   rmenu_default_positions_t default_pos;
   device_ptr->ctx_driver->rmenu_set_default_pos(&default_pos);

   if (g_extern.draw_menu)
      device_ptr->ctx_driver->set_blend(true);

   rarch_render_cached_frame();

   if(current_menu.input_poll)
      current_menu.input_poll(&current_menu, &rmenu_state);

#ifdef HAVE_OSKUTIL
   bool osk_init_succeeded = false;
   bool osk_callback_finished = false;
   if(rmenu_state.osk_init)
      osk_init_succeeded = rmenu_state.osk_init(&rmenu_state);

   if(osk_init_succeeded && rmenu_state.osk_unbind_after_finish)
      rmenu_state.osk_init = NULL;

   if(rmenu_state.osk_callback)
      osk_callback_finished = rmenu_state.osk_callback(&rmenu_state);

   if(osk_callback_finished && rmenu_state.osk_unbind_after_finish)
      rmenu_state.osk_callback = NULL;
#endif

   int repeat = true;

   if(current_menu.entry)
      repeat = current_menu.entry(&current_menu, &rmenu_state);

   if(current_menu.input_process)
      current_menu.input_process(&current_menu, &rmenu_state);

   if(g_extern.console.rmenu.mode == MODE_EMULATION || g_extern.console.rmenu.mode == MODE_EXIT || repeat == 0)
      goto deinit;

   msg = msg_queue_pull(g_extern.msg_queue);

   if (msg && g_extern.console.rmenu.state.msg_info.enable)
   {
      device_ptr->font_ctx->render_msg_place(device_ptr,default_pos.msg_queue_x_position, default_pos.msg_queue_y_position, default_pos.msg_queue_font_size, WHITE, msg);
   }

   device_ptr->ctx_driver->swap_buffers();

   bool quit, resize;
   unsigned width, height, frame_count;
   frame_count = 0;
   device_ptr->ctx_driver->check_window(&quit, &resize, &width, &height, frame_count);

   if (quit)
      g_extern.console.rmenu.mode = MODE_EXIT;

   if (g_extern.draw_menu)
      device_ptr->ctx_driver->set_blend(false);

   return true;

deinit:
   if (!(g_extern.lifecycle_state & (1ULL << RARCH_FRAMEADVANCE)))
   {
      // set a timer delay so that we don't instantly switch back to the menu when
      // press and holding L3 + R3 in the emulation loop (lasts for 30 frame ticks)
      SET_TIMER_EXPIRATION(0, 30);
   }

   if(g_extern.console.rmenu.state.ingame_menu.enable)
      menu_stack_pop();

   g_extern.draw_menu = false;
   g_extern.console.rmenu.state.ingame_menu.enable = false;

   device_ptr->ctx_driver->rmenu_free();

   preinit = true;

   return false;
}
