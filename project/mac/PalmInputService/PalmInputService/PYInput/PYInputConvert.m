#import "PYInputConvert.h"
#import "PYProvider.h"
#import "PYInputCandWindow.h"
#import "PYInputTipsWindow.h"
#import "PYInputPubDef.h"
#import "PYInputNotification.h"

extern PYProvider                *g_provider;
extern PYInputCandWindow         *g_cand;
extern PYInputTipsWindow         *g_tips;
extern PYInputSymbol             *g_symbol;
extern PYInputOption             *g_option;
extern int                        g_prefFlags;

@implementation PYInputConvert

- (id) init
{
    self = [super init];

    if (self)
    {
        //NSLog(@"PYInputConvert init");

        //init notification observer:
        [PYInputNotification AddObserver:self selector:@selector(switchLanguageNotify:) name:PY_NOTIFICATION_SWITCH_LANGUAGE_NAME];
        
        _appCaretPosExact = FALSE;
        _lastModifiers = 0;
        _lastEventType = 0;
        _lastKeyCode = 0;
        _lastKeyChar = 0;
        _shiftPressedTime = 0;
        _ctrlPressedTime = 0;
        _pageDownByPeriodTimes = 0;
        
        _option = [g_option getOption];
    }
    
    return self;
}

- (void) dealloc
{
    //NSLog(@"PYInputConvert dealloc");
    
    [g_provider save:TRUE];
    [PYInputNotification RemoveObserver:self];
}

- (void) setDelegate:(id) delegate
{
    _delegate = delegate;
}

- (void) activate:(id) sender
{
    //NSLog(@"PYInputConvert activate");

    _lastModifiers = 0;
    _lastEventType = 0;
    _lastKeyCode = 0;
    _lastKeyChar = 0;
    _shiftPressedTime = 0;
    _ctrlPressedTime = 0;
    _lastCommitTime = 0;
    _lastEventTime = 0;
    _pageDownByPeriodTimes = 0;
    
    if (g_prefFlags != PY_NOTIFICATION_PREFERENCE_CHANGED_VALUE_NONE)
    {
        [g_option update:g_prefFlags];
        g_prefFlags = PY_NOTIFICATION_PREFERENCE_CHANGED_VALUE_NONE;
    }

    [g_symbol reset];
    [g_provider reset];
    
    NSString *bundleIdentifier = @"";
    
    if (sender != nil)
    {
        bundleIdentifier = [sender bundleIdentifier];
    }
    
    if ([_option->appInitEnLang objectForKey:bundleIdentifier])
    {
        _pChineseMode = &_option->inputCnLang;
    }
    else
    {
        _pChineseMode = &_option->inputCnLang;
    }
    
    if ([bundleIdentifier isEqualToString:@"com.microsoft.Word"] ||
        [bundleIdentifier isEqualToString:@"com.adobe.Photoshop"] ||
        [bundleIdentifier isEqualToString:@"com.google.Chrome"])
    {
        _appCaretPosExact = FALSE;
    }
    else
    {
        _appCaretPosExact = TRUE;
    }

    if ([bundleIdentifier isEqualToString:@"com.microsoft.Outlook"])
    {
        _appMaxWindowLevel = TRUE;
    }
    else
    {
        _appMaxWindowLevel = FALSE;
    }
    
    [g_cand setDelegate:self];

   // [_sender overrideKeyboardWithKeyboardNamed:_option->keyboardLayout];
}

- (void) deactivate:(id) sender
{
    // NSLog(@"PYInputConvert deactivate");

    [g_provider save:FALSE];

    if ([[g_provider getKeyString] length] > 0)
    {
        [self composeString:@"" cursorPos:0 client:sender];
    }

    [g_provider reset];
    [g_cand hideWindow];
}

