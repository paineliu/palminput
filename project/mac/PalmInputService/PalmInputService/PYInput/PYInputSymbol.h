#import <Foundation/Foundation.h>

typedef enum PYSymbolType
{
    PYSymbolEnHalf,
    PYSymbolCnHalf,
    PYSymbolEnFull,
    PYSymbolCnFull,
    
} PY_SYMBOL_TYPE;

typedef enum PYSymbolTradKey
{
    PYSymbolTradNone,
    PYSymbolTradQuote,
    PYSymbolTradBracket,
    PYSymbolTradTotal,
    
} PY_SYMBOL_TRAD_KEY;

@interface PYInputSymbol : NSObject
{
    NSDictionary    *_dictCurrent;
    
    NSDictionary    *_dictCnHalfSymbol;
    NSDictionary    *_dictEnHalfSymbol;
    NSDictionary    *_dictCnFullSymbol;
    NSDictionary    *_dictEnFullSymbol;
    
    PY_SYMBOL_TRAD_KEY _tradSymbolInput;
    
    NSString        *_lastSimpDoubleQuote;
    NSString        *_lastSimpSingleQuote;
}

-(void) reset;

-(void) setInputTrad:(PY_SYMBOL_TRAD_KEY) keyType;

-(void) setOption:(PY_SYMBOL_TYPE) symType;

-(NSString *) getSymbol:(NSString *) aKey;

-(BOOL) isSymbol:(NSString *) aStr;

@end
