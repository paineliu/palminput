#import <Cocoa/Cocoa.h>
#import <InputMethodKit/InputMethodKit.h>
#import "PYInputSymbol.h"
#import "PYInputOption.h"

@interface PYInputConvert : NSObject
{
	id                              _sender;
    id                              _delegate;
    
    NSUInteger                      _pageDownByPeriodTimes;
    NSUInteger                      _lastModifiers;
    NSUInteger                      _lastEventType;
    UniChar                         _lastKeyChar;
    NSInteger                       _lastKeyCode;
    NSTimeInterval                  _lastEventTime;
    NSTimeInterval                  _lastCommitTime;
    NSTimeInterval                  _shiftPressedTime;
    NSTimeInterval                  _ctrlPressedTime;
    
    PY_INPUT_OPTION                 *_option;
    BOOL                            *_pChineseMode;
    
    BOOL                             _appCaretPosExact;
    BOOL                             _appMaxWindowLevel;
}

- (void) setDelegate:(id) delegate;

// controller

- (void) activate:(id) sender;
- (void) deactivate:(id) sender;
- (BOOL) handleEvent:(NSEvent *) event client:(id) sender;
// cand

- (BOOL) pageUp;
- (BOOL) pageDown;
- (BOOL) selPageItem:(int) itemId;

// delegate

- (void) commitString:(NSString *) string client:(id) sender;
- (void) composeString:(NSString *) string cursorPos:(int) cursorPos client:(id)sender;

//
- (BOOL) updateView:(id) sender;
- (BOOL) reset:(id) sender;

- (BOOL) processKey:(unichar) aKey client:(id) sender;

@end
