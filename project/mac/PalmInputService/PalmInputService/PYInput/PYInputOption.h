#import <Foundation/Foundation.h>

typedef struct _PY_INPUT_OPTION
{
    unsigned int cnLangUseEnPunc:1;          // 中文时使用英文标点
	
	unsigned int switchLangByShift:1;        // 中英切换键 - Shift
	unsigned int switchLangByCtrl:1;         // 中英切换键 - Ctrl
	unsigned int commitCompWhenSwitchLang:1; // 切换至英文时上屏写作串
    
	unsigned int pageTurnBySquareBrackets:1;   // 翻页键 - [ / ]
	unsigned int pageTurnByCommaPeriod:1;      // 翻页键 - , / .
	unsigned int pageTurnByMinusEqual:1;       // 翻页键 - - / =
	unsigned int pageTurnByUpDown:1;           // 翻页键 - Up / Down
	unsigned int pageTurnByShiftTabTab:1;      // 翻页键 - Shift Tab / Tab
    
    unsigned int sel23ItemByShift:1;           // 二三候选 - Shift
	unsigned int sel23ItemByCtrl:1;            // 二三候选 - Ctrl
    unsigned int sel23ItemByFenSplit:1;        // 二三候选 - ; '
        
    unsigned int delPhraseByCtrl:1;            // 删词键 - Ctrl
	unsigned int delPhraseByCtrlShift:1;       // 删词键 - Ctrl Shift
    
    unsigned int puncAutoConvert:1;            // 标点自动转换
    unsigned int inputHalfPunc:1;              // 输入模式 - 半角/全角

    // 快捷键 - 简体繁体 中标英标 全角半角

    unsigned int skeySimpTradEnabled:1;
    unsigned int skeyHalfFullPuncEnabled:1;
    unsigned int skeyEnCnPuncEnabled:1;
    
    unsigned int skeySimpTradCode;
    unsigned int skeyHalfFullPuncCode;
    unsigned int skeyEnCnPuncCode;

    unsigned int skeySimpTradFlags;
    unsigned int skeyHalfFullPuncFlags;
    unsigned int skeyEnCnPuncFlags;
    
    unsigned int pinyinType;
    unsigned int shuangpinType;
    
    BOOL inputCnLang;       // 输入模式 - 中文/英文

    NSString*       keyboardLayout;
    NSMutableDictionary* appInitEnLang;
    
} PY_INPUT_OPTION;

@interface PYInputOption : NSObject
{
    PY_INPUT_OPTION _pyOption;
}

-(PY_INPUT_OPTION *) getOption;

- (void) loadAll;

- (void) update:(int) nFlag;

@end
