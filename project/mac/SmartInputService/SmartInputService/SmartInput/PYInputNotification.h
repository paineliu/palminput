#import <Foundation/Foundation.h>

@interface PYInputNotification : NSObject

+ (void) AddObserver:(id)observer selector:(SEL)aSelector name:(NSString *)aName;

+ (void) RemoveObserver:(id)observer;

+ (void) PostNotification:(NSString *)aName userInfo:(NSDictionary *)aUserInfo;

@end
