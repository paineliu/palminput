#include <sys/types.h>
#include <sys/sysctl.h>
#import "InputUtils.h"

@implementation InputUtils

+ (NSString*) getGUID {
    
    io_service_t platformExpert = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("IOPlatformExpertDevice"));
    CFStringRef guidAsCFString = NULL;
    
    if (platformExpert) {
        
        guidAsCFString = IORegistryEntryCreateCFProperty(platformExpert, CFSTR(kIOPlatformSerialNumberKey), kCFAllocatorDefault, 0);
        IOObjectRelease(platformExpert);
    }
    
    NSString *guidAsNSString = nil;
    if (guidAsCFString) {
        
        guidAsNSString = [NSString stringWithString:(__bridge NSString*)guidAsCFString];
        CFRelease(guidAsCFString);
    }
    
    return guidAsNSString;
}

+ (NSString *) getSysVersion {
    
    //[[NSProcessInfo processInfo] operatingSystemVersionString];
    
    NSOperatingSystemVersion ver = [[NSProcessInfo processInfo] operatingSystemVersion];
    
    return [NSString stringWithFormat:@"%ld.%ld.%ld", ver.majorVersion, ver.minorVersion, ver.patchVersion];
}

+ (NSString *) getSysModel {
    
    size_t len = 0;
    sysctlbyname("hw.model", NULL, &len, NULL, 0);
    NSString *modelStr = @"";
    if (len) {
        char *model = (char *)malloc(len*sizeof(char));
        if (model) {
            sysctlbyname("hw.model", model, &len, NULL, 0);
            modelStr = [NSString stringWithCString:model encoding:NSUTF8StringEncoding];
            free(model);
        }
    }
    return modelStr;
}

@end
