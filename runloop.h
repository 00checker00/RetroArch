/*  RetroArch - A frontend for libretro.
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

#ifndef __RETROARCH_RUNLOOP_H
#define __RETROARCH_RUNLOOP_H

#include <setjmp.h>
#include "libretro.h"
#include "core_info.h"
#include "core_options.h"
#include "driver.h"
#include "rewind.h"
#include "autosave.h"
#include "movie.h"
#include "cheats.h"
#include "dynamic.h"
#include "system.h"
#include "performance.h"

#ifdef __cplusplus
extern "C" {
#endif

enum rarch_main_ctl_state
{
   RARCH_MAIN_CTL_IS_IDLE = 0,
   RARCH_MAIN_CTL_IS_SLOWMOTION
};

typedef struct rarch_resolution
{
   unsigned idx;
   unsigned id;
} rarch_resolution_t;

/* All run-time- / command line flag-related globals go here. */

typedef struct global
{
   bool verbosity;
   bool perfcnt_enable;
   bool force_fullscreen;
   bool core_shutdown_initiated;

   struct string_list *temporary_content;

   struct
   {
      core_info_list_t *list;
      core_info_t *current;
   } core_info;

   uint32_t content_crc;

   struct
   {
      char gb_rom[PATH_MAX_LENGTH];
      char bsx_rom[PATH_MAX_LENGTH];
      char sufami_rom[2][PATH_MAX_LENGTH];
      /* Config associated with global "default" config. */
      char config[PATH_MAX_LENGTH];
      char append_config[PATH_MAX_LENGTH];
      char input_config[PATH_MAX_LENGTH];
#ifdef HAVE_FILE_LOGGER
      char default_log[PATH_MAX_LENGTH];
#endif
      char fullpath[PATH_MAX_LENGTH];
      /* Config file associated with per-core configs. */
      char core_specific_config[PATH_MAX_LENGTH];
   } path;

   struct
   {
      bool input_descriptors;
      bool save_path;
      bool state_path;
      bool libretro_device[MAX_USERS];
      bool libretro;
      bool libretro_directory;
      bool verbosity;

      bool netplay_mode;
      bool username;
      bool netplay_ip_address;
      bool netplay_delay_frames;
      bool netplay_ip_port;

      bool ups_pref;
      bool bps_pref;
      bool ips_pref;
   } has_set;

   
   bool overrides_active;

   struct
   {
      char base[PATH_MAX_LENGTH];
      char savefile[PATH_MAX_LENGTH];
      char savestate[PATH_MAX_LENGTH];
      char cheatfile[PATH_MAX_LENGTH];
      char ups[PATH_MAX_LENGTH];
      char bps[PATH_MAX_LENGTH];
      char ips[PATH_MAX_LENGTH];
   } name;

   /* A list of save types and associated paths for all content. */
   struct string_list *savefiles;

   /* For --subsystem content. */
   char subsystem[PATH_MAX_LENGTH];
   struct string_list *subsystem_fullpaths;

   struct
   {
      /* Used on reentrancy to use a savestate dir. */
      char savefile[PATH_MAX_LENGTH];
      char savestate[PATH_MAX_LENGTH];
      char systemdir[PATH_MAX_LENGTH];
#ifdef HAVE_OVERLAY
      char overlay[PATH_MAX_LENGTH];
      char osk_overlay[PATH_MAX_LENGTH];
#endif
   } dir;

   struct
   {
      bool block_patch;
      bool ups_pref;
      bool bps_pref;
      bool ips_pref;
   } patch;

   struct
   {
      unsigned windowed_scale;
   } pending;

#ifdef HAVE_MENU
   struct
   {
      struct retro_system_info info;
      bool bind_mode_keyboard;
   } menu;
#endif

   bool exec;

   struct
   {
      /* Rewind support. */
      state_manager_t *state;
      size_t size;
   } rewind;

   struct
   {
      /* Movie playback/recording support. */
      bsv_movie_t *movie;
      char movie_path[PATH_MAX_LENGTH];
      bool movie_playback;
      bool eof_exit;

      /* Immediate playback/recording. */
      char movie_start_path[PATH_MAX_LENGTH];
      bool movie_start_recording;
      bool movie_start_playback;
      bool movie_end;
   } bsv;

   struct
   {
      bool load_disable;
      bool save_disable;
      bool use;
   } sram;

   /* Turbo support. */
   struct
   {
      bool frame_enable[MAX_USERS];
      uint16_t enable[MAX_USERS];
      unsigned count;
   } turbo;

   /* Autosave support. */
   struct
   {
      autosave_t **list;
      unsigned num;
   } autosave;

#ifdef HAVE_NETPLAY
   /* Netplay. */
   struct
   {
      char server[PATH_MAX_LENGTH];
      bool enable;
      bool is_client;
      bool is_spectate;
      unsigned sync_frames;
      unsigned port;
   } netplay;
#endif

   /* Recording. */
   struct
   {
      char path[PATH_MAX_LENGTH];
      char config[PATH_MAX_LENGTH];
      bool enable;
      unsigned width;
      unsigned height;

      uint8_t *gpu_buffer;
      size_t gpu_width;
      size_t gpu_height;
      char output_dir[PATH_MAX_LENGTH];
      char config_dir[PATH_MAX_LENGTH];
      bool use_output_dir;
   } record;

   struct
   {
      struct string_list *list;
      size_t ptr;
   } shader_dir;

   struct
   {
      struct string_list *list;
      size_t ptr;
   } filter_dir;

   cheat_manager_t *cheat;

   bool block_config_read;

   /* Settings and/or global state that is specific to 
    * a console-style implementation. */
   struct
   {
      struct
      {
         struct
         {
            rarch_resolution_t current;
            rarch_resolution_t initial;
            uint32_t *list;
            unsigned count;
            bool check;
		    unsigned width;
		    unsigned height;
         } resolutions;

         unsigned gamma_correction;
         unsigned char flicker_filter_index;
         unsigned char soft_filter_index;
         bool pal_enable;
         bool pal60_enable;
      } screen;

      struct
      {
         unsigned mode;
         bool system_bgm_enable;
      } sound;

      bool flickerfilter_enable;
      bool softfilter_enable;
   } console;

   /* If this is non-NULL. RARCH_LOG and friends 
    * will write to this file. */
   FILE *log_file;

   struct
   {
      bool main;
      bool content;
      bool error;
      struct
      {
         bool no_content;
         enum rarch_core_type type;
      } core;
   } inited;

   jmp_buf error_sjlj_context;
   char error_string[PATH_MAX_LENGTH];
   retro_keyboard_event_t frontend_key_event;
} global_t;

