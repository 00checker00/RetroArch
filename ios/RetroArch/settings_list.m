//
//  settings_list.m
//  RetroArch
//
//  Created by Jason Fetters on 2/8/13.
//  Copyright (c) 2013 RetroArch. All rights reserved.
//

#include "config_file.h"

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

   NSArray* values = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:[RetroArch_iOS get].overlay_path error:nil];
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

@interface enumeration_list : UITableViewController
@end

@implementation enumeration_list
{
   NSMutableDictionary* value;
   UITableView* view;
};

- (id)initWithSetting:(NSMutableDictionary*)setting fromTable:(UITableView*)table
{
   self = [super initWithStyle:UITableViewStyleGrouped];
   
   value = setting;
   view = table;
   [self setTitle: [value objectForKey:@"LABEL"]];
   return self;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
   return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
   return [[value objectForKey:@"VALUES"] count];
}

- (UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
   UITableViewCell* cell = [self.tableView dequeueReusableCellWithIdentifier:@"option"];
   cell = cell ? cell : [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"option"];
   
   cell.textLabel.text = [[value objectForKey:@"VALUES"] objectAtIndex:indexPath.row];

   return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
   [value setObject:[[value objectForKey:@"VALUES"] objectAtIndex:indexPath.row] forKey:@"VALUE"];

   [view reloadData];
   [[RetroArch_iOS get].navigator popViewControllerAnimated:YES];
}

@end

@implementation settings_list
{
   NSArray* settings;
   config_file_t* config;
};

- (id)init
{
   self = [super initWithStyle:UITableViewStyleGrouped];

   const char* const sd = [RetroArch_iOS get].system_directory;
   char config_path[PATH_MAX];
   snprintf(config_path, PATH_MAX, "%s/retroarch.cfg", sd);
   config_path[PATH_MAX - 1] = 0;
   
   config = config_file_new(config_path);
   if (!config) config = config_file_new(0);

   settings = [NSArray arrayWithObjects:
      [NSArray arrayWithObjects:@"Video",
         boolean_setting(config, @"video_smooth", @"Smooth Video", @"true"),
         boolean_setting(config, @"video_crop_overscan", @"Crop Overscan", @"false"),
         nil],

      [NSArray arrayWithObjects:@"Audio",
         boolean_setting(config, @"audio_enable", @"Enable Output", @"true"),
         boolean_setting(config, @"audio_sync", @"Sync on Audio Stream", @"true"),
         boolean_setting(config, @"audio_rate_control", @"Adjust for Better Sync", @"true"),
         nil],

      [NSArray arrayWithObjects:@"Input",
         subpath_setting(config, @"input_overlay", @"Input Overlay", @"", [RetroArch_iOS get].overlay_path, @"cfg"),
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

- (void)write_to_file
{
   for (int i = 0; i != [settings count]; i ++)
   {
      NSArray* group = [settings objectAtIndex:i];
   
      for (int j = 1; j < [group count]; j ++)
      {
         NSMutableDictionary* setting = [group objectAtIndex:j];
      
         NSString* name = [setting objectForKey:@"NAME"];
         NSString* value = [setting objectForKey:@"VALUE"];

         if ([[setting objectForKey:@"TYPE"] isEqualToString:@"F"])
            value = [[setting objectForKey:@"PATH"] stringByAppendingPathComponent:value];

         config_set_string(config, [name UTF8String], [value UTF8String]);
      }
   }

   const char* const sd = [RetroArch_iOS get].system_directory;
   char config_path[PATH_MAX];
   snprintf(config_path, PATH_MAX, "%s/retroarch.cfg", sd);
   config_path[PATH_MAX - 1] = 0;
   config_file_write(config, config_path);
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
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
   [self write_to_file];
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
         cell.accessoryView = [[UISwitch alloc] init];
         [cell setSelectionStyle:UITableViewCellSelectionStyleNone];
      }
      
      UISwitch* swt = (UISwitch*)cell.accessoryView;
      swt.on = [[setting valueForKey:@"VALUE"] isEqualToString:@"true"];
   }
   else if ([type isEqualToString:@"E"] || [type isEqualToString:@"F"])
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
