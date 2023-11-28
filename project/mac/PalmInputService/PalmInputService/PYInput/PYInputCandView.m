#import "PYInputPubDef.h"
#import "PYInputCandView.h"

@implementation PYInputCandView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    
    if (self)
    {
        _width = 200;
        _height = 48;
        
        _candItemFontName = @"";
        _candPrefixLen = 0;
        
        _candItemRect = [[NSMutableArray alloc] init];
        _candItemString = [[NSMutableArray alloc] init];
        
        _cloudSameNormal = NULL;
        _cloudSameFocus = NULL;
        _cloudDiffNormal = NULL;
        _cloudDiffFocus = NULL;
        _cloudRequestNormal = NULL;
        _cloudRequestFocus = NULL;
        _cloudFail = NULL;

        NSDictionary * fontInfo = [[NSDictionary alloc] initWithObjectsAndKeys:
                                @"Lucida Grande", PY_PREF_CAND_FONT_NAME,
                                [NSNumber numberWithInt:16], PY_PREF_CAND_FONT_SIZE,
                                nil];

        NSDictionary * skinInfo = [[NSDictionary alloc] initWithObjectsAndKeys:
                                [NSNumber numberWithFloat:0.0], PY_PREF_SKIN_BG_COLOR_R,
                                [NSNumber numberWithFloat:0.0], PY_PREF_SKIN_BG_COLOR_G,
                                [NSNumber numberWithFloat:0.0], PY_PREF_SKIN_BG_COLOR_B,
                                [NSNumber numberWithFloat:198.0], PY_PREF_SKIN_NOR_COLOR_R,
                                [NSNumber numberWithFloat:198.0], PY_PREF_SKIN_NOR_COLOR_G,
                                [NSNumber numberWithFloat:198.0], PY_PREF_SKIN_NOR_COLOR_B,
                                [NSNumber numberWithFloat:255.0], PY_PREF_SKIN_HOT_COLOR_R,
                                [NSNumber numberWithFloat:255.0], PY_PREF_SKIN_HOT_COLOR_G,
                                [NSNumber numberWithFloat:255.0], PY_PREF_SKIN_HOT_COLOR_B,
                                [NSNumber numberWithFloat:1.0], PY_PREF_SKIN_BG_COLOR_A,
                                nil];

        [self setSkin:skinInfo andFont:fontInfo];
        
        [self setCandidates:[[NSArray alloc] initWithObjects:@"输入法", @"输入", @"数", @"书", @"属",nil] hotItem:(0) prefixLen:0 pageUp:FALSE pageDown:TRUE showLabel:TRUE CloudState:CLOUD_STATE_NONE InputState:INPUT_STATE_NORMAL];
    }
    
    return self;
}

- (void) dealloc
{
    if (_cloudSameNormal)
        CGImageRelease(_cloudSameNormal);
    if (_cloudSameFocus)
        CGImageRelease(_cloudSameFocus);
    if (_cloudDiffNormal)
        CGImageRelease(_cloudDiffNormal);
    if (_cloudDiffFocus)
        CGImageRelease(_cloudDiffFocus);
    if (_cloudRequestNormal)
        CGImageRelease(_cloudRequestNormal);
    if (_cloudRequestFocus)
        CGImageRelease(_cloudRequestFocus);
    if (_cloudFail)
        CGImageRelease(_cloudFail);
}

- (BOOL) showWindow:(BOOL) appCaretPosExact maxWindowLevel:(BOOL) bMaxWindowLevel sender:(id) sender
{
    [self setNeedsDisplay:YES];
    [self setHidden:FALSE];
    return TRUE;
}

- (void) hideWindow
{
    [self setHidden:TRUE];
}

- (void) setSkin:(NSDictionary *) skinInfo andFont:(NSDictionary *) fontInfo
{
    [self setSkin:skinInfo update:NO];
    [self setFont:fontInfo update:YES];
}

