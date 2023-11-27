#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "PYHan.h"
#include "PYDyn.h"


static char * get_lex_string(PY_HAN *pHan, PY_UINT16 *pHanString, PY_UINT8 nHanLen)
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

int test_dyn_reader()
{

    PYDyn_Create((PY_PCSTR)"../../../Test/Data/user.lex", 1024 * 1024);

    PY_HAN stHan;
    PYHan_Initialize(&stHan, PYHan_GetHanData());

    PY_BLOCK stBlock;
    FILE *fp = fopen("../../../Test/Data/user.lex", "rb");
    FILE *fpOut = fopen("../../../Test/Data/user_out.log", "wb");
    fseek(fp,0L,SEEK_END);

    int flen = ftell(fp);
    stBlock.pAddress = (PY_BYTE *) malloc(flen);
    stBlock.nSize = flen;

    fseek(fp, 0l, SEEK_SET);

    fread(stBlock.pAddress, 1, stBlock.nSize, fp);
    fclose(fp);
    PY_DYN stDyn;
    PYDyn_Initialize(&stDyn, &stHan, &stBlock);
    FILE *fp_in = fopen("../../../Test/Data/user_lex.txt", "rb");
    if (fp_in)
    {
        char szLine[1024];
        int nLineTotal = 0;
        char szPhrase[256];
        char szPinyin[512];
        int  nFreq;

        PY_UINT16 nHanString[256];

        char szDecodePhrase[256];
        char szDecodePinyin[512];
        PY_UINT16 nDecodePhraseLen = 256;
        PY_UINT16 nDecodePinyinLen = 512;

        while (fgets(szLine, 1024, fp) != NULL)
        {
            nLineTotal++;

            if (sscanf(szLine, "%s%s%d", szPhrase, szPinyin, &nFreq) == 3)
            {
                int nLen = PYHan_EncodeUtf8(&stHan, nHanString, 256,
                    (PY_PCSTR)szPhrase, (PY_UINT16) strlen(szPhrase),
                    (PY_PCSTR)szPinyin, (PY_UINT16) strlen(szPinyin), '\'');

                if (nLen > 0)
                {
                    nDecodePhraseLen = 256;
                    nDecodePinyinLen = 512;
                    PYHan_DecodeUtf8(&stHan, (PY_UINT16*)nHanString, nLen,
                        (PY_CHAR*)szDecodePhrase, &nDecodePhraseLen,
                        (PY_CHAR*)szDecodePinyin, &nDecodePinyinLen, '\'');

                    if (strcmp(szPhrase, szDecodePhrase) != 0)
                    {
                        printf("decode err: %s %s %s %s\n", szPhrase, szDecodePhrase, szPinyin, szDecodePinyin);
                    }
                    else
                    {
                        PYDyn_AddPhrase(&stDyn, nHanString, nLen, 1);
                    }

                }
                else
                {
                    printf("err: %d\t%s", nLineTotal, szLine);
                }
            }
        }

        fclose(fp);

    }
    PY_UINT32 nGroupTotal = PYDyn_GetGroupTotal(&stDyn);

    for (int i = 0; i < nGroupTotal; i++)
    {
       // PY_UINT32 nPhraseId = PYDyn_GetFirstPhraseId(&stDyn, i);
       /* while (nPhraseId != 0)
        {
            printf("%d, 0x%08x, %s\t0x%08x\n", i, nPhraseId,
                get_lex_string(&stHan,
                    PYDyn_GetPhraseDataPtr(&stDyn, nPhraseId), PYDyn_GetPhraseLength(&stDyn, nPhraseId)), PYDyn_GetPhrasePtr(&stDyn, nPhraseId)->nNextItem);

            nPhraseId = PYDyn_GetNextPhraseId(&stDyn, nPhraseId);
        }*/
    //    for (int j = 0; j < nItemTotal; j++)
    //    {
    //        printf("%d,(%d/%d) 0x%08x, %s\t%d\n", i, j, nItemTotal,
    //            PYLex_GetGroupItemPhraseId(&stDyn, i, j),
    //            get_lex_string(&stHan,
    //            PYLex_GetGroupItemDataPtr(&stDyn, i, j),
    //            PYLex_GetGroupItemLength(&stDyn, i, j)),
    //            PYLex_GetGroupItemWeight(&stDyn, i, j));
    //        if (j >= 3)
    //        {
    //            break;
    //        }
    //    }
    }

    PY_UINT32 nTotal = PYDyn_GetPhraseTotal(&stDyn);

    for(int i=0; i<nTotal; i++)
    {
        printf("0x%08x, %s\t0x%08x\t%d\n", PYDyn_GetPhrasePtr(&stDyn, i)->nLength << 24 | i,
            get_lex_string(&stHan,
            PYDyn_GetPhraseDataPtr(&stDyn, i), PYDyn_GetPhraseLength(&stDyn, i)), PYDyn_GetPhrasePtr(&stDyn, i)->nNext, PYDyn_GetPhraseWeight(&stDyn,  i));
    }

    fclose(fpOut);

    PYDyn_SaveLex(&stBlock, "../../../Test/Data/user.lex");
    PYDyn_DumpFile("../../../Test/Data/user.lex", "../../../Test/Data/user.log");

    return 0;
}