global_t *global_get_ptr(void);

/**
 * rarch_main_iterate:
 *
 * Run Libretro core in RetroArch for one frame.
 *
 * Returns: 0 on successful run, 1 if we have to wait until button input in order
 * to wake up the loop, -1 if we forcibly quit out of the RetroArch iteration loop. 
 **/
int rarch_main_iterate(unsigned *sleep_ms);

void rarch_main_msg_queue_push(const char *msg, unsigned prio,
      unsigned duration, bool flush);

void rarch_main_msg_queue_push_new(uint32_t hash, unsigned prio,
      unsigned duration, bool flush);

const char *rarch_main_msg_queue_pull(void);

void rarch_main_msg_queue_free(void);

void rarch_main_msg_queue_init(void);

void rarch_main_clear_state(void);

bool rarch_main_verbosity(void);

FILE *rarch_main_log_file(void);

bool rarch_main_ctl(enum rarch_main_ctl_state state, void *data);

bool rarch_main_is_paused(void);

void rarch_main_set_slowmotion(unsigned enable);

void rarch_main_set_pause(unsigned enable);

void rarch_main_set_frame_limit_last_time(void);

void rarch_main_set_max_frames(unsigned val);

void rarch_main_set_idle(unsigned enable);

void rarch_main_state_free(void);

void rarch_main_global_free(void);

#ifdef __cplusplus
}
#endif

#endif