- (void) updateData
{
    if (_candItemFontName &&
        _candIndexNormalColor &&
        _candItemNormalColor &&
        _candIndexFocusColor &&
        _candItemFocusColor &&
        _candItemPrefixColor)
    {
        NSFont *indexFont = nil;
        NSFont *itemFont = nil;
        
        if (@available(macOS 10.14, *)) {
            indexFont = [NSFont systemFontOfSize:_candItemFontSize * 3 / 4 weight:NSFontWeightMedium];
            itemFont = [NSFont systemFontOfSize:_candItemFontSize weight:NSFontWeightMedium];
        } else {
            indexFont = [NSFont fontWithName:_candItemFontName size:_candItemFontSize * 3 / 4];
            itemFont  = [NSFont fontWithName:_candItemFontName size:_candItemFontSize];
        }
        
        _stringAttrCandIndexNormal = [self makeStringAttributesWithFont:indexFont andColor:_candIndexNormalColor];
        _stringAttrCandItemNormal  = [self makeStringAttributesWithFont:itemFont andColor:_candItemNormalColor];
        _stringAttrCandIndexFocus  = [self makeStringAttributesWithFont:indexFont andColor:_candIndexFocusColor];
        _stringAttrCandItemFocus   = [self makeStringAttributesWithFont:itemFont andColor:_candItemFocusColor];
        _stringAttrCandItemPrefix  = [self makeStringAttributesWithFont:itemFont andColor:_candItemPrefixColor];
        
        NSSize sizeChinese = [[NSString stringWithFormat:@"我"] sizeWithAttributes:_stringAttrCandItemNormal];

        _shadowTop = 12;
        _shadowBottom = 2;
        _shadowLeft = 8;
        _shadowRight = 8;
        
        _widthIndexSpace = 1;
        _widthWordSpace = 9;
        _widthCloudMark = 0;
        _widthPageMark = sizeChinese.height / 2;
        
        
        _marginLeft = sizeChinese.width / 3;
        _marginRight = sizeChinese.width / 3;
        _marginTop = 6;
        _marginBottom = 6;
        
        if (_marginLeft < 8)
        {
            _marginLeft = 8;
        }
        
        if (_marginRight < 8)
        {
            _marginRight = 8;
        }
    }
}

- (void) setFont:(NSDictionary *) fontInfo
{
    [self setFont:fontInfo update:YES];
}

- (void) setFont:(NSDictionary *) fontInfo update:(BOOL) updateData
{
    NSInteger fontSize = [[fontInfo objectForKey:PY_PREF_CAND_FONT_SIZE] intValue];
    NSString  *fontName = [fontInfo objectForKey:PY_PREF_CAND_FONT_NAME];
    
    if (fontSize < 12) {
        fontSize = 12;
    }else if (fontSize > 72){
        fontSize = 72;
    }
    fontSize = 16;
    _candItemFontSize = fontSize;
    
    if (fontName != nil)
    {
        _candItemFontName = fontName;
    }
    
    if (updateData)
    {
        [self updateData];
    }
}

- (void) setSkin:(NSDictionary *) skinInfo
{
    [self setSkin:skinInfo update:YES];
}

