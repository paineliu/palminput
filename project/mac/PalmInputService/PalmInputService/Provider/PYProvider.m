#import "PYProvider.h"
#include "PYEngine.h"
#include "PYDynEx.h"
#include "PDLexCore.h"

@implementation PYProvider

#define FT_ENGINE_STATE_WEBMAIL 4

// 用户文件
#define PY_DICT_USER_DICT_PATH_NAME                      @"~/Library/Application Support/BitInput/User"
#define PY_DICT_PINYIN_USER_DICT_FILE_NAME               @"py_user_v1.lex"
#define PY_DICT_PINYIN_USER_DICT_SIZE                    (4 * 1024 * 1024)

- (BOOL) makeDictDir
{
    // NSLog(@"makeDictDir");

    BOOL isExist;
    BOOL isDir;
    isExist = [[NSFileManager defaultManager] fileExistsAtPath:PY_DICT_USER_DICT_PATH_NAME isDirectory:&isDir];
    if (!isExist || !isDir) {
        return [[NSFileManager defaultManager] createDirectoryAtPath:[PY_DICT_USER_DICT_PATH_NAME stringByExpandingTildeInPath] withIntermediateDirectories:YES attributes:nil error:nil];
    }
    return YES;
}

- (BOOL) saveUserDict{
    
    BOOL ret = NO;
    if (_pUserDictPtr != PY_NULL && _pUserDictSize > 0)
    {
        NSString *userPhraseDict = [[PY_DICT_USER_DICT_PATH_NAME stringByAppendingPathComponent:PY_DICT_PINYIN_USER_DICT_FILE_NAME] stringByExpandingTildeInPath];
        FILE *pFile = fopen([userPhraseDict UTF8String], "wb");
        
        if (NULL != pFile)
        {
            fwrite(_pUserDictPtr, 1, _pUserDictSize, pFile);
            fclose(pFile);
            ret = TRUE;
        }
    }
    
    return ret;
}

- (BOOL) loadUserDict
{
    // NSLog(@"loadUserDict");
    
    BOOL ret = FALSE;

    _pUserDictPtr = PY_NULL;
    _pUserDictSize = 0;

    NSString* userPhraseDict = [[PY_DICT_USER_DICT_PATH_NAME stringByAppendingPathComponent:PY_DICT_PINYIN_USER_DICT_FILE_NAME] stringByExpandingTildeInPath];
    
    [self makeDictDir];
    
    if ([[NSFileManager defaultManager] fileExistsAtPath:userPhraseDict isDirectory:NULL] == NO)
    {
        PYDyn_Create([userPhraseDict UTF8String], PY_DICT_PINYIN_USER_DICT_SIZE);
    }

    FILE *pFile = fopen([userPhraseDict UTF8String], "rb");
    
    if (NULL != pFile)
    {
        fseek(pFile, 0, SEEK_END);
        int nSize = (int) ftell(pFile);
        if (nSize > 0)
        {
            _pUserDictPtr = malloc(nSize);
            if (_pUserDictPtr)
            {
                _pUserDictSize = nSize;
                fseek(pFile, 0, SEEK_SET);
                fread(_pUserDictPtr, 1, _pUserDictSize, pFile);
                ret = TRUE;
            }
        }

        fclose(pFile);
    }
    
    return ret;
}

- (void)save:(BOOL) force;
{
    // NSLog(@"save");

    if ((force && _commitTimes > 0) || _commitTimes > 200)
    {
        _commitTimes = 0;
        [self saveUserDict];
    }
}

