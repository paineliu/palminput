#include <sys/types.h>
#include <sys/sysctl.h>
#include <Carbon/Carbon.h>
#import "InputUtils.h"

@implementation FTTools

-(BOOL) inputSourceEnabled:(NSString *) name
{
    BOOL bRet = FALSE;
    
    CFArrayRef inputSrcList = TISCreateInputSourceList(NULL, TRUE);
    if (inputSrcList)
    {
        TISInputSourceRef sourceRef = NULL;
        long size = CFArrayGetCount(inputSrcList);
        for (long i = 0 ; i < size ; ++i)
        {
            sourceRef = (TISInputSourceRef) CFArrayGetValueAtIndex(inputSrcList, i);
            NSString *currentName = (__bridge NSString *) TISGetInputSourceProperty(sourceRef, kTISPropertyLocalizedName);
            if ([currentName isEqualToString: name])
            {
                CFBooleanRef value = TISGetInputSourceProperty(sourceRef, kTISPropertyInputSourceIsEnabled);
                
                bRet = value ? (BOOL) CFBooleanGetValue(value) : NO;
                break;
            }
        }
        
        CFRelease(inputSrcList);
    }
    
    return bRet;

}
- (BOOL) existInputSource:(NSString *) name
{
    BOOL bRet = FALSE;
    
    CFArrayRef inputSrcList = TISCreateInputSourceList(NULL, TRUE);
    if (inputSrcList)
    {
        TISInputSourceRef sourceRef = NULL;
        long size = CFArrayGetCount(inputSrcList);
        for (long i = 0 ; i < size ; ++i)
        {
            sourceRef = (TISInputSourceRef) CFArrayGetValueAtIndex(inputSrcList, i);
            NSString *currentName = (__bridge NSString *) TISGetInputSourceProperty(sourceRef, kTISPropertyLocalizedName);
            if ([currentName isEqualToString: name])
            {
                bRet = TRUE;
                break;
            }
        }
        
        CFRelease(inputSrcList);
    }
    
    return bRet;
}

- (BOOL) disableInputSource:(NSString *) name
{
    BOOL bRet = FALSE;

    CFArrayRef inputSrcList = TISCreateInputSourceList(NULL, TRUE);
    if (inputSrcList)
    {
        TISInputSourceRef sourceRef = NULL;
        long size = CFArrayGetCount(inputSrcList);
        for (long i = 0 ; i < size ; ++i)
        {
            sourceRef = (TISInputSourceRef) CFArrayGetValueAtIndex(inputSrcList, i);
            NSString *currentName = (__bridge NSString *) TISGetInputSourceProperty(sourceRef, kTISPropertyLocalizedName);
            if ([currentName isEqualToString: name])
            {
                SInt32 state = 0;
                state = TISDisableInputSource(sourceRef);
                bRet = state == noErr;
                break;
            }
        }
        
        CFRelease(inputSrcList);
    }
    
    return bRet;
}

- (BOOL) unregisterInputSource
{
    BOOL bRet = FALSE;
    
    CFURLRef inputUrl = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)@"/Library/Input Methods/FTInput.app", kCFURLPOSIXPathStyle, true);
    
    if (inputUrl)
    {
        TISRegisterInputSource(inputUrl);
        CFArrayRef inputSrcList = TISCreateInputSourceList(NULL, TRUE);
        if (inputSrcList)
        {
            TISInputSourceRef imRef = NULL;
            TISInputSourceRef pyRef = NULL;
            TISInputSourceRef tmpRef = NULL;
            
            long size = CFArrayGetCount(inputSrcList);
            for (long i = 0 ; i < size ; ++i)
            {
                tmpRef = (TISInputSourceRef)CFArrayGetValueAtIndex(inputSrcList, i);
                NSString *currentName = (__bridge NSString *)TISGetInputSourceProperty(tmpRef, kTISPropertyLocalizedName);
                if ([currentName isEqualToString: @"手心输入法"])
                {
                    imRef = tmpRef;
                }
                else if ([currentName isEqualToString: @"手心拼音"])
                {
                    pyRef = tmpRef;
                }

            }
            
            if (imRef && pyRef)
            {
                TISDisableInputSource(pyRef);
                TISDisableInputSource(imRef);
                
                bRet = TRUE;
            }
            
            CFRelease(inputSrcList);
        }
    }
    
    return bRet;
}

