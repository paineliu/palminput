#import <Foundation/Foundation.h>

@interface FTTools : NSObject

- (BOOL) unregisterInputSource;
- (BOOL) registerInputSource;
- (void) killProcess:(NSString *) procName;

@end
