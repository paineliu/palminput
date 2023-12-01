#include "PYEngine.h"
#include "PDLexCore.h"
#import "palmengine.h"

@implementation PalmEngine

#define FT_ENGINE_STATE_WEBMAIL 4

- (BOOL) makeDictDir
{
    // NSLog(@"makeDictDir");

//    BOOL isExist;
//    BOOL isDir;
//    isExist = [[NSFileManager defaultManager] fileExistsAtPath:PY_DICT_USER_DICT_PATH_NAME isDirectory:&isDir];
//    if (!isExist || !isDir) {
//        return [[NSFileManager defaultManager] createDirectoryAtPath:[PY_DICT_USER_DICT_PATH_NAME stringByExpandingTildeInPath] withIntermediateDirectories:YES attributes:nil error:nil];
//    }
    return YES;
}

- (BOOL) saveUserDict:(PY_BLOCK *)block{
    
    if(![self makeDictDir])
        return NO;

    BOOL ret = NO;
    if (block->pAddress != PY_NULL && block->nSize > 0)
    {
//        NSString *path = [[PY_DICT_USER_DICT_PATH_NAME stringByAppendingPathComponent:@"pyuser_v1.pidic"] stringByExpandingTildeInPath];
//        NSMutableData *data = [[NSMutableData alloc] init];
//        [data appendBytes:block->pAddress length:block->nSize];
//        ret = [data writeToFile:path atomically:YES];
    }
    
    return ret;
}

- (BOOL) loadUserDict:(PY_BLOCK *) block
{
    // NSLog(@"loadUserDict");
    
    BOOL ret = FALSE;

    block->pAddress = PY_NULL;
    block->nSize = 0;

//    NSString* userPhraseDict = [[PY_DICT_USER_DICT_PATH_NAME stringByAppendingPathComponent:@"pyuser_v1.pidic"] stringByExpandingTildeInPath];
//    
//    if ([[NSFileManager defaultManager] fileExistsAtPath:userPhraseDict isDirectory:NULL] == NO)
//    {
//        userPhraseDict = [[NSBundle mainBundle] resourcePath];
//        userPhraseDict = [userPhraseDict stringByAppendingPathComponent:@"pyuserempty.pidic"];
//    }
//
//    FILE *pFile = fopen([userPhraseDict UTF8String], "rb");
//    
//    if (NULL != pFile)
//    {
//        fseek(pFile, 0, SEEK_END);
//        int nSize = (int) ftell(pFile);
//        if (nSize > 0)
//        {
//            block->pAddress = malloc(nSize);
//            if (block->pAddress)
//            {
//                block->nSize = nSize;
//                fseek(pFile, 0, SEEK_SET);
//                fread(block->pAddress, 1, block->nSize, pFile);
//                ret = TRUE;
//            }
//        }
//
//        fclose(pFile);
//    }
    
    return ret;
}

- (void)save:(BOOL) force;
{
    // NSLog(@"save");

    if ((force && _commitTimes > 0) || _commitTimes > 200)
    {
        _commitTimes = 0;
    //    [self saveUserDict:&_ftUserBlock];
    }
}

- (id)init
{
    // NSLog(@"FTInputConvert init begin");

    self = [super init];
    
    if (self)
    {
        PY_BLOCK dictBlock[32];
        int nDictTotal = 0;
        
        _pySysLex = [[NSData alloc] initWithContentsOfFile:[NSString stringWithFormat:@"%@/pybase.pidic",[[NSBundle mainBundle] resourcePath]]options: NSDataReadingMappedIfSafe error:nil];
        
        if ([_pySysLex length] > 0)
        {
            dictBlock[nDictTotal].pAddress = (PY_BYTE *) [_pySysLex bytes];
            dictBlock[nDictTotal].nSize = (PY_UINT32) [_pySysLex length];
            nDictTotal ++;
        }
        dictBlock[0].nSize = PDLexCore_size();
        dictBlock[0].pAddress = PDLexCore_ptr();
        nDictTotal = 1;
        
        
//        [self loadUserDict:&_ftUserBlock];
//        if (_ftUserBlock.nSize > 0)
//        {
//            dictBlock[nDictTotal].pAddress = _ftUserBlock.pAddress;
//            dictBlock[nDictTotal].nSize = _ftUserBlock.nSize;
//            nDictTotal ++;
//        }
        
        PY_UINT32 nSize = PYEngine_GetInstanceSize();
        
        _pyEngineInst = malloc(nSize);
        if (_pyEngineInst)
        {
            _ftHandle = PYEngine_Initialize(_pyEngineInst, nSize, dictBlock, nDictTotal);
        }
        else
        {
            _ftHandle = nil;
        }
        
        _commitTimes = 0;
        _candMaxNum = 5;
        
        _candList = [[NSMutableArray alloc] init];
        _pageList = [[NSMutableArray alloc] init];
    }

    return self;
}