- (BOOL) handleEvent:(NSEvent *) event client:(id) sender
{
    // NSLog(@"PYInputConvert handleEvent");
    
    if (_lastEventTime == [event timestamp])
    {
        return NO;
    }
    
    _lastEventTime = [event timestamp];

    if (([event type] != NSEventTypeFlagsChanged) && ([event type] != NSEventTypeKeyDown))
    {
        return NO;
    }
    
    BOOL bHandled = NO;
    
    BOOL bPageDownByPeriod = FALSE;
    BOOL operCandByNumber = FALSE;
    
    _sender = sender;

    switch ([event type])
    {
        case NSEventTypeFlagsChanged:
        {
            NSUInteger modifiers = [event modifierFlags] & (~NSEventModifierFlagCapsLock);
            
            // 如果处于编辑状态则事件被消费
            
            bHandled = [self isEditMode];

            // 处理 Ctrl 切换中英文和选择二三候选功能
            
            if (_option->switchLangByCtrl || _option->sel23ItemByCtrl)
            {
                
                if (_lastModifiers == 0 && modifiers == NSEventModifierFlagControl)
                {
                    // 记录Ctrl按下时间
                    _ctrlPressedTime = [[NSDate date] timeIntervalSince1970];
                }
                else if (modifiers == 0 && _lastEventType == NSEventTypeFlagsChanged && _lastModifiers == NSEventModifierFlagControl)
                {
                    // 按键抬起，按压时间小于0.25秒
                    if ([[NSDate date] timeIntervalSince1970] - _ctrlPressedTime < 0.25)
                    {
                        // 是否触发了候选选择功能
                        
                        BOOL bSelPageItem = FALSE;
                        if (bHandled && _option->sel23ItemByCtrl)
                        {
                            if ([self selPageItem:1])
                            {
                                bSelPageItem = TRUE;
                            }
                        }
                        
                        // 没有触发按键选择候选功能
                        if (!bSelPageItem && _option->switchLangByCtrl)
                        {
                            // 这个功能容易误触关闭
                            //[self switchLang:sender];
                        }
                    }
                }
            }
            
            // 处理 Shift 切换中英文和选择二三候选功能
            
            if (_option->switchLangByShift || _option->sel23ItemByShift)
            {
                if (_lastModifiers == 0 && modifiers == NSEventModifierFlagShift)
                {
                    // 记录Shift按下时间
                    
                    _shiftPressedTime = [[NSDate date] timeIntervalSince1970];
                }
                else if (modifiers == 0 && _lastEventType == NSEventTypeFlagsChanged && _lastModifiers == NSEventModifierFlagShift)
                {
                    // 按键抬起，按压时间小于0.25秒
                    
                    if ([[NSDate date] timeIntervalSince1970] - _shiftPressedTime < 0.25)
                    {
                        // 是否触发了候选选择功能
                        
                        BOOL bSelPageItem = FALSE;
                        if (bHandled && _option->sel23ItemByShift)
                        {
                            if ([self selPageItem:1])
                            {
                                bSelPageItem = TRUE;
                            }
                        }
                        
                        // 没有触发按键选择候选功能，语言切换
                        
                        if (!bSelPageItem && _option->switchLangByShift)
                        {
                            [self switchLang:sender];
                        }
                    }
                }
            }
            
            _lastModifiers = modifiers;

            break;
        }
        
        case NSEventTypeKeyDown:
        {
            NSUInteger modifiers = [event modifierFlags];
            NSInteger keyCode = [event keyCode];
            NSString *keyStr = [event characters];
            unichar keyChar = [keyStr characterAtIndex:0];
            
            // 有按键时关闭tips提示
            [g_tips hide];
            
            // 不是删除时，清除最后一次提交候选时间
            if (keyCode != 51)
            {
                _lastCommitTime = 0;
            }
            
            // 切换按键直接响应
            
            // 中英符号切换
            
            if (_option->skeyEnCnPuncEnabled &&
                keyCode == _option->skeyEnCnPuncCode &&
                modifiers == _option->skeyEnCnPuncFlags)
            {
                [self reset:sender];
                if (_option->cnLangUseEnPunc)
                {
                    [g_tips showTips:InputTipsIdLanguageEnglish sender:sender];
                }
                else
                {
                    [g_tips showTips:InputTipsIdLanguageChinese sender:sender];
                }
                
                _option->cnLangUseEnPunc = !_option->cnLangUseEnPunc;
                
                bHandled = TRUE;
                break;
            }

            // 简繁候选切换
            
            if (_option->skeySimpTradEnabled &&
                keyCode == _option->skeySimpTradCode &&
                modifiers == _option->skeySimpTradFlags)
            {
                [self reset:sender];
                if ([g_provider isTradMode])
                {
                    [g_provider setTradMode:FALSE];
                    [g_tips showTips:InputTipsIdSimplifiedChinese sender:sender];
                }
                else
                {
                    [g_provider setTradMode:TRUE];
                    [g_tips showTips:InputTipsIdTraditionalChinese sender:sender];
                }
                
                bHandled = TRUE;
                break;
            }

            // 全角半角切换
            
            if (_option->skeyHalfFullPuncEnabled &&
                keyCode == _option->skeyHalfFullPuncCode &&
                modifiers == _option->skeyHalfFullPuncFlags)
            {
                [self reset:sender];
                
                _option->inputHalfPunc = !_option->inputHalfPunc;
                
                if (_option->inputHalfPunc)
                {
                    [g_tips showTips:InputTipsIdSymbolHalf sender:sender];
                }
                else
                {
                    [g_tips showTips:InputTipsIdSymbolFull sender:sender];
                }
                
                bHandled = TRUE;
                break;
            }

            // 英文输入模式
            
            if (*_pChineseMode == FALSE)
            {
                // 输入半角符号直接将按键交给系统处理
                if (_option->inputHalfPunc)
                {
                    bHandled = NO;
                }
                else
                {
                    // 提交按键对应全角英文字符
                    
                    [g_symbol setOption:PYSymbolEnFull];
                    NSString *symbol = [g_symbol getSymbol:keyStr];
                    
                    if (symbol != nil)
                    {
                        [self commitString:symbol client:sender];
                        bHandled = YES;
                    }
                    else
                    {
                        bHandled = NO;
                    }
                }
                
                break;
            }
            
            // 中文模式 有快捷键同时按下 strl option command 影响输入按键的处理
            // 中文模式 有快捷键同时按下 fn shift caps 不影响输入按键的处理，不做判断,继续按键处理流程
            
            if (modifiers & (NSEventModifierFlagControl | NSEventModifierFlagOption | NSEventModifierFlagCommand))
            {
                if ([self isEditMode])
                {
                    // ctrl组合按键处理
                    if (modifiers & NSEventModifierFlagControl)
                    {
                        // Ctrl | Ctrl Shift + 0-9 1-9 删除候选项、0 消费掉
                        if ((_option->delPhraseByCtrl ||
                             (_option->delPhraseByCtrlShift && (modifiers & NSEventModifierFlagShift))) && (keyChar >= '0' && keyChar <= '9'))
                        {
                            if ([self isSelChar:keyChar])
                            {
                                operCandByNumber = TRUE;
                                [self delPageItem:keyChar - '1'];
                            }
                            
                            bHandled = TRUE;
                            break;
                        }
                        else
                        {
                            // 其他情况上屏写作串
                            [self commitString:[g_provider getKeyString] client:sender];
                            [self reset:sender];
                            bHandled = YES;
                        }
                        
                        break;
                    }

                    // alt键会输入特殊符号，因此应该提交候选并追加提交的符号
                    
                    if (modifiers & NSEventModifierFlagOption)
                    {
                        [self commitString:[g_provider getCandItem:0] client:sender];
                        
                        [self reset:sender];
                        bHandled = FALSE;
                        break;
                    }
                    
                    // 不需要处理 忽略它，继续按键处理
                    if (modifiers & NSEventModifierFlagCommand)
                    {
                        bHandled = FALSE;
                    }
                }
                else
                {
                    bHandled = NO;
                    break;
                }
            }
            
            
            // 空格
            
            if (keyCode == 49)
            {
                if ([self isEditMode])
                {
                    if (![self selPageItem:[g_provider getHotspotPageItemId]])
                    {
                        [self commitComposition:sender];
                    }
                    bHandled = YES;
                    break;
                }
            }
            
            // 处理输入按键
            
            if ([self isAlphaChar:keyChar] || ([self isEditMode] && [self isInputChar:keyChar]))
            {
                // 如果点击过一次. 则撤销翻页功能，追加符号.到输入串
//                if (_pageDownByPeriodTimes == 1)
//                {
//                    [self processKey:'.' client:sender];
//                    _pageDownByPeriodTimes = 0;
//                }

                [self processKey:keyChar client:sender];
                bHandled = YES;
                break;
            }
            
            // 处理编辑模式下的功能按键
            if ([self isEditMode])
            {
                // num-keyboard
                
                if (keyCode == 69 ||  // +
                    keyCode == 78 ||  // -
                    keyCode == 67 ||  // *
                    keyCode == 75 ||  // /
                    keyCode == 65 ||  // .
                    keyCode == 82 ||  // 0
                    keyCode == 83 ||  // 1
                    keyCode == 84 ||  // 2
                    keyCode == 85 ||  // 3
                    keyCode == 86 ||  // 4
                    keyCode == 87 ||  // 5
                    keyCode == 88 ||  // 6
                    keyCode == 89 ||  // 7
                    keyCode == 91 ||  // 8
                    keyCode == 92)    // 9
                {
                    // [self processKey:keyChar client:sender];
                    bHandled = YES;
                    break;
                }
             
                // ; 选择候选
                if (_option->sel23ItemByFenSplit && keyCode == 41)
                {
                    [self selPageItem:1];
                    bHandled = YES;
                    break;
                }
                
                // ' 选择候选
                if (_option->sel23ItemByFenSplit && keyCode == 39)
                {
                    [self selPageItem:2];
                    bHandled = YES;
                    break;
                }
                
                // 单字模式
                if (keyChar == '0')
                {
                    operCandByNumber = TRUE;
                 
                    if ([g_provider getCandFilter] != PY_FILTER_SINGLE_WORD)
                    {
                        // 第一次进入单字模式
                        
                        [g_provider setCandFilter:PY_FILTER_SINGLE_WORD];
                        [self updateView:sender];
                        
                        bHandled = YES;
                    }
                    else
                    {
                        // 已经进入单字则翻页
                        [self pageDown];
                        bHandled = YES;
                    }
                }

                // 1-9 按键上屏候选操作
                if ([self isSelChar:keyChar])
                {
                    operCandByNumber = TRUE;
                    [self selPageItem:keyChar - '1'];
                    bHandled = YES;
                    break;
                }
                
                // 移动插入符位置
                
                // home key
                
                if (keyCode == 115)  // move comp home
                {
                    [self moveCaretHome];
                    bHandled = YES;
                    break;
                }

                // end key
                
                if (keyCode == 119)  // move comp end
                {
                    [self moveCaretEnd];
                    bHandled = YES;
                    break;
                }
                
                // left key
                if (keyCode == 123)  // move comp left
                {
                    if ([g_provider isFirstItemHasFocus]) {
                        [self moveCaretLeft];
                    }
                    else {
                        [self prevItem];
                    }
                    bHandled = YES;
                    break;
                }
                
                // right key
                
                if (keyCode == 124)  // move comp right
                {
                    if ([g_provider isCaretAtCompEnd]) {
                        [self nextItem];
                    }
                    else {
                        [self moveCaretRight];
                    }
                    bHandled = YES;
                    break;
                }

                // down key
                
                if ((!_option->pageTurnByUpDown) && keyCode == 125)  // move next item
                {
                    [self nextItem];
                    bHandled = YES;
                    break;
                }
                
                // up key
                
                if ((!_option->pageTurnByUpDown) && keyCode == 126)  // move prev item
                {
                    [self prevItem];
                    bHandled = YES;
                    break;
                }
                
                // 翻页处理
                
                if (keyCode == 116 ||
                    (_option->pageTurnByUpDown && keyCode == 126) ||
                    (_option->pageTurnByMinusEqual && keyCode == 27) ||
                    (_option->pageTurnByCommaPeriod && keyCode == 43) ||
                    (_option->pageTurnBySquareBrackets && keyCode == 33) ||
                    (_option->pageTurnByShiftTabTab && keyCode == 48 && modifiers == NSEventModifierFlagShift))
                {
                    if ([self isEditMode])
                    {
                        if (keyCode == 43)
                        {
                            // 逗号在第一页的时候直接上屏
                            
                            bHandled = [self pageUp];
                        }
                        else
                        {
                            [self pageUp];
                            bHandled = YES;
                        }
                        
                        if (bHandled)
                        {
                            break;
                        }
                    }
                }
                
                if (keyCode == 121 ||
                    (_option->pageTurnByUpDown && keyCode == 125) ||
                    (_option->pageTurnByMinusEqual && keyCode == 24) ||
                    (_option->pageTurnByCommaPeriod && keyCode == 47) ||
                    (_option->pageTurnBySquareBrackets && keyCode == 30) ||
                    (_option->pageTurnByShiftTabTab && keyCode == 48 && modifiers == 0))
                {
                    if ([self isEditMode])
                    {
                        if (keyCode == 47)
                        {
                            bPageDownByPeriod = TRUE;
                        }
                        [self pageDown];
                        bHandled = YES;
                        break;
                    }
                }

                // esc
                if (keyCode == 53)
                {
                    [self reset:sender];
                    bHandled = YES;
                    break;
                }
                
                if (keyCode == 51) // 删除
                {
                    [self processKey:0x08 client:sender];
                    bHandled = YES;
                    break;
                }
                
                if (keyCode == 36 || keyCode == 76) // enter | num-enter
                {
                    NSString* commitStr = [g_provider getConvertedString];
                    [self commitString:commitStr client:_sender];
                    [self reset:sender];
                    bHandled = YES;
                    break;
                }
                
                if (keyCode == 48) // tab
                {
                    bHandled = YES;
                }

                if ([g_symbol isSymbol:keyStr])
                {
                    {
                        if (_option->cnLangUseEnPunc)
                        {
                            if ([self isEditMode])
                            {
                                [self commitString:[g_provider getCandItem:0] client:sender];
                                [self reset:sender];
                            }
                            
                            bHandled = FALSE;
                            break;
                        }
                        else
                        {
                            if (_option->inputHalfPunc)
                            {
                                [g_symbol setOption:PYSymbolCnHalf];
                            }
                            else
                            {
                                [g_symbol setOption:PYSymbolCnFull];
                            }
                            
                            NSString *symbol = [g_symbol getSymbol:keyStr];
                            if (symbol != nil)
                            {
                                if ([self isEditMode])
                                {
                                    [self commitString:[g_provider getCandItem:0] client:sender];
                                    [self reset:sender];
                                }
                                
                                [self commitString:symbol client:sender];
                                
                                bHandled = YES;
                            }
                        }
                    }
                }
            }
            else
            {
                if ([g_symbol isSymbol:keyStr])
                {
                    {
                        if (_option->cnLangUseEnPunc)
                        {
                            if ([self isEditMode])
                            {
                                [self commitString:[g_provider getCandItem:0] client:sender];
                                [self reset:sender];
                            }
                            
                            bHandled = FALSE;
                            break;
                        }
                        else
                        {
                            if (_option->inputHalfPunc)
                            {
                                [g_symbol setOption:PYSymbolCnHalf];
                            }
                            else
                            {
                                [g_symbol setOption:PYSymbolCnFull];
                            }
                            
                            NSString *symbol = [g_symbol getSymbol:keyStr];
                            if (symbol != nil)
                            {
                                if ([self isEditMode])
                                {
                                    [self commitString:[g_provider getCandItem:0] client:sender];
                                    [self reset:sender];
                                }
                                
                                [self commitString:symbol client:sender];
                                
                                bHandled = YES;
                            }
                        }
                    }
                }
            }
 
            break;
        }
            
        default:
            bHandled = NO;
            break;
    }
    
    _lastEventType = [event type];
  
    
//    if ([event type] == NSEventTypeKeyDown && operCandByNumber == FALSE && commitSmartChar == FALSE)
//    {
//        _lastKeyChar = [[event characters] characterAtIndex:0];
//        _lastKeyCode = [event keyCode];
//    }
//    else
//    {
//        if ([event type] == NSEventTypeFlagsChanged && [event modifierFlags] == NSEventModifierFlagShift)
//        {
//            ;
//        }
//        else
//        {
//            _lastKeyChar = 0;
//            _lastKeyCode = 0;
//        }
//
//    }
//
    if (bPageDownByPeriod)
    {
        _pageDownByPeriodTimes ++;
    }
    else
    {
        _pageDownByPeriodTimes = 0;
    }
    
    return bHandled;
}

