/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2012 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2012 - Daniel De Matteis
 *  Copyright (C) 2012 - Michael Lelli
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
#include <gccore.h>
#include <ogc/pad.h>
#ifdef HW_RVL
#include <wiiuse/wpad.h>
#endif
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

#include "gx_input.h"
#include "../driver.h"
#include "../libretro.h"
#include <stdlib.h>

#define JOYSTICK_THRESHOLD 64

#define MAX_PADS 4

static uint64_t pad_state[MAX_PADS];

const struct platform_bind platform_keys[] = {
   { GX_GC_A, "GC A button" },
   { GX_GC_B, "GC B button" },
   { GX_GC_X, "GC X button" },
   { GX_GC_Y, "GC Y button" },
   { GX_GC_UP, "GC D-Pad Up" },
   { GX_GC_DOWN, "GC D-Pad Down" },
   { GX_GC_LEFT, "GC D-Pad Left" },
   { GX_GC_RIGHT, "GC D-Pad Right" },
   { GX_GC_Z_TRIGGER, "GC Z Trigger" },
   { GX_GC_START, "GC Start button" },
   { GX_GC_L_TRIGGER, "GC Left Trigger" },
   { GX_GC_R_TRIGGER, "GC Right Trigger" },
   { GX_GC_LSTICK_LEFT, "GC Main Stick Left" },
   { GX_GC_LSTICK_RIGHT, "GC Main Stick Right" },
   { GX_GC_LSTICK_UP, "GC Main Stick Up" },
   { GX_GC_LSTICK_DOWN, "GC Main Stick Down" },
   { GX_GC_LSTICK_LEFT | GX_GC_LEFT, "GC Main Stick D-Pad Left" },
   { GX_GC_LSTICK_RIGHT | GX_GC_RIGHT, "GC Main Stick D-Pad Right" },
   { GX_GC_LSTICK_UP | GX_GC_UP, "GC Main Stick D-Pad Up" },
   { GX_GC_LSTICK_DOWN | GX_GC_DOWN, "GC Main Stick D-Pad Down" },
   { GX_GC_RSTICK_LEFT, "GC C-Stick Left" },
   { GX_GC_RSTICK_RIGHT, "GC C-Stick Right" },
   { GX_GC_RSTICK_UP, "GC C-Stick Up" },
   { GX_GC_RSTICK_DOWN, "GC C-Stick Down" },
   { GX_GC_RSTICK_LEFT | GX_GC_LEFT, "GC C-Stick D-Pad Left" },
   { GX_GC_RSTICK_RIGHT | GX_GC_RIGHT, "GC C-Stick D-Pad Right" },
   { GX_GC_RSTICK_UP | GX_GC_UP, "GC C-Stick D-Pad Up" },
   { GX_GC_RSTICK_DOWN | GX_GC_DOWN, "GC C-Stick D-Pad Down" },

#ifdef HW_RVL
   // CLASSIC CONTROLLER
   { GX_CLASSIC_A, "Classic A button" },
   { GX_CLASSIC_B, "Classic B button" },
   { GX_CLASSIC_X, "Classic X button" },
   { GX_CLASSIC_Y, "Classic Y button" },
   { GX_CLASSIC_UP, "Classic D-Pad Up" },
   { GX_CLASSIC_DOWN, "Classic D-Pad Down" },
   { GX_CLASSIC_LEFT, "Classic D-Pad Left" },
   { GX_CLASSIC_RIGHT, "Classic D-Pad Right" },
   { GX_CLASSIC_PLUS, "Classic Plus button" },
   { GX_CLASSIC_MINUS, "Classic Minus button" },
   { GX_CLASSIC_HOME, "Classic Home button" },
   { GX_CLASSIC_L_TRIGGER, "Classic L Trigger" },
   { GX_CLASSIC_R_TRIGGER, "Classic R Trigger" },
   { GX_CLASSIC_ZL_TRIGGER, "Classic ZL Trigger" },
   { GX_CLASSIC_ZR_TRIGGER, "Classic ZR Trigger" },
   { GX_CLASSIC_LSTICK_LEFT, "Classic LStick Left" },
   { GX_CLASSIC_LSTICK_RIGHT, "Classic LStick Right" },
   { GX_CLASSIC_LSTICK_UP, "Classic LStick Up" },
   { GX_CLASSIC_LSTICK_DOWN, "Classic LStick Down" },
   { GX_CLASSIC_LSTICK_LEFT | GX_CLASSIC_LEFT, "Classic LStick D-Pad Left" },
   { GX_CLASSIC_LSTICK_RIGHT | GX_CLASSIC_RIGHT, "Classic LStick D-Pad Right" },
   { GX_CLASSIC_LSTICK_UP | GX_CLASSIC_UP, "Classic LStick D-Pad Up" },
   { GX_CLASSIC_LSTICK_DOWN | GX_CLASSIC_DOWN, "Classic LStick D-Pad Down" },
   { GX_CLASSIC_RSTICK_LEFT, "Classic RStick Left" },
   { GX_CLASSIC_RSTICK_RIGHT, "Classic RStick Right" },
   { GX_CLASSIC_RSTICK_UP, "Classic RStick Up" },
   { GX_CLASSIC_RSTICK_DOWN, "Classic RStick Down" },
   { GX_CLASSIC_RSTICK_LEFT | GX_CLASSIC_LEFT, "Classic RStick D-Pad Left" },
   { GX_CLASSIC_RSTICK_RIGHT | GX_CLASSIC_RIGHT, "Classic RStick D-Pad Right" },
   { GX_CLASSIC_RSTICK_UP | GX_CLASSIC_UP, "Classic RStick D-Pad Up" },
   { GX_CLASSIC_RSTICK_DOWN | GX_CLASSIC_DOWN, "Classic RStick D-Pad Down" },

   // WIIMOTE (PLUS OPTIONAL NUNCHUK)
   { GX_WIIMOTE_A, "Wiimote A button" },
   { GX_WIIMOTE_B, "Wiimote B button" },
   { GX_WIIMOTE_1, "Wiimote 1 button" },
   { GX_WIIMOTE_2, "Wiimote 2 button" },
   { GX_WIIMOTE_UP, "Wiimote D-Pad Up" },
   { GX_WIIMOTE_DOWN, "Wiimote D-Pad Down" },
   { GX_WIIMOTE_LEFT, "Wiimote D-Pad Left" },
   { GX_WIIMOTE_RIGHT, "Wiimote D-Pad Right" },
   { GX_WIIMOTE_PLUS, "Wiimote Plus button" },
   { GX_WIIMOTE_MINUS, "Wiimote Minus button" },
   { GX_WIIMOTE_HOME, "Wiimote Home button" },
   { GX_NUNCHUK_Z, "Nunchuk Z button" },
   { GX_NUNCHUK_C, "Nunchuk C button" },
   { GX_NUNCHUK_LEFT, "Nunchuk Stick Left" },
   { GX_NUNCHUK_RIGHT, "Nunchuk Stick Right" },
   { GX_NUNCHUK_UP, "Nunchuk Stick Up" },
   { GX_NUNCHUK_DOWN, "Nunchuk Stick Down" },
   { GX_NUNCHUK_LEFT | GX_WIIMOTE_LEFT, "Nunchuk Stick D-Pad Left" },
   { GX_NUNCHUK_RIGHT | GX_WIIMOTE_RIGHT, "Nunchuk Stick D-Pad Right" },
   { GX_NUNCHUK_UP | GX_WIIMOTE_UP, "Nunchuk Stick D-Pad Up" },
   { GX_NUNCHUK_DOWN | GX_WIIMOTE_DOWN, "Nunchuk Stick D-Pad Down" },
#endif
};

