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

#import "browser.h"

@implementation RADirectoryList
{
   NSString* _path;
   NSArray* _list;
}

+ (id)directoryListOrGridWithPath:(NSString*)path
{
   path = ra_ios_check_path(path);

   if ([UICollectionViewController instancesRespondToSelector:@selector(initWithCollectionViewLayout:)])
   {
      NSString* coverDir = [path stringByAppendingPathComponent:@".coverart"];
      if (ra_ios_is_directory(coverDir))
         return [[RADirectoryGrid alloc] initWithPath:path];
   }

   return [[RADirectoryList alloc] initWithPath:path];
}

- (id)initWithPath:(NSString*)path
{
   self = [super initWithStyle:UITableViewStylePlain];

   _path = path;
   _list = ra_ios_list_directory(_path);

   [self setTitle: [_path lastPathComponent]];
   
   return self;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
   RADirectoryItem* path = [_list objectAtIndex: indexPath.row];

   if(path.isDirectory)
      [[RetroArch_iOS get] pushViewController:[RADirectoryList directoryListOrGridWithPath:path.path] animated:YES];
   else
      [[RetroArch_iOS get] pushViewController:[[RAModuleList alloc] initWithGame:path.path] animated:YES];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
   return [_list count];
}

- (UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
   RADirectoryItem* path = [_list objectAtIndex: indexPath.row];

   UITableViewCell* cell = [self.tableView dequeueReusableCellWithIdentifier:@"path"];
   cell = (cell != nil) ? cell : [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"path"];
   cell.textLabel.text = [path.path lastPathComponent];
   cell.accessoryType = (path.isDirectory) ? UITableViewCellAccessoryDisclosureIndicator : UITableViewCellAccessoryNone;
   cell.imageView.image = [UIImage imageNamed:(path.isDirectory) ? @"ic_dir" : @"ic_file"];
   return cell;
}

@end
