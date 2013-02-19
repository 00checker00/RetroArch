#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include "conf/config_file.h"

@interface RAGameView : UIViewController
@end

@interface RAModuleInfo : NSObject
@property (strong) NSString* path;
@property config_file_t* data;

+ (RAModuleInfo*)moduleWithPath:(NSString*)thePath data:(config_file_t*)theData;
@end

@interface RAModuleInfoList : UITableViewController
- (id)initWithModuleInfo:(RAModuleInfo*)info;
@end

@interface RAModuleList : UITableViewController
@end

@interface RADirectoryFilterList : UITableViewController
- (id)initWithPath:(NSString*)path;
@end

@interface RADirectoryList : UITableViewController
+ (id)directoryListWithPath:(NSString*)path;
- (id)initWithPath:(NSString*)path filter:(NSRegularExpression*)regex;
@end

@interface RASettingsSubList : UITableViewController
- (id)initWithSettings:(NSArray*)values title:(NSString*)title;
- (void)writeSettings:(NSArray*)settingList toConfig:(config_file_t*)config;
@end

@interface RASettingsList : RASettingsSubList
+ (void)refreshConfigFile;
@end
