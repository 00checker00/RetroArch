//
//  dirlist.m
//  RetroArch
//
//  Created by Jason Fetters on 2/7/13.
//  Copyright (c) 2013 RetroArch. All rights reserved.
//

#include <dirent.h>
#import "dirlist.h"
#import "gameview.h"

struct dirent_list
{
   size_t count;
   struct dirent* entries;
};

const struct dirent* get_dirent_at_index(struct dirent_list* list, unsigned index)
{
   if (!list) return 0;
   return (index < list->count) ? &list->entries[index] : 0;
}

unsigned get_dirent_list_count(struct dirent_list* list)
{
   return list ? list->count : 0;
}

void free_dirent_list(struct dirent_list* list)
{
   if (list) free(list->entries);
   free(list);
}

static int compare_dirent(const void *left, const void *right)
{
   const struct dirent* l = (const struct dirent*) left;
   const struct dirent* r = (const struct dirent*) right;
   
   // Directories first
   if ((l->d_type & DT_DIR) != (r->d_type & DT_DIR))
   {
      return (l->d_type & DT_DIR) ? -1 : 1;
   }
   
   // Name
   return strcmp(l->d_name, r->d_name);
}

struct dirent_list* build_dirent_list(const char* path)
{
   struct dirent_list* result = 0;

   DIR* dir = opendir(path);
   if (dir)
   {
      struct dirent* ent = 0;
   
      // Count the number of items
      size_t count = 0;
      while ((ent = readdir(dir)))
      {
         count += (strcmp(ent->d_name, ".") ? 1 : 0);
      }
      rewinddir(dir);
     
      // Build and fill the result
      result = malloc(sizeof(struct dirent_list));
      result->count = count;
      result->entries = malloc(sizeof(struct dirent) * count);
      
      size_t index = 0;
      while ((ent = readdir(dir)))
      {
         if (strcmp(ent->d_name, ".") == 0) continue;
         memcpy(&result->entries[index ++], ent, sizeof(struct dirent));
      }
            
      closedir(dir);

      qsort(result->entries, result->count, sizeof(struct dirent), &compare_dirent);
   }
   
   return result;
}

@implementation dirlist_view
{
   char path[4096];
   UITableView* table;
   struct dirent_list* files;
   
   UIImage* file_icon;
   UIImage* folder_icon;
};

-(void)dealloc
{
   free_dirent_list(files);
   files = 0;
}

- (void)viewDidLoad
{
   file_icon = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"ic_file" ofType:@"png"]];
   folder_icon = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"ic_dir" ofType:@"png"]];

   [super viewDidLoad];
   
   strcpy(path, "/");
   files = build_dirent_list(path);
   
   table = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 640, 480) style:UITableViewStylePlain];
   table.dataSource = self;
   table.delegate = self;

   self.view = table;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
   const struct dirent* item = get_dirent_at_index(files, indexPath.row);

   if (!item) return;

   strcat(path, item->d_name);

   if (item->d_type & DT_DIR)
   {
      strcat(path, "/");
      free_dirent_list(files);
      files = build_dirent_list(path);
      [table reloadData];
   }
   else
   {
      UIWindow *window = [UIApplication sharedApplication].keyWindow;

      if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
         window.rootViewController = [[game_view alloc] initWithNibName:@"ViewController_iPhone" bundle:nil];
      else
         window.rootViewController = [[game_view alloc] initWithNibName:@"ViewController_iPad" bundle:nil];
    
      game_view* game = (game_view*)window.rootViewController;
      
      extern void ios_load_game(const char*);
      ios_load_game(path);
   }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
   return get_dirent_list_count(files);
}

- (UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
   UITableViewCell* cell = [table dequeueReusableCellWithIdentifier:@"path"];
   cell = (cell != nil) ? cell : [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"path"];
   
   const struct dirent* item = get_dirent_at_index(files, indexPath.row);
   
   if (item)
   {
      cell.textLabel.text = [[NSString string] initWithUTF8String:item->d_name];
      
      if (item->d_type & DT_DIR)
      {
         cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
         cell.imageView.image = folder_icon;
      }
      else
      {
         cell.accessoryType = UITableViewCellAccessoryNone;
         cell.imageView.image = file_icon;
      }
      
      [cell.imageView sizeToFit];
   }

   return cell;
}

@end
