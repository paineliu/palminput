#import "PYInputOption.h"
#import "PYInputPreference.h"
#import "PYInputCandWindow.h"
#import "PYInputTipsWindow.h"
#import "PYProvider.h"
#import "PYInputSymbol.h"

extern PYProvider           *g_provider;
extern PYInputCandWindow    *g_cand;
extern PYInputTipsWindow    *g_tips;
extern PYInputSymbol        *g_symbol;

@implementation PYInputOption

-(id) init
{
    self = [super init];
    
    if (self)
    {
        _pyOption.puncAutoConvert = YES;
        
        _pyOption.inputCnLang = YES;
        _pyOption.inputHalfPunc = YES;
        
        _pyOption.cnLangUseEnPunc = NO;
        
        _pyOption.switchLangByShift = YES;
        _pyOption.switchLangByCtrl = YES;
        _pyOption.commitCompWhenSwitchLang = YES;
        
        _pyOption.pageTurnBySquareBrackets = YES;
        _pyOption.pageTurnByUpDown = YES;
        _pyOption.pageTurnByCommaPeriod = NO;
        _pyOption.pageTurnByMinusEqual = YES;
        _pyOption.pageTurnByShiftTabTab = NO;
        
        _pyOption.sel23ItemByShift = NO;
        _pyOption.sel23ItemByCtrl = NO;
        _pyOption.sel23ItemByFenSplit = NO;
        
        _pyOption.delPhraseByCtrl = NO;
        _pyOption.delPhraseByCtrlShift = YES;
        
        _pyOption.skeySimpTradEnabled = YES;
        _pyOption.skeyHalfFullPuncEnabled = YES;
        _pyOption.skeyEnCnPuncEnabled = YES;
        
        _pyOption.skeySimpTradCode = 17;
        _pyOption.skeyHalfFullPuncCode = 49;
        _pyOption.skeyEnCnPuncCode = 47;
        
        _pyOption.skeySimpTradFlags = 262144;
        _pyOption.skeyHalfFullPuncFlags = 131072;
        _pyOption.skeyEnCnPuncFlags = 262144;
        
        _pyOption.pinyinType = 0;
        _pyOption.shuangpinType = 0;
        
        _pyOption.keyboardLayout = 0;
        _pyOption.appInitEnLang = [[NSMutableDictionary alloc] init];
    }
    
    return self;
}

-(PY_INPUT_OPTION *) getOption
{
    return &_pyOption;
}


- (void) loadAll
{
}

- (void) update:(int) nFlag
{
    [self loadOption:nFlag firstTime:FALSE];
}

- (void) loadOption:(int) nFlag firstTime:(BOOL) firstTime
{

}

@end
