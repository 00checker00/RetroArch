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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "../boolean.h"

#include "../general.h"
#include "rarch_console_input.h"

struct platform_bind
{
   uint64_t joykey;
   const char *label;
};

extern const struct platform_bind platform_keys[];
extern const unsigned int platform_keys_size;

static uint64_t rarch_input_find_previous_platform_key(uint64_t joykey)
{
   size_t arr_size = platform_keys_size / sizeof(platform_keys[0]);

   if (joykey == NO_BTN)
      return platform_keys[arr_size - 1].joykey;

   if (platform_keys[0].joykey == joykey)
      return NO_BTN;

   for (size_t i = 1; i < arr_size; i++)
   {
      if (platform_keys[i].joykey == joykey)
         return platform_keys[i - 1].joykey;
   }

   return NO_BTN;
}

static uint64_t rarch_input_find_next_platform_key(uint64_t joykey)
{
   size_t arr_size = platform_keys_size / sizeof(platform_keys[0]);

   if (joykey == NO_BTN)
      return platform_keys[0].joykey;

   if (platform_keys[arr_size - 1].joykey == joykey)
      return NO_BTN;

   for (size_t i = 0; i < arr_size - 1; i++)
   {
      if (platform_keys[i].joykey == joykey)
         return platform_keys[i + 1].joykey;
   }

   return NO_BTN;
}

const char *rarch_input_find_platform_key_label(uint64_t joykey)
{
   if (joykey == NO_BTN)
      return "No button";

   size_t arr_size = platform_keys_size / sizeof(platform_keys[0]);
   for (size_t i = 0; i < arr_size; i++)
   {
      if (platform_keys[i].joykey == joykey)
         return platform_keys[i].label;
   }

   return "Unknown";
}

void rarch_input_set_keybind(unsigned player, unsigned keybind_action, uint64_t default_retro_joypad_id)
{
   uint64_t *key = &g_settings.input.binds[player][default_retro_joypad_id].joykey;

   switch (keybind_action)
   {
      case KEYBIND_DECREMENT:
         *key = rarch_input_find_previous_platform_key(*key);
         break;

      case KEYBIND_INCREMENT:
         *key = rarch_input_find_next_platform_key(*key);
         break;

      case KEYBIND_DEFAULT:
         *key = g_settings.input.default_binds[default_retro_joypad_id];
         break;

      default:
         break;
   }
}

void rarch_input_set_default_keybinds(unsigned player)
{
   for (unsigned i = 0; i < RARCH_CUSTOM_BIND_LIST_END; i++)
   {
      g_settings.input.binds[player][i].id = i;
      g_settings.input.binds[player][i].joykey = g_settings.input.default_binds[i];
   }
   g_settings.input.dpad_emulation[player] = DPAD_EMULATION_LSTICK;
}
