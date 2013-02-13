//
//  dirlist.m
//  RetroArch
//
//  Created by Jason Fetters on 2/7/13.
//  Copyright (c) 2013 RetroArch. All rights reserved.
//

static BOOL is_directory(NSString* path)
{
   BOOL result = NO;
   [[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&result];
   return result;
}

@implementation directory_list
{
   NSString* directory;
   NSArray* list;
};

- (id)initWithPath:(NSString*)path
{
   self = [super initWithStyle:UITableViewStylePlain];

   if (path == nil)
   {
      if (is_directory(@"/var/mobile/RetroArchGames"))    path = @"/var/mobile/RetroArchGames";
      else if (is_directory(@"/var/mobile"))              path = @"/var/mobile";
      else                                                path = @"/";
   }

   directory = path;

   list = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:directory error:nil];
   list = [directory stringsByAppendingPaths:list];
   
   list = [list sortedArrayUsingComparator:^(id left, id right)
   {
      const BOOL left_is_dir = is_directory((NSString*)left);
      const BOOL right_is_dir = is_directory((NSString*)right);
      
      return (left_is_dir != right_is_dir) ?
               (left_is_dir ? -1 : 1) :
               ([left caseInsensitiveCompare:right]);
   }];

   self.navigationItem.rightBarButtonItem = [RetroArch_iOS get].settings_button;
   [self setTitle: [directory lastPathComponent]];
   
   return self;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
   NSString* path = [list objectAtIndex: indexPath.row];

   if(is_directory(path))
   {
      [[RetroArch_iOS get] pushViewController:[[directory_list alloc] initWithPath:path]];
   }
   else
   {
      [[RetroArch_iOS get] runGame:path];
   }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
   return [list count];
}

- (UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
   NSString* path = [list objectAtIndex: indexPath.row];
   BOOL isdir = is_directory(path);

   UITableViewCell* cell = [self.tableView dequeueReusableCellWithIdentifier:@"path"];
   cell = (cell != nil) ? cell : [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"path"];
   cell.textLabel.text = [path lastPathComponent];
   cell.accessoryType = (isdir) ? UITableViewCellAccessoryDisclosureIndicator : UITableViewCellAccessoryNone;
   cell.imageView.image = (isdir) ? [RetroArch_iOS get].folder_icon : [RetroArch_iOS get].file_icon;
   return cell;
}

@end
