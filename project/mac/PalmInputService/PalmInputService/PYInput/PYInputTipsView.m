#import "PYInputTipsView.h"
#import "PYInputPubDef.h"

@implementation PYInputTipsView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    
    if (self)
    {
        NSDictionary * skinInfo = [[NSDictionary alloc] initWithObjectsAndKeys:
                                   [NSNumber numberWithFloat:0.0], PY_PREF_SKIN_BG_COLOR_R,
                                   [NSNumber numberWithFloat:0.0], PY_PREF_SKIN_BG_COLOR_G,
                                   [NSNumber numberWithFloat:0.0], PY_PREF_SKIN_BG_COLOR_B,
                                   [NSNumber numberWithFloat:1.0], PY_PREF_SKIN_BG_COLOR_A,
                                   [NSNumber numberWithFloat:255.0], PY_PREF_SKIN_HOT_COLOR_R,
                                   [NSNumber numberWithFloat:255.0], PY_PREF_SKIN_HOT_COLOR_G,
                                   [NSNumber numberWithFloat:255.0], PY_PREF_SKIN_HOT_COLOR_B,
                                   nil];
        
        [self setSkin:skinInfo];
    }
    
    return self;
}

- (void) setSkin:(NSDictionary *) skinInfo {
    
    float background_aplha = [[skinInfo objectForKey:PY_PREF_SKIN_BG_COLOR_A] floatValue];

    _itemColor = [NSColor colorWithSRGBRed:([[skinInfo objectForKey:PY_PREF_SKIN_HOT_COLOR_R] floatValue]/255.0) green:([[skinInfo objectForKey:PY_PREF_SKIN_HOT_COLOR_G] floatValue]/255.0) blue:([[skinInfo objectForKey:PY_PREF_SKIN_HOT_COLOR_B] floatValue]/255.0) alpha:1.0];
    _bgColor = [NSColor colorWithCalibratedRed:([[skinInfo objectForKey:PY_PREF_SKIN_BG_COLOR_R] floatValue]/255.0) green:([[skinInfo objectForKey:PY_PREF_SKIN_BG_COLOR_G] floatValue]/255.0) blue:([[skinInfo objectForKey:PY_PREF_SKIN_BG_COLOR_B] floatValue]/255.0) alpha:background_aplha];
    
    _borderColor = [NSColor colorWithCalibratedRed:([[skinInfo objectForKey:PY_PREF_SKIN_BG_COLOR_R] floatValue]/255.0) green:([[skinInfo objectForKey:PY_PREF_SKIN_BG_COLOR_G] floatValue]/255.0) blue:([[skinInfo objectForKey:PY_PREF_SKIN_BG_COLOR_B] floatValue]/255.0) alpha:background_aplha * 0.7];
}

