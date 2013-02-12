//
//  settings_list.m
//  RetroArch
//
//  Created by Jason Fetters on 2/8/13.
//  Copyright (c) 2013 RetroArch. All rights reserved.
//

#import <objc/runtime.h>
#import "settings.h"
#include "config_file.h"

static const char* const SETTINGID = "SETTING";

static NSString* get_value_from_config(config_file_t* config, NSString* name, NSString* defaultValue)
{
   NSString* value = nil;

   char* v = 0;
   if (config_get_string(config, [name UTF8String], &v))
   {
      value = [[NSString alloc] initWithUTF8String:v];
      free(v);
   }
   else
   {
      value = defaultValue;
   }

   return value;
}

static NSMutableDictionary* boolean_setting(config_file_t* config, NSString* name, NSString* label, NSString* defaultValue)
{
   NSString* value = get_value_from_config(config, name, defaultValue);

   return [[NSMutableDictionary alloc] initWithObjectsAndKeys:
            @"B", @"TYPE",
            name, @"NAME",
            label, @"LABEL",
            value, @"VALUE",
            nil];
}

static NSMutableDictionary* button_setting(config_file_t* config, NSString* name, NSString* label, NSString* defaultValue)
{
   NSString* value = get_value_from_config(config, name, defaultValue);

   return [[NSMutableDictionary alloc] initWithObjectsAndKeys:
            @"C", @"TYPE",
            name, @"NAME",
            label, @"LABEL",
            value, @"VALUE",
            nil];
}

static NSMutableDictionary* enumeration_setting(config_file_t* config, NSString* name, NSString* label, NSString* defaultValue, NSArray* values)
{
   NSString* value = get_value_from_config(config, name, defaultValue);

   return [[NSMutableDictionary alloc] initWithObjectsAndKeys:
            @"E", @"TYPE",
            name, @"NAME",
            label, @"LABEL",
            value, @"VALUE",
            values, @"VALUES",
            nil];
}

static NSMutableDictionary* subpath_setting(config_file_t* config, NSString* name, NSString* label, NSString* defaultValue, NSString* path, NSString* extension)
{
   NSString* value = get_value_from_config(config, name, defaultValue);
   value = [value stringByReplacingOccurrencesOfString:path withString:@""];

   NSArray* values = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:path error:nil];
   values = [values pathsMatchingExtensions:[NSArray arrayWithObject:extension]];

   return [[NSMutableDictionary alloc] initWithObjectsAndKeys:
            @"F", @"TYPE",
            name, @"NAME",
            label, @"LABEL",
            value, @"VALUE",
            values, @"VALUES",
            path, @"PATH",
            nil];
}


@implementation settings_list
{
   NSArray* settings;
   config_file_t* config;
};

- (id)init
{
   self = [super initWithStyle:UITableViewStyleGrouped];

   config = config_file_new([[RetroArch_iOS get].config_file_path UTF8String]);
   if (!config) config = config_file_new(0);

   NSString* overlay_path = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/overlays/"];
   NSString* shader_path = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/shaders/"];

   settings = [NSArray arrayWithObjects:
      [NSArray arrayWithObjects:@"Video",
         boolean_setting(config, @"video_smooth", @"Smooth Video", @"true"),
         boolean_setting(config, @"video_crop_overscan", @"Crop Overscan", @"false"),
         subpath_setting(config, @"video_bsnes_shader", @"Shader", @"", shader_path, @"shader"),
         nil],

      [NSArray arrayWithObjects:@"Audio",
         boolean_setting(config, @"audio_enable", @"Enable Output", @"true"),
         boolean_setting(config, @"audio_sync", @"Sync on Audio Stream", @"true"),
         boolean_setting(config, @"audio_rate_control", @"Adjust for Better Sync", @"true"),
         nil],

      [NSArray arrayWithObjects:@"Input",
         subpath_setting(config, @"input_overlay", @"Input Overlay", @"", overlay_path, @"cfg"),
         nil],
         
      [NSArray arrayWithObjects:@"System Keys",
         button_setting(config, @"input_save_state", @"Save State", @"f2"),
         button_setting(config, @"input_load_state", @"Load State", @"f4"),
         button_setting(config, @"input_state_slot_increase", @"Next State Slot", @"f7"),
         button_setting(config, @"input_state_slot_decrease", @"Previous State Slot", @"f6"),
         button_setting(config, @"input_toggle_fast_forward", @"Toggle Fast Forward", @"space"),
         button_setting(config, @"input_hold_fast_forward", @"Hold Fast Forward", @"l"),
         button_setting(config, @"input_rewind", @"Rewind", @"r"),
         button_setting(config, @"input_slowmotion", @"Slow Motion", @"e"),
         button_setting(config, @"input_pause_toogle", @"Pause", @"p"),
         button_setting(config, @"input_frame_advance", @"Advance Frame", @"k"),
         button_setting(config, @"input_reset", @"Reset", @"h"),
         button_setting(config, @"input_exit_emulator", @"Close Game", @"escape"),
         nil],
         
      [NSArray arrayWithObjects:@"Save States",
         boolean_setting(config, @"rewind_enable", @"Enable Rewinding", @"false"),
         boolean_setting(config, @"block_sram_overwrite", @"Disable SRAM on Load", @"false"),
         boolean_setting(config, @"savestate_auto_save", @"Auto Save on Exit", @"false"),
         boolean_setting(config, @"savestate_auto_load", @"Auto Load on Startup", @"true"),
         nil],
      nil
   ];
  
   [self setTitle:@"RetroArch Settings"];
   return self;
}

