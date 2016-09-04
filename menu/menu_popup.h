/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
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

#ifndef _MENU_POPUP_H
#define _MENU_POPUP_H

#include <stdint.h>
#include <stdlib.h>

#include <retro_common_api.h>

RETRO_BEGIN_DECLS

int menu_popup_iterate_help(menu_handle_t *menu, 
      char *s, size_t len, const char *label);

void menu_popup_push(menu_handle_t *menu);

RETRO_END_DECLS

#endif
