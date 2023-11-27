#include <stdio.h>
#include "PYHan.h"
#include "PYLex.h"


PY_BOOL PYLex_Initialize(PY_LEX *pLex, PY_HAN *pHan, const PY_BLOCK *pBlock)
{
    PY_UINT32 i;

    PY_BYTE  *pBaseAddress = pBlock->pAddress;
    PY_FILE_HEAD *pHead = (PY_FILE_HEAD *) pBlock->pAddress;
    PY_FILE_BLOCK  *pBlockItem = (PY_FILE_BLOCK *) &pBaseAddress[pHead->nBlockOffset];
    PY_BYTE  *pDataAddress = &pBaseAddress[pHead->nDataOffset];

    PY_memset(&pLex->stData, 0, sizeof(PY_LEX_DATA));

    for (i = 0; i < pHead->nBlockTotal; ++i)
    {
        if (pBlockItem[i].nBlockId == PY_FILE_BLOCK_PHRASE_DATA)
        {
            pLex->stData.pPhraseData = (PY_UINT16 *) &pDataAddress[pBlockItem[i].nOffset];
            pLex->stData.pPhraseWeight = pLex->stData.pPhraseData - 1;
        }
        else if (pBlockItem[i].nBlockId == PY_FILE_BLOCK_PINYIN_GROUP_L1)
        {
            pLex->stData.nGroupType = PY_FILE_BLOCK_PINYIN_GROUP_L1;
            pLex->stData.pPyGroupHead = (PY_UINT32 *) &pDataAddress[pBlockItem[i].nOffset];
        }
        else if (pBlockItem[i].nBlockId == PY_FILE_BLOCK_PINYIN_GROUP_L2)
        {
            pLex->stData.nGroupType = PY_FILE_BLOCK_PINYIN_GROUP_L2;
            pLex->stData.pPyGroupHead = (PY_UINT32 *) &pDataAddress[pBlockItem[i].nOffset];
        }
        else if (pBlockItem[i].nBlockId == PY_FILE_BLOCK_SORTED_PHRASE_ID)
        {
            pLex->stData.nSortedPhraseTotal = pBlockItem[i].nSize / sizeof(PY_UINT32);
            pLex->stData.pSortedPhraseId = (PY_UINT32*)&pDataAddress[pBlockItem[i].nOffset];
        }
        else if (pBlockItem[i].nBlockId == PY_FILE_BLOCK_PHRASE_ID)
        {
            pLex->stData.pPhraseId = (PY_UINT32 *) &pDataAddress[pBlockItem[i].nOffset];
            pLex->stData.nPhraseTotal = pBlockItem[i].nSize / sizeof(PY_UINT32);
        }
    }

    pLex->pHan = pHan;

    return PY_TRUE;
}


PY_VOID PYLex_Terminate(PY_LEX *pLex)
{
    PY_UNREFERENCED_PARAMETER(pLex);
}

PY_UINT32 PYLex_GetSortedTotal(PY_LEX* pLex)
{
    return pLex->stData.nSortedPhraseTotal;
}

PY_UINT32 PYLex_GetSortedItemPhraseId(PY_LEX* pLex, PY_INT32 nIndex)
{
    return pLex->stData.pSortedPhraseId[nIndex];
}

PY_UINT32 PYLex_GetTotal(PY_LEX *pLex)
{
    return pLex->stData.nPhraseTotal;
}

PY_UINT32 PYLex_GetItemPhraseId(PY_LEX* pLex, PY_INT32 nIndex)
{
    return pLex->stData.pPhraseId[nIndex];
}

PY_UINT16 PYLex_GetItemWeight(PY_LEX *pLex, PY_INT32 nIndex)
{
    return pLex->stData.pPhraseWeight[pLex->stData.pPhraseId[nIndex] & 0x00FFFFFF];
}

PY_UINT8 PYLex_GetItemLength(PY_LEX *pLex, PY_INT32 nIndex)
{
    return (pLex->stData.pPhraseId[nIndex] & 0xFF000000) >> 24;
}