-(void) setPageItemMaxNum:(int) maxNum
{
    // NSLog(@"setPageItemMaxNum");

    if (maxNum > 2 && maxNum < 10)
    {
        _candMaxNum = maxNum;
    }
}

-(BOOL) setOptions:(const int *) options
{
    // NSLog(@"setOptions");

    return FALSE;
}

-(void) getOptions:(int *) options
{
    // NSLog(@"getOptions");
}

- (void) dealloc
{
    // NSLog(@"dealloc");

    if (_ftHandle)
    {
        PYEngine_Terminate(_ftHandle);
    }

    if (_pyEngineInst)
    {
        free(_pyEngineInst);
        _pyEngineInst = nil;
    }
}

-(void) resetContext
{
    // NSLog(@"resetContext");

    if (_ftHandle)
    {
        PYEngine_Reset(_ftHandle);
    }
}

-(void) reset
{
    // NSLog(@"reset");

    if (_ftHandle)
    {
        PYEngine_Reset(_ftHandle);
    }
}

-(BOOL) isFreeEdit
{
    // NSLog(@"isFreeEdit");

    if (([self getInputState] & FT_ENGINE_STATE_WEBMAIL) && _pageCurrent == 0)
    {
        return TRUE;
    }
    else
    {
        NSString *strKeys = [self getKeyString];
        if (strKeys && [strKeys length] > 0)
        {
            return ([self getCandTotal] == 0);
        }
    }
    
    return FALSE;
}

-(int) insertSpace:(UniChar *) cInput charLen:(int) charLen maxLen:(int) maxLen
{
    // NSLog(@"insertSpace");

    int nSpaceNum = 0;
    
    if (charLen > 0)
    {
        for (int i=1; i<charLen; i++)
        {
            if ((cInput[i-1] < 127 && cInput[i] > 127) || (cInput[i-1] > 127 && cInput[i] < 127))
            {
                nSpaceNum ++;
            }
        }
        
        if (nSpaceNum != 0 && nSpaceNum + charLen < maxLen)
        {
            cInput[nSpaceNum + charLen] = 0;
            int nPos = nSpaceNum + charLen - 1;
            for (int i=charLen-1; i>0; i--)
            {
                if (nPos > 0)
                {
                    cInput[nPos] = cInput[i];
                }
                nPos --;
                
                if ((cInput[i-1] < 127 && cInput[i] > 127) || (cInput[i-1] > 127 && cInput[i] < 127))
                {
                    if (nPos > 0)
                    {
                        cInput[nPos] = ' ';
                    }
                    nPos --;
                }
            }
        }
    }
    
    return nSpaceNum + charLen;
}

-(NSString *) getKeyString
{
    // NSLog(@"getKeyString");

    if (_ftHandle)
    {
        UniChar cString[128] = {0};
        int nLen = PYEngine_GetConvertedString(_ftHandle, cString, 128);
        NSString *string = [[NSString alloc] initWithBytes:cString
                                                    length:nLen * 2
                                                    encoding:NSUTF16LittleEndianStringEncoding];
        return string;
    }
    
    return @"";
}

-(BOOL) processKey:(unichar) aKey
{
    // NSLog(@"processKey");

    BOOL bRet = FALSE;
    if (_ftHandle)
    {
//        if (_ftEngine.pfnAstIsActive(_ftHandle))
//        {
//            bRet = _ftEngine.pfnAstAddKey(_ftHandle, aKey);
//        }
//        else
        {
            bRet = PYEngine_KeyEvent(_ftHandle, aKey, 0);
        }
        
        [self resetPage];
    }
    
    return bRet;
}

