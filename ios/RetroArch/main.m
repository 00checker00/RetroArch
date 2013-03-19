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

#import <UIKit/UIKit.h>
#include "input/RAInputResponder.h"

#define GSEVENT_TYPE 2
#define GSEVENT_FLAGS 12
#define GSEVENTKEY_KEYCODE 15
#define GSEVENT_TYPE_KEYDOWN 10
#define GSEVENT_TYPE_KEYUP 11

NSString *const GSEventKeyDownNotification = @"GSEventKeyDownHackNotification";
NSString *const GSEventKeyUpNotification = @"GSEventKeyUpHackNotification";

static RAInputResponder* inputResponder;

@interface RApplication : UIApplication
@end

@implementation RApplication

- (RApplication*)init
{
   self = [super init];
   inputResponder = [RAInputResponder sharedInstance];
   return self;
}

- (void)sendEvent:(UIEvent *)event
{
   [super sendEvent:event];
   
   if ([[event allTouches] count])
   {
      [inputResponder handleTouches:[[event allTouches] allObjects]];
   }
   // Stolen from: http://nacho4d-nacho4d.blogspot.com/2012/01/catching-keyboard-events-in-ios.html
   else if ([event respondsToSelector:@selector(_gsEvent)])
   {
      int* eventMem = (int *)(void*)CFBridgingRetain([event performSelector:@selector(_gsEvent)]);
      int eventType = eventMem ? eventMem[GSEVENT_TYPE] : 0;
      
      if (eventMem && (eventType == GSEVENT_TYPE_KEYDOWN || eventType == GSEVENT_TYPE_KEYUP))
      {
         // Read keycode from GSEventKey
         int tmp = eventMem[GSEVENTKEY_KEYCODE];
         UniChar *keycode = (UniChar *)&tmp;

         // Post notification
         NSDictionary *inf = [[NSDictionary alloc] initWithObjectsAndKeys:
                             [NSNumber numberWithShort:keycode[0]], @"keycode",
                             nil];
                   
         [[NSNotificationCenter defaultCenter]
             postNotificationName:(eventType == GSEVENT_TYPE_KEYDOWN) ? GSEventKeyDownNotification : GSEventKeyUpNotification
             object:nil userInfo:inf];
      }
       
      CFBridgingRelease(eventMem);
   }
}

@end

int main(int argc, char *argv[])
{
    @autoreleasepool {
        return UIApplicationMain(argc, argv, NSStringFromClass([RApplication class]), NSStringFromClass([RetroArch_iOS class]));
    }
}

