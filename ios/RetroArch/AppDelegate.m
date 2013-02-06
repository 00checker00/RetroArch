//
//  AppDelegate.m
//  RetroArch
//
//  Copyright (c) 2013 RetroArch. All rights reserved.
//

#import "AppDelegate.h"

#import "ViewController.h"

#include "general.h"

@implementation AppDelegate

- (void)runMain:(id)sender
{
   const char* filename = [[[NSBundle mainBundle] pathForResource:@"test" ofType:@"img"] UTF8String];
   const char* libretro = [[[NSBundle mainBundle] pathForResource:@"libretro" ofType:@"dylib"] UTF8String];

   printf("%s\n", libretro);

   const char* argv[] = {"retroarch", "-L", libretro, filename, 0};
   if (rarch_main_init(4, (char**)argv) == 0)
   {
      rarch_init_msg_queue();
      while (rarch_main_iterate());
      rarch_main_deinit();
      rarch_deinit_msg_queue();
      
#ifdef PERF_TEST
      rarch_perf_log();
#endif
      
      rarch_main_clear_state();
   }
}

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
   self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

   // Override point for customization after application launch.
   if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
      self.viewController = [[ViewController alloc] initWithNibName:@"ViewController_iPhone" bundle:nil];
   else
      self.viewController = [[ViewController alloc] initWithNibName:@"ViewController_iPad" bundle:nil];

   self.window.rootViewController = self.viewController;
   [self.window makeKeyAndVisible];

   [self performSelector:@selector(runMain:) withObject:nil afterDelay:0.2f];
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
