/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2017 - Daniel De Matteis
 *  Copyright (C) 2016-2017 - Gregor Richards
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
#include <sys/types.h>

#include "../../version.h"

#include <boolean.h>
#include <compat/strl.h>
#include <retro_assert.h>
#include <string/stdstring.h>
#include <net/net_http.h>

#ifdef HAVE_DISCORD
#include <discord/discord.h>
#endif

#include <file/file_path.h>

#include "netplay_discovery.h"
#include "netplay_private.h"

#include "../../configuration.h"
#include "../../frontend/frontend_driver.h"
#include "../../tasks/task_content.h"
#include "../../tasks/tasks_internal.h"
#include "../../file_path_special.h"
#include "../../paths.h"
#include "../../command.h"
#include "../../retroarch.h"

/* Only used before init_netplay */
static bool netplay_enabled                   = false;
static bool netplay_is_client                 = false;

/* Used while Netplay is running */
static netplay_t *netplay_data                = NULL;

/* Used for deferred netplay initialization */
static bool      netplay_client_deferred      = false;
static char      server_address_deferred[512] = "";
static unsigned  server_port_deferred         = 0;

/* Used */
static bool is_mitm                           = false;

static void netplay_disconnect(netplay_t *netplay);

#ifdef HAVE_DISCORD
/* TODO/FIXME - global */
extern bool discord_is_inited;
#endif

static bool netplay_is_alive(netplay_t *netplay)
{
   if (!netplay)
      return false;
   return (netplay->is_server) ||
          (!netplay->is_server && netplay->self_mode >= NETPLAY_CONNECTION_CONNECTED);
}

/**
 * If we're fast-forward replaying to resync, check if we
 * should actually show frame.
 */
static bool netplay_should_skip(netplay_t *netplay)
{
   return netplay->is_replay && (netplay->self_mode >= NETPLAY_CONNECTION_CONNECTED);
}

static bool netplay_get_self_input_state(netplay_t *netplay)
{
   unsigned i;
   struct delta_frame *ptr        = &netplay->buffer[netplay->self_ptr];
   netplay_input_state_t istate   = NULL;
   uint32_t devices, used_devices = 0, devi, dev_type, local_device;

   if (!netplay_delta_frame_ready(netplay, ptr, netplay->self_frame_count))
      return false;

   /* We've already read this frame! */
   if (ptr->have_local)
      return true;

   devices      = netplay->self_devices;
   used_devices = 0;

   for (devi = 0; devi < MAX_INPUT_DEVICES; devi++)
   {
      if (!(devices & (1<<devi)))
         continue;

      /* Find an appropriate local device */
      dev_type = netplay->config_devices[devi]&RETRO_DEVICE_MASK;

      for (local_device = 0; local_device < MAX_INPUT_DEVICES; local_device++)
      {
         if (used_devices & (1<<local_device))
            continue;
         if ((netplay->config_devices[local_device] & RETRO_DEVICE_MASK) 
               == dev_type)
            break;
      }

      if (local_device == MAX_INPUT_DEVICES)
         local_device = 0;
      used_devices |= (1<<local_device);

      istate = netplay_input_state_for(&ptr->real_input[devi],
            /* If we're a slave, we write our own input to 
             * MAX_CLIENTS to keep it separate */
            (netplay->self_mode==NETPLAY_CONNECTION_SLAVE)
            ? MAX_CLIENTS
            : netplay->self_client_num,
            netplay_expected_input_size(netplay, 1 << devi),
            true, false);
      if (!istate)
         continue; /* FIXME: More severe? */

      /* First frame we always give zero input since relying on
       * input from first frame screws up when we use -F 0. */
      if (     !input_driver_is_libretro_input_blocked()
            && netplay->self_frame_count > 0)
      {
         uint32_t *state        = istate->data;
         retro_input_state_t cb = netplay->cbs.state_cb;
         unsigned dtype         = netplay->config_devices[devi] & 
            RETRO_DEVICE_MASK;

         switch (dtype)
         {
            case RETRO_DEVICE_ANALOG:
               for (i = 0; i < 2; i++)
               {
                  int16_t tmp_x = cb(local_device,
                        RETRO_DEVICE_ANALOG, (unsigned)i, 0);
                  int16_t tmp_y = cb(local_device,
                        RETRO_DEVICE_ANALOG, (unsigned)i, 1);
                  state[1 + i] = (uint16_t)tmp_x | (((uint16_t)tmp_y) << 16);
               }
               /* no break */

            case RETRO_DEVICE_JOYPAD:
               for (i = 0; i <= RETRO_DEVICE_ID_JOYPAD_R3; i++)
               {
                  int16_t tmp = cb(local_device,
                        RETRO_DEVICE_JOYPAD, 0, (unsigned)i);
                  state[0] |= tmp ? 1 << i : 0;
               }
               break;

            case RETRO_DEVICE_MOUSE:
            case RETRO_DEVICE_LIGHTGUN:
            {
               int16_t tmp_x = cb(local_device, dtype, 0, 0);
               int16_t tmp_y = cb(local_device, dtype, 0, 1);
               state[1] = (uint16_t)tmp_x | (((uint16_t)tmp_y) << 16);
               for (i = 2;
                     i <= (unsigned)((dtype == RETRO_DEVICE_MOUSE) ?
                           RETRO_DEVICE_ID_MOUSE_HORIZ_WHEELDOWN :
                           RETRO_DEVICE_ID_LIGHTGUN_START);
                     i++)
               {
                  int16_t tmp = cb(local_device, dtype, 0,
                        (unsigned) i);
                  state[0] |= tmp ? 1 << i : 0;
               }
               break;
            }

            case RETRO_DEVICE_KEYBOARD:
            {
               unsigned key, word = 0, bit = 1;
               for (key = 1; key < NETPLAY_KEY_LAST; key++)
               {
                  state[word] |=
                        cb(local_device, RETRO_DEVICE_KEYBOARD,
                              0, netplay_key_ntoh(key)) ?
                              (1U << bit) : 0;
                  bit++;
                  if (bit >= 32)
                  {
                     bit = 0;
                     word++;
                     if (word >= istate->size)
                        break;
                  }
               }
               break;
            }
         }
      }
   }

   ptr->have_local = true;
   if (netplay->self_mode == NETPLAY_CONNECTION_PLAYING)
   {
      ptr->have_real[netplay->self_client_num]            = true;
      netplay->read_ptr[netplay->self_client_num]         = 
         NEXT_PTR(netplay->self_ptr);
      netplay->read_frame_count[netplay->self_client_num] = 
         netplay->self_frame_count + 1;
   }

   /* And send this input to our peers */
   for (i = 0; i < netplay->connections_size; i++)
   {
      struct netplay_connection *connection = &netplay->connections[i];
      if (  connection->active && 
            connection->mode >= NETPLAY_CONNECTION_CONNECTED)
         netplay_send_cur_input(netplay, &netplay->connections[i]);
   }

   /* Handle any delayed state changes */
   if (netplay->is_server)
      netplay_delayed_state_change(netplay);

   return true;
}

