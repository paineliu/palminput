#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <ctype.h>

#include <map>
#include <cmath>
#include <algorithm>
#include <time.h>
#include "PYPriDef.h"
#include "PYLex.h"
#include "PYLexEx.h"

using namespace std;

typedef struct _PHRASE_ITEM 
{
    PY_UINT16 szPhrase[64];
    int nOffset; // 数据偏移地址
    int nLen;    // 数据长度
    int nFreq;   // 词频
    int weight;  // 权重 -- 计算得出
    int group;   // 分组 -- 计算得出
    PY_UINT32 getId() {
        return (nLen << 24) | nOffset;
    }
} PHRASE_ITEM;

bool PYLex_FreqSort (PHRASE_ITEM a,PHRASE_ITEM b) { return (a.nFreq > b.nFreq); }
bool PYLex_AscSort(PHRASE_ITEM a, PHRASE_ITEM b) 
{
    return PY_wcscmp(a.szPhrase, b.szPhrase) < 0;
}

char * PYLex_strlwr(char *str)
{
    char *orign=str;
    for (; *str!='\0'; str++)
        *str = tolower(*str);
    return orign;
}

char * PYLex_strupr(char *str)
{
    char *orign=str;
    for (; *str!='\0'; str++)
        *str = toupper(*str);
    return orign;
}

PY_BOOL PYLex_SaveToCpp(const char* pBinFileName, const char* pCppFileName)
{
    PY_BOOL bRet = PY_FALSE;
    char szHxxFileName[PY_MAX_PATH];
    int i;
    int nLen = strlen(pCppFileName);
    char fname[260];
    char fname_upper[260];
    char fname_lower[260];

    strcpy(szHxxFileName, pCppFileName);
    szHxxFileName[nLen - 1] = 'h';
    int nBegin = 0;

    for (i = nLen - 1; i > 0; i--)
    {
        if (pCppFileName[i] == '\\' || pCppFileName[i] == '/')
        {
            nBegin = i+1;
            break;
        }
    }
    strcpy(fname, &pCppFileName[nBegin]);
    nLen = strlen(fname);
    for (i = 0; i < nLen; i++)
    {
        if (fname[i] == '.')
        {
            fname[i] = 0;
            break;
        }
    }

    strcpy(fname_upper, fname);
    strcpy(fname_lower, fname);
    PYLex_strupr(fname_upper);
    PYLex_strlwr(fname_lower);

    FILE* fp = fopen(pBinFileName, "rb");
    FILE* fp_h = fopen(szHxxFileName, "w");
    if (fp_h != NULL)
    {

        fprintf(fp_h, "#ifndef __%s_H__\n", fname_upper);
        fprintf(fp_h, "#define __%s_H__\n", fname_upper);
        fprintf(fp_h, "\n");
        fprintf(fp_h, "#ifdef __cplusplus\n");
        fprintf(fp_h, "extern \"C\" {\n");
        fprintf(fp_h, "#endif /* __cplusplus */\n");
        fprintf(fp_h, "\n");
        fprintf(fp_h, "const unsigned char* %s_ptr();\n", fname);
        fprintf(fp_h, "const unsigned int %s_size();\n", fname);
        fprintf(fp_h, "\n");
        fprintf(fp_h, "#ifdef __cplusplus\n");
        fprintf(fp_h, "}\n");
        fprintf(fp_h, "#endif /* __cplusplus */\n");
        fprintf(fp_h, "\n");
        fprintf(fp_h, "#endif /* __%s_H__ */\n", fname_upper);
        fprintf(fp_h, "\n");

        fclose(fp_h);
    }

    FILE* fp_w = fopen(pCppFileName, "w");
    int nTotal = 0;
    unsigned char cRead;

    if (fp != NULL && fp_w != NULL)
    {
        fprintf(fp_w, "#include \"%s.h\"\n", fname);
        fprintf(fp_w, "\n");
        fprintf(fp_w, "const unsigned char g_%s[] = {\n", fname_lower);

        while (fread(&cRead, 1, 1, fp))
        {
            if (nTotal % 8 == 0)
            {
                fprintf(fp_w, "    ");
            }

            fprintf(fp_w, "0x%02x,", cRead);

            if (nTotal % 8 == 7)
            {
                fprintf(fp_w, "\n");
            }
            else
            {
                fprintf(fp_w, " ");
            }

            nTotal++;
        }

        fprintf(fp_w, "};\n");
        fprintf(fp_w, "\n");

        fprintf(fp_w, "const unsigned char* %s_ptr()\n", fname);
        fprintf(fp_w, "{\n");
        fprintf(fp_w, "    return g_%s;\n", fname_lower);
        fprintf(fp_w, "}\n");
        fprintf(fp_w, "\n");

        fprintf(fp_w, "const unsigned int %s_size()\n", fname);
        fprintf(fp_w, "{\n");
        fprintf(fp_w, "    return %d;\n", nTotal);
        fprintf(fp_w, "}\n");
        fprintf(fp_w, "\n");

        bRet = PY_TRUE;
    }

    if (fp)
    {
        fclose(fp);
    }

    if (fp_w)
    {
        fclose(fp_w);
    }

    return bRet;
}