PY_UINT16 * PYLex_GetItemDataPtr(PY_LEX *pLex, PY_INT32 nIndex)
{
    return &pLex->stData.pPhraseData[pLex->stData.pPhraseId[nIndex] & 0x00FFFFFF];
}

PY_UINT32 PYLex_GetGroupTotal(PY_LEX* pLex)
{   
    if (pLex->stData.nGroupType == PY_FILE_BLOCK_PINYIN_GROUP_L2 && pLex->stData.pPyGroupHead != PY_NULL)
    {
        return PYHan_GetPinyinGroup2Total(pLex->pHan);
    }
    else if (pLex->stData.nGroupType == PY_FILE_BLOCK_PINYIN_GROUP_L1 && pLex->stData.pPyGroupHead != PY_NULL)
    {
        return PYHan_GetPinyinTotal(pLex->pHan);
    }
    
    return 0;
}

//inline PY_UINT32 PYLex_GetGroupItemTotal(PY_LEX* pLex, PY_INT32 nGroupId)
//{
//   return pLex->stData.pPyGroupHead[nGroupId + 1] - pLex->stData.pPyGroupHead[nGroupId];
//}

//inline PY_UINT32 PYLex_GetGroupItemPhraseId(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex)
//{
//   return pLex->stData.pPhraseId[pLex->stData.pPyGroupHead[nGroupId] + nIndex];
//}

//inline PY_UINT16 PYLex_GetGroupItemWeight(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex)
//{
//   PY_UINT32 nPhraseId = pLex->stData.pPhraseId[pLex->stData.pPyGroupHead[nGroupId] + nIndex];
//   return pLex->stData.pPhraseWeight[nPhraseId & 0x00FFFFFF];
//}

//inline PY_UINT8 PYLex_GetGroupItemLength(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex)
//{
//   PY_UINT32 nPhraseId = pLex->stData.pPhraseId[pLex->stData.pPyGroupHead[nGroupId] + nIndex];
//   return nPhraseId >> 24;
//}

//inline PY_UINT16* PYLex_GetGroupItemDataPtr(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex)
//{
//   PY_UINT32 nPhraseId = pLex->stData.pPhraseId[pLex->stData.pPyGroupHead[nGroupId] + nIndex];
//   return &pLex->stData.pPhraseData[nPhraseId & 0x00FFFFFF];
//}

PY_UINT16* PYLex_GetPhraseDataPtr(PY_LEX* pLex, PY_UINT32 nPhraseId)
{
    return &pLex->stData.pPhraseData[nPhraseId & 0x00FFFFFF];
}

PY_UINT16  PYLex_GetPhraseWeight(PY_LEX* pLex, PY_UINT32 nPhraseId)
{
    return pLex->stData.pPhraseWeight[nPhraseId & 0x00FFFFFF];
}

PY_UINT8   PYLex_GetPhraseLength(PY_LEX *pLex, PY_UINT32 nPhraseId)
{
    return nPhraseId >> 24;
}

PY_PCSTR PYLex_GetPhraseDescribe(PY_LEX* pLex, PY_UINT32 nPhraseId)
{
    static char szLex[256];

    sprintf(szLex, "%s", 
        PYHan_GetHanDescribe(pLex->pHan, 
            PYLex_GetPhraseDataPtr(pLex, nPhraseId),
            PYLex_GetPhraseLength(pLex, nPhraseId)));

    return szLex;
}

int PYLex_SortedPhraseCompare(const PY_UINT16* pHanString, const PY_UINT32 * pPhraseId, PY_LEX* pLex)
{
    return PY_wcscmp(pHanString, PYLex_GetPhraseDataPtr(pLex, *pPhraseId));
}

PY_UINT32 PYLex_GetSortedPhraseIdBeginPos(PY_LEX* pLex, PY_UINT16* pHanString, PY_UINT16 nHanLen)
{
    return PY_binsearchmin(pHanString, pLex->stData.pSortedPhraseId, pLex->stData.nSortedPhraseTotal, sizeof(PY_UINT32), PYLex_SortedPhraseCompare, pLex);

}