-(void) setTradMode:(BOOL) bTrad
{
    // NSLog(@"setTradMode");

//    FT_OPTION options = {0};
//
//    if (_ftHandle)
//    {
//        _ftEngine.pfnGetOption(_ftHandle, &options);
//        options.stPinyinOption.bTradition = bTrad;
//        _ftEngine.pfnSetOption(_ftHandle, &options);
//        [self resetPage];
//    }
}

-(BOOL) isTradMode
{
    // NSLog(@"isTradMode");

//    FT_OPTION options = {0};
//
//    if (_ftHandle)
//    {
//        _ftEngine.pfnGetOption(_ftHandle, &options);
//
//        return options.stPinyinOption.bTradition;
//    }
//
    return FALSE;
}

-(BOOL) setCandFilter:(int) filterId
{
    // NSLog(@"setCandRangeWordOnly");

    if (_ftHandle)
    {
        PYEngine_SetFilterType(_ftHandle, filterId);
        [self resetPage];
        
        return TRUE;
    }
    
    return FALSE;
}

-(int) getCandFilter
{
    // NSLog(@"isCandRangeWordOnly");

    if (_ftHandle)
    {
        return PYEngine_GetFilterType(_ftHandle);
    }
    
    return 0;
}

-(int) getCandTotal
{
    // NSLog(@"getCandTotal");

    int nTotal = -1;
    if (_ftHandle)
    {
        nTotal = PYEngine_GetCandTotal(_ftHandle);
    }
    
    return nTotal;
}