bool PYLex_LoadPhrase(int nGroupType, map<PY_UINT32, vector<PHRASE_ITEM> > &mPhraseGroup, vector<PHRASE_ITEM> &vPhraseList, vector<PY_UINT16> &vPhraseData, const char * pTxtFileName, const char * pErrFileName)
{
    int64_t nFreqTotal = 0;
    char szPhrase[256];
    char szPinyin[512];
    int  nFreq;

    PY_UINT16 nHanString[256];

    char szDecodePhrase[256];
    char szDecodePinyin[512];
    PY_UINT16 nDecodePhraseLen = 256;
    PY_UINT16 nDecodePinyinLen = 512;
    PY_HAN stHan;

    PYHan_Initialize(&stHan, PYHan_GetHanData());
    PY_HAN* pHan = &stHan;

    mPhraseGroup.clear();
    vPhraseList.clear();
    vPhraseData.clear();
    int nGroupMaxNum = 0;

    switch (nGroupType)
    {
    case 1:
        nGroupMaxNum = PYHan_GetPinyinGroup1Total(pHan);
        break;
    case 2:
        nGroupMaxNum = PYHan_GetPinyinGroup2Total(pHan);
        break;

    default:
        break;
    }

    // 初始化分组表
    for (int i = 0; i < nGroupMaxNum; i++)
    {
        vector<PHRASE_ITEM> vPhrase;
        mPhraseGroup.insert(make_pair(i, vPhrase));
    }

    FILE* fp = fopen(pTxtFileName, "rb");
    FILE* fpErr = fopen(pErrFileName, "wb");

    int nLineTotal = 0;
    if (fp && fpErr)
    {
        vPhraseData.push_back(0);
        char szLine[1024];
        while (fgets(szLine, 1024, fp) != NULL)
        {
            nLineTotal++;

            if (sscanf(szLine, "%s%s%d", szPhrase, szPinyin, &nFreq) == 3)
            {
                PHRASE_ITEM phraseItem = { 0 };

                PY_UINT16 nLen = PYHan_EncodeUtf8(pHan, nHanString, 256, 
                    (PY_PCSTR)szPhrase, (PY_UINT16) strlen(szPhrase),
                    (PY_PCSTR)szPinyin, (PY_UINT16) strlen(szPinyin), '\'');

                if (nLen > 0)
                {
                    nDecodePhraseLen = 256;
                    nDecodePinyinLen = 512;
                    PYHan_DecodeUtf8(pHan, (PY_UINT16*)nHanString, nLen,
                        (PY_CHAR*) szDecodePhrase, &nDecodePhraseLen, 
                        (PY_CHAR*) szDecodePinyin, &nDecodePinyinLen, '\'');

                    if (strcmp(szPhrase, szDecodePhrase) != 0)
                    {
                        printf("decode err: %s %s %s %s\n", szPhrase, szDecodePhrase, szPinyin, szDecodePinyin);
                        fprintf(fpErr, "decode err: %s %s %s %s\n", szPhrase, szDecodePhrase, szPinyin, szDecodePinyin);
                    }

                    PY_wcsncpy(phraseItem.szPhrase, nHanString, nLen);
                    phraseItem.szPhrase[nLen] = 0;

                    phraseItem.nOffset = (int) vPhraseData.size();
                    phraseItem.nLen = nLen;
                    phraseItem.nFreq = nFreq;
                    nFreqTotal += nFreq;
                    vPhraseList.push_back(phraseItem);

                    for (int i = 0; i < nLen; i++)
                    {
                        vPhraseData.push_back(nHanString[i]);
                    }
                    vPhraseData.push_back(0);
                }
                else
                {
                    fprintf(fpErr, "%d\t%s", nLineTotal, szLine);
                    printf("err: %d\t%s", nLineTotal, szLine);
                }
            }
        }

        fclose(fp);
        fclose(fpErr);

        vPhraseData.pop_back();

        // 计算生成weight 以及 group
        int nGroupId = 0;
        if (nFreqTotal < 100000000)
        {
            nFreqTotal *= 1000;
        }
		int i;

        for (i = 0; i < vPhraseList.size(); i++)
        {
            vPhraseList[i].weight = (int) log(((double)vPhraseList[i].nFreq / nFreqTotal)) * (-1000);
            vPhraseData[vPhraseList[i].nOffset - 1] = vPhraseList[i].weight;

            switch (nGroupType)
            {
            case 1:
                nGroupId = PYHan_GetPinyinGroup1(pHan, (PY_UINT16*)&vPhraseData[vPhraseList[i].nOffset],
                    vPhraseList[i].nLen);
                break;
            case 2:
                nGroupId = PYHan_GetPinyinGroup2(pHan, (PY_UINT16*)&vPhraseData[vPhraseList[i].nOffset],
                    vPhraseList[i].nLen);
                break;

            default:
                nGroupId = 0;
                break;
            }

            vPhraseList[i].group = nGroupId;

        }

        // 词条按照频率排序
        sort(vPhraseList.begin(), vPhraseList.end(), PYLex_FreqSort);

        // 词条写入到分组数据
        for (i = 0; i < vPhraseList.size(); i++)
        {
            mPhraseGroup[vPhraseList[i].group].push_back(vPhraseList[i]);
        }

        return true;
    }

    return false;
}