const unsigned int platform_keys_size = sizeof(platform_keys);
static bool g_quit;

static int16_t wii_input_state(void *data, const struct retro_keybind **binds,
      unsigned port, unsigned device,
      unsigned index, unsigned id)
{
   (void)data;
   (void)index;

   if (port >= MAX_PADS || device != RETRO_DEVICE_JOYPAD)
      return 0;

   return (binds[port][id].joykey & pad_state[port]) ? 1 : 0;
}

static void wii_free_input(void *data)
{
   (void)data;
}

static void reset_callback(void)
{
   g_quit = true;
}

static void wii_input_set_analog_dpad_mapping(unsigned device, unsigned map_dpad_enum, unsigned controller_id)
{
   switch (device)
   {
#ifdef HW_RVL
      case GX_DEVICE_WIIMOTE:
         g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_UP].joykey    = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_UP].joykey;
         g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_DOWN].joykey  = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_DOWN].joykey;
         g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_LEFT].joykey  = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_LEFT].joykey;
         g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_RIGHT].joykey = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_RIGHT].joykey;
         break;
      case GX_DEVICE_NUNCHUK:
         if (map_dpad_enum == DPAD_EMULATION_NONE)
         {
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_UP].joykey    = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_UP].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_DOWN].joykey  = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_DOWN].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_LEFT].joykey  = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_LEFT].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_RIGHT].joykey = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_RIGHT].joykey;
         }
         else
         {
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_UP].joykey    = platform_keys[GX_DEVICE_WIIMOTE_ID_LSTICK_UP_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_DOWN].joykey  = platform_keys[GX_DEVICE_WIIMOTE_ID_LSTICK_DOWN_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_LEFT].joykey  = platform_keys[GX_DEVICE_WIIMOTE_ID_LSTICK_LEFT_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_RIGHT].joykey = platform_keys[GX_DEVICE_WIIMOTE_ID_LSTICK_RIGHT_DPAD].joykey;
         }
         break;
      case GX_DEVICE_CLASSIC:
         if (map_dpad_enum == DPAD_EMULATION_NONE)
         {
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_UP].joykey    = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_UP].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_DOWN].joykey  = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_DOWN].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_LEFT].joykey  = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_LEFT].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_RIGHT].joykey = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_RIGHT].joykey;
         }
         else if (map_dpad_enum == DPAD_EMULATION_LSTICK)
         {
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_UP].joykey    = platform_keys[GX_DEVICE_CLASSIC_ID_LSTICK_UP_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_DOWN].joykey  = platform_keys[GX_DEVICE_CLASSIC_ID_LSTICK_DOWN_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_LEFT].joykey  = platform_keys[GX_DEVICE_CLASSIC_ID_LSTICK_LEFT_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_RIGHT].joykey = platform_keys[GX_DEVICE_CLASSIC_ID_LSTICK_RIGHT_DPAD].joykey;
         }
         else if (map_dpad_enum == DPAD_EMULATION_RSTICK)
         {
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_UP].joykey    = platform_keys[GX_DEVICE_CLASSIC_ID_RSTICK_UP_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_DOWN].joykey  = platform_keys[GX_DEVICE_CLASSIC_ID_RSTICK_DOWN_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_LEFT].joykey  = platform_keys[GX_DEVICE_CLASSIC_ID_RSTICK_LEFT_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_RIGHT].joykey = platform_keys[GX_DEVICE_CLASSIC_ID_RSTICK_RIGHT_DPAD].joykey;
         }
         break;