bool init_netplay_deferred(const char* server, unsigned port)
{
   if (!string_is_empty(server) && port != 0)
   {
      strlcpy(server_address_deferred, server,
            sizeof(server_address_deferred));
      server_port_deferred    = port;
      netplay_client_deferred = true;
   }
   else
      netplay_client_deferred = false;
   return netplay_client_deferred;
}

static void netplay_poll(netplay_t *netplay)
{
   size_t i;
   uint32_t client;
   bool blocking             = false;
   retro_time_t current_time = cpu_features_get_time_usec();

   /* Grab our own input state and send this frame's input state 
    * (self and remote) over the network */
   if (!netplay_get_self_input_state(netplay))
      goto catastrophe;

   /* If we're not connected, we're done */
   if (netplay->self_mode == NETPLAY_CONNECTION_NONE)
      return;

   /* Read Netplay input, block if we're configured 
    * to stall for input every frame */
   netplay_update_unread_ptr(netplay);

   blocking = netplay->stateless_mode &&
       (netplay->connected_players>1) &&
       netplay->unread_frame_count <= netplay->run_frame_count;

   if (netplay_poll_net_input(netplay, blocking) == -1)
      goto catastrophe;

   /* Resolve and/or simulate the input if we don't have real input */
   netplay_resolve_input(netplay, netplay->run_ptr, false);

   /* Handle any slaves */
   if (netplay->is_server && netplay->connected_slaves)
      netplay_handle_slaves(netplay);

   netplay_update_unread_ptr(netplay);

   /* Figure out how many frames of input latency we should be using to hide
    * network latency */
   if (netplay->frame_run_time_avg || netplay->stateless_mode)
   {
      /* FIXME: Using fixed 60fps for this calculation */
      unsigned frames_per_frame     = netplay->frame_run_time_avg ?
         (16666 / netplay->frame_run_time_avg) :
         0;
      unsigned frames_ahead         = (netplay->run_frame_count > netplay->unread_frame_count) ?
         (netplay->run_frame_count - netplay->unread_frame_count) :
         0;
      settings_t *settings          = config_get_ptr();
      unsigned netplay_frames_min   = settings->uints.netplay_input_latency_frames_min;
      unsigned netplay_frames_range = settings->uints.netplay_input_latency_frames_range;
      bool runahead_enabled         = settings->bools.run_ahead_enabled;
      unsigned runahead_frames      = settings->uints.run_ahead_frames;
      int input_latency_frames_min  = netplay_frames_min -
         (runahead_enabled ? runahead_frames : 0);
      int input_latency_frames_max  = input_latency_frames_min + netplay_frames_range;

      /* Assume we need a couple frames worth of time to actually run the
       * current frame */
      if (frames_per_frame > 2)
         frames_per_frame -= 2;
      else
         frames_per_frame = 0;

      /* Shall we adjust our latency? */
      if (netplay->stateless_mode)
      {
         /* In stateless mode, we adjust up if we're "close" and down if we
          * have a lot of slack */
         if (netplay->input_latency_frames < input_latency_frames_min ||
               (netplay->unread_frame_count == netplay->run_frame_count + 1 &&
                netplay->input_latency_frames < input_latency_frames_max))
            netplay->input_latency_frames++;
         else if (netplay->input_latency_frames > input_latency_frames_max ||
               (netplay->unread_frame_count > netplay->run_frame_count + 2 &&
                netplay->input_latency_frames > input_latency_frames_min))
            netplay->input_latency_frames--;
      }
      else if (netplay->input_latency_frames < input_latency_frames_min ||
               (frames_per_frame < frames_ahead &&
                netplay->input_latency_frames < input_latency_frames_max))
      {
         /* We can't hide this much network latency with replay, so hide some
          * with input latency */
         netplay->input_latency_frames++;
      }
      else if (netplay->input_latency_frames > input_latency_frames_max ||
               (frames_per_frame > frames_ahead + 2 &&
                netplay->input_latency_frames > input_latency_frames_min))
      {
         /* We don't need this much latency (any more) */
         netplay->input_latency_frames--;
      }
   }

   /* If we're stalled, consider unstalling */
   switch (netplay->stall)
   {
      case NETPLAY_STALL_RUNNING_FAST:
         {
            if (netplay->unread_frame_count + NETPLAY_MAX_STALL_FRAMES - 2
                  > netplay->self_frame_count)
            {
               netplay->stall = NETPLAY_STALL_NONE;
               for (i = 0; i < netplay->connections_size; i++)
               {
                  struct netplay_connection *connection = &netplay->connections[i];
                  if (connection->active && connection->stall)
                     connection->stall = NETPLAY_STALL_NONE;
               }
            }
            break;
         }

      case NETPLAY_STALL_SPECTATOR_WAIT:
         if (netplay->self_mode == NETPLAY_CONNECTION_PLAYING || netplay->unread_frame_count > netplay->self_frame_count)
            netplay->stall = NETPLAY_STALL_NONE;
         break;

      case NETPLAY_STALL_INPUT_LATENCY:
         /* Just let it recalculate momentarily */
         netplay->stall = NETPLAY_STALL_NONE;
         break;

      case NETPLAY_STALL_SERVER_REQUESTED:
         /* See if the stall is done */
         if (netplay->connections[0].stall_frame == 0)
         {
            /* Stop stalling! */
            netplay->connections[0].stall = NETPLAY_STALL_NONE;
            netplay->stall = NETPLAY_STALL_NONE;
         }
         else
            netplay->connections[0].stall_frame--;
         break;
      case NETPLAY_STALL_NO_CONNECTION:
      default: /* not stalling */
         break;
   }

   /* If we're not stalled, consider stalling */
   if (!netplay->stall)
   {
      /* Have we not read enough latency frames? */
      if (netplay->self_mode == NETPLAY_CONNECTION_PLAYING &&
          netplay->connected_players &&
          netplay->run_frame_count + netplay->input_latency_frames > netplay->self_frame_count)
      {
         netplay->stall      = NETPLAY_STALL_INPUT_LATENCY;
         netplay->stall_time = 0;
      }

      /* Are we too far ahead? */
      if (netplay->unread_frame_count + NETPLAY_MAX_STALL_FRAMES
            <= netplay->self_frame_count)
      {
         netplay->stall      = NETPLAY_STALL_RUNNING_FAST;
         netplay->stall_time = current_time;

         /* Figure out who to blame */
         if (netplay->is_server)
         {
            for (client = 1; client < MAX_CLIENTS; client++)
            {
               struct netplay_connection *connection;
               if (!(netplay->connected_players & (1<<client)))
                  continue;
               if (  netplay->read_frame_count[client] > 
                     netplay->unread_frame_count)
                  continue;
               connection = &netplay->connections[client-1];
               if (connection->active &&
                   connection->mode == NETPLAY_CONNECTION_PLAYING)
               {
                  connection->stall      = NETPLAY_STALL_RUNNING_FAST;
                  connection->stall_time = netplay->stall_time;
               }
            }
         }

      }

      /* If we're a spectator, are we ahead at all? */
      if (!netplay->is_server &&
          (netplay->self_mode == NETPLAY_CONNECTION_SPECTATING ||
           netplay->self_mode == NETPLAY_CONNECTION_SLAVE) &&
          netplay->unread_frame_count <= netplay->self_frame_count)
      {
         netplay->stall      = NETPLAY_STALL_SPECTATOR_WAIT;
         netplay->stall_time = current_time;
      }
   }

   /* If we're stalling, consider disconnection */
   if (netplay->stall && netplay->stall_time)
   {
      retro_time_t now = current_time;

      /* Don't stall out while they're paused */
      if (netplay->remote_paused)
         netplay->stall_time = now;
      else if (
            now - netplay->stall_time >=
            (netplay->is_server 
             ? MAX_SERVER_STALL_TIME_USEC 
             : MAX_CLIENT_STALL_TIME_USEC)
            )
      {
         /* Stalled out! */
         if (!netplay->is_server)
            goto catastrophe;

         {
            bool fixed = false;
            for (i = 0; i < netplay->connections_size; i++)
            {
               struct netplay_connection *connection = &netplay->connections[i];
               if (connection->active &&
                   connection->mode == NETPLAY_CONNECTION_PLAYING &&
                   connection->stall)
               {
                  netplay_hangup(netplay, connection);
                  fixed = true;
               }
            }

            if (fixed)
            {
               /* Not stalled now */
               netplay->stall = NETPLAY_STALL_NONE;
               return;
            }
         }
      }
   }

   return;

catastrophe:
   for (i = 0; i < netplay->connections_size; i++)
      netplay_hangup(netplay, &netplay->connections[i]);
}