- (BOOL) processKey:(unichar) aKey client:(id)sender
{
    if ([g_provider processKey:aKey])
    {
        [self updateView:sender];
        return TRUE;
    }

    return FALSE;
}

- (BOOL) reset:(id) sender
{
    [self composeString:@"" cursorPos:0 client:sender];
    [g_provider reset];
    
    [g_cand hideWindow];
    
	return TRUE;
}

- (BOOL) updateView:(id) sender
{
    NSString* keyString = [g_provider getKeyString];
    NSUInteger len = [keyString length];
    
    if (len > 0)
    {
        NSArray *theCandidates = [g_provider getPageItems];
        
        // 通知sender更新inline串
        int caretPos = 0;
        NSString *compString = [g_provider getCompString:&caretPos];
        
        [self composeString:compString cursorPos:caretPos client:sender];
        
        int nInputState = [g_provider getInputState];

        if ([theCandidates count] > 0)
        {
            PY_INPUT_STATE eInputState = INPUT_STATE_NORMAL;
            if (len > 1)
            {
                if (nInputState & PY_INPUT_STATE_MODEV)
                    eInputState = INPUT_STATE_MODE_V_IN;
                else if (nInputState & PY_INPUT_STATE_MODEU)
                    eInputState = INPUT_STATE_MODE_U_IN;
            }
            
            [g_cand setCandidates:theCandidates hotItem:[g_provider getHotspotPageItemId] prefixLen:0 pageUp:[g_provider hasPrevPage] pageDown:[g_provider hasNextPage] showLabel:YES CloudState:[g_provider getCloudState] InputState:eInputState];
            [g_cand showWindow:_appCaretPosExact maxWindowLevel:_appMaxWindowLevel sender:sender];
        }
        else
           {
            if (len == 1 && ((nInputState & PY_INPUT_STATE_MODEU) || (nInputState & PY_INPUT_STATE_MODEV)))
            {
                PY_INPUT_STATE eInputState = INPUT_STATE_NORMAL;
                if (nInputState & PY_INPUT_STATE_MODEV)
                    eInputState = INPUT_STATE_MODE_V_ENTER;
                else if (nInputState & PY_INPUT_STATE_MODEU)
                    eInputState = INPUT_STATE_MODE_U_ENTER;
                
                [g_cand setCandidates:theCandidates hotItem:[g_provider getHotspotPageItemId] prefixLen:0 pageUp:[g_provider hasPrevPage] pageDown:[g_provider hasNextPage] showLabel:YES CloudState:[g_provider getCloudState] InputState:eInputState];
                [g_cand showWindow:_appCaretPosExact maxWindowLevel:_appMaxWindowLevel sender:sender];
            }
            else
            {
                [g_cand setCandidates:[NSArray arrayWithObject:keyString] hotItem:0 prefixLen:0 pageUp:FALSE pageDown:FALSE showLabel:FALSE CloudState:CLOUD_STATE_NONE InputState:0]; 
                [g_cand showWindow:_appCaretPosExact maxWindowLevel:_appMaxWindowLevel sender:sender];
            }
        }
    }
	else
    {
        [self reset:sender];
    }
    
	return TRUE;
}