#endif
      case GX_DEVICE_GAMECUBE:
         if (map_dpad_enum == DPAD_EMULATION_NONE)
         {
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_UP].joykey    = platform_keys[GX_DEVICE_GC_ID_JOYPAD_UP].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_DOWN].joykey  = platform_keys[GX_DEVICE_GC_ID_JOYPAD_DOWN].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_LEFT].joykey  = platform_keys[GX_DEVICE_GC_ID_JOYPAD_LEFT].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_RIGHT].joykey = platform_keys[GX_DEVICE_GC_ID_JOYPAD_RIGHT].joykey;
         }
         else if (map_dpad_enum == DPAD_EMULATION_LSTICK)
         {
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_UP].joykey    = platform_keys[GX_DEVICE_GC_ID_LSTICK_UP_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_DOWN].joykey  = platform_keys[GX_DEVICE_GC_ID_LSTICK_DOWN_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_LEFT].joykey  = platform_keys[GX_DEVICE_GC_ID_LSTICK_LEFT_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_RIGHT].joykey = platform_keys[GX_DEVICE_GC_ID_LSTICK_RIGHT_DPAD].joykey;
         }
         else if (map_dpad_enum == DPAD_EMULATION_RSTICK)
         {
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_UP].joykey    = platform_keys[GX_DEVICE_GC_ID_RSTICK_UP_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_DOWN].joykey  = platform_keys[GX_DEVICE_GC_ID_RSTICK_DOWN_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_LEFT].joykey  = platform_keys[GX_DEVICE_GC_ID_RSTICK_LEFT_DPAD].joykey;
            g_settings.input.binds[controller_id][RETRO_DEVICE_ID_JOYPAD_RIGHT].joykey = platform_keys[GX_DEVICE_GC_ID_RSTICK_RIGHT_DPAD].joykey;
         }
         break;
      default:
         break;
   }
}

