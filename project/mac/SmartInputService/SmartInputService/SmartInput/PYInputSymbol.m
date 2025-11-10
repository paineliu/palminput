#import "PYInputSymbol.h"

@implementation PYInputSymbol

-(id) init
{
    self = [super init];

    if (self)
    {
        
        _lastSimpSingleQuote = @"";
        _lastSimpDoubleQuote = @"";
        
        _tradSymbolInput = PYSymbolTradNone;
        
        _dictEnHalfSymbol = [[NSDictionary alloc]init];

        _dictCnHalfSymbol = [[NSDictionary alloc]initWithObjectsAndKeys:
                             @"·", @"`",
                             @"-", @"-",
                             @"=", @"=",
                             @"【", @"[",
                             @"】", @"]",
                             @"、", @"\\",
                             @"；", @";",
                             @"‘’", @"'",
                             @"，", @",",
                             @"。", @".",
                             @"、", @"/",
                             @"～", @"~",
                             @"！", @"!",
                             @"@", @"@",
                             @"#", @"#",
                             @"￥", @"$",
                             @"%", @"%",
                             @"……", @"^",
                             @"&", @"&",
                             @"*", @"*",
                             @"（", @"(",
                             @"）", @")",
                             @"——", @"_",
                             @"+", @"+",
                             @"｛", @"{",
                             @"｝", @"}",
                             @"|", @"|",
                             @"：", @":",
                             @"“”", @"\"",
                             @"《", @"<",
                             @"》", @">",
                             @"？", @"?",
                             nil];
        
        _dictEnFullSymbol = [[NSDictionary alloc]initWithObjectsAndKeys:
                             @"０",@"0",
                             @"１",@"1",
                             @"２",@"2",
                             @"３",@"3",
                             @"４",@"4",
                             @"５",@"5",
                             @"６",@"6",
                             @"７",@"7",
                             @"８",@"8",
                             @"９",@"9",
                             
                             @"ａ",@"a",
                             @"ｂ",@"b",
                             @"ｃ",@"c",
                             @"ｄ",@"d",
                             @"ｅ",@"e",
                             @"ｆ",@"f",
                             @"ｇ",@"g",
                             @"ｈ",@"h",
                             @"ｉ",@"i",
                             @"ｊ",@"j",
                             @"ｋ",@"k",
                             @"ｌ",@"l",
                             @"ｍ",@"m",
                             @"ｎ",@"n",
                             @"ｏ",@"o",
                             @"ｐ",@"p",
                             @"ｑ",@"q",
                             @"ｒ",@"r",
                             @"ｓ",@"s",
                             @"ｔ",@"t",
                             @"ｕ",@"u",
                             @"ｖ",@"v",
                             @"ｗ",@"w",
                             @"ｘ",@"x",
                             @"ｙ",@"y",
                             @"ｚ",@"z",

                             @"Ａ",@"A",
                             @"Ｂ",@"B",
                             @"Ｃ",@"C",
                             @"Ｄ",@"D",
                             @"Ｅ",@"E",
                             @"Ｆ",@"F",
                             @"Ｇ",@"G",
                             @"Ｈ",@"H",
                             @"Ｉ",@"I",
                             @"Ｊ",@"J",
                             @"Ｋ",@"K",
                             @"Ｌ",@"L",
                             @"Ｍ",@"M",
                             @"Ｎ",@"N",
                             @"Ｏ",@"O",
                             @"Ｐ",@"P",
                             @"Ｑ",@"Q",
                             @"Ｒ",@"R",
                             @"Ｓ",@"S",
                             @"Ｔ",@"T",
                             @"Ｕ",@"U",
                             @"Ｖ",@"V",
                             @"Ｗ",@"W",
                             @"Ｘ",@"X",
                             @"Ｙ",@"Y",
                             @"Ｚ",@"Z",
                             
                             @"｀", @"`",
                             @"－", @"-",
                             @"＝", @"=",
                             @"［", @"[",
                             @"］", @"]",
                             @"＼", @"\\",
                             @"；", @";",
                             @"＇", @"'",
                             @"，", @",",
                             @"．", @".",
                             @"／", @"/",
                             @"～", @"~",
                             @"！", @"!",
                             @"＠", @"@",
                             @"＃", @"#",
                             @"＄", @"$",
                             @"％", @"%",
                             @"＾", @"^",
                             @"＆", @"&",
                             @"＊", @"*",
                             @"（", @"(",
                             @"）", @")",
                             @"＿", @"_",
                             @"＋", @"+",
                             @"｛", @"{",
                             @"｝", @"}",
                             @"｜", @"|",
                             @"：", @":",
                             @"＂", @"\"",
                             @"＜", @"<",
                             @"＞", @">",
                             @"？", @"?",
                             @"　", @" ",
                             nil];
        
        _dictCnFullSymbol = [[NSDictionary alloc]initWithObjectsAndKeys:
                             @"０",@"0",
                             @"１",@"1",
                             @"２",@"2",
                             @"３",@"3",
                             @"４",@"4",
                             @"５",@"5",
                             @"６",@"6",
                             @"７",@"7",
                             @"８",@"8",
                             @"９",@"9",
                             
                             @"ａ",@"a",
                             @"ｂ",@"b",
                             @"ｃ",@"c",
                             @"ｄ",@"d",
                             @"ｅ",@"e",
                             @"ｆ",@"f",
                             @"ｇ",@"g",
                             @"ｈ",@"h",
                             @"ｉ",@"i",
                             @"ｊ",@"j",
                             @"ｋ",@"k",
                             @"ｌ",@"l",
                             @"ｍ",@"m",
                             @"ｎ",@"n",
                             @"ｏ",@"o",
                             @"ｐ",@"p",
                             @"ｑ",@"q",
                             @"ｒ",@"r",
                             @"ｓ",@"s",
                             @"ｔ",@"t",
                             @"ｕ",@"u",
                             @"ｖ",@"v",
                             @"ｗ",@"w",
                             @"ｘ",@"x",
                             @"ｙ",@"y",
                             @"ｚ",@"z",
                             
                             @"Ａ",@"A",
                             @"Ｂ",@"B",
                             @"Ｃ",@"C",
                             @"Ｄ",@"D",
                             @"Ｅ",@"E",
                             @"Ｆ",@"F",
                             @"Ｇ",@"G",
                             @"Ｈ",@"H",
                             @"Ｉ",@"I",
                             @"Ｊ",@"J",
                             @"Ｋ",@"K",
                             @"Ｌ",@"L",
                             @"Ｍ",@"M",
                             @"Ｎ",@"N",
                             @"Ｏ",@"O",
                             @"Ｐ",@"P",
                             @"Ｑ",@"Q",
                             @"Ｒ",@"R",
                             @"Ｓ",@"S",
                             @"Ｔ",@"T",
                             @"Ｕ",@"U",
                             @"Ｖ",@"V",
                             @"Ｗ",@"W",
                             @"Ｘ",@"X",
                             @"Ｙ",@"Y",
                             @"Ｚ",@"Z",
                             
                             @"·", @"`",
                             @"－", @"-",
                             @"＝", @"=",
                             @"【", @"[",
                             @"】", @"]",
                             @"、", @"\\",
                             @"；", @";",
                             @"‘’", @"'",
                             @"，", @",",
                             @"。", @".",
                             @"、", @"/",
                             @"～", @"~",
                             @"！", @"!",
                             @"＠", @"@",
                             @"＃", @"#",
                             @"￥", @"$",
                             @"％", @"%",
                             @"……", @"^",
                             @"＆", @"&",
                             @"×", @"*",
                             @"（", @"(",
                             @"）", @")",
                             @"＿", @"_",
                             @"＋", @"+",
                             @"｛", @"{",
                             @"｝", @"}",
                             @"｜", @"|",
                             @"：", @":",
                             @"“”", @"\"",
                             @"《", @"<",
                             @"》", @">",
                             @"？", @"?",
                             @"　", @" ",
                             nil];
        
        _dictCurrent = _dictCnHalfSymbol;
    }
    
    return self;
}

