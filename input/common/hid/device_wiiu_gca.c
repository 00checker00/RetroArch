/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2017 - Daniel De Matteis
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
#include "hid_device_driver.h"

#ifdef WII
static uint8_t activation_packet[] = { 0x01, 0x13 };
#else
static uint8_t activation_packet[] = { 0x13 };
#endif

#define GCA_PORT_INITIALIZING 0x00
#define GCA_PORT_POWERED      0x04
#define GCA_PORT_CONNECTED    0x10


typedef struct wiiu_gca_instance {
  void *handle;
  bool online;
  uint8_t device_state[37];
  joypad_connection_t *pads[4];
} wiiu_gca_instance_t;

typedef struct gca_pad_data
{
   void *gca_handle;     // instance handle for the GCA adapter
   hid_driver_t *driver; // HID system driver interface
   uint8_t data[9];      // pad data
   uint32_t slot;        // slot this pad occupies
   uint32_t buttons;     // digital button state
} gca_pad_t;


static void update_pad_state(wiiu_gca_instance_t *instance);
static void unregister_pad(wiiu_gca_instance_t *instance, int port);

extern pad_connection_interface_t wiiu_gca_pad_connection;

static void *wiiu_gca_init(void *handle)
{
   RARCH_LOG("[gca]: allocating driver instance...\n");
   wiiu_gca_instance_t *instance = calloc(1, sizeof(wiiu_gca_instance_t));
   if(instance == NULL) goto error;
   memset(instance, 0, sizeof(wiiu_gca_instance_t));
   instance->handle = handle;

   hid_instance.os_driver->send_control(handle, activation_packet, sizeof(activation_packet));
   hid_instance.os_driver->read(handle, instance->device_state, sizeof(instance->device_state));
   instance->online = true;

   RARCH_LOG("[gca]: init done\n");
   return instance;

   error:
      RARCH_ERR("[gca]: init failed\n");
      if(instance)
         free(instance);
      return NULL;
}

static void wiiu_gca_free(void *data) {
   wiiu_gca_instance_t *instance = (wiiu_gca_instance_t *)data;
   int i;

   if(instance) {
      instance->online = false;

      for(i = 0; i < 4; i++)
         unregister_pad(instance, i);

      free(instance);
   }
}

static void wiiu_gca_handle_packet(void *data, uint8_t *buffer, size_t size)
{
   wiiu_gca_instance_t *instance = (wiiu_gca_instance_t *)data;
   if(!instance || !instance->online)
   {
      RARCH_WARN("[gca]: instance null or not ready yet.\n");
      return;
   }

   if(size > sizeof(instance->device_state))
   {
      RARCH_WARN("[gca]: packet size %d is too big for buffer of size %d\n",
         size, sizeof(instance->device_state));
      return;
   }

   memcpy(instance->device_state, buffer, size);
   update_pad_state(instance);
}

static void update_pad_state(wiiu_gca_instance_t *instance)
{
   int i, port;
   unsigned char port_connected;

   if(!instance || !instance->online)
      return;

   joypad_connection_t *pad;
   /* process each pad */
   //RARCH_LOG_BUFFER(instance->device_state, 37);
   for(i = 1; i < 37; i += 9)
   {
      port = i / 9;
      pad = instance->pads[port];

      port_connected = instance->device_state[i] & GCA_PORT_CONNECTED;

      if(port_connected)
      {
         if(pad == NULL)
         {
            RARCH_LOG("[gca]: Gamepad at port %d connected.\n", port+1);
            instance->pads[port] = hid_pad_register(instance, &wiiu_gca_pad_connection);
            pad = instance->pads[port];
            if(pad == NULL)
            {
               RARCH_ERR("[gca]: Failed to register pad.\n");
               break;
            }
         }

         pad->iface->packet_handler(pad->data, &instance->device_state[i], 9);
      } else {
         if(pad != NULL) {
            RARCH_LOG("[gca]: Gamepad at port %d disconnected.\n", port+1);
            unregister_pad(instance, port);
         }
      }
   }
}

static void unregister_pad(wiiu_gca_instance_t *instance, int slot)
{
   if(!instance || slot < 0 || slot >= 4 || instance->pads[slot] == NULL)
      return;

   joypad_connection_t *pad = instance->pads[slot];
   instance->pads[slot] = NULL;

   hid_pad_deregister(pad);
}

static bool wiiu_gca_detect(uint16_t vendor_id, uint16_t product_id) {
   return vendor_id == VID_NINTENDO && product_id == PID_NINTENDO_GCA;
}

hid_device_t wiiu_gca_hid_device = {
   wiiu_gca_init,
   wiiu_gca_free,
   wiiu_gca_handle_packet,
   wiiu_gca_detect,
   "Wii U Gamecube Adapter"
};

/**
 * Pad connection interface implementation. This handles each individual
 * GC controller (as opposed to the above that handles the GCA itself).
 */

