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

#import "gameview.h"
#include "general.h"

static game_view *current_view;
static GLKView *gl_view;
static float screen_scale;
static bool ra_initialized = false;
static bool ra_done = false;

@implementation game_view
{
   EAGLContext *gl_context;
}

- (void)schedule_iterate
{
   if (ra_initialized && !ra_done)
      [self performSelector:@selector(rarch_iterate:) withObject:nil afterDelay:0.002f];
}

- (void)rarch_iterate:(id)sender
{
   if (ra_initialized && !ra_done)
      ra_done = !rarch_main_iterate();
    
   [self schedule_iterate];
}

- (void)viewDidLoad
{
   [super viewDidLoad];

   gl_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
   [EAGLContext setCurrentContext:gl_context];
   
   gl_view = [[GLKView alloc] initWithFrame:CGRectMake(0, 0, 640, 480) context:gl_context];
   self.view = gl_view;

   screen_scale = [[UIScreen mainScreen] scale];
   current_view = self;
}

- (void)dealloc
{
   if ([EAGLContext currentContext] == gl_context) [EAGLContext setCurrentContext:nil];
   gl_context = nil;
   gl_view = nil;
}

@end

void flip_game_view()
{
   if (gl_view)
   {
      [gl_view setNeedsDisplay];
      [gl_view bindDrawable];
   }
}

void get_game_view_size(unsigned *width, unsigned *height)
{
   if (gl_view)
   {
      *width  = gl_view.bounds.size.width * screen_scale;
      *height = gl_view.bounds.size.height * screen_scale;
   }
}

void ios_close_game()
{
   if (ra_initialized)
   {
      rarch_main_deinit();
      rarch_deinit_msg_queue();
      
#ifdef PERF_TEST
      rarch_perf_log();
#endif
      
      rarch_main_clear_state();
      
      ra_done = true;
   }
   
   ra_initialized = false;
}

void ios_load_game(const char* file_name)
{
   if(!ra_initialized && file_name)
   {
      const char* libretro = [[[NSBundle mainBundle] pathForResource:@"libretro" ofType:@"dylib"] UTF8String];
      const char* overlay = [[[NSBundle mainBundle] pathForResource:@"overlay" ofType:@"cfg"] UTF8String];

      strcpy(g_settings.input.overlay, overlay ? overlay : "");

      const char* argv[] = {"retroarch", "-L", libretro, file_name, 0};
      if (rarch_main_init(6, (char**)argv) == 0)
      {
         rarch_init_msg_queue();
         ra_initialized = TRUE;

         if (current_view) [current_view schedule_iterate];
      }
   }
}