-(NSString *) requestCloud
{
    // NSLog(@"requestCloud");

//    if (!_cloudObj)
//        _cloudObj = [[FTCloud alloc] initWithDelegate:self];
//    if (_cloudObj)
//    {
//        if (_ftHandle)
//        {
//            FT_COMP_RANGE stInputRange = {0};
//            UniChar cString[128] = {0};
//            _ftEngine.pfnCandGetCompString(_ftHandle, 0, cString, 128, NULL, &stInputRange, FT_FALSE);
//            NSString *szInputString = [[[NSString alloc] initWithBytes:&cString[stInputRange.stCand.nBegin]
//                                                                length:(stInputRange.stCand.nLen+stInputRange.stRemain.nLen) * 2
//                                                                encoding:NSUTF16LittleEndianStringEncoding] autorelease];
//            
//            FT_COMP_RANGE stCompRange = {0};
//            FT_UINT16 nLocalCaretPos = 0;
//            _ftEngine.pfnCandGetCompString(_ftHandle, 0, cString, 128, &nLocalCaretPos, &stCompRange, FT_TRUE);
//            NSString *szCompString = [[[NSString alloc] initWithBytes:&cString[stCompRange.stCand.nBegin]
//                                                                length:(stCompRange.stCand.nLen+stCompRange.stRemain.nLen) * 2
//                                                                encoding:NSUTF16LittleEndianStringEncoding] autorelease];
//            
//            FT_UINT32 nPinyinLen = _ftEngine.pfnCandGetItemEncode(_ftHandle, 0, cString, 128);
//            NSString *szPinyinString = [[[NSString alloc] initWithBytes:cString
//                                                               length:nPinyinLen * 2
//                                                               encoding:NSUTF16LittleEndianStringEncoding] autorelease];
//            
//            FT_UINT32 nItemLen = _ftEngine.pfnCandGetItemDisplay(_ftHandle, 0, cString, 128);
//            NSString *szItemString = [[[NSString alloc] initWithBytes:cString
//                                                               length:nItemLen * 2
//                                                               encoding:NSUTF16LittleEndianStringEncoding] autorelease];
//            
//            FT_OPTION			stEngineOption;
//            FT_SCHEME_ID        ePinyinSchemeID;
//            
//            _ftEngine.pfnGetOption(_ftHandle, &stEngineOption);
//            
//            ePinyinSchemeID = _ftEngine.pfnGetSchemeId(_ftHandle);
//            if (ePinyinSchemeID > FT_SCHEME_PINYIN_SHUANGPIN_CUSTOM)
//                ePinyinSchemeID = FT_SCHEME_PINYIN_SHUANGPIN_CUSTOM;
//            else if (ePinyinSchemeID < FT_SCHEME_PINYIN_QUANPIN)
//                ePinyinSchemeID = FT_SCHEME_PINYIN_QUANPIN;
//            
//            FTCloudResponseData* pData = [_cloudObj syncRequestCloud:szInputString :_ftEngine.pfnInputGetCaret(_ftHandle)-stInputRange.stHan.nLen :stEngineOption.nFuzzy :stEngineOption.bCorrect :ePinyinSchemeID :szItemString :szPinyinString :szCompString :nLocalCaretPos-stCompRange.stHan.nLen];
//            if (pData)
//            {
//                // 在本地查询到云的结果
//                FT_CLOUD_ITEM stFTCloudItem = {0};
//                
//                memcpy(stFTCloudItem.wPhraseString, (FT_PCWSTR)[pData->pResultString cStringUsingEncoding:NSUTF16LittleEndianStringEncoding], [pData->pResultString length]*sizeof(FT_WCHAR));
//                stFTCloudItem.nPhraseLen = [pData->pResultString length];
//                memcpy(stFTCloudItem.wPinyinString, (FT_PCWSTR)[pData->pPinyinString cStringUsingEncoding:NSUTF16LittleEndianStringEncoding], [pData->pPinyinString length]*sizeof(FT_WCHAR));
//                stFTCloudItem.nPinyinLen = [pData->pPinyinString length];
//                memcpy(stFTCloudItem.wCompString, (FT_PCWSTR)[pData->pCompString cStringUsingEncoding:NSUTF16LittleEndianStringEncoding], [pData->pCompString length]*sizeof(FT_WCHAR));
//                stFTCloudItem.nCompLen = [pData->pCompString length];
//                
//                stFTCloudItem.nCaretPos = pData->nCaretPos;
//                stFTCloudItem.nWeight = pData->nWeight;
//                stFTCloudItem.nItemFlag = pData->nItemFlag | FT_CAND_PHRASE_INFO_COMPLETE | FT_CLOUD_ITEM_FLAG_SENTENCE;
//                
//                _ftEngine.pfnCloudAddPhrases(_ftHandle, FT_TRUE, &stFTCloudItem, 1);
//                
//                FT_CAND_ITEM_INFO nInfo = _ftEngine.pfnCandGetItemInfo(_ftHandle, 1);
//                if ((nInfo.nItemInfo & FT_CAND_PHRASE_INFO_REPEAT_CLOUD) == 0)
//                    [_cloudObj setCloudState:CLOUD_STATE_DIFF];
//                else
//                    [_cloudObj setCloudState:CLOUD_STATE_SAME];
//                
//                UniChar cString[128] = {0};
//                int nLen = _ftEngine.pfnCandGetItemDisplay(_ftHandle, 1, cString, 128);
//                
//                NSString *string = [[[NSString alloc] initWithBytes:cString
//                                                             length:nLen * 2
//                                                             encoding:NSUTF16LittleEndianStringEncoding] autorelease];
//                return string;
//            }
//        }
//    }
//    
    return nil;
}

