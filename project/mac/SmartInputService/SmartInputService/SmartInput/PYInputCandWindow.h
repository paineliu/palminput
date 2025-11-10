#import <Foundation/Foundation.h>
#import "PYInputCandView.h"

@interface PYInputCandWindow : NSWindow
{
@private
    PYInputCandView*              _candView;
}

- (void) setDelegate:(id) delegate;

- (void) setSkin:(NSDictionary *) skinInfo andFont:(NSDictionary *) fontInfo;

- (void) setCandidates:(NSArray *) candidates hotItem:(int) hotItem prefixLen:(int) prefixLen pageUp: (BOOL) pageUpEnabled pageDown: (BOOL) pageDownEnabled showLabel: (BOOL) labelVisable CloudState: (PY_CLOUD_STATE) cloudState InputState: (PY_INPUT_STATE) nInputState;

- (BOOL) showWindow:(BOOL) appCaretPosExact maxWindowLevel:(BOOL) bMaxWindowLevel sender:(id) sender;
- (void) hideWindow;

@end