- (void)drawRect:(NSRect)dirtyRect {
    
    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    
    CGFloat minx = 1;
    CGFloat maxx = PY_TIPS_WINDOW_WIDTH - 1;
    CGFloat miny = 1;
    CGFloat maxy = PY_TIPS_WINDOW_HEIGHT - 1;
    
    CGMutablePathRef path = CGPathCreateMutable();
    
    CGPathMoveToPoint(path, NULL, minx, miny + 3);
    
    CGPathAddArcToPoint(path, NULL, minx, maxy, maxx, maxy, 3);
    CGPathAddArcToPoint(path, NULL, maxx, maxy, maxx, miny, 3);
    CGPathAddArcToPoint(path, NULL, maxx, miny, minx, miny, 3);
    CGPathAddArcToPoint(path, NULL, minx, miny, minx, maxy, 3);
    
    CGPathCloseSubpath(path);
    
    CGColorRef shadowColor = CGColorCreateGenericRGB(0.0, 0.0, 0.0, 0.3);
    CGContextSetShadowWithColor(context, CGSizeMake(0, 0), 1, shadowColor);
    CGColorRelease(shadowColor);
    
    CGContextSetStrokeColorWithColor(context, [_borderColor CGColor]);
    CGContextSetLineWidth(context, 1.0);
    CGContextAddPath(context, path);
    CGContextStrokePath(context);
    
    CGContextAddPath(context, path);
    CGContextSetFillColorWithColor(context, [_bgColor CGColor]);
    CGContextFillPath(context);
    
    CGContextAddPath(context, path);
    CGContextClip(context);
    
    CGPathRelease(path);
    
    CGContextSetShadowWithColor(context, CGSizeMake(0, 0), 0, NULL);

    NSMutableDictionary *stringAttributes = [[NSMutableDictionary alloc] initWithCapacity:2];
    
    [stringAttributes setObject:_itemColor forKey:NSForegroundColorAttributeName];
    
    if (_tipsId == InputTipsIdSymbolFull)
    {
        [stringAttributes setObject:[NSFont systemFontOfSize:16] forKey:NSFontAttributeName];
        NSString *tipsString1 = @"，";
        NSString *tipsString2 = @"。";
        
        NSSize strSize1 = [tipsString1 sizeWithAttributes:stringAttributes];
        NSSize strSize2 = [tipsString2 sizeWithAttributes:stringAttributes];
        
        [tipsString1 drawInRect:NSMakeRect((PY_TIPS_WINDOW_WIDTH-strSize1.width)/2, (PY_TIPS_WINDOW_HEIGHT-strSize1.height)/2, strSize1.width, strSize1.height) withAttributes:stringAttributes];
        [tipsString2 drawInRect:NSMakeRect(PY_TIPS_WINDOW_WIDTH / 3 + (PY_TIPS_WINDOW_WIDTH-strSize2.width)/2, (PY_TIPS_WINDOW_HEIGHT-strSize2.height) / 2, strSize2.width, strSize2.height) withAttributes:stringAttributes];
    }
    else
    {
        NSString* tipsString = nil;
        
        switch (_tipsId) {
            case InputTipsIdLanguageChinese:
                [stringAttributes setObject:[NSFont systemFontOfSize:16] forKey:NSFontAttributeName];
                tipsString = @"中";
                break;
            case InputTipsIdLanguageEnglish:
                [stringAttributes setObject:[NSFont systemFontOfSize:12] forKey:NSFontAttributeName];
                tipsString = @"EN";
                break;
            case InputTipsIdSymbolFull:
                [stringAttributes setObject:[NSFont systemFontOfSize:16] forKey:NSFontAttributeName];
                tipsString = @"全";
                break;
            case InputTipsIdSymbolHalf:
                [stringAttributes setObject:[NSFont systemFontOfSize:16] forKey:NSFontAttributeName];
                tipsString = @"半";
                break;
            case InputTipsIdSimplifiedChinese:
                [stringAttributes setObject:[NSFont systemFontOfSize:16] forKey:NSFontAttributeName];
                tipsString = @"简";
                break;
            case InputTipsIdTraditionalChinese:
                [stringAttributes setObject:[NSFont systemFontOfSize:16] forKey:NSFontAttributeName];
                tipsString = @"繁";
                break;
            case InputTipsIdSymbolEnglish:
                [stringAttributes setObject:[NSFont systemFontOfSize:16] forKey:NSFontAttributeName];
                tipsString = @",.";
                break;
            default:
                break;
        }
        
        NSSize tipsSize = [tipsString sizeWithAttributes:stringAttributes];
        [tipsString drawInRect:NSMakeRect((PY_TIPS_WINDOW_WIDTH-tipsSize.width)/2, (PY_TIPS_WINDOW_HEIGHT-tipsSize.height)/2, tipsSize.width, tipsSize.height) withAttributes:stringAttributes];
    }
}

- (void)showTips:(InputTipsId) tipsId {
    
    _tipsId = tipsId;
}

@end