/* Poll the network if necessary */
void input_poll_net(netplay_t *netplay)
{
   if (!netplay_should_skip(netplay) && netplay->can_poll)
   {
      netplay->can_poll         = false;

      /* Polls network to see if we have anything new. If our
       * network buffer is full, we simply have to block
       * for new input data.
       */
      netplay_poll(netplay);
   }
}

/* Netplay polling callbacks */
void video_frame_net(const void *data, unsigned width,
      unsigned height, size_t pitch)
{
   netplay_t *netplay = netplay_data;
   if (!netplay_should_skip(netplay))
      netplay->cbs.frame_cb(data, width, height, pitch);
}

void audio_sample_net(int16_t left, int16_t right)
{
   netplay_t *netplay = netplay_data;
   if (!netplay_should_skip(netplay) && !netplay->stall)
      netplay->cbs.sample_cb(left, right);
}

size_t audio_sample_batch_net(const int16_t *data, size_t frames)
{
   netplay_t *netplay = netplay_data;
   if (!netplay_should_skip(netplay) && !netplay->stall)
      return netplay->cbs.sample_batch_cb(data, frames);
   return frames;
}

static int16_t netplay_input_state(netplay_t *netplay,
      unsigned port, unsigned device,
      unsigned idx, unsigned id)
{
   netplay_input_state_t istate;
   struct delta_frame *delta        = NULL;
   const uint32_t *curr_input_state = NULL;
   size_t ptr                       = netplay->is_replay 
      ? netplay->replay_ptr 
      : netplay->run_ptr;

   if (port >= MAX_INPUT_DEVICES)
      return 0;

   /* If the port doesn't seem to correspond to the device, "correct" it. This
    * is common with devices that typically only have one instance, such as
    * keyboards, mice and lightguns. */
   if (device != RETRO_DEVICE_JOYPAD &&
       (netplay->config_devices[port]&RETRO_DEVICE_MASK) != device)
   {
      for (port = 0; port < MAX_INPUT_DEVICES; port++)
      {
         if ((netplay->config_devices[port]&RETRO_DEVICE_MASK) == device)
            break;
      }
      if (port == MAX_INPUT_DEVICES)
         return 0;
   }

   delta  = &netplay->buffer[ptr];
   istate = delta->resolved_input[port];
   if (!istate || !istate->used || istate->size == 0)
      return 0;

   curr_input_state = istate->data;

   switch (device)
   {
      case RETRO_DEVICE_JOYPAD:
         if (id == RETRO_DEVICE_ID_JOYPAD_MASK)
            return curr_input_state[0];
         return ((1 << id) & curr_input_state[0]) ? 1 : 0;
      case RETRO_DEVICE_ANALOG:
         if (istate->size == 3)
         {
            uint32_t state = curr_input_state[1 + idx];
            return (int16_t)(uint16_t)(state >> (id * 16));
         }
         break;
      case RETRO_DEVICE_MOUSE:
      case RETRO_DEVICE_LIGHTGUN:
         if (istate->size == 2)
         {
            if (id <= RETRO_DEVICE_ID_MOUSE_Y)
               return (int16_t)(uint16_t)(curr_input_state[1] >> (id * 16));
            return ((1 << id) & curr_input_state[0]) ? 1 : 0;
         }
         break;
      case RETRO_DEVICE_KEYBOARD:
         {
            unsigned key = netplay_key_hton(id);
            if (key != NETPLAY_KEY_UNKNOWN)
            {
               unsigned word = key / 32;
               unsigned bit  = key % 32;
               if (word <= istate->size)
                  return ((1U<<bit) & curr_input_state[word]) ? 1 : 0;
            }
         }
         break;
      default:
         break;
   }

   return 0;
}