static void *wii_input_initialize(void)
{
   PAD_Init();
#ifdef HW_RVL
   WPAD_Init();
#endif
   SYS_SetResetCallback(reset_callback);
   SYS_SetPowerCallback(reset_callback);
   return (void*)-1;
}

#define STUB_DEVICE 0

static void wii_input_post_init(void)
{
   for(unsigned i = 0; i < MAX_PADS; i++)
      wii_input_set_analog_dpad_mapping(STUB_DEVICE, g_settings.input.dpad_emulation[i], i);
}

#define wii_stick_x(x) ((s8)((sin((x).ang * M_PI / 180.0f)) * (x).mag * 128.0f))
#define wii_stick_y(x) ((s8)((cos((x).ang * M_PI / 180.0f)) * (x).mag * 128.0f))

static void wii_input_poll(void *data)
{
   //TODO: Hack, analog stick twitchiness needs to be properly fixed
   gx_video_t *gx = (gx_video_t*)driver.video_data;
   (void)data;
   bool quit_gc = false;
   bool quit_classic = false;
   bool quit_wiimote = false;

   PAD_ScanPads();
#ifdef HW_RVL
   WPAD_ScanPads();
#endif

   for (unsigned port = 0; port < MAX_PADS; port++)
   {
      uint64_t state = 0;
      {
         uint16_t down = PAD_ButtonsHeld(port);

         state |= (down & PAD_BUTTON_A) ? GX_GC_A : 0;
         state |= (down & PAD_BUTTON_B) ? GX_GC_B : 0;
         state |= (down & PAD_BUTTON_X) ? GX_GC_X : 0;
         state |= (down & PAD_BUTTON_Y) ? GX_GC_Y : 0;
         state |= (down & PAD_BUTTON_UP) ? GX_GC_UP : 0;
         state |= (down & PAD_BUTTON_DOWN) ? GX_GC_DOWN : 0;
         state |= (down & PAD_BUTTON_LEFT) ? GX_GC_LEFT : 0;
         state |= (down & PAD_BUTTON_RIGHT) ? GX_GC_RIGHT : 0;
         state |= (down & PAD_BUTTON_START) ? GX_GC_START : 0;
         state |= (down & PAD_TRIGGER_Z) ? GX_GC_Z_TRIGGER : 0;
         state |= (PAD_TriggerL(port) > 127) ? GX_GC_L_TRIGGER : 0;
         state |= (PAD_TriggerR(port) > 127) ? GX_GC_R_TRIGGER : 0;

         s8 x = PAD_StickX(port);
         s8 y = PAD_StickY(port);

         if (abs(x) > JOYSTICK_THRESHOLD)
         {
            state |= x > 0 ? GX_GC_LSTICK_RIGHT : GX_GC_LSTICK_LEFT;
         }
         if (abs(y) > JOYSTICK_THRESHOLD)
         {
            state |= y > 0 ? GX_GC_LSTICK_UP : GX_GC_LSTICK_DOWN;
         }
         
         x = PAD_SubStickX(port);
         y = PAD_SubStickY(port);

         if (abs(x) > JOYSTICK_THRESHOLD)
         {
            state |= x > 0 ? GX_GC_RSTICK_RIGHT : GX_GC_RSTICK_LEFT;
         }
         if (abs(y) > JOYSTICK_THRESHOLD)
         {
            state |= y > 0 ? GX_GC_RSTICK_UP : GX_GC_RSTICK_DOWN;
         }

         if ((state & (GX_GC_LSTICK_DOWN | GX_GC_RSTICK_DOWN | GX_GC_L_TRIGGER | GX_GC_R_TRIGGER)) == (GX_GC_LSTICK_DOWN | GX_GC_RSTICK_DOWN | GX_GC_L_TRIGGER | GX_GC_R_TRIGGER))
            quit_gc = true;
      }

#ifdef HW_RVL
      {
         uint32_t down = WPAD_ButtonsHeld(port);

         state |= (down & WPAD_BUTTON_A) ? GX_WIIMOTE_A : 0;
         state |= (down & WPAD_BUTTON_B) ? GX_WIIMOTE_B : 0;
         state |= (down & WPAD_BUTTON_1) ? GX_WIIMOTE_1 : 0;
         state |= (down & WPAD_BUTTON_2) ? GX_WIIMOTE_2 : 0;
         state |= (down & WPAD_BUTTON_PLUS) ? GX_WIIMOTE_PLUS : 0;
         state |= (down & WPAD_BUTTON_MINUS) ? GX_WIIMOTE_MINUS : 0;
         state |= (down & WPAD_BUTTON_HOME) ? GX_WIIMOTE_HOME : 0;

         if((down & WPAD_BUTTON_HOME) && (down & WPAD_BUTTON_B))
            quit_wiimote = true;

         expansion_t exp;
         WPAD_Expansion(port, &exp);
         switch (exp.type)
         {
            case WPAD_EXP_NUNCHUK:
            {
               // wiimote is held upright with nunchuk, do not change d-pad orientation
               state |= (down & WPAD_BUTTON_UP) ? GX_WIIMOTE_UP : 0;
               state |= (down & WPAD_BUTTON_DOWN) ? GX_WIIMOTE_DOWN : 0;
               state |= (down & WPAD_BUTTON_LEFT) ? GX_WIIMOTE_LEFT : 0;
               state |= (down & WPAD_BUTTON_RIGHT) ? GX_WIIMOTE_RIGHT : 0;

               state |= (down & WPAD_NUNCHUK_BUTTON_Z) ? GX_NUNCHUK_Z : 0;
               state |= (down & WPAD_NUNCHUK_BUTTON_C) ? GX_NUNCHUK_C : 0;

               s8 x = wii_stick_x(exp.nunchuk.js);
               s8 y = wii_stick_y(exp.nunchuk.js);

               if (abs(x) > JOYSTICK_THRESHOLD)
               {
                  state |= x > 0 ? GX_NUNCHUK_RIGHT : GX_NUNCHUK_LEFT;
               }
               if (abs(y) > JOYSTICK_THRESHOLD)
               {
                  state |= y > 0 ? GX_NUNCHUK_UP : GX_NUNCHUK_DOWN;
               }
               break;
            }
            case WPAD_EXP_CLASSIC:
            {
               state |= (down & WPAD_CLASSIC_BUTTON_A) ? GX_CLASSIC_A : 0;
               state |= (down & WPAD_CLASSIC_BUTTON_B) ? GX_CLASSIC_B : 0;
               state |= (down & WPAD_CLASSIC_BUTTON_X) ? GX_CLASSIC_X : 0;
               state |= (down & WPAD_CLASSIC_BUTTON_Y) ? GX_CLASSIC_Y : 0;
               state |= (down & WPAD_CLASSIC_BUTTON_UP) ? GX_CLASSIC_UP : 0;
               state |= (down & WPAD_CLASSIC_BUTTON_DOWN) ? GX_CLASSIC_DOWN : 0;
               state |= (down & WPAD_CLASSIC_BUTTON_LEFT) ? GX_CLASSIC_LEFT : 0;
               state |= (down & WPAD_CLASSIC_BUTTON_RIGHT) ? GX_CLASSIC_RIGHT : 0;
               state |= (down & WPAD_CLASSIC_BUTTON_PLUS) ? GX_CLASSIC_PLUS : 0;
               state |= (down & WPAD_CLASSIC_BUTTON_MINUS) ? GX_CLASSIC_MINUS : 0;
               state |= (down & WPAD_CLASSIC_BUTTON_HOME) ? GX_CLASSIC_HOME : 0;
               state |= (down & WPAD_CLASSIC_BUTTON_FULL_L) ? GX_CLASSIC_L_TRIGGER : 0;
               state |= (down & WPAD_CLASSIC_BUTTON_FULL_R) ? GX_CLASSIC_R_TRIGGER : 0;
               state |= (down & WPAD_CLASSIC_BUTTON_ZL) ? GX_CLASSIC_ZL_TRIGGER : 0;
               state |= (down & WPAD_CLASSIC_BUTTON_ZR) ? GX_CLASSIC_ZR_TRIGGER : 0;

               if((down & WPAD_CLASSIC_BUTTON_HOME) && (down & WPAD_CLASSIC_BUTTON_ZL) && (down & WPAD_CLASSIC_BUTTON_ZR))
                  quit_classic = true;

	       //TODO: Hack, analog stick twitchiness needs to be properly fixed
               if(gx->menu_render)
               {
                  s8 x = wii_stick_x(exp.classic.ljs);
                  s8 y = wii_stick_y(exp.classic.ljs);

                  if (abs(x) > JOYSTICK_THRESHOLD)
                     state |= x > 0 ? GX_CLASSIC_LSTICK_RIGHT : GX_CLASSIC_LSTICK_LEFT;
                  if (abs(y) > JOYSTICK_THRESHOLD)
                     state |= y > 0 ? GX_CLASSIC_LSTICK_UP : GX_CLASSIC_LSTICK_DOWN;

                  x = wii_stick_x(exp.classic.rjs);
                  y = wii_stick_y(exp.classic.rjs);

                  if (abs(x) > JOYSTICK_THRESHOLD)
                     state |= x > 0 ? GX_CLASSIC_RSTICK_RIGHT : GX_CLASSIC_RSTICK_LEFT;
                  if (abs(y) > JOYSTICK_THRESHOLD)
                     state |= y > 0 ? GX_CLASSIC_RSTICK_UP : GX_CLASSIC_RSTICK_DOWN;
               }
               else
               {
                  u8 ls_x = exp.classic.ljs.pos.x;
                  u8 ls_y = exp.classic.ljs.pos.y;
                  u8 rs_x = exp.classic.rjs.pos.x;
                  u8 rs_y = exp.classic.rjs.pos.y;

                  if(ls_x > 40)
                     state |= GX_CLASSIC_LSTICK_RIGHT;
                  if(ls_x < 25)
                     state |= GX_CLASSIC_LSTICK_LEFT;
                  if(ls_y > 45)
                     state |= GX_CLASSIC_LSTICK_UP;
                  if(ls_y < 20)
                     state |= GX_CLASSIC_LSTICK_DOWN;

                  if(rs_x > 40)
                     state |= GX_CLASSIC_RSTICK_RIGHT;
                  if(rs_x < 25)
                     state |= GX_CLASSIC_RSTICK_LEFT;
                  if(rs_y > 45)
                     state |= GX_CLASSIC_RSTICK_UP;
                  if(rs_y < 20)
                     state |= GX_CLASSIC_RSTICK_DOWN;
               }
               // do not return, fall through for wiimote d-pad
            }
            default:
               // rotated d-pad
               state |= (down & WPAD_BUTTON_UP) ? GX_WIIMOTE_LEFT : 0;
               state |= (down & WPAD_BUTTON_DOWN) ? GX_WIIMOTE_RIGHT : 0;
               state |= (down & WPAD_BUTTON_LEFT) ? GX_WIIMOTE_DOWN : 0;
               state |= (down & WPAD_BUTTON_RIGHT) ? GX_WIIMOTE_UP : 0;
               break;
         }
      }
#endif

      if ((state & (GX_GC_LSTICK_UP | GX_GC_RSTICK_UP | GX_GC_L_TRIGGER | GX_GC_R_TRIGGER)) == (GX_GC_LSTICK_UP | GX_GC_RSTICK_UP | GX_GC_L_TRIGGER | GX_GC_R_TRIGGER))
      {
         state |= GX_WIIMOTE_HOME;
      }

      if (quit_gc || quit_wiimote || quit_classic)
         state |= GX_QUIT_KEY;

      pad_state[port] = state;
   }

   if (g_quit)
   {
      pad_state[0] |= GX_WIIMOTE_HOME;
      g_quit = false;
   }
}

