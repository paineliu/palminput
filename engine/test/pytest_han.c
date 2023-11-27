#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "PYHanData.h"
#include "PYHan.h"

int test_han()
{
    int i;
    PY_HAN stHan;
    PYHan_Initialize(&stHan, PYHan_GetHanData());

    printf("pinyin maxlen = %d\n", PYHan_GetPinyinMaxLen(&stHan));

    for (i=0; i<PYHan_GetPinyinTotal(&stHan); i++)
    {
        printf("pinyin item (%d), str = %s, len = %d id = %d\n", i, PYHan_GetPinyinString(&stHan, i),
               PYHan_GetPinyinStringLen(&stHan, i), PYHan_GetPinyinId(&stHan, PYHan_GetPinyinString(&stHan, i), PYHan_GetPinyinStringLen(&stHan, i)));
    }

    int hanTotal = PYHan_GetHanTotal(&stHan);
    printf("han total = %d\n", hanTotal);
    char szUtf8[4];

    for (i = 0; i < hanTotal; ++i) {
        memset(szUtf8, 0, 4);
        PYHan_GetHanUtf8(&stHan, PYHan_GetHanUnicode(&stHan, i), (PY_CHAR *)szUtf8, 4);
        printf("%d %s %s\n", i, szUtf8, PYHan_GetPinyinString(&stHan, PYHan_GetHanPinyinId(&stHan, i)));
    }

    PY_WCHAR szUnicode[] = {0x4e00, 0x4e00, 0x0000};
    char  szPinyin[] = "yi'yi";
    PY_WCHAR wszPinyin[64];
    for (i = 0; i<=strlen(szPinyin); i++)
    {
        wszPinyin[i] = szPinyin[i];
    }

    PY_UINT16 szHancode[4];

    PYHan_Encode(&stHan, szHancode, 4, szUnicode, 2, wszPinyin, strlen(szPinyin), '\'');

    char szDUtf8[64] = "";
    char szDPinyin[64] = "";
    PY_UINT16 nUtf8Len = 64;
    PY_UINT16 nPinyinLen = 64;

    PYHan_DecodeUtf8(&stHan, szHancode, 2, (PY_PSTR) szDUtf8, &nUtf8Len, (PY_PSTR) szDPinyin, &nPinyinLen, '\'');
    printf("%s, %s\n", szDUtf8, szDPinyin);

    int nSegmentTotal = PYHan_GetNodeSegmentTotal(&stHan);
    for (i=0; i<nSegmentTotal; i++)
    {
        const PY_RANGE16 * pRange = PYHan_GetNodeSegmentRange(&stHan, i);
        printf("encode(len = %d) range = %d, %d\n", i, pRange->nBegin, pRange->nEnd);
        for(int j=pRange->nBegin; j<pRange->nEnd; j++)
        {
            const PY_NODE_ITEM *  pNodeItem = PYHan_GetNodeItem(&stHan, j);
            printf("encode(%d) = %s len = %d id = %d\n", j, PYHan_GetNodeString(&stHan, j), PYHan_GetNodeStringLen(&stHan, j),
                    PYHan_GetNodeId(&stHan, PYHan_GetNodeString(&stHan, j), PYHan_GetNodeStringLen(&stHan, j)));
            const PY_RANGE16 *pPinyinRange = PYHan_GetNodePinyinRange(&stHan, j);
            printf("encode(%d) = begin = %d end = %d\n", j, pPinyinRange->nBegin, pPinyinRange->nEnd);
            for(int k=pPinyinRange->nBegin; k<pPinyinRange->nEnd; k++)
            {
                printf("%d %d\n", k, PYHan_GetNodePinyinItem(&stHan, k)->nPinyinId);
            }
        }
    }

    for (i=0; i<stHan.pData->nNodePinyinTotal; i++)
    {
        printf("%d %d\n", i, stHan.nNodeEnabled[i]);
    }


    PYHan_Terminate(&stHan);

    return 0;
}
