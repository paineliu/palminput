#import <Cocoa/Cocoa.h>
#import <InputMethodKit/InputMethodKit.h>
#import "palmengine.h"
#import "PYInputPubDef.h"
#import "PYInputCandWindow.h"
#import "PYInputTipsWindow.h"
#import "PYInputSymbol.h"
#import "PYInputOption.h"

PYInputCandWindow*       g_cand = nil;
PYInputTipsWindow*       g_tips = nil;
PYInputSymbol*           g_symbol = nil;
PalmEngine*              g_provider = nil;
PYInputOption*           g_option = nil;
int                      g_prefFlags = PY_NOTIFICATION_PREFERENCE_CHANGED_VALUE_NONE;

int main(int argc, const char * argv[])
{
    const NSString *kConnectionName = @"PYInput_1_Connection";
    IMKServer *server __unused = [[IMKServer alloc] initWithName:(NSString *) kConnectionName
                                                bundleIdentifier:[NSBundle mainBundle].bundleIdentifier];
    
    // create global object
    g_option = [[PYInputOption alloc] init];
    g_provider = [[PalmEngine alloc] init];
    g_symbol = [[PYInputSymbol alloc] init];

    [g_option loadAll];

    // load the bundle explicitly because in this case the input method is a background only application
    
    [[NSBundle mainBundle] loadNibNamed:@"MainMenu" owner:[NSApplication sharedApplication] topLevelObjects:nil];
    
    g_cand = [[PYInputCandWindow alloc] init];
    g_tips = [[PYInputTipsWindow alloc] init];
    
    [[NSApplication sharedApplication] run];
}
