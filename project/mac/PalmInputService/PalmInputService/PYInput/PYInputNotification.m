#import "PYInputNotification.h"

@implementation PYInputNotification

+ (void) AddObserver:(id)observer selector:(SEL)aSelector name:(NSString*)aName {
    
    [[NSDistributedNotificationCenter defaultCenter] addObserver:observer selector:aSelector name:aName object:nil];
}

+ (void) RemoveObserver:(id)observer {
    
    [[NSDistributedNotificationCenter defaultCenter] removeObserver:observer];
}

+ (void) PostNotification:(NSString *)aName userInfo:(NSDictionary *)aUserInfo {
    
    [[NSDistributedNotificationCenter defaultCenter] postNotificationName:aName object:nil userInfo:aUserInfo];
}
@end
