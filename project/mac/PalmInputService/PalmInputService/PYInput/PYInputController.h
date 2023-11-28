#import <Cocoa/Cocoa.h>
#import <InputMethodKit/InputMethodKit.h>
#import "PYInputConvert.h"


@interface PYInputController : IMKInputController
{
    PYInputConvert                  *_convert;
}

- (void) commitString:(NSString *) string client:(id) sender;
- (void) composeString:(NSString *) string cursorPos:(int) cursorPos client:(id)sender;

@end