static void netplay_announce_cb(retro_task_t *task,
      void *task_data, void *user_data, const char *error)
{
   if (task_data)
   {
      unsigned i, ip_len, port_len;
      http_transfer_data_t *data     = (http_transfer_data_t*)task_data;
      struct netplay_room *host_room = netplay_get_host_room();
      struct string_list *lines      = NULL;
      char *mitm_ip                  = NULL;
      char *mitm_port                = NULL;
      char *buf                      = NULL;
      char *host_string              = NULL;

      if (data->len == 0)
      {
         free(task_data);
         return;
      }

      buf = (char*)calloc(1, data->len + 1);

      memcpy(buf, data->data, data->len);

      lines = string_split(buf, "\n");

      if (lines->size == 0)
      {
         string_list_free(lines);
         free(buf);
         free(task_data);
         return;
      }

      memset(host_room, 0, sizeof(*host_room));

      for (i = 0; i < lines->size; i++)
      {
         const char *line = lines->elems[i].data;

         if (!string_is_empty(line))
         {
            struct string_list 
               *kv          = string_split(line, "=");
            const char *key = NULL;
            const char *val = NULL;

            if (!kv)
               continue;

            if (kv->size != 2)
            {
               string_list_free(kv);
               continue;
            }

            key = kv->elems[0].data;
            val = kv->elems[1].data;

            if (string_is_equal(key, "id"))
               sscanf(val, "%i", &host_room->id);
            if (string_is_equal(key, "username"))
               strlcpy(host_room->nickname, val,
                     sizeof(host_room->nickname));
            if (string_is_equal(key, "ip"))
               strlcpy(host_room->address, val,
                     sizeof(host_room->address));
            if (string_is_equal(key, "mitm_ip"))
            {
               mitm_ip = strdup(val);
               strlcpy(host_room->mitm_address, val,
                     sizeof(host_room->mitm_address));
            }
            if (string_is_equal(key, "port"))
               sscanf(val, "%i", &host_room->port);
            if (string_is_equal(key, "mitm_port"))
            {
               mitm_port = strdup(val);
               sscanf(mitm_port, "%i", &host_room->mitm_port);
            }
            if (string_is_equal(key, "core_name"))
               strlcpy(host_room->corename, val,
                     sizeof(host_room->corename));
            if (string_is_equal(key, "frontend"))
               strlcpy(host_room->frontend, val,
                     sizeof(host_room->frontend));
            if (string_is_equal(key, "core_version"))
               strlcpy(host_room->coreversion, val,
                     sizeof(host_room->coreversion));
            if (string_is_equal(key, "game_name"))
               strlcpy(host_room->gamename, val,
                     sizeof(host_room->gamename));
            if (string_is_equal(key, "game_crc"))
               sscanf(val, "%08d", &host_room->gamecrc);
            if (string_is_equal(key, "host_method"))
               sscanf(val, "%i", &host_room->host_method);
            if (string_is_equal(key, "has_password"))
            {
               if (     string_is_equal_noncase(val, "true") 
                     || string_is_equal(val, "1"))
                  host_room->has_password = true;
               else
                  host_room->has_password = false;
            }
            if (string_is_equal(key, "has_spectate_password"))
            {
               if (  string_is_equal_noncase(val, "true") || 
                     string_is_equal(val, "1"))
                  host_room->has_spectate_password = true;
               else
                  host_room->has_spectate_password = false;
            }
            if (string_is_equal(key, "fixed"))
            {
               if (     string_is_equal_noncase(val, "true") 
                     || string_is_equal(val, "1"))
                  host_room->fixed = true;
               else
                  host_room->fixed = false;
            }
            if (string_is_equal(key, "retroarch_version"))
               strlcpy(host_room->retroarch_version,
                     val, sizeof(host_room->retroarch_version));
            if (string_is_equal(key, "country"))
               strlcpy(host_room->country,
                     val, sizeof(host_room->country));

            string_list_free(kv);
         }
      }

      if (mitm_ip && mitm_port)
      {
         ip_len   = (unsigned)strlen(mitm_ip);
         port_len = (unsigned)strlen(mitm_port);

         /* Enable Netplay client mode */
         if (netplay_driver_ctl(RARCH_NETPLAY_CTL_IS_DATA_INITED, NULL))
         {
            command_event(CMD_EVENT_NETPLAY_DEINIT, NULL);
            is_mitm                = true;
            host_room->host_method = NETPLAY_HOST_METHOD_MITM;
         }

         netplay_driver_ctl(RARCH_NETPLAY_CTL_ENABLE_CLIENT, NULL);

         host_string = (char*)calloc(1, ip_len + port_len + 2);

         memcpy(host_string, mitm_ip, ip_len);
         memcpy(host_string + ip_len, "|", 1);
         memcpy(host_string + ip_len + 1, mitm_port, port_len);

         /* Enable Netplay */
         command_event(CMD_EVENT_NETPLAY_INIT_DIRECT_DEFERRED,
               (void*)host_string);
         command_event(CMD_EVENT_NETPLAY_INIT, (void*)host_string);

         free(host_string);
      }

#ifdef HAVE_DISCORD
      if (discord_is_inited)
      {
         discord_userdata_t userdata;
         userdata.status = DISCORD_PRESENCE_NETPLAY_HOSTING;
         command_event(CMD_EVENT_DISCORD_UPDATE, &userdata);
      }
#endif

      string_list_free(lines);
      free(buf);
      free(task_data);
      if (mitm_ip)
         free(mitm_ip);
      if (mitm_port)
         free(mitm_port);
   }
}

