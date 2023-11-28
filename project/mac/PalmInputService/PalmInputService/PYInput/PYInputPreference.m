#import "PYInputPreference.h"

@implementation PYInputPreference

static PYInputPreference *s_sharedInstance = nil;

+ (id<IFTPreferenceProductor>)sharedPreferenceProductor {
    
    @synchronized(self) {
        
        if (s_sharedInstance == nil) {
            
            s_sharedInstance = [[self alloc] init];
        }
    }
    
    return s_sharedInstance;
}

+ (id<IPYInputPreferenceConsumer>)sharedPreferenceConsumer {
    
    @synchronized(self) {
        
        if (s_sharedInstance == nil) {
            
            s_sharedInstance = [[self alloc] init];
        }
    }
    
    return s_sharedInstance;
}

+ (id)allocWithZone:(NSZone *) zone {
    
    @synchronized(self) {
        
        if (s_sharedInstance == nil) {
            
            s_sharedInstance = [super allocWithZone:zone];
            return s_sharedInstance;
        }
    }
    return nil;
}

//copy返回单例本身
- (id)copyWithZone:(NSZone *)zone {
    
    return self;
}


- (id)init {

    self = [super init];
    if (self != nil) {
        
        NSString* userFilePath = [PY_PREF_PATH_NAME stringByAppendingPathComponent:PY_PREF_PREFERENCE_FILE_NAME];
        _userPreferenceDict = [[NSMutableDictionary alloc] initWithContentsOfFile:[userFilePath stringByExpandingTildeInPath]];
        if (_userPreferenceDict == nil)
            _userPreferenceDict = [[NSMutableDictionary alloc] init];
        
        [self loadDefaultPref];
    }
    
    return self;
}

- (BOOL)updatePreference {
    
    @synchronized(self) {
                
        NSString* userFilePath = [PY_PREF_PATH_NAME stringByAppendingPathComponent:PY_PREF_PREFERENCE_FILE_NAME];
        _userPreferenceDict = [[NSMutableDictionary alloc] initWithContentsOfFile:[userFilePath stringByExpandingTildeInPath]];
        if (_userPreferenceDict == nil)
            _userPreferenceDict = [[NSMutableDictionary alloc] init];
        
        return (_userPreferenceDict != nil);
    }
    
    return FALSE;
}

- (BOOL)savePreference {

    @synchronized(self) {
        
        if (_userPreferenceDict != nil) {
            
            [[NSFileManager defaultManager] createDirectoryAtPath:[PY_PREF_PATH_NAME stringByExpandingTildeInPath] withIntermediateDirectories:YES attributes:nil error:nil];
            
            NSString* userFilePath = [PY_PREF_PATH_NAME stringByAppendingPathComponent:PY_PREF_PREFERENCE_FILE_NAME];
            return [_userPreferenceDict writeToFile:[userFilePath stringByExpandingTildeInPath] atomically:YES];
        }
    }
    
    return FALSE;
}

- (id)valueByKey:(NSString*)key {
    
    id value = [_userPreferenceDict valueForKey:key];
    if (value == nil) {
        value = [_defaultPreferenceDict valueForKey:key];
    }
    
    return value;
}

- (void)setValue:(id)value ByKey:(NSString*)key {
    
    if (_userPreferenceDict) {
        [_userPreferenceDict setObject:value forKey:key];
    }
}

- (void)loadDefaultPref {
    
}

@end
