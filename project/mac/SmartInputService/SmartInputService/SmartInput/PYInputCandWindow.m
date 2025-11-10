#import <InputMethodKit/InputMethodKit.h>
#import "PYInputCandWindow.h"

@implementation PYInputCandWindow

- (id) init
{
    self = [super initWithContentRect:NSRectFromCGRect((CGRectMake(0, 0, 0, 0))) styleMask:NSWindowStyleMaskBorderless backing:NSBackingStoreBuffered defer:NO];
    
    if (self)
    {
        [self setBackgroundColor:[NSColor clearColor]];
        [self setOpaque:NO];
        
        _candView = [[PYInputCandView  alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];

        [self setContentView:_candView];
    }

    return self;
}

- (void) setDelegate:(id) delegate
{
    [_candView setDelegate:delegate];
}

- (void) setSkin:(NSDictionary *) skinInfo andFont:(NSDictionary *) fontInfo
{
    [_candView setSkin:skinInfo andFont:fontInfo];
}

- (void) setCandidates:(NSArray *) candidates hotItem:(int) hotItem prefixLen:(int) prefixLen pageUp: (BOOL) pageUpEnabled  pageDown: (BOOL) pageDownEnabled showLabel: (BOOL) labelVisable CloudState:(PY_CLOUD_STATE) cloudState InputState:(PY_INPUT_STATE)nInputState
{
    [_candView setCandidates:candidates hotItem:hotItem prefixLen:prefixLen pageUp:pageUpEnabled pageDown:pageDownEnabled showLabel:labelVisable CloudState:cloudState InputState:nInputState];
}

- (BOOL) showWindow:(BOOL) appCaretPosExact maxWindowLevel:(BOOL) bMaxWindowLevel sender:(id) sender
{
    id _sender = sender;
    NSRect cursorRect = NSZeroRect;
    
    if (appCaretPosExact)
    {
        NSRect beginRect = NSZeroRect;

        [_sender attributesForCharacterIndex:0 lineHeightRectangle:&beginRect];

        cursorRect.origin.x = beginRect.origin.x;
        cursorRect.origin.y = beginRect.origin.y;

        NSInteger attrLen = [_sender markedRange].length;
        
        if (attrLen > 1)
        {
            NSRect endRect = NSZeroRect;

            [_sender attributesForCharacterIndex:(attrLen - 1) lineHeightRectangle:&endRect];

            if ((int)endRect.origin.y != 0)
            {
                cursorRect.origin.y = endRect.origin.y;
            }
        }
    }
    else
    {
        NSRect beginRect = NSZeroRect;
        [_sender attributesForCharacterIndex:0 lineHeightRectangle:&beginRect];
        if (beginRect.origin.x != 0 || beginRect.origin.y != 0)
        {
            cursorRect = beginRect;
        }
    }
        
    NSSize windowSize = [_candView getBoundSize];
    NSPoint windowOffset = NSMakePoint(0, 0);
    
    [_candView setNeedsDisplay:YES];

    if (bMaxWindowLevel)
    {
        [self setLevel:([(id<IMKTextInput>)_sender windowLevel] + 10000)];
    }
    else
    {
        [self setLevel:([(id<IMKTextInput>)_sender windowLevel] + 1)];
    }
    
    [self setFrame:[self getFrameRectForScreenWithCursorRect:cursorRect andWindowSize:windowSize andWindowOffset:windowOffset andScreenRect:[self getProperScreenRect:cursorRect]] display:YES];
    [self orderFront:self];
    
    return TRUE;
}

- (void) hideWindow
{
    [self setFrame:NSZeroRect display:YES];
    [self orderOut:self];
}

- (NSRect) getFrameRectForScreenWithCursorRect:(NSRect) cursorRect andWindowSize:(NSSize) windowSize andWindowOffset:(NSPoint) windowOffset andScreenRect:(NSRect) screenRect
{
    NSPoint windowPos;
    windowPos.x = cursorRect.origin.x;
    windowPos.y = cursorRect.origin.y - windowSize.height;
    
    CGFloat minX = NSMinX(screenRect);
    CGFloat maxX = NSMaxX(screenRect);
    CGFloat minY = NSMinY(screenRect);
    
    windowPos.x -= windowOffset.x;
    windowPos.y -= (windowOffset.y + 3);
    
    if (windowPos.x > maxX - windowSize.width)
        windowPos.x = maxX - windowSize.width;
    
    if (windowPos.x < minX)
        windowPos.x = minX;
    
    if (windowPos.y < minY)
        windowPos.y = cursorRect.origin.y > minY ? cursorRect.origin.y + cursorRect.size.height : minY;
    
    if (self.frame.origin.y != 0 && (fabs(self.frame.origin.y - windowPos.y) < cursorRect.size.height))
        windowPos.y = self.frame.origin.y;
    
    return NSRectFromCGRect(CGRectMake(windowPos.x, windowPos.y, windowSize.width, windowSize.height));
}

- (NSRect) getProperScreenRect:(NSRect) cursorRect
{
    NSRect screenRect = [[NSScreen mainScreen] visibleFrame];
    NSUInteger screenCount = [[NSScreen screens] count];
    
    if (screenCount > 1)
    {
        NSArray* screens = [NSScreen screens];

        for (int i = 0; i < screenCount; ++i)
        {
            NSRect rect = [[screens objectAtIndex:i] visibleFrame];
            if (NSPointInRect(cursorRect.origin, rect))
            {
                screenRect = rect;
                break;
            }
        }
    }
    
    return screenRect;
}

@end
