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

#ifndef MENU_COMMON_H__
#define MENU_COMMON_H__

#ifndef __cplusplus
#include <stdbool.h>
#else
extern "C" {
#endif

#ifdef HAVE_FILEBROWSER
#include "utils/file_browser.h"
#else
#include "utils/file_list.h"
#endif

#if defined(HAVE_CG) || defined(HAVE_HLSL) || defined(HAVE_GLSL)
#define HAVE_SHADER_MANAGER
#include "../../gfx/shader_parse.h"
#endif

#define RGUI_MAX_SHADERS 8

typedef enum
{
   RGUI_FILE_PLAIN,
   RGUI_FILE_DIRECTORY,
   RGUI_FILE_DEVICE,
   RGUI_SETTINGS,

   // Shader stuff
   RGUI_SETTINGS_SHADER_MANAGER,
   RGUI_SETTINGS_SHADER_FILTER,
   RGUI_SETTINGS_SHADER_PRESET,
   RGUI_SETTINGS_SHADER_APPLY,
   RGUI_SETTINGS_SHADER_PASSES,
   RGUI_SETTINGS_SHADER_0,
   RGUI_SETTINGS_SHADER_0_FILTER,
   RGUI_SETTINGS_SHADER_0_SCALE,
   RGUI_SETTINGS_SHADER_LAST = RGUI_SETTINGS_SHADER_0_SCALE + (3 * (RGUI_MAX_SHADERS - 1)),

   // settings options are done here too
   RGUI_SETTINGS_OPEN_FILEBROWSER,
   RGUI_SETTINGS_CORE,
   RGUI_SETTINGS_CORE_OPTIONS,
   RGUI_SETTINGS_REWIND_ENABLE,
   RGUI_SETTINGS_REWIND_GRANULARITY,
   RGUI_SETTINGS_SAVESTATE_SAVE,
   RGUI_SETTINGS_SAVESTATE_LOAD,
#ifdef HAVE_SCREENSHOTS
   RGUI_SETTINGS_SCREENSHOT,
#endif
   RGUI_SETTINGS_RESTART_GAME,
   RGUI_SETTINGS_VIDEO_FILTER,
   RGUI_SETTINGS_VIDEO_SOFT_FILTER,
#ifdef GEKKO
   RGUI_SETTINGS_VIDEO_RESOLUTION,
#endif
   RGUI_SETTINGS_VIDEO_GAMMA,
   RGUI_SETTINGS_VIDEO_ASPECT_RATIO,
   RGUI_SETTINGS_CUSTOM_VIEWPORT,
   RGUI_SETTINGS_CUSTOM_VIEWPORT_2,
   RGUI_SETTINGS_VIDEO_ROTATION,
   RGUI_SETTINGS_AUDIO_MUTE,
   RGUI_SETTINGS_AUDIO_CONTROL_RATE,
   RGUI_SETTINGS_ZIP_EXTRACT,
   RGUI_SETTINGS_SRAM_DIR,
   RGUI_SETTINGS_STATE_DIR,
   RGUI_SETTINGS_CONTROLLER_1,
   RGUI_SETTINGS_CONTROLLER_2,
   RGUI_SETTINGS_CONTROLLER_3,
   RGUI_SETTINGS_CONTROLLER_4,
   RGUI_SETTINGS_DEBUG_TEXT,
   RGUI_SETTINGS_RESTART_EMULATOR,
   RGUI_SETTINGS_RESUME_GAME,
   RGUI_SETTINGS_QUIT_RARCH,

   RGUI_SETTINGS_BIND_DEVICE,
   RGUI_SETTINGS_BIND_DPAD_EMULATION,
   RGUI_SETTINGS_BIND_UP,
   RGUI_SETTINGS_BIND_DOWN,
   RGUI_SETTINGS_BIND_LEFT,
   RGUI_SETTINGS_BIND_RIGHT,
   RGUI_SETTINGS_BIND_A,
   RGUI_SETTINGS_BIND_B,
   RGUI_SETTINGS_BIND_X,
   RGUI_SETTINGS_BIND_Y,
   RGUI_SETTINGS_BIND_START,
   RGUI_SETTINGS_BIND_SELECT,
   RGUI_SETTINGS_BIND_L,
   RGUI_SETTINGS_BIND_R,
   RGUI_SETTINGS_BIND_L2,
   RGUI_SETTINGS_BIND_R2,
   RGUI_SETTINGS_BIND_L3,
   RGUI_SETTINGS_BIND_R3,

   RGUI_SETTINGS_CORE_OPTION_NONE = 0xffff,
   RGUI_SETTINGS_CORE_OPTION_START = 0x10000
} rgui_file_type_t;

typedef enum
{
   RGUI_ACTION_UP,
   RGUI_ACTION_DOWN,
   RGUI_ACTION_LEFT,
   RGUI_ACTION_RIGHT,
   RGUI_ACTION_OK,
   RGUI_ACTION_CANCEL,
   RGUI_ACTION_REFRESH,
   RGUI_ACTION_SETTINGS,
   RGUI_ACTION_START,
   RGUI_ACTION_MESSAGE,
   RGUI_ACTION_NOOP
} rgui_action_t;

typedef struct
{
   uint16_t *frame_buf;
   size_t frame_buf_pitch;

#ifdef HAVE_FILEBROWSER
   filebrowser_t *browser;
   unsigned menu_type;
#else
   rgui_list_t *menu_stack;
   rgui_list_t *selection_buf;
#endif
   size_t selection_ptr;
   bool need_refresh;
   bool msg_force;

   char base_path[PATH_MAX];

   const uint8_t *font;
   bool alloc_font;

#ifdef HAVE_DYNAMIC
   char libretro_dir[PATH_MAX];
#endif
   struct retro_system_info info;

#ifdef HAVE_OSKUTIL
   unsigned osk_param;
   bool (*osk_init)(void *data);
   bool (*osk_callback)(void *data);
#endif
#ifdef HAVE_SHADER_MANAGER
   struct gfx_shader shader;
#endif
} rgui_handle_t;

extern rgui_handle_t *rgui;

void menu_init(void);
bool menu_iterate(void);
void menu_free(void);

int rgui_iterate(rgui_handle_t *rgui, uint64_t action);

rgui_handle_t *rgui_init(void);
void rgui_free(rgui_handle_t *rgui);

#ifdef HAVE_SHADER_MANAGER
void shader_manager_init(rgui_handle_t *rgui);
void shader_manager_get_str(struct gfx_shader *shader,
      char *type_str, size_t type_str_size, unsigned type);
#endif

#ifdef __cplusplus
}
#endif

#endif