int PYLex_LoadDataFile(PY_BLOCK* pBlock, const char* pFileName)
{
    pBlock->pAddress = NULL;
    pBlock->nSize = 0;

    FILE* fp = fopen(pFileName, "rb");
    if (fp != NULL)
    {
        fseek(fp, 0L, SEEK_END); /* 定位到文件末尾 */

        int flen = ftell(fp); /* 得到文件大小 */
        pBlock->pAddress = (PY_BYTE*)malloc(flen);
        pBlock->nSize = flen;

        fseek(fp, 0l, SEEK_SET);

        if (pBlock->pAddress)
        {
            fread(pBlock->pAddress, 1, pBlock->nSize, fp);
        }

        fclose(fp);

        return 0;
    }

    return -1;
}

int PYLex_FixLexCreate(int nLexType, int nGroupType, const char* pExtFileName, const char* pTxtFileName, const char* pLexFileName, const char* pErrFileName, const char* pLogFileName, const char* pCppFileName)
{
    int bIncludeSortedPhraseId = 1;
    int bIncludeBigram = 0;

    map<PY_UINT32, vector<PHRASE_ITEM> > mPhraseGroup;
    vector<PHRASE_ITEM> vPhraseList;
    vector<PY_UINT16>   vPhraseData;
    PY_BLOCK stExtData;
    PYLex_LoadDataFile(&stExtData, pExtFileName);
    PYLex_LoadPhrase(nGroupType, mPhraseGroup, vPhraseList, vPhraseData, pTxtFileName, pErrFileName);

    FILE* fpDic = fopen(pLexFileName, "wb");
    if (fpDic != NULL)
    {
        int nBlockTotal = 2;

        if (nGroupType != 0)
        {
            nBlockTotal++;
        }
        
        if (stExtData.nSize != 0)
        {
            nBlockTotal++;
        }

        if (bIncludeSortedPhraseId != 0)
        {
            nBlockTotal++;
        }

        if (bIncludeBigram != 0)
        {
            nBlockTotal+=2;
        }

        int nFileHeadSize = sizeof(PY_FILE_HEAD);
        int nFileBlockSize = sizeof(PY_FILE_BLOCK) * nBlockTotal;
        int nFileDataSize = 0;

        // 1、扩展信息
        if (stExtData.nSize != 0)
        {
            nFileDataSize += stExtData.nSize; 
            nFileDataSize = (nFileDataSize + 3) / 4 * 4;
        }

        // 2、分组信息
        if (nGroupType != 0)
        {
            nFileDataSize += (int) (mPhraseGroup.size() + 1) * sizeof(int32_t);
            nFileDataSize = (nFileDataSize + 3) / 4 * 4;
        }

        // 3、分组词条ID
        nFileDataSize += (int)(vPhraseList.size() * sizeof(int32_t)); // 词条ID
        nFileDataSize = (nFileDataSize + 3) / 4 * 4;

        // 4、词条数据
        nFileDataSize += (int)(vPhraseData.size() * sizeof(short));
        nFileDataSize = (nFileDataSize + 3) / 4 * 4;

        // 5、字典序词条ID
        if (bIncludeSortedPhraseId)
        {
            nFileDataSize += (int)(vPhraseList.size() * sizeof(int32_t)); // 词条ID
            nFileDataSize = (nFileDataSize + 3) / 4 * 4;
        }

        // 6、二元数据
        if (bIncludeBigram)
        {

        }

        time_t t = time(NULL);
        struct tm* p = localtime(&t);
        char szDay[9];
        sprintf(szDay, "%04d%02d%02d", 1900 + p->tm_year, (1 + p->tm_mon), p->tm_mday);

        PY_FILE_HEAD stFileHead;
        if (nLexType == 1)
        {
            stFileHead.nFileMask = PY_FILE_PY_SYSTEM_MASK;
            stFileHead.nVersion = PY_FILE_PY_SYSTEM_VER;
        }
        else if (nLexType == 2)
        {
            stFileHead.nFileMask = PY_FILE_PY_TERM1_MASK;
            stFileHead.nVersion = PY_FILE_PY_TERM1_VER;
        }
        else
        {
            stFileHead.nFileMask = PY_FILE_PY_TERM2_MASK;
            stFileHead.nVersion = PY_FILE_PY_TERM2_VER;
        }

        stFileHead.nFileSize = nFileHeadSize + nFileBlockSize + nFileDataSize;
        stFileHead.nTimeStamp = (szDay[0] - '0') << 4 | (szDay[1] - '0') << 0 | (szDay[2] - '0') << 12 | (szDay[3] - '0') << 8 | (szDay[4] - '0') << 20 | (szDay[5] - '0') << 16 | (szDay[6] - '0') << 28 | (szDay[7] - '0') << 24;

        stFileHead.nBlockTotal = nBlockTotal;
        stFileHead.nBlockOffset = nFileHeadSize;
        stFileHead.nDataOffset = nFileHeadSize + nFileBlockSize;
        stFileHead.nReserved = 0;

        fwrite(&stFileHead, sizeof(PY_FILE_HEAD), 1, fpDic);


        PY_FILE_BLOCK stBlockExtendInfo;
        PY_FILE_BLOCK stBlockPhraseGroup;
        PY_FILE_BLOCK stBlockPhraseId;
        PY_FILE_BLOCK stBlockPhraseData;
        PY_FILE_BLOCK stBlockSortedPhraseId;
        PY_FILE_BLOCK stBlockBigramPreWord;
        PY_FILE_BLOCK stBlockBigramData;

        int nDataOffsetSize = 0;

        if (stExtData.nSize > 0)
        {
            stBlockExtendInfo.nBlockId = PY_FILE_BLOCK_EXTEND_INFO;
            stBlockExtendInfo.nOffset = nDataOffsetSize;
            stBlockExtendInfo.nSize = stExtData.nSize;
            stBlockExtendInfo.nReserved = 0;

            nDataOffsetSize += (stBlockExtendInfo.nSize + 3) / 4 * 4;

            fwrite(&stBlockExtendInfo, sizeof(PY_FILE_BLOCK), 1, fpDic);
        }

        if (nGroupType != 0)
        {
            switch (nGroupType)
            {
            case 1:
                stBlockPhraseGroup.nBlockId = PY_FILE_BLOCK_PINYIN_GROUP_L1;
                break;
            case 2:
                stBlockPhraseGroup.nBlockId = PY_FILE_BLOCK_PINYIN_GROUP_L2;
                break;
            }
                
            stBlockPhraseGroup.nOffset = nDataOffsetSize;
            stBlockPhraseGroup.nSize = (int)((mPhraseGroup.size() + 1) * sizeof(int32_t));
            stBlockPhraseGroup.nReserved = 0;

            nDataOffsetSize += (stBlockPhraseGroup.nSize + 3) / 4 * 4;

            fwrite(&stBlockPhraseGroup, sizeof(PY_FILE_BLOCK), 1, fpDic);
        }


        stBlockPhraseId.nBlockId = PY_FILE_BLOCK_PHRASE_ID;
        stBlockPhraseId.nOffset = nDataOffsetSize;
        stBlockPhraseId.nSize = (int)(vPhraseList.size() * sizeof(int32_t));
        stBlockPhraseId.nReserved = 0;
        fwrite(&stBlockPhraseId, sizeof(PY_FILE_BLOCK), 1, fpDic);

        nDataOffsetSize += (stBlockPhraseId.nSize + 3) / 4 * 4;

        if (bIncludeSortedPhraseId)
        {
            stBlockSortedPhraseId.nBlockId = PY_FILE_BLOCK_SORTED_PHRASE_ID;
            stBlockSortedPhraseId.nOffset = nDataOffsetSize;
            stBlockSortedPhraseId.nSize = (int)(vPhraseList.size() * sizeof(int32_t));
            stBlockSortedPhraseId.nReserved = 0;
            fwrite(&stBlockSortedPhraseId, sizeof(PY_FILE_BLOCK), 1, fpDic);
            nDataOffsetSize += (stBlockSortedPhraseId.nSize + 3) / 4 * 4;
        }

        stBlockPhraseData.nBlockId = PY_FILE_BLOCK_PHRASE_DATA;
        stBlockPhraseData.nOffset = nDataOffsetSize;
        stBlockPhraseData.nSize = (int)(vPhraseData.size() * sizeof(short));
        stBlockPhraseData.nReserved = 0;
        fwrite(&stBlockPhraseData, sizeof(PY_FILE_BLOCK), 1, fpDic);

        nDataOffsetSize += stBlockPhraseData.nSize;



        // 4字节对齐填充数据
        int nFillLen = 0;
        char szFillBuf[4] = {0};
        memset(szFillBuf, 0, 4);

        // 1、扩展信息，需要4字节对齐

        fwrite(stExtData.pAddress, sizeof(char), stExtData.nSize, fpDic);
        nFillLen = (stExtData.nSize + 3) / 4 * 4 - stExtData.nSize;
        if (nFillLen > 0)
        {
            fwrite(szFillBuf, sizeof(char), nFillLen, fpDic);
        }

        
        // 2、写入分组数据，不需要4字节对齐
        
        int nGroupOffset = 0;
        int nGroupTotal = (int) mPhraseGroup.size();

        fwrite(&nGroupOffset, sizeof(int32_t), 1, fpDic); // 起始数据为0 [Begin, End）

        for (int i = 0; i < nGroupTotal; i++) 
        {
            nGroupOffset += (int) mPhraseGroup[i].size();
            fwrite(&nGroupOffset, sizeof(int32_t), 1, fpDic);
        }

        // 3、写入分组词条ID，不需要4字节对齐

        for (int i = 0; i < nGroupTotal; i++) 
        {
            for (int j = 0; j < mPhraseGroup[i].size(); ++j)
            {
                unsigned int id = mPhraseGroup[i][j].getId();
                fwrite(&id, sizeof(int32_t), 1, fpDic);
            }
        }

        // 4、写入字典序词条ID，不需要4字节对齐
        sort(vPhraseList.begin(), vPhraseList.end(), PYLex_AscSort);

        for (int i = 0; i < vPhraseList.size(); i++)
        {
            unsigned int id = vPhraseList[i].getId();
            fwrite(&id, sizeof(int32_t), 1, fpDic);
        }

        // 5、写入词条数据，需要4字节对齐

        fwrite(&vPhraseData[0], sizeof(short), vPhraseData.size(), fpDic);
        nFillLen = (int)((vPhraseData.size() * sizeof(short) + 3) / 4 * 4 - vPhraseData.size() * sizeof(short));
        if (nFillLen > 0)
        {
            fwrite(szFillBuf, sizeof(char), nFillLen, fpDic);
        }

        fclose(fpDic);

        if (pCppFileName != PY_NULL)
        {
            PYLex_SaveToCpp(pLexFileName, pCppFileName);
        }

        // 写入字典序词条数据


        return 0;
    }
    
    return -1;
}


int PYLex_LexMaker(int nLexType, const char* pTxtFileName, const char* pLexFileName, const char* pCppFileName)
{
    char szInfFileName[260];
    char szLogFileName[260];
    char szErrFileName[260];

    sprintf(szInfFileName, "%s.inf", pTxtFileName);
    sprintf(szLogFileName, "%s.log", pLexFileName);
    sprintf(szErrFileName, "%s.err", pLexFileName);

    if (nLexType == 1)
    {
        return PYLex_FixLexCreate(nLexType, 2, szInfFileName, pTxtFileName, pLexFileName, szErrFileName, szLogFileName, pCppFileName);
    }
    else if (nLexType == 2)
    {
        return PYLex_FixLexCreate(nLexType, 1, szInfFileName, pTxtFileName, pLexFileName, szErrFileName, szLogFileName, pCppFileName);
    }
    else if (nLexType == 3)
    {
        return PYLex_FixLexCreate(nLexType, 2, szInfFileName, pTxtFileName, pLexFileName, szErrFileName, szLogFileName, pCppFileName);
    }

    return 0;
}