-(NSString *) getCandItem:(int) candItemId
{
    // NSLog(@"getCandItem");

    if (_ftHandle)
    {
        UniChar cString[128] = {0};
        int nLen = PYEngine_GetCandItemString(_ftHandle, candItemId, cString, 128);
        
//        FT_ITEM_INFO itemInfo;
//        _ftEngine.pfnCandGetItemInfo(_ftHandle, candItemId, &itemInfo);
//        if (itemInfo.nItemInfo & FT_ITEM_INFO_CLOUD)
//        {
//            NSString* cloudString = [self requestCloud];
//            if (cloudString)
//                return cloudString;
//        }

//        if (_separatedSpace)
//        {
//            nLen = [self insertSpace:cString charLen:nLen maxLen:128];
//        }
        
        NSString *string = [[NSString alloc] initWithBytes:cString
                                                     length:nLen * 2
                                                   encoding:NSUTF16LittleEndianStringEncoding];
        
//        if (itemInfo.nItemInfo & FT_ITEM_INFO_EMOJI)
//        {
//            unsigned unicodeInt = 0;
//            [[NSScanner scannerWithString:string] scanHexInt:&unicodeInt];
//
//            if (unicodeInt != 0)
//            {
//                char chars[8];
//                int len = 4;
//
//                chars[0] = (unicodeInt >> 24) & (1 << 24) - 1;
//                chars[1] = (unicodeInt >> 16) & (1 << 16) - 1;
//                chars[2] = (unicodeInt >> 8) & (1 << 8) - 1;
//                chars[3] = unicodeInt & (1 << 8) - 1;
//
//                if (unicodeInt < 0x3000)
//                {
//                    unicodeInt = 0xFE0F;
//
//                    chars[4] = (unicodeInt >> 24) & (1 << 24) - 1;
//                    chars[5] = (unicodeInt >> 16) & (1 << 16) - 1;
//                    chars[6] = (unicodeInt >> 8) & (1 << 8) - 1;
//                    chars[7] = unicodeInt & (1 << 8) - 1;
//
//                    len = 8;
//                }
//
//                string = [[NSString alloc] initWithBytes:chars length:len encoding:NSUTF32StringEncoding];
//            }
//        }

        return string;
    }
    
    return nil;
}

-(BOOL) delCandItem:(int) aItem
{
    // NSLog(@"delCandItem");

    if (_ftHandle)
    {
        _commitTimes ++;
        
//        return _ftEngine.pfnCandDelItem(_ftHandle, aItem);
    }
    
    return FALSE;
}

-(void) undoLastCommit
{
    // NSLog(@"undoLastCommit");

    if (_ftHandle)
    {
        //_ftEngine.pfnInputRollback(_ftHandle);
    }
}

-(NSString *) selCandItem:(int) aItem
{
    // NSLog(@"selCandItem");

    if (_ftHandle)
    {
        if (PYEngine_GetCandTotal(_ftHandle) > aItem)
        {
//            FT_ITEM_INFO nInfo;
//            _ftEngine.pfnCandGetItemInfo(_ftHandle, aItem, &nInfo);
            
//            FT_UINT32 nSelectFlag = 0;
            PY_UINT16 nRet = PYEngine_SelCandItem(_ftHandle, aItem);
            
            if (nRet == 0)
            {
                [self resetPage];
            }
            else if (nRet == 1)
            {
                UniChar cString[4096] = {0};
                int nLen = PYEngine_GetConvertedString(_ftHandle, cString, 4096);
                if (nLen)
                {
                    cString[nLen] = 0;
//                    if (_separatedSpace)
//                    {
//                        nLen = [self insertSpace:cString charLen:nLen maxLen:4096];
//                    }
                    
                    NSString *string = [[NSString alloc] initWithBytes:cString length:nLen * 2
                                                              encoding:NSUTF16LittleEndianStringEncoding];
                    
//                    if (nInfo.nItemInfo & FT_CAND_PHRASE_INFO_EMOJI)
//                    {
//                        unsigned unicodeInt = 0;
//                        [[NSScanner scannerWithString:string] scanHexInt:&unicodeInt];
//                        
//                        if (unicodeInt != 0)
//                        {
//                            char chars[8];
//                            int len = 4;
//                            
//                            chars[0] = (unicodeInt >> 24) & (1 << 24) - 1;
//                            chars[1] = (unicodeInt >> 16) & (1 << 16) - 1;
//                            chars[2] = (unicodeInt >> 8) & (1 << 8) - 1;
//                            chars[3] = unicodeInt & (1 << 8) - 1;
//                            
//                            if (unicodeInt < 0x3000)
//                            {
//                                unicodeInt = 0xFE0F;
//                                
//                                chars[4] = (unicodeInt >> 24) & (1 << 24) - 1;
//                                chars[5] = (unicodeInt >> 16) & (1 << 16) - 1;
//                                chars[6] = (unicodeInt >> 8) & (1 << 8) - 1;
//                                chars[7] = unicodeInt & (1 << 8) - 1;
//                                
//                                len = 8;
//                            }
//                            
//                            string = [[[NSString alloc] initWithBytes:chars length:len encoding:NSUTF32StringEncoding] autorelease];
//                        }
//                    }

                    _commitTimes ++;
                                  
                    return string;
                }
            }
        }
    }
    
    return nil;
}

