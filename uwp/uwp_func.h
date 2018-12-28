/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2018 - Krzysztof Haładyn
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

#ifndef _UWP_FUNC_H
#define _UWP_FUNC_H

#include <retro_miscellaneous.h>

#ifdef __cplusplus
extern "C" {
#endif

extern char uwp_dir_install[PATH_MAX_LENGTH];
extern char uwp_dir_data[PATH_MAX_LENGTH];
extern char uwp_device_family[128];

void* uwp_get_corewindow(void);

#ifdef __cplusplus
}
#endif

#endif _UWP_FUNC_H