- (CGImageRef)imageWithImage:(CGImageRef)image MaskColor:(CGColorRef)maskColor
{
    CGImageRef resultImage = nil;
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate (NULL, CGImageGetWidth(image), CGImageGetHeight(image), 8, 0, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(colorSpace);
    
    if (context)
    {
        CGContextClipToMask(context, CGRectMake(0, 0, CGImageGetWidth(image), CGImageGetHeight(image)), image);
        CGContextSetFillColorWithColor(context, maskColor);
        CGContextFillRect(context, CGRectMake(0, 0, CGImageGetWidth(image), CGImageGetHeight(image)));
        resultImage = CGBitmapContextCreateImage(context);
        CGContextRelease(context);
    }
    
    return resultImage;
}


- (void) setSkin:(NSDictionary *) skinInfo update:(BOOL) updateData
{
    float background_aplha = [[skinInfo objectForKey:PY_PREF_SKIN_BG_COLOR_A] floatValue];
    
    // 正常候选颜色
    _candIndexNormalColor = [NSColor colorWithCalibratedRed:([[skinInfo objectForKey:PY_PREF_SKIN_NOR_COLOR_R] floatValue]/255.0) green:([[skinInfo objectForKey:PY_PREF_SKIN_NOR_COLOR_G] floatValue]/255.0) blue:([[skinInfo objectForKey:PY_PREF_SKIN_NOR_COLOR_B] floatValue]/255.0) alpha:1.0];
    
    _candItemNormalColor  = [NSColor colorWithCalibratedRed:([[skinInfo objectForKey:PY_PREF_SKIN_NOR_COLOR_R] floatValue]/255.0) green:([[skinInfo objectForKey:PY_PREF_SKIN_NOR_COLOR_G] floatValue]/255.0) blue:([[skinInfo objectForKey:PY_PREF_SKIN_NOR_COLOR_B] floatValue]/255.0) alpha:1.0];
    
    // 焦点候选颜色
    _candIndexFocusColor  = [NSColor colorWithCalibratedRed:([[skinInfo objectForKey:PY_PREF_SKIN_HOT_COLOR_R] floatValue]/255.0) green:([[skinInfo objectForKey:PY_PREF_SKIN_HOT_COLOR_G] floatValue]/255.0) blue:([[skinInfo objectForKey:PY_PREF_SKIN_HOT_COLOR_B] floatValue]/255.0) alpha:1.0];
    _candItemFocusColor   = [NSColor colorWithCalibratedRed:([[skinInfo objectForKey:PY_PREF_SKIN_HOT_COLOR_R] floatValue]/255.0) green:([[skinInfo objectForKey:PY_PREF_SKIN_HOT_COLOR_G] floatValue]/255.0) blue:([[skinInfo objectForKey:PY_PREF_SKIN_HOT_COLOR_B] floatValue]/255.0) alpha:1.0];
    
    // 候选前缀颜色
    _candItemPrefixColor  = [NSColor colorWithCalibratedRed:([[skinInfo objectForKey:PY_PREF_SKIN_HOT_COLOR_R] floatValue]/255.0) green:([[skinInfo objectForKey:PY_PREF_SKIN_HOT_COLOR_G] floatValue]/255.0) blue:([[skinInfo objectForKey:PY_PREF_SKIN_HOT_COLOR_B] floatValue]/255.0) alpha:0.6];
    
    // 翻页符号颜色
    _pageMarkEnableColor  = [NSColor colorWithCalibratedRed:([[skinInfo objectForKey:PY_PREF_SKIN_NOR_COLOR_R] floatValue]/255.0) green:([[skinInfo objectForKey:PY_PREF_SKIN_NOR_COLOR_G] floatValue]/255.0) blue:([[skinInfo objectForKey:PY_PREF_SKIN_NOR_COLOR_B] floatValue]/255.0) alpha:1.0];
    
    _pageMarkDisableColor = [NSColor colorWithCalibratedRed:([[skinInfo objectForKey:PY_PREF_SKIN_NOR_COLOR_R] floatValue]/255.0) green:([[skinInfo objectForKey:PY_PREF_SKIN_NOR_COLOR_G] floatValue]/255.0) blue:([[skinInfo objectForKey:PY_PREF_SKIN_NOR_COLOR_B] floatValue]/255.0) alpha:0.6];
    
    // 背景颜色
    _candBackgroundColor  = [NSColor colorWithCalibratedRed:([[skinInfo objectForKey:PY_PREF_SKIN_BG_COLOR_R] floatValue]/255.0) green:([[skinInfo objectForKey:PY_PREF_SKIN_BG_COLOR_G] floatValue]/255.0) blue:([[skinInfo objectForKey:PY_PREF_SKIN_BG_COLOR_B] floatValue]/255.0) alpha:background_aplha];
    
    // 边框颜色
    _candBorderColor  = [NSColor colorWithCalibratedRed:([[skinInfo objectForKey:PY_PREF_SKIN_BG_COLOR_R] floatValue]/255.0) green:([[skinInfo objectForKey:PY_PREF_SKIN_BG_COLOR_G] floatValue]/255.0) blue:([[skinInfo objectForKey:PY_PREF_SKIN_BG_COLOR_B] floatValue]/255.0) alpha:background_aplha*0.6];
    
    if (@available(macOS 10.14, *))
    {
        _candIndexFocusColor = [NSColor disabledControlTextColor];
        _candItemNormalColor = [NSColor controlTextColor];
        _candIndexFocusColor = [NSColor alternateSelectedControlTextColor];
        _candItemFocusColor = [NSColor alternateSelectedControlTextColor];
        _pageMarkEnableColor = [NSColor controlTextColor];
        _pageMarkDisableColor = [NSColor disabledControlTextColor];
        _candBackgroundColor = [NSColor windowBackgroundColor];

        CGFloat red = 0.0, green = 0.0, blue = 0.0, alpha = 0.3;
        _candBorderColor = [NSColor colorWithCalibratedRed:red green:green blue:blue alpha:alpha];
    }
    
//    // 加载云的资源
//    NSString* path = nil;
//    CGDataProviderRef provider = nil;
//    CGImageRef maskImage = nil;
//    CGColorRef color = nil;
//    path = [[NSBundle mainBundle] pathForImageResource:PY_CLOUD_RES_SAME_FILE_NAME];
//    if (path) {
//        provider = CGDataProviderCreateWithFilename([path UTF8String]);
//        if (provider) {
//            maskImage = CGImageCreateWithPNGDataProvider(provider, NULL, YES, kCGRenderingIntentDefault);
//            if (maskImage) {
//                color = CGColorCreateGenericRGB([_candItemNormalColor redComponent], [_candItemNormalColor greenComponent], [_candItemNormalColor blueComponent], 0.8);
//                if (color) {
//                    _cloudSameNormal = [self imageWithImage:maskImage MaskColor:color];
//                    CGColorRelease(color);
//                }
//                color = CGColorCreateGenericRGB([_candItemFocusColor redComponent], [_candItemFocusColor greenComponent], [_candItemFocusColor blueComponent], 0.8);
//                if (color) {
//                    _cloudSameFocus = [self imageWithImage:maskImage MaskColor:color];
//                    CGColorRelease(color);
//                }
//                CGImageRelease(maskImage);
//            }
//            CGDataProviderRelease(provider);
//        }
//    }
//
//    path = nil;
//    provider = nil;
//    maskImage = nil;
//    color = nil;
//    path = [[NSBundle mainBundle] pathForImageResource:PY_CLOUD_RES_DIFF_FILE_NAME];
//    if (path) {
//        provider = CGDataProviderCreateWithFilename([path UTF8String]);
//        if (provider) {
//            maskImage = CGImageCreateWithPNGDataProvider(provider, NULL, YES, kCGRenderingIntentDefault);
//            if (maskImage) {
//                color = CGColorCreateGenericRGB([_candItemNormalColor redComponent], [_candItemNormalColor greenComponent], [_candItemNormalColor blueComponent], 0.8);
//                if (color) {
//                    _cloudDiffNormal = [self imageWithImage:maskImage MaskColor:color];
//                    CGColorRelease(color);
//                }
//                color = CGColorCreateGenericRGB([_candItemFocusColor redComponent], [_candItemFocusColor greenComponent], [_candItemFocusColor blueComponent], 0.8);
//                if (color) {
//                    _cloudDiffFocus = [self imageWithImage:maskImage MaskColor:color];
//                    CGColorRelease(color);
//                }
//                CGImageRelease(maskImage);
//            }
//            CGDataProviderRelease(provider);
//        }
//    }
//
//    path = nil;
//    provider = nil;
//    maskImage = nil;
//    color = nil;
//    path = [[NSBundle mainBundle] pathForImageResource:PY_CLOUD_RES_LOAD_FILE_NAME];
//    if (path) {
//        provider = CGDataProviderCreateWithFilename([path UTF8String]);
//        if (provider) {
//            maskImage = CGImageCreateWithPNGDataProvider(provider, NULL, YES, kCGRenderingIntentDefault);
//            if (maskImage) {
//                color = CGColorCreateGenericRGB([_candItemNormalColor redComponent], [_candItemNormalColor greenComponent], [_candItemNormalColor blueComponent], 0.8);
//                if (color) {
//                    _cloudRequestNormal = [self imageWithImage:maskImage MaskColor:color];
//                    CGColorRelease(color);
//                }
//                color = CGColorCreateGenericRGB([_candItemFocusColor redComponent], [_candItemFocusColor greenComponent], [_candItemFocusColor blueComponent], 0.8);
//                if (color) {
//                    _cloudRequestFocus = [self imageWithImage:maskImage MaskColor:color];
//                    CGColorRelease(color);
//                }
//                CGImageRelease(maskImage);
//            }
//            CGDataProviderRelease(provider);
//        }
//    }
//
//    path = nil;
//    provider = nil;
//    maskImage = nil;
//    color = nil;
//    path = [[NSBundle mainBundle] pathForImageResource:PY_CLOUD_RES_FAIL_FILE_NAME];
//    if (path) {
//        provider = CGDataProviderCreateWithFilename([path UTF8String]);
//        if (provider) {
//            maskImage = CGImageCreateWithPNGDataProvider(provider, NULL, YES, kCGRenderingIntentDefault);
//            if (maskImage) {
//                color = CGColorCreateGenericRGB([_candItemNormalColor redComponent], [_candItemNormalColor greenComponent], [_candItemNormalColor blueComponent], 0.3);
//                if (color) {
//                    _cloudFail = [self imageWithImage:maskImage MaskColor:color];
//                    CGColorRelease(color);
//                }
//                CGImageRelease(maskImage);
//            }
//            CGDataProviderRelease(provider);
//        }
//    }
    
    if (updateData)
    {
        [self updateData];
    }
}

- (NSDictionary *) makeStringAttributesWithFont:(NSFont *)font andColor: (NSColor *) theColor
{
    NSMutableDictionary* stringAttributes = [[NSMutableDictionary alloc] initWithCapacity:2];
    
    [stringAttributes setObject:font forKey:NSFontAttributeName];
    [stringAttributes setObject:theColor forKey:NSForegroundColorAttributeName];
    
    return stringAttributes;
}

- (void) setCandidates:(NSArray *) candidates hotItem:(int) hotItem  prefixLen:(int) prefixLen pageUp: (BOOL) pageUpEnabled  pageDown: (BOOL) pageDownEnabled showLabel: (BOOL) labelVisable CloudState:(PY_CLOUD_STATE) cloudState InputState:(PY_INPUT_STATE)nInputState
{
    int width = 0;
    int height = 0;
    
    _hotItem = hotItem;
    _pageUpEnabled = pageUpEnabled;
    _pageDownEnabled = pageDownEnabled;
    _candPrefixLen = prefixLen;
    _labelVisable = labelVisable;
    _cloudState = cloudState;
    _inputState = nInputState;
    
    [_candItemRect removeAllObjects];
    [_candItemString removeAllObjects];
    
    if (_inputState == INPUT_STATE_MODE_U_ENTER || _inputState == INPUT_STATE_MODE_V_ENTER)
    {
        NSString* itemString = nil;
        if (_inputState == INPUT_STATE_MODE_U_ENTER)
        {
            itemString = PY_TIP_MODE_U;
        }
        else
        {
            itemString = PY_TIP_MODE_V;
        }
        
        NSSize sizeItem = [itemString sizeWithAttributes:_stringAttrCandItemNormal];
        
        [_candItemString addObject:itemString];
        [_candItemRect addObject:[NSValue valueWithRect:NSMakeRect(_shadowLeft + _marginLeft, _marginTop + _shadowTop, sizeItem.width, sizeItem.height)]];
        
        _height = sizeItem.height + _marginTop + _marginBottom + _shadowTop + _shadowBottom;
        _width = sizeItem.width + _shadowLeft + _marginLeft + _marginRight + _shadowRight;
    }
    else
    {
        NSInteger count = [candidates count];
        
        for (NSUInteger i = 0; i<count; i++)
        {
            [_candItemString addObject:[candidates objectAtIndex:i]];
        }
        
        NSString *item = nil;
        NSSize sizeItem;
        NSSize sizeIndex;
        NSRect rectItem;
        int widthItem;
        int heightItem;
        
        width += _shadowLeft + _marginLeft;
        
        for (int i=0; i<count; i++)
        {
            item = [candidates objectAtIndex:i];
            
            if (_labelVisable)
            {
                if (_inputState == INPUT_STATE_MODE_V_IN)
                    sizeIndex = [[NSString stringWithFormat:@"%c.",i+'a'] sizeWithAttributes:_stringAttrCandIndexNormal];
                else
                    sizeIndex = [[NSString stringWithFormat:@"%d.",i+1] sizeWithAttributes:_stringAttrCandIndexNormal];
            }
            else
            {
                sizeIndex = NSZeroSize;
            }
            
            if (i == 1 && cloudState != CLOUD_STATE_NONE)
            {
                if (_cloudState == CLOUD_STATE_DIFF)
                {
                    NSString *candStr = [NSString stringWithFormat:@"%@%@", item, PY_CLOUD_REPLACE_STRING_SHORT];
                    sizeItem = [candStr sizeWithAttributes:_stringAttrCandItemNormal];
                }
                else if (cloudState == CLOUD_STATE_REQUEST || cloudState == CLOUD_STATE_SAME || cloudState == CLOUD_STATE_FAIL)
                {
                    sizeItem = [PY_CLOUD_REPLACE_STRING_LONG sizeWithAttributes:_stringAttrCandItemNormal];
                }
            }
            else
            {
                sizeItem = [item sizeWithAttributes:_stringAttrCandItemNormal];
            }
            widthItem = sizeIndex.width + sizeItem.width;
            heightItem = sizeItem.height;
            
            rectItem = NSMakeRect(width, _marginTop + _shadowTop, widthItem, heightItem);
            [_candItemRect addObject:[NSValue valueWithRect:rectItem]];
            
            width +=  widthItem + _widthWordSpace + _widthIndexSpace;
            
            if (height < heightItem)
            {
                height = heightItem;
            }
        }
        
        _height = height + _marginTop + _marginBottom + _shadowTop + _shadowBottom;
        _width = width + _marginRight + _shadowRight;
        
        if (count != 1 || _labelVisable == TRUE || _pageDownEnabled == TRUE)
        {
            _pageUpRect.origin.x = width;
            _pageUpRect.origin.y = _marginTop + _shadowTop + height / 2;
            _pageUpRect.size.width = _widthPageMark;
            _pageUpRect.size.height = height / 2;
            
            _pageDownRect.origin.x = width;
            _pageDownRect.origin.y = _marginTop + _shadowTop;
            _pageDownRect.size.width = _widthPageMark;
            _pageDownRect.size.height = height / 2;
            
            _width += _widthPageMark;
        }
    }
    
    [self setFrameSize:NSMakeSize(_width, _height)];
}

- (void) setDelegate:(id) delegate
{
    _delegate = delegate;
}

- (void) mouseDown:(NSEvent *) theEvent
{
    NSPoint event_location = [theEvent locationInWindow];
    
    if (NSPointInRect(event_location, _pageUpRect))
    {
        [_delegate pageUp];
        return;
    }
    
    if (NSPointInRect(event_location, _pageDownRect))
    {
        [_delegate pageDown];
        return;
    }
    
    NSUInteger size = [_candItemRect count];
    for (int i = 0; i <size; ++i)
    {
        NSRect rect = [[_candItemRect objectAtIndex:i] rectValue];
        if (NSPointInRect(event_location, rect))
        {
            [_delegate selPageItem:i];
            break;
        }
    }
}

#pragma mark - Draw

- (void) drawCandidates:(CGContextRef) context
{
    if (_inputState == INPUT_STATE_MODE_U_ENTER || _inputState == INPUT_STATE_MODE_V_ENTER)
    {
        [[_candItemString objectAtIndex:0] drawInRect:[[_candItemRect objectAtIndex:0] rectValue] withAttributes:_stringAttrCandItemNormal];
    }
    else
    {
        if ([_candItemString count] != 1 || _labelVisable == TRUE || _pageDownEnabled == TRUE)
        {
            [self drawPageMark:context];
        }
        
        NSInteger count = [_candItemString count];
        NSString *itemString = nil;
        NSString *indexString = nil;
        NSSize sizeItem;
        NSSize sizeIndex;
        NSRect rectItem;
        NSRect rectIndex;
        
        for (int i=0; i<count; i++)
        {
            itemString = [_candItemString objectAtIndex:i];
            rectItem = [[_candItemRect objectAtIndex:i] rectValue];
            sizeItem = [itemString sizeWithAttributes:_stringAttrCandItemNormal];
            
            if (_labelVisable)
            {
                if (_inputState == INPUT_STATE_MODE_V_IN)
                    indexString = [NSString stringWithFormat:@"%c.",i+'a'];
                else
                    indexString = [NSString stringWithFormat:@"%d.",i+1];
                
                
                sizeIndex = [indexString sizeWithAttributes:_stringAttrCandIndexNormal];
                rectIndex = NSMakeRect(rectItem.origin.x,
                                       rectItem.origin.y + (rectItem.size.height - sizeIndex.height) / 2,
                                       sizeIndex.width,
                                       sizeIndex.height);
                
                rectItem = NSMakeRect(rectItem.origin.x + sizeIndex.width + _widthIndexSpace,
                                      rectItem.origin.y,
                                      sizeItem.width,
                                      sizeItem.height);
                
                [indexString drawInRect:rectIndex withAttributes:(i == _hotItem ? _stringAttrCandIndexFocus : _stringAttrCandIndexNormal)];
            }
            else
            {
                rectItem = NSMakeRect(rectItem.origin.x,
                                      rectItem.origin.y,
                                      sizeItem.width,
                                      sizeItem.height);
            }
            
            if (i == 0 && _candPrefixLen > 0)
            {
                NSString *prefix = [itemString substringToIndex:_candPrefixLen];
                NSSize    prefixSize = [prefix sizeWithAttributes:_stringAttrCandItemNormal];
                
                rectItem = [[_candItemRect objectAtIndex:i] rectValue];
                
                NSRect prefixRect = NSMakeRect(rectItem.origin.x + sizeIndex.width + _widthIndexSpace,
                                                  rectItem.origin.y,
                                                  prefixSize.width,
                                                  prefixSize.height);
                
                [prefix drawInRect:prefixRect withAttributes:(_stringAttrCandItemPrefix)];
                
                NSString *postfix = [itemString substringFromIndex:_candPrefixLen];
                NSSize postfixSize = [postfix sizeWithAttributes:_stringAttrCandItemNormal];
                
                NSRect postfixRect = NSMakeRect(rectItem.origin.x + sizeIndex.width + _widthIndexSpace + prefixSize.width,
                                                rectItem.origin.y,
                                                postfixSize.width,
                                                postfixSize.height);
                
                [postfix drawInRect:postfixRect withAttributes:(i == _hotItem ? _stringAttrCandItemFocus : _stringAttrCandItemNormal)];
            }
            else if (i == 1 && _cloudState != CLOUD_STATE_NONE)
            {
                if (_cloudState == CLOUD_STATE_REQUEST)
                {
                    size_t fHeight = CGImageGetHeight(_cloudRequestNormal);
                    size_t offset = (rectItem.size.height-fHeight)/2;
                    CGContextDrawImage(context, CGRectMake(rectItem.origin.x, rectItem.origin.y+offset, CGImageGetWidth(_cloudRequestNormal), fHeight), (i == _hotItem ? _cloudRequestFocus : _cloudRequestNormal));
                }
                else if (_cloudState == CLOUD_STATE_DIFF)
                {
                    NSSize wordSize = [itemString sizeWithAttributes:_stringAttrCandItemNormal];
                    [itemString drawInRect:rectItem withAttributes:(i == _hotItem ? _stringAttrCandItemFocus : _stringAttrCandItemNormal)];
                    CGContextDrawImage(context, CGRectMake(rectItem.origin.x + wordSize.width + 2, rectItem.origin.y+rectItem.size.height-CGImageGetHeight(_cloudDiffNormal), CGImageGetWidth(_cloudDiffNormal), CGImageGetHeight(_cloudDiffNormal)), (i == _hotItem ? _cloudDiffFocus : _cloudDiffNormal));
                }
                else if (_cloudState == CLOUD_STATE_SAME)
                {
                    size_t fHeight = CGImageGetHeight(_cloudSameNormal);
                    size_t offset = (rectItem.size.height-fHeight)/2;
                    CGContextDrawImage(context, CGRectMake(rectItem.origin.x, rectItem.origin.y+offset, CGImageGetWidth(_cloudSameNormal), fHeight), (i == _hotItem ? _cloudSameFocus : _cloudSameNormal));
                }
                else if (_cloudState == CLOUD_STATE_FAIL)
                {
                    size_t fHeight = CGImageGetHeight(_cloudFail);
                    size_t offset = (rectItem.size.height-fHeight)/2;
                    CGContextDrawImage(context, CGRectMake(rectItem.origin.x, rectItem.origin.y+offset, CGImageGetWidth(_cloudFail), fHeight), (i == _hotItem ? _cloudFail : _cloudFail));
                }
            }
            else
            {
                [itemString drawInRect:rectItem withAttributes:(i == _hotItem ? _stringAttrCandItemFocus : _stringAttrCandItemNormal)];
                
            }
        }
    }
}

- (void) drawFocus:(CGContextRef) context rect:(CGRect)rect
{
    CGContextSaveGState(context);
    
    CGFloat minx = rect.origin.x - 3;
    CGFloat maxx = rect.origin.x + rect.size.width + 6;
    CGFloat miny = rect.origin.y - 2;
    CGFloat maxy = rect.origin.y + rect.size.height + 4;
    
    CGMutablePathRef path = CGPathCreateMutable();
    CGPathMoveToPoint(path, NULL, minx, miny + 5);
    CGPathAddArcToPoint(path, NULL, minx, maxy, maxx, maxy, 5);
    CGPathAddArcToPoint(path, NULL, maxx, maxy, maxx, miny, 5);
    CGPathAddArcToPoint(path, NULL, maxx, miny, minx, miny, 5);
    CGPathAddArcToPoint(path, NULL, minx, miny, minx, maxy, 5);
    CGPathCloseSubpath(path);
    
    CGContextAddPath(context, path);
    
    if (@available(macOS 10.14, *)) {
        CGContextSetFillColorWithColor(context, [NSColor controlAccentColor].CGColor);
    } else {
        CGContextSetFillColorWithColor(context, [NSColor blueColor].CGColor);
    }
    
    CGContextFillPath(context);
    
    CGContextAddPath(context, path);
    CGContextClip(context);
    
    CGPathRelease(path);
    
    CGContextRestoreGState(context);
}

- (void) drawBackground:(CGContextRef) context
{
    CGContextSaveGState(context);
    
    CGFloat minx = _shadowLeft;
    CGFloat maxx = _width - _shadowRight;
    CGFloat miny = _shadowTop;
    CGFloat maxy = _height - _shadowBottom;
    
    CGMutablePathRef path = CGPathCreateMutable();
    CGPathMoveToPoint(path, NULL, minx, miny + 5);
    CGPathAddArcToPoint(path, NULL, minx, maxy, maxx, maxy, 5);
    CGPathAddArcToPoint(path, NULL, maxx, maxy, maxx, miny, 5);
    CGPathAddArcToPoint(path, NULL, maxx, miny, minx, miny, 5);
    CGPathAddArcToPoint(path, NULL, minx, miny, minx, maxy, 5);
    CGPathCloseSubpath(path);
    
    CGColorRef shadowColor = CGColorCreateGenericRGB(0, 0, 0, 0.4);
    CGContextSetShadowWithColor(context, CGSizeMake(0, -4), 8, shadowColor);
    CGColorRelease(shadowColor);
    
    CGContextSetStrokeColorWithColor(context, [_candBorderColor CGColor]);
    CGContextSetLineWidth(context, 1.0);
    CGContextAddPath(context, path);
    CGContextStrokePath(context);
    
    CGContextAddPath(context, path);
    CGContextSetFillColorWithColor(context, [_candBackgroundColor CGColor]);
    CGContextFillPath(context);
    
    CGContextAddPath(context, path);
    CGContextClip(context);
    
    CGPathRelease(path);
    
    CGContextRestoreGState(context);
}

// 画翻页按钮
- (void) drawPageMark:(CGContextRef) context
{
    CGContextSaveGState(context);
    //CGContextSetShadowWithColor(context, CGSizeMake(0, 0), 0, NULL);
    CGMutablePathRef path;
    
    float x1 = _width - _shadowRight - _marginRight - (float)_widthPageMark / 2;
    float y1 = _shadowTop + _marginTop + (float)_widthPageMark * 14 / 54;
    float x2 = x1 - (float)_widthPageMark / 2;
    float y2 = y1 + (float)_widthPageMark * 2 / 3;
    float x3 = x1 + (float)_widthPageMark / 2;
    float y3 = y1 + (float)_widthPageMark * 2 / 3;
    
    path = CGPathCreateMutable();
    CGPathMoveToPoint(path, NULL, x1, y1);
    CGPathAddLineToPoint(path, NULL, x2, y2);
    CGPathAddLineToPoint(path, NULL, x3, y3);
    CGPathAddLineToPoint(path, NULL, x1, y1);
    CGPathCloseSubpath(path);
    CGContextAddPath(context, path);
    
    CGContextSetFillColorWithColor(context, _pageDownEnabled ?  [_pageMarkEnableColor CGColor] :[_pageMarkDisableColor CGColor]);
    
    CGContextFillPath(context);
    CGPathRelease(path);
    
    float x11 = x1;
    float y11 = _height - _shadowBottom - _marginBottom - (float)_widthPageMark * 14 / 54;
    float x12 = x2;
    float y12 = y11 - (float)_widthPageMark * 2 / 3;
    float x13 = x3;
    float y13 = y11 - (float)_widthPageMark * 2 / 3;
    
    path = CGPathCreateMutable();
    CGPathMoveToPoint(path, NULL, x11, y11);
    CGPathAddLineToPoint(path, NULL, x12, y12);
    CGPathAddLineToPoint(path, NULL, x13, y13);
    CGPathAddLineToPoint(path, NULL, x11, y11);
    CGPathCloseSubpath(path);
    CGContextAddPath(context, path);
    
    CGContextSetFillColorWithColor(context, _pageUpEnabled ?  [_pageMarkEnableColor CGColor] :[_pageMarkDisableColor CGColor]);
    
    CGContextFillPath(context);
    CGPathRelease(path);
    
    CGContextRestoreGState(context);
}

- (void) drawRect:(NSRect) dirtyRect
{
    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];

    [self drawBackground:context];
    [self drawFocus:context rect:[[_candItemRect objectAtIndex:_hotItem] rectValue]];
    [self drawCandidates:context];
}

- (NSSize) getBoundSize
{
    return NSMakeSize(_width, _height);
}

@end
