#import <Cocoa/Cocoa.h>

#define PY_TIPS_WINDOW_WIDTH         24
#define PY_TIPS_WINDOW_HEIGHT        24

typedef enum _INPUT_TIPS_ID {
    
	InputTipsIdLanguageChinese = 0,
    InputTipsIdLanguageEnglish,
    InputTipsIdSymbolFull,
    InputTipsIdSymbolHalf,
    InputTipsIdSymbolChinese,
    InputTipsIdSymbolEnglish,
    InputTipsIdTraditionalChinese,
    InputTipsIdSimplifiedChinese,
    
} InputTipsId;

@interface PYInputTipsView : NSView {
    
    NSColor *_itemColor;
    NSColor *_bgColor;
    NSColor *_borderColor;
    
    InputTipsId _tipsId;
}

- (void) setSkin:(NSDictionary *) skinInfo;
- (void) showTips:(InputTipsId) tipsId;

@end