static void *wiiu_gca_pad_init(void *data, uint32_t slot, hid_driver_t *driver)
{
   gca_pad_t *pad = (gca_pad_t *)calloc(1, sizeof(gca_pad_t));

   if(!pad)
      return NULL;

   memset(pad, 0, sizeof(gca_pad_t));

   pad->gca_handle = data;
   pad->driver = driver;
   pad->slot = slot;

   return pad;
}

static void wiiu_gca_pad_deinit(void *data)
{
   gca_pad_t *pad = (gca_pad_t *)data;

   if(pad)
   {
      input_autoconfigure_disconnect(pad->slot, wiiu_gca_pad_connection.get_name(pad));
      free(pad);
   }
}

static void wiiu_gca_get_buttons(void *data, input_bits_t *state)
{
   gca_pad_t *pad = (gca_pad_t *)data;
   if(pad)
   {
      BITS_COPY16_PTR(state, pad->buttons);
   } else {
      BIT256_CLEAR_ALL_PTR(state);
   }
}

static void update_buttons(gca_pad_t *pad)
{
   uint32_t i, pressed_keys;

   static const uint32_t button_mapping[12] =
   {
      RETRO_DEVICE_ID_JOYPAD_A,
      RETRO_DEVICE_ID_JOYPAD_B,
      RETRO_DEVICE_ID_JOYPAD_X,
      RETRO_DEVICE_ID_JOYPAD_Y,
      RETRO_DEVICE_ID_JOYPAD_LEFT,
      RETRO_DEVICE_ID_JOYPAD_RIGHT,
      RETRO_DEVICE_ID_JOYPAD_DOWN,
      RETRO_DEVICE_ID_JOYPAD_UP,
      RETRO_DEVICE_ID_JOYPAD_START,
      RETRO_DEVICE_ID_JOYPAD_SELECT,
      RETRO_DEVICE_ID_JOYPAD_R,
      RETRO_DEVICE_ID_JOYPAD_L,
   };

   if(!pad)
      return;

   pressed_keys = pad->data[1] | (pad->data[2] << 8);
   pad->buttons = 0;

   for(i = 0; i < 12; i++)
      pad->buttons |= (pressed_keys & (1 << i)) ?
         (1 << button_mapping[i]) : 0;
}


/**
 * The USB packet provides a 9-byte data packet for each pad.
 *
 * byte 0: connection status (0x14 = connected, 0x04 = disconnected)
 * bytes 1-2: digital buttons
 * bytes 3-4: left analog stick x/y
 * bytes 5-6: right analog stick x/y
 * bytes 7-8: L/R analog state (note that these have digital buttons too)
 */
static void wiiu_gca_packet_handler(void *data, uint8_t *packet, uint16_t size)
{
   gca_pad_t *pad = (gca_pad_t *)data;
   uint32_t i, pressed_keys;

   if(!pad || !packet || size > sizeof(pad->data))
      return;

   memcpy(pad->data, packet, size);
   update_buttons(pad);
}



static void wiiu_gca_set_rumble(void *data, enum retro_rumble_effect effect, uint16_t strength)
{
   (void)data;
   (void)effect;
   (void)strength;
}

static unsigned decode_axis(unsigned axis)
{
  unsigned positive = AXIS_POS_GET(axis);
  unsigned negative = AXIS_NEG_GET(axis);

  return positive >= 4 ? negative : positive;
}

static int16_t wiiu_gca_get_axis(void *data, unsigned axis)
{
   axis = decode_axis(axis);
   int16_t val;
   gca_pad_t *pad = (gca_pad_t *)data;

   if(!pad || axis >= 4)
      return 0;

   val = pad->data[3+axis];

   switch(axis)
   {
      /* The Y axes are inverted. */
      case 0: /* left Y */
      case 2: /* right Y */
         val = 0x8000 - (val << 8);
         break;
      default:
         val = (val << 8) - 0x8000;
         break;
   }

   if(val > 0x1000 || val < -0x1000)
      return 0;

   return val;
}

static const char *wiiu_gca_get_name(void *data)
{
  gca_pad_t *pad = (gca_pad_t *)data;

  return "GameCube Controller";
}

/**
 * Button bitmask values:
 * 0x0001 - A   0x0010 - left    0x0100 - Start/Pause
 * 0x0002 - B   0x0020 - right   0x0200 - Z
 * 0x0004 - X   0x0040 - down    0x0400 - R
 * 0x0008 - Y   0x0080 - up      0x0800 - L
 */

static bool wiiu_gca_button(void *data, uint16_t joykey)
{
  gca_pad_t *pad = (gca_pad_t *)data;

  if(!pad || joykey > 31)
    return false;

  return pad->buttons & (1 << joykey);
}

pad_connection_interface_t wiiu_gca_pad_connection = {
   wiiu_gca_pad_init,
   wiiu_gca_pad_deinit,
   wiiu_gca_packet_handler,
   wiiu_gca_set_rumble,
   wiiu_gca_get_buttons,
   wiiu_gca_get_axis,
   wiiu_gca_get_name,
   wiiu_gca_button
};
