/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2011-2016 - Daniel De Matteis
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

#include <string.h>
#include <errno.h>
#include <file/nbio.h>
#include <formats/image.h>
#include <compat/strl.h>
#include <retro_assert.h>
#include <retro_miscellaneous.h>
#include <lists/string_list.h>
#include <rhash.h>

#include "tasks_internal.h"
#include "../verbosity.h"

static int task_file_transfer_iterate_transfer(nbio_handle_t *nbio)
{
   size_t i;

   if (!nbio)
      return -1;
   
   nbio->pos_increment = 5;

   if (nbio->is_finished)
      return 0;

   for (i = 0; i < nbio->pos_increment; i++)
   {
      if (nbio_iterate(nbio->handle))
         return -1;
   }

   return 0;
}

static int task_file_transfer_iterate_parse(nbio_handle_t *nbio)
{
   if (!nbio)
      return -1;

   if (nbio->cb)
   {
      int len = 0;
      if (nbio->cb(nbio, len) == -1)
         return -1;
   }

   return 0;
}

void rarch_task_file_load_handler(retro_task_t *task)
{
   nbio_handle_t         *nbio  = (nbio_handle_t*)task->state;

   switch (nbio->status)
   {
      case NBIO_STATUS_TRANSFER_PARSE:
         if (task_file_transfer_iterate_parse(nbio) == -1)
            task->cancelled = true;
         nbio->status = NBIO_STATUS_TRANSFER_PARSE_FREE;
         break;
      case NBIO_STATUS_TRANSFER:
         if (task_file_transfer_iterate_transfer(nbio) == -1)
            nbio->status = NBIO_STATUS_TRANSFER_PARSE;
         break;
      case NBIO_STATUS_TRANSFER_PARSE_FREE:
      case NBIO_STATUS_POLL:
      default:
         break;
   }

   if (nbio->image)
   {
      if (!rarch_task_image_load_handler(task))
         goto task_finished;
   }
   else
      if (nbio->is_finished)
         goto task_finished;


   if (task->cancelled)
   {
      task->error = strdup("Task canceled.");
      goto task_finished;
   }

   return;

task_finished:
   task->finished = true;

   nbio_free(nbio->handle);
   nbio->handle      = NULL;
   nbio->is_finished = false;
   free(nbio);
}
