/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2013 - Jason Fetters
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

@interface RAConfig : NSObject
- (id)initWithPath:(NSString*)path;
- (void)writeToFile:(NSString*)path;

- (bool)getBoolNamed:(NSString*)name withDefault:(bool)def;
- (int)getIntNamed:(NSString*)name withDefault:(int)def;
- (unsigned)getUintNamed:(NSString*)name withDefault:(unsigned)def;
- (double)getDoubleNamed:(NSString*)name withDefault:(double)def;
- (NSString*)getStringNamed:(NSString*)name withDefault:(NSString*)def;

- (void)putIntNamed:(NSString*)name value:(int)value;
- (void)putStringNamed:(NSString*)name value:(NSString*)value;
@end
