#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "PYHan.h"
#include "PYLex.h"

char * get_lex_string(PY_HAN *pHan, PY_UINT16 *pHanString, PY_UINT8 nHanLen)
{
    static char szLex[256];

    char szDUtf8[64] = "";
    char szDPinyin[256] = "";
    PY_UINT16 nUtf8Len = 64;
    PY_UINT16 nPinyinLen = 256;

    PYHan_DecodeUtf8(pHan, pHanString, nHanLen, (PY_PSTR) szDUtf8, &nUtf8Len, (PY_PSTR) szDPinyin, &nPinyinLen, '\'');
    sprintf(szLex, "%s\t%s", szDUtf8, szDPinyin);
    return szLex;
}

int test_lex_reader()
{
    PY_HAN stHan;
    PYHan_Initialize(&stHan, PYHan_GetHanData());

    PY_BLOCK stBlock;
    FILE *fp = fopen("../../data/phrase/py_sys.lex", "rb");
    FILE *fpOut = fopen("../../data/phrase/py_sys.lex.out", "wb");
    if (fp == NULL || fpOut == NULL)
    {
        if (fp)
        {
            fclose(fp);
        }
        if (fpOut)
        {
            fclose(fpOut);
        }

        return -1;
    }

    fseek(fp,0L,SEEK_END); /* 定位到文件末尾 */

    int flen = ftell(fp); /* 得到文件大小 */
    stBlock.pAddress = (PY_BYTE *) malloc(flen);
    stBlock.nSize = flen;

    fseek(fp, 0l, SEEK_SET);

    fread(stBlock.pAddress, 1, stBlock.nSize, fp);
    fclose(fp);
    
    PY_LEX stLex;
    PYLex_Initialize(&stLex, &stHan, &stBlock);

    PY_UINT32 nGroupTotal = PYLex_GetGroupTotal(&stLex);

    for (int i = 0; i < nGroupTotal; i++)
    {
        int nItemTotal = PYLex_GetGroupItemTotal(&stLex, i);
        for (int j = 0; j < nItemTotal; j++)
        {
            printf("%d,(%d/%d) 0x%08x, %s\t%d\n", i, j, nItemTotal,
                PYLex_GetGroupItemPhraseId(&stLex, i, j),
                get_lex_string(&stHan,
                PYLex_GetGroupItemDataPtr(&stLex, i, j),
                PYLex_GetGroupItemLength(&stLex, i, j)),
                PYLex_GetGroupItemWeight(&stLex, i, j));
            if (j >= 3)
            {
                break;
            }
        }
    }

    PY_UINT32 nTotal = PYLex_GetTotal(&stLex);

    for(int i=0; i<nTotal; i++)
    {
        fprintf(fpOut, "%s\t%d\n", get_lex_string(&stHan,
                PYLex_GetItemDataPtr(&stLex, i), PYLex_GetItemLength(&stLex, i)), PYLex_GetItemWeight(&stLex, i));
        printf("%s\t%d\n", get_lex_string(&stHan,
            PYLex_GetItemDataPtr(&stLex, i), PYLex_GetItemLength(&stLex, i)), PYLex_GetItemWeight(&stLex, i));
        if (i > 1000)
        {
            break;
        }
    }

    PY_UINT32 nSortTotal = PYLex_GetSortedTotal(&stLex);

    for (int i = 0; i < nSortTotal; i++)
    {
        PY_UINT32 nPhraseId = PYLex_GetSortedItemPhraseId(&stLex, i);

        fprintf(fpOut, "%s\t%d\n", get_lex_string(&stHan,
            PYLex_GetPhraseDataPtr(&stLex, nPhraseId), PYLex_GetPhraseLength(&stLex, nPhraseId)), PYLex_GetPhraseWeight(&stLex, nPhraseId));
        printf("%s\t%d\n", get_lex_string(&stHan,
            PYLex_GetPhraseDataPtr(&stLex, nPhraseId), PYLex_GetPhraseLength(&stLex, nPhraseId)), PYLex_GetPhraseWeight(&stLex, nPhraseId));
        if (i > 1000)
        {
            break;
        }
    }

    fclose(fpOut);
    return 0;
}
