#import "PYInputController.h"
#import "PYProvider.h"
#import "PYInputCandWindow.h"
#import "PYInputTipsWindow.h"
#import "PYInputPubDef.h"
#import "PYInputNotification.h"

extern PYProvider              *g_provider;
extern PYInputCandWindow         *g_cand;
extern PYInputTipsWindow         *g_tips;
extern PYInputSymbol             *g_symbol;
extern PYInputOption           *g_option;
extern int                     g_prefFlags;


@implementation PYInputController

- (id) initWithServer:(IMKServer *) server delegate:(id) delegate client:(id) inputClient
{
    self = [super initWithServer:server delegate:delegate client:inputClient];

    if (self)
    {
        _convert = [[PYInputConvert alloc] init];
        [_convert setDelegate:self];
    }
    
    return self;
}

- (void) inputControllerWillClose
{
    //NSLog(@PalmInputController inputControllerWillClose");

}

- (void) dealloc
{
    //NSLog(@"FEInputController dealloc");
//    [g_provider save:TRUE];
//    [PYInputNotification RemoveObserver:self];
}

- (void) activateServer:(id) sender
{
    //NSLog(@"FEInputController activateServer");
    [_convert activate:sender];
}

- (void) deactivateServer:(id) sender
{
    // NSLog(@"FEInputController deactivateServer");
    [_convert deactivate:sender];
}

- (BOOL) handleEvent:(NSEvent *) event client:(id) sender
{
    // NSLog(@"FEInputController handleEvent");
    return [_convert handleEvent:event client:sender];
    
}

- (NSUInteger) recognizedEvents:(id) sender
{
    NSUInteger regFlags =  (NSEventMaskKeyDown | NSEventMaskFlagsChanged);
//    if ([_convert isEditMode])
//    {
//        regFlags |= (NSEventMaskLeftMouseDown | NSEventMaskLeftMouseUp);
//    }
    
    return regFlags;
}

// 更新inline串
- (void) composeString:(NSString *) string cursorPos:(int) cursorPos client:(id) sender
{  
    int strLen = (int) [string length];
    if (strLen > 0)
    {
        NSDictionary* attrs = [self markForStyle:kTSMHiliteSelectedRawText atRange:NSMakeRange(0, [string length])];
        NSAttributedString* attrString = [[NSAttributedString alloc] initWithString:string attributes:attrs];

        [sender setMarkedText:attrString selectionRange:NSMakeRange((cursorPos < strLen ? cursorPos : strLen), 0) replacementRange:NSMakeRange(NSNotFound, NSNotFound)];
    }
    else
    {
        [sender setMarkedText:@"" selectionRange:NSMakeRange(0, 0) replacementRange:NSMakeRange(NSNotFound, NSNotFound)];
    }
}

// 上屏提交串
- (void) commitString:(NSString *) string client:(id)sender
{
    [sender insertText:string replacementRange:NSMakeRange(NSNotFound, NSNotFound)];
}


- (void) setValue:(id) value forTag:(long) tag client:(id) sender
{
	NSString *newModeString = (NSString *) value;
	
    if ([newModeString isEqualToString:@"com.freeinput.PalmInput.pinyin"])
	{
        
	}
	else if ([newModeString isEqualToString:@"com.freeinput.PalmInput.wubi"])
	{
        
	}
}

- (IBAction) onSetting:(id) sender
{
    NSArray* apps = [NSRunningApplication
                     runningApplicationsWithBundleIdentifier:@"com.freeinput.PalmInputPreference"];
    if ([apps count] > 0) {
        [(NSRunningApplication*)[apps objectAtIndex:0]
         activateWithOptions: NSApplicationActivateIgnoringOtherApps];
    }
    else{
        NSString *path = [[NSBundle mainBundle] pathForResource:@"PalmInputPreference" ofType:@"app"];
        [[NSWorkspace sharedWorkspace] launchApplication:path];
    }
    
}

- (IBAction) onFeedback:(id) sender
{
    NSURL *url = [NSURL URLWithString:@"http://www.freeinput.com"];
    [[NSWorkspace sharedWorkspace] openURL:url];
}

- (IBAction) onAbout:(id) sender
{
    NSArray* apps = [NSRunningApplication
                     runningApplicationsWithBundleIdentifier:@"com.freeinput.PalmInputAbout"];
    if ([apps count] > 0)
    {
        [(NSRunningApplication*)[apps objectAtIndex:0]
         activateWithOptions: NSApplicationActivateIgnoringOtherApps];
    }
    else
    {
        NSString *path = [[NSBundle mainBundle] pathForResource:@"PalmInputAbout" ofType:@"app"];
        [[NSWorkspace sharedWorkspace] launchApplication:path];
    }
}

-(NSMenu *) menu
{
    NSMenu *theMenu = [[NSMenu alloc] initWithTitle:@"Contextual Menu"];
    
    if (theMenu)
    {
        [theMenu insertItemWithTitle:NSLocalizedString(@"menu_item_preference", nil) action:@selector(onSetting:) keyEquivalent:@"" atIndex:0];
        [theMenu insertItemWithTitle:NSLocalizedString(@"menu_item_about", nil) action:@selector(onAbout:) keyEquivalent:@"" atIndex:1];
    }
    
    return theMenu;
}

@end