static void netplay_announce(void)
{
   char buf[4600];
   char frontend_architecture[PATH_MAX_LENGTH];
   char frontend_architecture_tmp[PATH_MAX_LENGTH];
   const frontend_ctx_driver_t *frontend_drv =  NULL;
   char url[2048]                   = "http://lobby.libretro.com/add/";
   char *username                   = NULL;
   char *corename                   = NULL;
   char *gamename                   = NULL;
   char *subsystemname              = NULL;
   char *coreversion                = NULL;
   char *frontend_ident             = NULL;
   settings_t *settings             = config_get_ptr();
   struct retro_system_info *system = runloop_get_libretro_system_info();
   uint32_t content_crc             = content_get_crc();
   struct string_list *subsystem    = path_get_subsystem_list();

   buf[0] = '\0';

   if (subsystem)
   {
      unsigned i;

      for (i = 0; i < subsystem->size; i++)
      {
         strlcat(buf, path_basename(subsystem->elems[i].data), sizeof(buf));
         if (i < subsystem->size - 1)
            strlcat(buf, "|", sizeof(buf));
      }
      net_http_urlencode(&gamename, buf);
      net_http_urlencode(&subsystemname, path_get(RARCH_PATH_SUBSYSTEM));
      content_crc = 0;
   }
   else
   {
      net_http_urlencode(&gamename,
         !string_is_empty(path_basename(path_get(RARCH_PATH_BASENAME))) ?
         path_basename(path_get(RARCH_PATH_BASENAME)) : "N/A");
      net_http_urlencode(&subsystemname, "N/A");
   }

   frontend_drv =  
      (const frontend_ctx_driver_t*)frontend_driver_get_cpu_architecture_str(
            frontend_architecture_tmp, sizeof(frontend_architecture_tmp));
   snprintf(frontend_architecture, sizeof(frontend_architecture), "%s %s",
         frontend_drv->ident, frontend_architecture_tmp);

#ifdef HAVE_DISCORD
   if (discord_is_ready())
      net_http_urlencode(&username, discord_get_own_username());
   else
#endif
   net_http_urlencode(&username, settings->paths.username);
   net_http_urlencode(&corename, system->library_name);
   net_http_urlencode(&coreversion, system->library_version);
   net_http_urlencode(&frontend_ident, frontend_architecture);

   buf[0] = '\0';

   snprintf(buf, sizeof(buf), "username=%s&core_name=%s&core_version=%s&"
      "game_name=%s&game_crc=%08X&port=%d&mitm_server=%s"
      "&has_password=%d&has_spectate_password=%d&force_mitm=%d"
      "&retroarch_version=%s&frontend=%s&subsystem_name=%s",
      username, corename, coreversion, gamename, content_crc,
      settings->uints.netplay_port,
      settings->arrays.netplay_mitm_server,
      *settings->paths.netplay_password ? 1 : 0,
      *settings->paths.netplay_spectate_password ? 1 : 0,
      settings->bools.netplay_use_mitm_server,
      PACKAGE_VERSION, frontend_architecture, subsystemname);
   task_push_http_post_transfer(url, buf, true, NULL,
         netplay_announce_cb, NULL);

   if (username)
      free(username);
   if (corename)
      free(corename);
   if (gamename)
      free(gamename);
   if (coreversion)
      free(coreversion);
   if (frontend_ident)
      free(frontend_ident);
}

int16_t input_state_net(unsigned port, unsigned device,
      unsigned idx, unsigned id)
{
   netplay_t *netplay = netplay_data;
   /* Check if input port/index is controlled by netplay or not. */
   if (netplay_is_alive(netplay))
      return netplay_input_state(netplay, port, device, idx, id);
   return netplay->cbs.state_cb(port, device, idx, id);
}

/* Inform Netplay of the frontend's pause state (paused or otherwise) */
static void netplay_frontend_paused(netplay_t *netplay, bool paused)
{
   size_t i;
   uint32_t paused_ct = 0;

   /* Nothing to do if we already knew this */
   if (netplay->local_paused == paused)
      return;

   netplay->local_paused = paused;

   /* Communicating this is a bit odd: If exactly one other connection is
    * paused, then we must tell them that we're unpaused, as from their
    * perspective we are. If more than one other connection is paused, then our
    * status as proxy means we are NOT unpaused to either of them. */
   for (i = 0; i < netplay->connections_size; i++)
   {
      struct netplay_connection *connection = &netplay->connections[i];
      if (connection->active && connection->paused)
         paused_ct++;
   }
   if (paused_ct > 1)
      return;

   /* Send our unpaused status. Must send manually 
    * because we must immediately flush the buffer: 
    * If we're paused, we won't be polled. */
   for (i = 0; i < netplay->connections_size; i++)
   {
      struct netplay_connection *connection = &netplay->connections[i];
      if (  connection->active && 
            connection->mode >= NETPLAY_CONNECTION_CONNECTED)
      {
         if (paused)
            netplay_send_raw_cmd(netplay, connection, NETPLAY_CMD_PAUSE,
               netplay->nick, NETPLAY_NICK_LEN);
         else
            netplay_send_raw_cmd(netplay, connection, NETPLAY_CMD_RESUME,
               NULL, 0);

         /* We're not going to be polled, so we need 
          * to flush this command now */
         netplay_send_flush(&connection->send_packet_buffer,
               connection->fd, true);
      }
   }
}

static bool netplay_pre_frame(netplay_t *netplay)
{
   static int reannounce           = 0;
   bool sync_stalled               = false;
   settings_t *settings            = config_get_ptr();
   bool netplay_public_announce    = settings->bools.netplay_public_announce;
   bool netplay_use_mitm_server    = settings->bools.netplay_use_mitm_server;
   const char *netplay_password    = settings->paths.netplay_password;
   const char *netplay_spectate_pw = settings->paths.netplay_spectate_password;

   retro_assert(netplay);

   if (netplay_public_announce)
   {
      reannounce++;
      if ((netplay->is_server || is_mitm) && (reannounce % 600 == 0))
         netplay_announce();
   }
   /* Make sure that if announcement is turned on mid-game, it gets announced */
   else
      reannounce = -1;

   /* FIXME: This is an ugly way to learn we're not paused anymore */
   if (netplay->local_paused)
      netplay_frontend_paused(netplay, false);

   /* Are we ready now? */
   if (netplay->quirks & NETPLAY_QUIRK_INITIALIZATION)
      netplay_try_init_serialization(netplay);

   if (netplay->is_server && !netplay_use_mitm_server)
   {
      /* Advertise our server */
      netplay_lan_ad_server(netplay);

      /* NAT traversal if applicable */
      if (netplay->nat_traversal &&
          !netplay->nat_traversal_task_oustanding &&
          netplay->nat_traversal_state.request_outstanding &&
          !netplay->nat_traversal_state.have_inet4)
      {
         struct timeval tmptv = {0};
         fd_set fds = netplay->nat_traversal_state.fds;
         if (socket_select(netplay->nat_traversal_state.nfds,
                  &fds, NULL, NULL, &tmptv) > 0)
            natt_read(&netplay->nat_traversal_state);

#ifndef HAVE_SOCKET_LEGACY
         if (!netplay->nat_traversal_state.request_outstanding ||
             netplay->nat_traversal_state.have_inet4)
            netplay_announce_nat_traversal(netplay);
#endif
      }
   }

   sync_stalled = !netplay_sync_pre_frame(netplay,
         netplay_password,
         netplay_spectate_pw
         );

   /* If we're disconnected, deinitialize */
   if (!netplay->is_server && !netplay->connections[0].active)
   {
      netplay_disconnect(netplay);
      return true;
   }

   if (sync_stalled ||
       ((!netplay->is_server || (netplay->connected_players>1)) &&
        (netplay->stall || netplay->remote_paused)))
   {
      /* We may have received data even if we're stalled, 
       * so run post-frame sync */
      netplay_sync_post_frame(netplay, true);
      return false;
   }
   return true;
}