// 更新inline串
- (void) composeString:(NSString *) string cursorPos:(int) cursorPos client:(id) sender
{
    _lastCommitTime = [[NSDate date] timeIntervalSince1970];
    [_delegate composeString:string cursorPos:cursorPos client:sender];
}

// 上屏提交串
- (void) commitString:(NSString *) string client:(id)sender
{
    [_delegate commitString:string client:sender];
    
}

- (void) commitComposition:(id) sender
{
    if ([self isEditMode])
    {
        [self commitString:[g_provider getConvertedString] client:_sender];
        [self reset:sender];
    }
}

- (BOOL) isEditMode
{
    return [[g_provider getKeyString] length] > 0;
}

- (BOOL) isSelChar:(unichar) aChar
{
    return (aChar >='1' && aChar <='9');
}

- (BOOL) isAlphaChar:(unichar) aChar
{
    return (aChar >='a' && aChar <='z') || (aChar >='A' && aChar <='Z');
}

- (BOOL) isInputChar:(unichar) aChar
{
    return (aChar >='a' && aChar <='z') || (aChar >='A' && aChar <='Z') || (aChar == '\'');
}

- (BOOL) selPageItem:(int) aItem
{
    if (aItem < [g_provider getPageItemTotal])
    {
        NSString *convertedString = [g_provider selPageItem:aItem];

        if ([convertedString length] > 0)
        {
            [self commitString:convertedString client:_sender];
            [self reset:_sender];
        }
        else
        {
            [self updateView:_sender];
        }
        
        return TRUE;
    }
    
    return FALSE;
}