- (id)init
{
    // NSLog(@"PYProvider init begin");

    self = [super init];
    
    if (self)
    {
        PY_BLOCK dictBlock[32];
        int nDictTotal = 0;
        
        _pySysLex = [[NSData alloc] initWithContentsOfFile:[NSString stringWithFormat:@"%@/py_sys.lex",[[NSBundle mainBundle] resourcePath]]options: NSDataReadingMappedIfSafe error:nil];
        
        if ([_pySysLex length] > 0)
        {
            dictBlock[nDictTotal].pAddress = (PY_BYTE *) [_pySysLex bytes];
            dictBlock[nDictTotal].nSize = (PY_UINT32) [_pySysLex length];
            nDictTotal ++;
        }
        else
        {
            dictBlock[nDictTotal].nSize = PDLexCore_size();
            dictBlock[nDictTotal].pAddress = (PY_UINT8 *) PDLexCore_ptr();
            nDictTotal ++;
            
        }
        
        
        [self loadUserDict];
        
        if (_pUserDictPtr != PY_NULL && _pUserDictSize > 0)
        {
            dictBlock[nDictTotal].pAddress = _pUserDictPtr;
            dictBlock[nDictTotal].nSize = _pUserDictSize;
            nDictTotal ++;
        }
        
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

-(void) reset
{
    // NSLog(@"reset");

    if (_ftHandle)
    {
        PYEngine_Reset(_ftHandle);
    }
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
        int nLen = PYEngine_GetKeyString(_ftHandle, cString, 128);
        NSString *string = [[NSString alloc] initWithBytes:cString
                                                    length:nLen * 2
                                                    encoding:NSUTF16LittleEndianStringEncoding];
        return string;
    }
    
    return @"";
}

-(NSString *) getConvertedString
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
        bRet = PYEngine_KeyEvent(_ftHandle, aKey, 0);
        
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
    
    return @"";
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

-(NSString *) getCompString:(int *) caretPos
{
    // NSLog(@"getCompString");

    if (_ftHandle)
    {
        UniChar cString[128] = {0};
        PY_UINT16 nPos = 0;
        int index = 0;
        int nLen = PYEngine_GetCompString(_ftHandle, index, cString, 128, &nPos, TRUE);
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
        PYEngine_MoveCaret(_ftHandle, caretOper);
        
        return TRUE;
    }
    
    return FALSE;
}

-(void) resetPage
{
    // NSLog(@"resetPage");

    _pageCurrent = 0;
    _candHotItem = 0;
    _pageTotal =  ([self getCandTotal] + _candMaxNum - 1) / _candMaxNum;
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

-(BOOL) getPage:(NSUInteger) pageId
{
    unsigned long nBeginItem = pageId * _candMaxNum;
    unsigned long nEndItem =  (pageId + 1) * _candMaxNum;
    
    [_candList removeAllObjects];
    
    if (nEndItem > [self getCandTotal])
    {
        nEndItem = [self getCandTotal];
    }
    
    for (unsigned long i=nBeginItem; i<nEndItem; i++)
    {
        [_candList addObject:[self getCandItem:(int)(i)]];
    }
    
    return [_candList count] > 0;
}

-(NSArray *) getPageItems
{
    // NSLog(@"getPageItems");

    [self getPage:_pageCurrent];

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
            _candHotItem =_candMaxNum - 1;
            
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

- (BOOL) hasPrevPage
{
    // NSLog(@"testPageUp");

    return (_pageCurrent > 0);
}

-(BOOL) hasNextPage
{
    // NSLog(@"testPageDown");
    return (_pageCurrent + 1 < _pageTotal);
}

-(BOOL) pageUp
{
    // NSLog(@"pageUp");

    if ([self hasPrevPage])
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

    if ([self hasNextPage])
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

    if (_pageCurrent < _pageTotal)
    {
        return [self getCandItem:_pageCurrent * _candMaxNum + aItem];
    }
    
    return nil;
}

-(NSString *) selPageItem:(int) aItem
{
    // NSLog(@"selPageItem");
    if (_pageCurrent < _pageTotal)
    {
        return [self selCandItem:_pageCurrent * _candMaxNum + aItem];
    }
    
    return nil;
}

-(BOOL) delPageItem:(int) aItem
{
    // NSLog(@"delPageItem");

    if (_pageCurrent < _pageTotal)
    {
        if ([self delCandItem:(int) _pageCurrent * _candMaxNum + aItem])
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
