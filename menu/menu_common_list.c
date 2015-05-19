/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
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

#include "menu_common_list.h"
#include "menu.h"
#include "menu_entries_cbs.h"

void menu_common_list_delete(void *data, size_t idx,
      size_t list_size)
{
   menu_file_list_cbs_t *cbs = NULL;
   file_list_t *list = (file_list_t*)data;

   if (!list)
      return;

   cbs = (menu_file_list_cbs_t*)list->list[idx].actiondata;

   if (cbs)
   {
      cbs->action_start         = NULL;
      cbs->action_ok            = NULL;
      cbs->action_cancel        = NULL;
      cbs->action_left          = NULL;
      cbs->action_right         = NULL;
      cbs->action_deferred_push = NULL;
      free(list->list[idx].actiondata);
   }
   list->list[idx].actiondata = NULL;
}