static bool wii_key_pressed(void *data, int key)
{
   (void)data;

   gx_video_t *gx = driver.video_data;

   switch (key)
   {
      case RARCH_QUIT_KEY:
      if(IS_TIMER_EXPIRED(gx))
      {
         uint64_t goto_menu_pressed_classic = pad_state[0] & GX_CLASSIC_HOME;
         uint64_t goto_menu_pressed_wiimote = pad_state[0] & GX_WIIMOTE_HOME;
         uint64_t quit_rarch = pad_state[0] & GX_QUIT_KEY;
         bool retval = false;
         g_console.menu_enable = ((goto_menu_pressed_classic || goto_menu_pressed_wiimote || quit_rarch) && IS_TIMER_EXPIRED(gx));

         if(g_console.menu_enable)
         {
            g_console.mode_switch = MODE_MENU;
	    SET_TIMER_EXPIRATION(gx, 30);
            retval = g_console.menu_enable;
         }

         if(quit_rarch)
            g_console.mode_switch = MODE_EXIT;

	 retval = g_console.menu_enable;
         return retval;
      }
      default:
         return false;
   }
}

static void wii_set_default_keybind_lut(unsigned device, unsigned port)
{
   (void)port;

   switch (device)
   {
#ifdef HW_RVL
      case GX_DEVICE_WIIMOTE:
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_B]      = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_B].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_Y]      = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_A].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_SELECT] = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_MINUS].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_START]  = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_PLUS].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_UP]     = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_UP].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_DOWN]   = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_DOWN].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_LEFT]   = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_LEFT].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_RIGHT]  = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_RIGHT].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_A]      = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_1].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_X]      = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_2].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_L]      = 0;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_R]      = 0;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_L2]     = 0;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_R2]     = 0;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_L3]     = 0;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_R3]     = 0;
         break;
      case GX_DEVICE_NUNCHUK:
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_B]      = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_B].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_Y]      = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_2].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_SELECT] = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_MINUS].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_START]  = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_PLUS].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_UP]     = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_UP].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_DOWN]   = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_DOWN].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_LEFT]   = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_LEFT].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_RIGHT]  = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_RIGHT].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_A]      = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_A].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_X]      = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_1].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_L]      = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_Z].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_R]      = platform_keys[GX_DEVICE_WIIMOTE_ID_JOYPAD_C].joykey;;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_L2]     = 0;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_R2]     = 0;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_L3]     = 0;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_R3]     = 0;
         break;
      case GX_DEVICE_CLASSIC:
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_B]      = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_B].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_Y]      = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_Y].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_SELECT] = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_MINUS].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_START]  = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_PLUS].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_UP]     = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_UP].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_DOWN]   = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_DOWN].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_LEFT]   = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_LEFT].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_RIGHT]  = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_RIGHT].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_A]      = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_A].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_X]      = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_X].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_L]      = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_L_TRIGGER].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_R]      = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_R_TRIGGER].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_L2]     = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_ZL_TRIGGER].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_R2]     = platform_keys[GX_DEVICE_CLASSIC_ID_JOYPAD_ZR_TRIGGER].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_L3]     = 0;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_R3]     = 0;
         break;