- (BOOL) registerInputSource
{
    BOOL bRet = FALSE;
    
    CFURLRef inputUrl = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)@"/Library/Input Methods/FTInput.app", kCFURLPOSIXPathStyle, true);
    
    if (inputUrl)
    {
        TISRegisterInputSource(inputUrl);
        CFArrayRef inputSrcList = TISCreateInputSourceList(NULL, TRUE);
        if (inputSrcList)
        {
            TISInputSourceRef imRef = NULL;
            TISInputSourceRef pyRef = NULL;
            TISInputSourceRef tmpRef = NULL;
            
            long size = CFArrayGetCount(inputSrcList);
            for (long i=0 ; i<size ; i++)
            {
                tmpRef = (TISInputSourceRef)CFArrayGetValueAtIndex(inputSrcList, i);
                NSString *currentName = (__bridge NSString *)TISGetInputSourceProperty(tmpRef, kTISPropertyLocalizedName);
                if ([currentName isEqualToString: @"手心输入法"])
                {
                    imRef = tmpRef;
                }
                else if ([currentName isEqualToString: @"手心拼音"])
                {
                    pyRef = tmpRef;
                }
            }
            
            if (imRef && pyRef)
            {
                TISEnableInputSource(imRef);
                TISEnableInputSource(pyRef);
                
                bRet = TRUE;
            }
            
            CFRelease(inputSrcList);
        }
    }
    
    return bRet;
}

- (BOOL) enableInputSource:(NSString *) name
{
    BOOL bRet = FALSE;
    
    CFArrayRef inputSrcList = TISCreateInputSourceList(NULL, TRUE);
    if (inputSrcList)
    {
        TISInputSourceRef sourceRef = NULL;
        long size = CFArrayGetCount(inputSrcList);
        for (long i = 0 ; i < size ; ++i)
        {
            sourceRef = (TISInputSourceRef) CFArrayGetValueAtIndex(inputSrcList, i);
            NSString *currentName = (__bridge NSString *) TISGetInputSourceProperty(sourceRef, kTISPropertyLocalizedName);
            if ([currentName isEqualToString: name])
            {
                SInt32 state = 0;
                state = TISEnableInputSource(sourceRef);
                bRet = state == noErr;
                break;
            }
        }
        
        CFRelease(inputSrcList);
    }
    
    return bRet;
}

- (void) killProcess:(NSString *) procName
{
    int mib[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL};
    size_t length;
    struct kinfo_proc *theProcessInfoData;
    struct kinfo_proc *theProcessInfo;
    NSString *theProcessName = NULL;
    pid_t theProcessID;
    
    if (sysctl (mib, 3, NULL, &length, NULL, 0) != -1)
    {
        theProcessInfoData = (struct kinfo_proc *) malloc(length);
        
        if (theProcessInfoData)
        {
            if (sysctl(mib, 3, theProcessInfoData, &length, NULL, 0) != -1)
            {
                theProcessInfo = theProcessInfoData;
                
                do
                {
                    theProcessName = [NSString stringWithCString:theProcessInfo->kp_proc.p_comm encoding:NSUTF8StringEncoding];
                    if (theProcessName && [theProcessName isEqualToString:procName])
                    {
                        theProcessID = theProcessInfo->kp_proc.p_pid;
                        kill(theProcessID, SIGKILL);
                    }
                    
                    theProcessInfo ++;
                    
                } while ((size_t) theProcessInfo < (size_t) theProcessInfoData + length);
            }
            
            free ((void *) theProcessInfoData);
        }
    }
}

@end
