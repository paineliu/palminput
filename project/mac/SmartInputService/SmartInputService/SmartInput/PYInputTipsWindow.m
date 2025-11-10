#import <carbon/Carbon.h>
#import "PYInputTipsWindow.h"

@implementation PYInputTipsWindow

- (id)init
{
    self = [super initWithContentRect:NSRectFromCGRect((CGRectMake(0, 0, PY_TIPS_WINDOW_WIDTH, PY_TIPS_WINDOW_HEIGHT))) styleMask:NSWindowStyleMaskBorderless backing:NSBackingStoreBuffered defer:NO];
    
    if (self)
    {
        [self setAlphaValue:1.0];
        [self setBackgroundColor:[NSColor clearColor]];
        [self setOpaque:NO];
        [self setMovableByWindowBackground:NO];
        [self setLevel:CGShieldingWindowLevel()];
        [self setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces];
        
        _view = [[PYInputTipsView alloc] init];
        [self setContentView:_view];
        
        _timer = nil;
        
        return self;
    }

    return nil;
}

-(void) disappear:(NSTimer *) timer
{
    [NSAnimationContext beginGrouping];
    [[NSAnimationContext currentContext] setDuration:0.15];
    [[self animator] setAlphaValue:0.0];
    [NSAnimationContext endGrouping];
}

- (void) setSkin:(NSDictionary *) skinInfo
{
    [_view setSkin:skinInfo];
}

- (void)showTips:(InputTipsId) tipsId sender:(id) sender
{
    NSRect cursorRect;
    [(id<IMKTextInput>) sender attributesForCharacterIndex:0 lineHeightRectangle:&cursorRect];

    NSPoint point = cursorRect.origin;
    NSRect frameRect = [[NSScreen mainScreen] frame];

    NSArray* screens = [NSScreen screens];
    NSUInteger screenCount = [screens count];

    if (screenCount > 1)
    {
        for (NSUInteger i = 0; i < screenCount; i++)
        {
            NSRect rect = [[screens objectAtIndex:i] frame];
            if (NSPointInRect(cursorRect.origin, rect))
            {
                frameRect = rect;
                break;
            }
        }
    }

    frameRect.origin.y    += PY_TIPS_WINDOW_HEIGHT;
    frameRect.size.height -= PY_TIPS_WINDOW_HEIGHT;
    frameRect.size.width  -= PY_TIPS_WINDOW_WIDTH;
    
    if (!NSPointInRect(point, frameRect))
    {
        point = [NSEvent mouseLocation];
    }

    [_view showTips:tipsId];
    [_view setNeedsDisplay:YES];
    
    [self setFrameTopLeftPoint:NSMakePoint(point.x + 4, point.y - 1)];
 
    if (_timer)
    {
        [_timer invalidate];
    }
    
    _timer = [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(disappear:) userInfo:nil repeats:NO];
    
    [self setAlphaValue:0.0];
    [NSAnimationContext beginGrouping];
    [[NSAnimationContext currentContext] setDuration:0.15];
    [[self animator] setAlphaValue:1.0];
    [NSAnimationContext endGrouping];
    
    [self orderFront:sender];
    [[NSApplication sharedApplication] arrangeInFront:nil];
}

- (void) hide
{
    [self orderOut:nil];
}

@end