- (BOOL) isFirstItemHasFocus
{
    // NSLog(@"isFirstItemHasFocus");
    
    return _candHotItem == 0 && _pageCurrent == 0;
}

- (BOOL) isCaretAtCompEnd
{
    
    // NSLog(@"isCaretAtCompEnd");
    
    PY_UINT16 nPos = 0;
    UniChar cString[128] = {0};
    int index = 0;
    int nLen = PYEngine_GetCompString(_ftHandle, index, cString, 128, &nPos, false);
    // NSLog(@"comp len = %d, pos = %d", nLen, nPos);
    return nLen == nPos;
}

-(NSString *) getCompString:(int *) caretPos withSplit:(BOOL) bSplit
{
    // NSLog(@"getCompString");

    if (_ftHandle)
    {
        UniChar cString[128] = {0};
        PY_UINT16 nPos = 0;
        int index = 0;
        int nLen = PYEngine_GetCompString(_ftHandle, index, cString, 128, &nPos, bSplit);
        NSString *string = [[NSString alloc] initWithBytes:cString length:nLen * 2 encoding:NSUTF16LittleEndianStringEncoding];
        if (caretPos)
        {
            *caretPos = nPos;
        }
        
        return string;
    }
    
    return nil;
}

-(BOOL) moveCaret:(int) caretOper
{
    // NSLog(@"moveCaret");

    if (_ftHandle)
    {
        BOOL ret = PYEngine_MoveCaret(_ftHandle, caretOper);
        if (ret)
        {
            [self resetPage];
        }
        
        return ret;
    }
    
    return FALSE;
}

-(void) resetPage
{
    // NSLog(@"resetPage");

//    if (_cloudObj)
//        [_cloudObj cancelLastRequest];
    
    _pageCurrent = 0;
    _candHotItem = 0;
    [_pageList removeAllObjects];
}

-(int)  getHotspotPageItemId
{
    // NSLog(@"getPageHotItemIndex");

    return _candHotItem;
}

-(int)  getPageItemTotal
{
    // NSLog(@"getPageItemTotal");

    return (int) [_candList count];
}

-(NSString *) getCandPrefix
{
    // NSLog(@"getCandPrefix");

//    if (_ftHandle)
//    {
//        UniChar cString[128] = {0};
//        int nLen = _ftEngine.pfnCandGetPrefixPhrase(_ftHandle, cString, 128);
//        NSString *string = [[NSString alloc] initWithBytes:cString
//                                                    length:nLen * 2
//                                                    encoding:NSUTF16LittleEndianStringEncoding];
//
//        return string;
//    }
    
    return @"";
}

-(BOOL) getNewPage:(NSUInteger) beginPos maxNum:(NSUInteger) maxNum range:(NSRange *) aRange
{
    // NSLog(@"getNewPage");

    NSRange range = NSMakeRange(0, 0);
    
    if (beginPos + maxNum <= [self getCandTotal])
    {
        range.location = beginPos;
        range.length = maxNum;
    }
   
    
    if (range.length != 0)
    {
        for (NSUInteger i=range.location; i<range.location + range.length; i++)
        {
            if (i == range.location)
            {
                NSString * strItem = [NSString stringWithFormat:@"%@%@", [self getCandPrefix] , [self getCandItem:(int)(i)]];
                
                [_candList addObject:strItem];
            }
            else
            {
                [_candList addObject:[self getCandItem:(int)(i)]];
            }
        }
        
        if (aRange)
        {
            *aRange = range;
        }
        
        return TRUE;
    }
    
    return FALSE;
}

