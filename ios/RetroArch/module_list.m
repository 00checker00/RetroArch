//
//  module_list.m
//  RetroArch
//
//  Created by Jason Fetters on 2/8/13.
//  Copyright (c) 2013 RetroArch. All rights reserved.
//

#import "module_list.h"
#import "dirlist.h"

@interface module_list ()

@end

@implementation module_list
{
   UITableView* table;
   
   NSString* module_dir;
   NSMutableArray* modules;
};

- (void)viewDidLoad
{
   [super viewDidLoad];

   // Get the contents of the modules directory of the bundle.
   module_dir = [NSString stringWithFormat:@"%@/%@",
                   [[NSBundle mainBundle] bundlePath],
                   @"modules"];
   
   NSArray *module_list = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:module_dir error:nil];
   
   if (module_list == nil || [module_list count] == 0)
   {
      // TODO: Handle error!
   }

   // Remove non .dylib files from the list
   modules = [NSMutableArray arrayWithArray:module_list];
   for (int i = 0; i < [modules count];)
   {
      if (![[modules objectAtIndex:i] hasSuffix:@".dylib"])
      {
         [modules removeObjectAtIndex:i];
      }
      else
      {
         i ++;
      }
   }
   
   [self setTitle:@"Choose Emulator"];
   table = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 640, 480) style:UITableViewStylePlain];
   table.dataSource = self;
   table.delegate = self;
   self.view = table;

   self.navigationItem.backBarButtonItem = [[UIBarButtonItem alloc]
                                            initWithTitle:@"Parent"
                                            style:UIBarButtonItemStyleBordered
                                            target:nil action:nil];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
   [RetroArch_iOS get].module_path = [NSString stringWithFormat:@"%@/%@", module_dir, [modules objectAtIndex:indexPath.row]];
   [[RetroArch_iOS get].navigator pushViewController:[[[dirlist_view alloc] init] load_path:"/"] animated:YES];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
   return modules ? [modules count] : 0;
}

- (UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
   UITableViewCell* cell = [table dequeueReusableCellWithIdentifier:@"module"];
   cell = (cell != nil) ? cell : [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"module"];
   
   if (modules)
   {
      cell.textLabel.text = [modules objectAtIndex:indexPath.row];
   }

   return cell;
}

@end