- (void)dealloc
{
   [self write_to_file];
}

+ (void)refresh_config_file
{
   [[[settings_list alloc] init] write_to_file];
}

- (void)write_to_file
{
   config_set_string(config, "system_directory", [[RetroArch_iOS get].system_directory UTF8String]);

   for (int i = 0; i != [settings count]; i ++)
   {
      NSArray* group = [settings objectAtIndex:i];
   
      for (int j = 1; j < [group count]; j ++)
      {
         NSMutableDictionary* setting = [group objectAtIndex:j];
      
         NSString* name = [setting objectForKey:@"NAME"];
         NSString* value = [setting objectForKey:@"VALUE"];

         if ([[setting objectForKey:@"TYPE"] isEqualToString:@"F"] && [value length] > 0)
            value = [[setting objectForKey:@"PATH"] stringByAppendingPathComponent:value];

         config_set_string(config, [name UTF8String], [value UTF8String]);
      }
   }

   config_file_write(config, [[RetroArch_iOS get].config_file_path UTF8String]);
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
   NSMutableDictionary* setting = [[settings objectAtIndex:indexPath.section] objectAtIndex:indexPath.row + 1];
   NSString* type = [setting valueForKey:@"TYPE"];
   
   if ([type isEqualToString:@"E"] || [type isEqualToString:@"F"])
   {
      [[RetroArch_iOS get].navigator
         pushViewController:[[enumeration_list alloc] initWithSetting:setting fromTable:(UITableView*)self.view]
         animated:YES];
   }
   else if([type isEqualToString:@"C"])
   {
      (void)[[button_getter alloc] initWithSetting:setting fromTable:(UITableView*)self.view];
   }
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
   return [settings count];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
   return [[settings objectAtIndex:section] count] -1;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
   return [[settings objectAtIndex:section] objectAtIndex:0];
}

- (void)handle_boolean_switch:(UISwitch*)swt
{
   NSDictionary* setting = objc_getAssociatedObject(swt, SETTINGID);
   [setting setValue: (swt.on ? @"true" : @"false") forKey:@"VALUE"];
}

- (UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
   NSMutableDictionary* setting = [[settings objectAtIndex:indexPath.section] objectAtIndex:indexPath.row + 1];
   UITableViewCell* cell = nil;
   
   NSString* type = [setting valueForKey:@"TYPE"];
   
   if ([type isEqualToString:@"B"])
   {
      cell = [self.tableView dequeueReusableCellWithIdentifier:@"boolean"];
   
      if (cell == nil)
      {
         cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"boolean"];
         
         UISwitch* accessory = [[UISwitch alloc] init];
         [accessory addTarget:self action:@selector(handle_boolean_switch:) forControlEvents:UIControlEventValueChanged];
         
         cell.accessoryView = accessory;
         [cell setSelectionStyle:UITableViewCellSelectionStyleNone];
      }
      
      UISwitch* swt = (UISwitch*)cell.accessoryView;
      swt.on = [[setting valueForKey:@"VALUE"] isEqualToString:@"true"];
      
      objc_setAssociatedObject(swt, SETTINGID, setting, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
   }
   else if ([type isEqualToString:@"E"] || [type isEqualToString:@"F"] || [type isEqualToString:@"C"])
   {
      cell = [self.tableView dequeueReusableCellWithIdentifier:@"enumeration"];
   
      if (cell == nil)
      {
         cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:@"enumeration"];
      }
   }

   cell.textLabel.text = [setting valueForKey:@"LABEL"];
   cell.detailTextLabel.text = [setting valueForKey:@"VALUE"];

   return cell;
}

@end