-(NSArray *) getPageItems
{
    // NSLog(@"getPageItems");

    [_candList removeAllObjects];
    
    int candMaxNum = _candMaxNum;
    if ([self getInputState] & FT_ENGINE_STATE_WEBMAIL)
    {
        if ([_pageList count] == 0)
        {
            candMaxNum = 1;
        }
        else
        {
            candMaxNum = 3;
        }
    }
    
    if (_pageCurrent < [_pageList count])
    {
        NSRange range = [[_pageList objectAtIndex:_pageCurrent] rangeValue];
        [self getNewPage:range.location maxNum:range.length range:nil];
    }
    else
    {
        NSRange range = NSMakeRange(0, candMaxNum);
        
        if ([_pageList count] > 0)
        {
            range = [[_pageList objectAtIndex:([_pageList count] - 1)] rangeValue];
            range.location += range.length;
            range.length = candMaxNum;
        }
        
        NSRange range1;
        [self getNewPage:range.location maxNum:range.length range:&range1];
        [_pageList addObject:[NSValue valueWithRange: range1]];
    }
    
    return _candList;
}

- (BOOL) moveHotspotLeft
{
    // NSLog(@"moveHotspotLeft");

    if (_candHotItem > 0)
    {
        _candHotItem --;
        return TRUE;
    }
    else
    {
        if ([self pageUp])
        {
            NSRange range = [[_pageList objectAtIndex:_pageCurrent] rangeValue];
            _candHotItem = (int) range.length - 1;
            
            return TRUE;
        }
    }
    
    return FALSE;
}

- (BOOL) moveHotspotRight
{
    // NSLog(@"moveHotspotRight");

    if ([_candList count] > 0)
    {
        if (_candHotItem + 1 < [_candList count])
        {
            _candHotItem ++;
            return TRUE;
        }
        else
        {
            return [self pageDown];
        }
    }
    
    return FALSE;
}

- (BOOL) testPageUp
{
    // NSLog(@"testPageUp");

    return (_pageCurrent > 0);
}

-(BOOL) testPageDown
{
    // NSLog(@"testPageDown");

    return (_pageCurrent + 1 < [_pageList count]);
}

-(BOOL) pageUp
{
    // NSLog(@"pageUp");

    if ([self testPageUp])
    {
        _candHotItem = 0;
        _pageCurrent --;
        return TRUE;
    }
    
    return FALSE;
}

-(BOOL) pageDown
{
    // NSLog(@"pageDown");

    if ([self testPageDown])
    {
        _candHotItem = 0;
        _pageCurrent ++;
        return TRUE;
    }
    
    return FALSE;
}

-(NSString *) getPageItem:(int) aItem
{
    // NSLog(@"getPageItem");

    if (_pageCurrent < [_pageList count] )
    {
        NSRange range = [[_pageList objectAtIndex:_pageCurrent] rangeValue];
        return [self getCandItem:(int) range.location + aItem];
    }
    
    return nil;
}

-(NSString *) selPageItem:(int) aItem
{
    // NSLog(@"selPageItem");

    if (_pageCurrent < [_pageList count] )
    {
        NSRange range = [[_pageList objectAtIndex:_pageCurrent] rangeValue];
        return [self selCandItem:(int) range.location + aItem];
    }
    
    return nil;
}

-(BOOL) delPageItem:(int) aItem
{
    // NSLog(@"delPageItem");

    if (_pageCurrent < [_pageList count])
    {
        NSRange range = [[_pageList objectAtIndex:_pageCurrent] rangeValue];
        if ([self delCandItem:(int) range.location + aItem])
        {
            [self resetPage];
            return TRUE;
        }
    }
    
    return FALSE;
}

-(PY_CLOUD_STATE) getCloudState
{
    // NSLog(@"getCloudState");

//    if (_pageCurrent == 0 && _cloudObj)
//        return [_cloudObj getCloudState];
    
    return CLOUD_STATE_NONE;
}

-(int) getInputState
{
    // NSLog(@"getInputState");
   
    if (_ftHandle)
    {
 //       return _ftEngine.pfnInputGetState(_ftHandle);
    }
    
    return 0;
}

