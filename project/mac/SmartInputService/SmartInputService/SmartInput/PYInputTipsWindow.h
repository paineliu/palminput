#import <Foundation/Foundation.h>
#import "PYInputTipsView.h"

@interface PYInputTipsWindow : NSWindow {
    NSTimer     *_timer;
    PYInputTipsView  *_view;
}

- (void) setSkin:(NSDictionary *) skinInfo;
- (void) showTips:(InputTipsId) tipsId sender:(id) sender;
- (void) hide;

@end
