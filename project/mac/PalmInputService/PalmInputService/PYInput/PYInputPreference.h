#import "PYInputPubDef.h"

@protocol IPYInputPreferenceConsumer <NSObject>

@required
- (BOOL)updatePreference;
- (id)valueByKey:(NSString*)key;

@end


@protocol IFTPreferenceProductor <NSObject>

@required
- (BOOL)updatePreference;
- (BOOL)savePreference;
- (id)valueByKey:(NSString *)key;
- (void)setValue:(id)value ByKey:(NSString *)key;

@end


@interface PYInputPreference : NSObject <IPYInputPreferenceConsumer, IFTPreferenceProductor> {
    NSMutableDictionary     *_userPreferenceDict;
    NSMutableDictionary     *_defaultPreferenceDict;
}

+ (id<IFTPreferenceProductor>)sharedPreferenceProductor;
+ (id<IPYInputPreferenceConsumer>)sharedPreferenceConsumer;

- (BOOL)updatePreference;
- (BOOL)savePreference;
- (id)valueByKey:(NSString*)key;
- (void)setValue:(id)value ByKey:(NSString *)key;

@end
