#import <Cocoa/Cocoa.h>
#import "palmengine.h"

@protocol FTCandViewDelegate

- (BOOL) pageUp;
- (BOOL) pageDown;
- (BOOL) selPageItem:(int) pageItemId;

@end


@interface PYInputCandView : NSView
{
    int _width;
    int _height;
    
    NSString *_candItemFontName;
    NSInteger _candItemFontSize;
    
    NSColor *_candItemNormalColor;
    NSColor *_candItemFocusColor;
    NSColor *_candItemPrefixColor;
    NSColor *_candIndexNormalColor;
    NSColor *_candIndexFocusColor;
    NSColor *_pageMarkEnableColor;
    NSColor *_pageMarkDisableColor;
    NSColor *_candBackgroundColor;
    NSColor *_candBorderColor;
    
    NSDictionary *_stringAttrCandItemNormal;
    NSDictionary *_stringAttrCandItemPrefix;
    NSDictionary *_stringAttrCandItemFocus;
    NSDictionary *_stringAttrCandIndexNormal;
    NSDictionary *_stringAttrCandIndexFocus;

    NSMutableArray *_candItemString;
    NSMutableArray *_candItemRect;
    
    CGImageRef  _cloudSameNormal;
    CGImageRef  _cloudSameFocus;
    CGImageRef  _cloudDiffNormal;
    CGImageRef  _cloudDiffFocus;
    CGImageRef  _cloudRequestNormal;
    CGImageRef  _cloudRequestFocus;
    CGImageRef  _cloudFail;
    
    NSRect   _pageUpRect;
    NSRect   _pageDownRect;
    
    BOOL _pageUpEnabled;
    BOOL _pageDownEnabled;
    BOOL _labelVisable;
    
    PY_CLOUD_STATE _cloudState;
    PY_INPUT_STATE _inputState;
    
    int _shadowTop;
    int _shadowBottom;
    int _shadowLeft;
    int _shadowRight;
    
    int _marginTop;
    int _marginBottom;
    int _marginLeft;
    int _marginRight;
    
    int _widthIndexSpace;
    int _widthWordSpace;
    int _widthPageMark;
    int _widthCloudMark;

    int _hotItem;
    int _candPrefixLen;
    
    id<FTCandViewDelegate>  _delegate;
}

- (void) setDelegate:(id) delegate;

- (void) setFont:(NSDictionary *) fontInfo;

- (void) setSkin:(NSDictionary *) skinInfo;

- (void) setSkin:(NSDictionary *) skinInfo andFont:(NSDictionary *) fontInfo;

- (void) setCandidates:(NSArray *) candidates hotItem:(int) hotItem  prefixLen:(int) prefixLen pageUp: (BOOL) pageUpEnabled  pageDown: (BOOL) pageDownEnabled showLabel: (BOOL) labelVisable CloudState:(PY_CLOUD_STATE) cloudState InputState:(PY_INPUT_STATE)nInputState;

- (NSSize) getBoundSize;

- (CGImageRef) imageWithImage:(CGImageRef)image MaskColor:(CGColorRef)maskColor;

- (BOOL) showWindow:(BOOL) appCaretPosExact maxWindowLevel:(BOOL) bMaxWindowLevel sender:(id) sender;
- (void) hideWindow;

@end