#endif
      case GX_DEVICE_GAMECUBE:
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_B]      = platform_keys[GX_DEVICE_GC_ID_JOYPAD_B].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_Y]      = platform_keys[GX_DEVICE_GC_ID_JOYPAD_Y].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_SELECT] = platform_keys[GX_DEVICE_GC_ID_JOYPAD_Z_TRIGGER].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_START]  = platform_keys[GX_DEVICE_GC_ID_JOYPAD_START].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_UP]     = platform_keys[GX_DEVICE_GC_ID_JOYPAD_UP].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_DOWN]   = platform_keys[GX_DEVICE_GC_ID_JOYPAD_DOWN].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_LEFT]   = platform_keys[GX_DEVICE_GC_ID_JOYPAD_LEFT].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_RIGHT]  = platform_keys[GX_DEVICE_GC_ID_JOYPAD_RIGHT].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_A]      = platform_keys[GX_DEVICE_GC_ID_JOYPAD_A].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_X]      = platform_keys[GX_DEVICE_GC_ID_JOYPAD_X].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_L]      = platform_keys[GX_DEVICE_GC_ID_JOYPAD_L_TRIGGER].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_R]      = platform_keys[GX_DEVICE_GC_ID_JOYPAD_R_TRIGGER].joykey;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_L2]     = 0;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_R2]     = 0;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_L3]     = 0;
         rarch_default_keybind_lut[RETRO_DEVICE_ID_JOYPAD_R3]     = 0;
         break;
      default:
         break;
   }
}

const input_driver_t input_wii = {
   .init = wii_input_initialize,
   .poll = wii_input_poll,
   .input_state = wii_input_state,
   .key_pressed = wii_key_pressed,
   .free = wii_free_input,
   .set_default_keybind_lut = wii_set_default_keybind_lut,
   .set_analog_dpad_mapping = wii_input_set_analog_dpad_mapping,
   .post_init = wii_input_post_init,
   .max_pads = MAX_PADS,
   .ident = "wii",
};