static void netplay_post_frame(netplay_t *netplay)
{
   size_t i;
   retro_assert(netplay);
   netplay_update_unread_ptr(netplay);
   netplay_sync_post_frame(netplay, false);

   for (i = 0; i < netplay->connections_size; i++)
   {
      struct netplay_connection *connection = &netplay->connections[i];
      if (connection->active &&
          !netplay_send_flush(&connection->send_packet_buffer,
             connection->fd, false))
         netplay_hangup(netplay, connection);
   }

   /* If we're disconnected, deinitialize */
   if (!netplay->is_server && !netplay->connections[0].active)
      netplay_disconnect(netplay);
}

/**
 * netplay_force_future
 * @netplay              : pointer to netplay object
 *
 * Force netplay to ignore all past input, typically because 
 * we've just loaded a state or reset.
 */
static void netplay_force_future(netplay_t *netplay)
{
   /* Wherever we're inputting, that's where we consider our 
    * state to be loaded */
   netplay->run_ptr         = netplay->self_ptr;
   netplay->run_frame_count = netplay->self_frame_count;

   /* We need to ignore any intervening data from the other side,
    * and never rewind past this */
   netplay_update_unread_ptr(netplay);

   if (netplay->unread_frame_count < netplay->run_frame_count)
   {
      uint32_t client;
      for (client = 0; client < MAX_CLIENTS; client++)
      {
         if (!(netplay->connected_players & (1<<client)))
            continue;

         if (netplay->read_frame_count[client] < netplay->run_frame_count)
         {
            netplay->read_ptr[client] = netplay->run_ptr;
            netplay->read_frame_count[client] = netplay->run_frame_count;
         }
      }
      if (netplay->server_frame_count < netplay->run_frame_count)
      {
         netplay->server_ptr = netplay->run_ptr;
         netplay->server_frame_count = netplay->run_frame_count;
      }
      netplay_update_unread_ptr(netplay);
   }
   if (netplay->other_frame_count < netplay->run_frame_count)
   {
      netplay->other_ptr = netplay->run_ptr;
      netplay->other_frame_count = netplay->run_frame_count;
   }
}

static void netplay_send_savestate(netplay_t *netplay,
   retro_ctx_serialize_info_t *serial_info, uint32_t cx,
   struct compression_transcoder *z)
{
   uint32_t header[4];
   uint32_t rd, wn;
   size_t i;

   /* Compress it */
   z->compression_backend->set_in(z->compression_stream,
      (const uint8_t*)serial_info->data_const, (uint32_t)serial_info->size);
   z->compression_backend->set_out(z->compression_stream,
      netplay->zbuffer, (uint32_t)netplay->zbuffer_size);
   if (!z->compression_backend->trans(z->compression_stream, true, &rd,
         &wn, NULL))
   {
      /* Catastrophe! */
      for (i = 0; i < netplay->connections_size; i++)
         netplay_hangup(netplay, &netplay->connections[i]);
      return;
   }

   /* Send it to relevant peers */
   header[0] = htonl(NETPLAY_CMD_LOAD_SAVESTATE);
   header[1] = htonl(wn + 2*sizeof(uint32_t));
   header[2] = htonl(netplay->run_frame_count);
   header[3] = htonl(serial_info->size);

   for (i = 0; i < netplay->connections_size; i++)
   {
      struct netplay_connection *connection = &netplay->connections[i];
      if (!connection->active ||
          connection->mode < NETPLAY_CONNECTION_CONNECTED ||
          connection->compression_supported != cx)
         continue;

      if (!netplay_send(&connection->send_packet_buffer, connection->fd, header,
            sizeof(header)) ||
          !netplay_send(&connection->send_packet_buffer, connection->fd,
            netplay->zbuffer, wn))
         netplay_hangup(netplay, connection);
   }
}

/**
 * netplay_load_savestate
 * @netplay              : pointer to netplay object
 * @serial_info          : the savestate being loaded, NULL means
 *                         "load it yourself"
 * @save                 : Whether to save the provided serial_info
 *                         into the frame buffer
 *
 * Inform Netplay of a savestate load and send it to the other side
 **/
void netplay_load_savestate(netplay_t *netplay,
      retro_ctx_serialize_info_t *serial_info, bool save)
{
   retro_ctx_serialize_info_t tmp_serial_info;

   netplay_force_future(netplay);

   /* Record it in our own buffer */
   if (save || !serial_info)
   {
      /* FIXME: This is a critical failure! */
      if (!netplay_delta_frame_ready(netplay,
               &netplay->buffer[netplay->run_ptr], netplay->run_frame_count))
         return;

      if (!serial_info)
      {
         tmp_serial_info.size = netplay->state_size;
         tmp_serial_info.data = netplay->buffer[netplay->run_ptr].state;
         if (!core_serialize(&tmp_serial_info))
            return;
         tmp_serial_info.data_const = tmp_serial_info.data;
         serial_info = &tmp_serial_info;
      }
      else
      {
         if (serial_info->size <= netplay->state_size)
            memcpy(netplay->buffer[netplay->run_ptr].state,
                  serial_info->data_const, serial_info->size);
      }
   }

   /* Don't send it if we're expected to be desynced */
   if (netplay->desync)
      return;

   /* If we can't send it to the peer, loading a state was a bad idea */
   if (netplay->quirks & (
              NETPLAY_QUIRK_NO_SAVESTATES
            | NETPLAY_QUIRK_NO_TRANSMISSION))
      return;

   /* Send this to every peer 
    *
    * Send a loaded savestate to those connected peers using the 
    * given compression scheme.
    */
   if (netplay->compress_nil.compression_backend)
      netplay_send_savestate(netplay, serial_info, 0, &netplay->compress_nil);
   if (netplay->compress_zlib.compression_backend)
      netplay_send_savestate(netplay, serial_info, NETPLAY_COMPRESSION_ZLIB,
         &netplay->compress_zlib);
}