-(BOOL) delPageItem:(int) aItem
{
    if ([g_provider delPageItem:aItem])
    {
        [self updateView:_sender];
        return TRUE;
    }

    return FALSE;
}

-(BOOL) prevItem
{
    if ([g_provider moveHotspotLeft])
    {
        return [self updateView:_sender];
    }
    
    return FALSE;
}

-(BOOL) nextItem
{
    if ([g_provider moveHotspotRight])
    {
        return [self updateView:_sender];
    }
    
    return FALSE;
}

-(BOOL) pageUp
{
    if ([g_provider pageUp])
    {
        return [self updateView:_sender];
    }
    
    return FALSE;
}

-(BOOL) pageDown
{
    if ([g_provider pageDown])
    {
        return [self updateView:_sender];
    }
    
    return FALSE;
}

-(BOOL) moveCaretHome
{
    if ([g_provider moveCaret:PY_CARET_MOVE_HOME])
    {
        return [self updateView:_sender];
    }
    
    return FALSE;
}

-(BOOL) moveCaretEnd
{
    if ([g_provider moveCaret:PY_CARET_MOVE_END])
    {
        return [self updateView:_sender];
    }
    
    return FALSE;
}

-(BOOL) moveCaretLeft
{
    if ([g_provider moveCaret:PY_CARET_MOVE_LEFT_UNIT])
    {
        return [self updateView:_sender];
    }
    
    return FALSE;
}