-(void) setOption:(PY_SYMBOL_TYPE) symType
{
    switch (symType) {
        
        case PYSymbolEnHalf:
            _dictCurrent = _dictEnHalfSymbol;
            break;
        
        case PYSymbolCnHalf:
            _dictCurrent = _dictCnHalfSymbol;
            break;

        case PYSymbolEnFull:
            _dictCurrent = _dictEnFullSymbol;
            break;
            
        case PYSymbolCnFull:
            _dictCurrent = _dictCnFullSymbol;
            break;

        default:
            _dictCurrent = _dictEnHalfSymbol;
            break;
    }
}



-(void) reset
{
    _lastSimpSingleQuote = @"";
    _lastSimpDoubleQuote = @"";
}

-(void) setInputTrad:(PY_SYMBOL_TRAD_KEY) keyType
{
    _tradSymbolInput = keyType;
}

-(BOOL) isSymbol:(NSString *) aStr
{
    NSString *str = [_dictEnFullSymbol objectForKey:aStr];
    
    return str != nil;
}


-(NSString *) getSymbol:(NSString *) aKey
{
    NSString *str = [_dictCurrent objectForKey:aKey];
    
    if ([str isEqualToString:@"“”"])
    {
        if ([_lastSimpDoubleQuote isEqualToString:@"“"])
        {
            _lastSimpDoubleQuote = @"";
            str =  @"”";
        }
        else
        {
            _lastSimpDoubleQuote = @"“";
            str = @"“";
        }
    }
    else if ([str isEqualToString:@"‘’"])
    {
        if ([_lastSimpSingleQuote isEqualToString:@"‘"])
        {
            _lastSimpSingleQuote = @"";
            str =  @"’";
        }
        else
        {
            _lastSimpSingleQuote = @"‘";
            str = @"‘";
        }
    }
    
    if (_tradSymbolInput == PYSymbolTradQuote)
    {
        if ([str isEqualToString:@"‘"])
        {
            str = @"「";
        }
        else if ([str isEqualToString:@"’"])
        {
            str = @"」";
        }
        else if ([str isEqualToString:@"“"])
        {
            str = @"『";
        }
        else if ([str isEqualToString:@"”"])
        {
            str = @"』";
        }
    }
    else if (_tradSymbolInput == PYSymbolTradBracket)
    {
        if ([str isEqualToString:@"【"])
        {
            str = @"「";
        }
        else if ([str isEqualToString:@"】"])
        {
            str = @"」";
        }
        else if ([str isEqualToString:@"｛"])
        {
            str = @"『";
        }
        else if ([str isEqualToString:@"｝"])
        {
            str = @"』";
        }
    }

    return str;
}

@end