static void netplay_core_reset(netplay_t *netplay)
{
   uint32_t cmd[3];
   size_t i;

   /* Ignore past input */
   netplay_force_future(netplay);

   /* Request that our peers reset */
   cmd[0] = htonl(NETPLAY_CMD_RESET);
   cmd[1] = htonl(sizeof(uint32_t));
   cmd[2] = htonl(netplay->self_frame_count);

   for (i = 0; i < netplay->connections_size; i++)
   {
      struct netplay_connection *connection = &netplay->connections[i];
      if (!connection->active ||
            connection->mode < NETPLAY_CONNECTION_CONNECTED)
         continue;

      if (!netplay_send(&connection->send_packet_buffer, connection->fd, cmd,
               sizeof(cmd)))
         netplay_hangup(netplay, connection);
   }
}

/**
 * netplay_settings_share_mode
 *
 * Get the preferred share mode
 */
uint8_t netplay_settings_share_mode(unsigned share_digital,
      unsigned share_analog)
{
   if (share_digital || share_analog)
   {
      uint8_t share_mode     = 0;

      switch (share_digital)
      {
         case RARCH_NETPLAY_SHARE_DIGITAL_OR:
            share_mode |= NETPLAY_SHARE_DIGITAL_OR;
            break;
         case RARCH_NETPLAY_SHARE_DIGITAL_XOR:
            share_mode |= NETPLAY_SHARE_DIGITAL_XOR;
            break;
         case RARCH_NETPLAY_SHARE_DIGITAL_VOTE:
            share_mode |= NETPLAY_SHARE_DIGITAL_VOTE;
            break;
         default:
            share_mode |= NETPLAY_SHARE_NO_PREFERENCE;
      }

      switch (share_analog)
      {
         case RARCH_NETPLAY_SHARE_ANALOG_MAX:
            share_mode |= NETPLAY_SHARE_ANALOG_MAX;
            break;
         case RARCH_NETPLAY_SHARE_ANALOG_AVERAGE:
            share_mode |= NETPLAY_SHARE_ANALOG_AVERAGE;
            break;
         default:
            share_mode |= NETPLAY_SHARE_NO_PREFERENCE;
      }

      return share_mode;
   }
   return 0;
}

/* Toggle between play mode and spectate mode */
static void netplay_toggle_play_spectate(netplay_t *netplay)
{
   switch (netplay->self_mode)
   {
      case NETPLAY_CONNECTION_PLAYING:
      case NETPLAY_CONNECTION_SLAVE:
         /* Switch to spectator mode immediately */
         netplay->self_mode = NETPLAY_CONNECTION_SPECTATING;
         netplay_cmd_mode(netplay, NETPLAY_CONNECTION_SPECTATING);
         break;
      case NETPLAY_CONNECTION_SPECTATING:
         /* Switch only after getting permission */
         netplay_cmd_mode(netplay, NETPLAY_CONNECTION_PLAYING);
         break;
      default:
         break;
   }
}

static void netplay_disconnect(netplay_t *netplay)
{
   size_t i;

   for (i = 0; i < netplay->connections_size; i++)
      netplay_hangup(netplay, &netplay->connections[i]);

   deinit_netplay();

#ifdef HAVE_DISCORD
   if (discord_is_inited)
   {
      discord_userdata_t userdata;
      userdata.status = DISCORD_PRESENCE_NETPLAY_NETPLAY_STOPPED;
      command_event(CMD_EVENT_DISCORD_UPDATE, &userdata);
   }
#endif
}

void deinit_netplay(void)
{
   if (netplay_data)
   {
      netplay_free(netplay_data);
      netplay_enabled   = false;
      netplay_is_client = false;
      is_mitm           = false;
   }
   netplay_data = NULL;
   core_unset_netplay_callbacks();
}

/**
 * init_netplay
 * @direct_host          : Host to connect to directly, if applicable (client only)
 * @server               : server address to connect to (client only)
 * @port                 : TCP port to host on/connect to
 *
 * Initializes netplay.
 *
 * If netplay is already initialized, will return false (0).
 *
 * Returns: true (1) if successful, otherwise false (0).
 **/
bool init_netplay(void *direct_host, const char *server, unsigned port)
{
   struct retro_callbacks cbs      = {0};
   settings_t *settings            = config_get_ptr();
   uint64_t serialization_quirks   = 0;
   uint64_t quirks                 = 0;
   bool _netplay_is_client         = netplay_is_client;
   bool _netplay_enabled           = netplay_enabled;
   const char *username            = settings->paths.username;
   bool netplay_public_announce    = settings->bools.netplay_public_announce;
   bool netplay_start_as_spectator = settings->bools.netplay_start_as_spectator;
   bool netplay_stateless_mode     = settings->bools.netplay_stateless_mode;
   bool netplay_nat_traversal      = settings->bools.netplay_nat_traversal;
   bool netplay_use_mitm_server    = settings->bools.netplay_use_mitm_server;
   const char *netplay_password    = settings->paths.netplay_password;
   const char *netplay_spectate_pw = settings->paths.netplay_spectate_password;
   int netplay_check_frames        = settings->ints.netplay_check_frames;
#ifdef HAVE_DISCORD
   const char *discord_username    = discord_get_own_username();
   const char *path_username       = discord_username 
      ? discord_username
      : username;
#else
   const char *path_username       = username;
#endif

   if (!_netplay_enabled)
      return false;

   core_set_default_callbacks(&cbs);
   if (!core_set_netplay_callbacks())
      return false;

   /* Map the core's quirks to our quirks */
   serialization_quirks = core_serialization_quirks();
   if (serialization_quirks & ~((uint64_t) NETPLAY_QUIRK_MAP_UNDERSTOOD))
   {
      /* Quirks we don't support! Just disable everything. */
      quirks |= NETPLAY_QUIRK_NO_SAVESTATES;
   }
   if (serialization_quirks & NETPLAY_QUIRK_MAP_NO_SAVESTATES)
      quirks |= NETPLAY_QUIRK_NO_SAVESTATES;
   if (serialization_quirks & NETPLAY_QUIRK_MAP_NO_TRANSMISSION)
      quirks |= NETPLAY_QUIRK_NO_TRANSMISSION;
   if (serialization_quirks & NETPLAY_QUIRK_MAP_INITIALIZATION)
      quirks |= NETPLAY_QUIRK_INITIALIZATION;
   if (serialization_quirks & NETPLAY_QUIRK_MAP_ENDIAN_DEPENDENT)
      quirks |= NETPLAY_QUIRK_ENDIAN_DEPENDENT;
   if (serialization_quirks & NETPLAY_QUIRK_MAP_PLATFORM_DEPENDENT)
      quirks |= NETPLAY_QUIRK_PLATFORM_DEPENDENT;

   if (_netplay_is_client)
   {
      RARCH_LOG("[netplay] %s\n",
            msg_hash_to_str(MSG_CONNECTING_TO_NETPLAY_HOST));
   }
   else
   {
      RARCH_LOG("[netplay] %s\n",
            msg_hash_to_str(MSG_WAITING_FOR_CLIENT));
      runloop_msg_queue_push(
         msg_hash_to_str(MSG_WAITING_FOR_CLIENT),
         0, 180, false,
         NULL, MESSAGE_QUEUE_ICON_DEFAULT, MESSAGE_QUEUE_CATEGORY_INFO);

      if (netplay_public_announce)
         netplay_announce();
   }

   netplay_data = (netplay_t*)netplay_new(
         _netplay_is_client ? direct_host : NULL,
         _netplay_is_client ? (!netplay_client_deferred ? server
            : server_address_deferred) : NULL,
         _netplay_is_client ? (!netplay_client_deferred ? port
            : server_port_deferred   ) 
         : (port != 0 ? port : RARCH_DEFAULT_PORT),
         netplay_stateless_mode,
         netplay_check_frames,
         &cbs,
         netplay_nat_traversal 
         && !netplay_use_mitm_server,
         path_username,
         netplay_password,
         netplay_spectate_pw,
         quirks);

   if (netplay_data)
   {
      if (   netplay_data->is_server && 
            !netplay_start_as_spectator)
         netplay_toggle_play_spectate(netplay_data);
      return true;
   }

   RARCH_WARN("%s\n", msg_hash_to_str(MSG_NETPLAY_FAILED));

   runloop_msg_queue_push(
         msg_hash_to_str(MSG_NETPLAY_FAILED),
         0, 180, false,
         NULL, MESSAGE_QUEUE_ICON_DEFAULT, MESSAGE_QUEUE_CATEGORY_INFO);
   return false;
}