-(BOOL) updateCateDicts:(NSArray*)arrayCateName SPAssistDict:(NSString*)strSPAssistGUID {

    // NSLog(@"updateCandDicts");

//    if (arrayCateName) {
//        //一次只加载前50个下载的词库
//        if ([arrayCateName count] > 50) {
//            arrayCateName =[arrayCateName subarrayWithRange:NSMakeRange(0, 50)];
//        }
//    }
//    
//    if (_ftHandle)
//    {
//        FT_BYTE* pNeedFreeBlocks[65] = {0};
//        uint     nNeedFreeBlocksNum = 0;
//        
//        // 加载个性短语词库
//        if (_ftSpecBlock.pAddress)
//        {
//            pNeedFreeBlocks[nNeedFreeBlocksNum++] = _ftSpecBlock.pAddress;
//            _ftSpecBlock.pAddress = FT_NULL;
//            _ftSpecBlock.nSize = 0;
//        }
//        if ([self loadPhraseDict:FT_SPEC2PHRASE_DICT_FILE_NAME toDict:&_ftSpecBlock])
//        {
//            FT_DICT_BLOCK specDictBlock;
//            specDictBlock.nDictId = FT_DICT_ID_SPEC2_PHRASE;
//            specDictBlock.nPriority = 0;
//            specDictBlock.stBlock.pAddress = _ftSpecBlock.pAddress;
//            specDictBlock.stBlock.nSize = _ftSpecBlock.nSize;
//            
//            _ftEngine.pfnUpdateDict(_ftHandle, eUpdateSpecPhraseDict, &specDictBlock, 1);
//        }
//        
//        // 加载分类词库
//        for (int i = 0 ; i < _cateBlocksNum ; ++i)
//        {
//            if (_ftCateBlocks[i].pAddress)
//            {
//                pNeedFreeBlocks[nNeedFreeBlocksNum++] = _ftCateBlocks[i].pAddress;
//                _ftCateBlocks[i].pAddress = FT_NULL;
//                _ftCateBlocks[i].nSize = 0;
//            }
//        }
//        _cateBlocksNum = 0;
//        
//        FT_DICT_BLOCK cateDictBlock[64];
//        int nCateDictTotal = 0;
//        for (NSString* cateDictName in arrayCateName)
//        {
//            if ([self loadPhraseDict:cateDictName toDict:&_ftCateBlocks[_cateBlocksNum]])
//            {
//                cateDictBlock[nCateDictTotal].nDictId = FT_DICT_ID_PY_CAT;
//                cateDictBlock[nCateDictTotal].nPriority = 0;
//                cateDictBlock[nCateDictTotal].stBlock.pAddress = _ftCateBlocks[_cateBlocksNum].pAddress;
//                cateDictBlock[nCateDictTotal].stBlock.nSize = _ftCateBlocks[_cateBlocksNum].nSize;
//                
//                ++_cateBlocksNum;
//                ++nCateDictTotal;
//            }
//        }
//        _ftEngine.pfnUpdateDict(_ftHandle, eUpdateCategoryDict, cateDictBlock, nCateDictTotal);
//        
//        // 加载双拼辅助码词库
//        if (_ftSPAssistBlock.pAddress)
//        {
//            pNeedFreeBlocks[nNeedFreeBlocksNum++] = _ftSPAssistBlock.pAddress;
//            _ftSPAssistBlock.pAddress = FT_NULL;
//            _ftSPAssistBlock.nSize = 0;
//        }
//        if ([self loadPhraseDict:strSPAssistGUID toDict:&_ftSPAssistBlock])
//        {
//            FT_DICT_BLOCK spAssistDictBlock;
//            spAssistDictBlock.nDictId = FT_DICT_ID_SHUANGPIN_ASSIST;
//            spAssistDictBlock.nPriority = 0;
//            spAssistDictBlock.stBlock.pAddress = _ftSPAssistBlock.pAddress;
//            spAssistDictBlock.stBlock.nSize = _ftSPAssistBlock.nSize;
//            
//            _ftEngine.pfnUpdateDict(_ftHandle, eUpdataSPAuxCodeDict, &spAssistDictBlock, 1);
//        }
//    
//        for (int i = 0 ; i < nNeedFreeBlocksNum ; ++i)
//        {
//            if (pNeedFreeBlocks[i])
//            {
//                free(pNeedFreeBlocks[i]);
//            }
//        }
//        
//        return TRUE;
//    }
    
    return FALSE;
}


@end