-(BOOL) moveCaretRight
{
    if ([g_provider moveCaret:PY_CARET_MOVE_RIGHT_CHAR])
    {
        return [self updateView:_sender];
    }
    
    return FALSE;
}

- (void) switchLang:(id) sender
{
    if (*_pChineseMode)
    {
        if ([self isEditMode])
        {
            if (_option->commitCompWhenSwitchLang)
            {
                [self commitString:[g_provider getKeyString] client:_sender];
            }
            
            [self reset:_sender];
        }
        
        [g_tips showTips:InputTipsIdLanguageEnglish sender:sender];
        *_pChineseMode = FALSE;
    }
    else
    {
        [g_tips showTips:InputTipsIdLanguageChinese sender:sender];
        *_pChineseMode = TRUE;
    }
    
}

- (void)switchLanguageNotify:(NSNotification *)notification{
    
    if (notification) {
        NSNumber *value = [[notification userInfo] valueForKey:PY_NOTIFICATION_SWITCH_LANGUAGE_KEY];
        if (value.intValue == PY_NOTIFICATION_SWITCH_LANGUAGE_VALUE_CN) {
            *_pChineseMode = YES;
        }else if (value.intValue == PY_NOTIFICATION_SWITCH_LANGUAGE_VALUE_EN){
            *_pChineseMode = FALSE;
        }
    }
}

@end

