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

#ifndef WIN32_COMMON_H__
#define WIN32_COMMON_H__

#include <string.h>

#ifndef _XBOX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <boolean.h>
#include "../../driver.h"
#include "../video_context_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _XBOX
#include "../drivers_wm/win32_resource.h"

LRESULT win32_handle_keyboard_event(HWND hwnd, UINT message,
      WPARAM wparam, LPARAM lparam);

LRESULT win32_menu_loop(HWND handle, WPARAM wparam);

void win32_monitor_init(void);

void win32_monitor_from_window(HWND data, bool destroy);

void win32_monitor_get_info(void);

void win32_monitor_info(void *data, void *hm_data);
#endif

bool win32_suppress_screensaver(void *data, bool enable);

bool win32_get_metrics(void *data,
	enum display_metric_types type, float *value);

void win32_show_cursor(bool state);

void win32_check_window(void);

#ifdef __cplusplus
}
#endif

#endif