/* Frontend access to Netplay functionality */
bool netplay_driver_ctl(enum rarch_netplay_ctl_state state, void *data)
{
   /* Used to avoid recursive netplay calls */
   static bool in_netplay     = false;
   netplay_t *netplay         = netplay_data;

   if (in_netplay)
      return true;
   in_netplay                 = true;

   if (!netplay)
   {
      switch (state)
      {
         case RARCH_NETPLAY_CTL_ENABLE_SERVER:
            netplay_enabled   = true;
            netplay_is_client = false;
            in_netplay        = false;
            return true;
         case RARCH_NETPLAY_CTL_ENABLE_CLIENT:
            netplay_enabled   = true;
            netplay_is_client = true;
            break;
         case RARCH_NETPLAY_CTL_DISABLE:
            netplay_enabled   = false;
#ifdef HAVE_DISCORD
            if (discord_is_inited)
            {
               discord_userdata_t userdata;
               userdata.status = DISCORD_PRESENCE_NETPLAY_NETPLAY_STOPPED;
               command_event(CMD_EVENT_DISCORD_UPDATE, &userdata);
            }
#endif
            in_netplay        = false;
            return true;
         case RARCH_NETPLAY_CTL_IS_ENABLED:
            in_netplay        = false;
            return netplay_enabled;
         case RARCH_NETPLAY_CTL_IS_REPLAYING:
         case RARCH_NETPLAY_CTL_IS_DATA_INITED:
         case RARCH_NETPLAY_CTL_IS_CONNECTED:
            in_netplay        = false;
            return false;
         case RARCH_NETPLAY_CTL_IS_SERVER:
            in_netplay        = false;
            return netplay_enabled && !netplay_is_client;
         default:
            in_netplay        = false;
            return true;
      }
   }

   switch (state)
   {
      case RARCH_NETPLAY_CTL_ENABLE_SERVER:
      case RARCH_NETPLAY_CTL_ENABLE_CLIENT:
      case RARCH_NETPLAY_CTL_IS_DATA_INITED:
      case RARCH_NETPLAY_CTL_IS_ENABLED:
         break;
      case RARCH_NETPLAY_CTL_IS_REPLAYING:
         in_netplay           = false;
         return netplay->is_replay;
      case RARCH_NETPLAY_CTL_IS_SERVER:
         in_netplay           = false;
         return netplay_enabled && !netplay_is_client;
      case RARCH_NETPLAY_CTL_IS_CONNECTED:
         in_netplay           = false;
         return netplay->is_connected;
      case RARCH_NETPLAY_CTL_POST_FRAME:
         /* We check if we have new input and replay from recorded input.
          * Call this after running retro_run().
          */
         netplay_post_frame(netplay);
         break;
      case RARCH_NETPLAY_CTL_PRE_FRAME:
         /*
          * Call netplay_pre_frame before running retro_run().
          *
          * Returns true if the frontend is cleared to 
          * render the frame, false if we're stalled or paused
          */
         in_netplay           = false;
         return netplay_pre_frame(netplay);
      case RARCH_NETPLAY_CTL_GAME_WATCH:
         netplay_toggle_play_spectate(netplay);
         break;
      case RARCH_NETPLAY_CTL_PAUSE:
         netplay_frontend_paused(netplay, true);
         break;
      case RARCH_NETPLAY_CTL_UNPAUSE:
         netplay_frontend_paused(netplay, false);
         break;
      case RARCH_NETPLAY_CTL_LOAD_SAVESTATE:
         netplay_load_savestate(netplay,
               (retro_ctx_serialize_info_t*)data, true);
         break;
      case RARCH_NETPLAY_CTL_RESET:
         /* Indicate that the core has been reset to netplay peers */
         netplay_core_reset(netplay);
         break;
      case RARCH_NETPLAY_CTL_DISCONNECT:
         if (netplay)
            netplay_disconnect(netplay);
         break;
      case RARCH_NETPLAY_CTL_FINISHED_NAT_TRAVERSAL:
         netplay->nat_traversal_task_oustanding = false;
#ifndef HAVE_SOCKET_LEGACY
         netplay_announce_nat_traversal(netplay);
#endif
         break;
      case RARCH_NETPLAY_CTL_DESYNC_PUSH:
         netplay->desync++;
         break;
      case RARCH_NETPLAY_CTL_DESYNC_POP:
         if (netplay->desync)
         {
            netplay->desync--;
            if (!netplay->desync)
               netplay_load_savestate(netplay, NULL, true);
         }
         break;
      case RARCH_NETPLAY_CTL_NONE:
      case RARCH_NETPLAY_CTL_DISABLE:
      default:
         in_netplay           = false;
         return false;
   }

   in_netplay                 = false;
   return true;
}
