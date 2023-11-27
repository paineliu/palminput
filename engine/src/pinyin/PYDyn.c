#include <stdio.h>
#include "PYDyn.h"

PY_BOOL PYDyn_Initialize(PY_DYN *pDyn, PY_HAN *pHan, const PY_BLOCK *pBlock)
{
    PY_BYTE  *pBaseAddress = pBlock->pAddress;
    PY_UINT32 i;
    PY_FILE_HEAD *pHead = (PY_FILE_HEAD *) pBlock->pAddress;
    PY_FILE_BLOCK  *pBlockItem = (PY_FILE_BLOCK *) &pBaseAddress[pHead->nBlockOffset];
    PY_BYTE  *pDataAddress = &pBaseAddress[pHead->nDataOffset];

    PY_memset(&pDyn->stData, 0, sizeof(PY_DYN_DATA));

    for (i = 0; i < pHead->nBlockTotal; ++i)
    {
        if (pBlockItem[i].nBlockId == PY_FILE_BLOCK_DYN_SUMMARY)
        {
            pDyn->stData.pSummary = (PY_DYN_SUMMARY *) &pDataAddress[pBlockItem[i].nOffset];
        }
        else if (pBlockItem[i].nBlockId == PY_FILE_BLOCK_DYN_GROUP)
        {
            pDyn->stData.pGroup = (PY_DYN_GROUP *)&pDataAddress[pBlockItem[i].nOffset];
        }
        else if (pBlockItem[i].nBlockId == PY_FILE_BLOCK_DYN_BUFFER)
        {
            pDyn->stData.pBuffer = (PY_BYTE*)&pDataAddress[pBlockItem[i].nOffset];
            pDyn->stData.pPhraseItem = (PY_DYN_PHRASE*)&pDataAddress[pBlockItem[i].nOffset];
            pDyn->stData.pPhraseData = (PY_UINT16*)&pDataAddress[pBlockItem[i].nOffset];
        }
    }

    pDyn->pHan = pHan;

    return PY_TRUE;
}

PY_BOOL PYDyn_IsInit(PY_DYN *pDyn)
{
    return pDyn->stData.pSummary != PY_NULL;
}

PY_VOID PYDyn_Terminate(PY_DYN *pDyn)
{
    PY_UNREFERENCED_PARAMETER(pDyn);
}

PY_UINT32  PYDyn_AddPhrase(PY_DYN* pDyn, PY_UINT16* pPhraseString, PY_UINT16 nPhraseLen, PY_UINT32 nPhraseFreq)
{
    PY_UINT32 nPhraseId = PYDyn_GetPhraseId(pDyn, pPhraseString, nPhraseLen);
    if (nPhraseId != 0)
    {
        PYDyn_IncPhrase(pDyn, nPhraseId);
        return nPhraseId;
    }

    PY_UINT32 nPhraseTotal = pDyn->stData.pSummary->nPhraseTotal;
    PY_UINT32 nGroupId = PYHan_GetPinyinGroup1(pDyn->pHan, pPhraseString, nPhraseLen);
    
    if (nPhraseTotal < pDyn->stData.pSummary->nPhraseCapacity)
    {
        PY_UINT32 nPhraseId = (nPhraseLen << 24) | nPhraseTotal;

        pDyn->stData.pSummary->nFreqTotal += nPhraseFreq;

        pDyn->stData.pPhraseItem[nPhraseTotal].nFreq = nPhraseFreq;
        pDyn->stData.pPhraseItem[nPhraseTotal].nDel = PY_FALSE;
        pDyn->stData.pPhraseItem[nPhraseTotal].nLastStamp = nPhraseTotal;
        pDyn->stData.pPhraseItem[nPhraseTotal].nReserved = 0;
        pDyn->stData.pPhraseItem[nPhraseTotal].nLength = nPhraseLen;
        pDyn->stData.pPhraseItem[nPhraseTotal].nNext = 0;

        PY_wcsncpy(pDyn->stData.pPhraseItem[nPhraseTotal].nPhraseString, pPhraseString, nPhraseLen);

        if (pDyn->stData.pGroup[nGroupId].nFirst == 0x00000000)
        {
            pDyn->stData.pGroup[nGroupId].nFirst = nPhraseId;
            pDyn->stData.pGroup[nGroupId].nLast = nPhraseId;
        }
        else
        {
            pDyn->stData.pPhraseItem[pDyn->stData.pGroup[nGroupId].nLast & 0x00FFFFFF].nNext = nPhraseId;
            pDyn->stData.pGroup[nGroupId].nLast = nPhraseId;
        }

        pDyn->stData.pSummary->nPhraseTotal++;
    }
    else
    {
        PY_UINT32 nPhraseId = pDyn->stData.pGroup[nGroupId].nFirst;
        PY_DYN_PHRASE* pDelPhrase = PY_NULL;
        PY_DYN_PHRASE* pCurPhrase = PY_NULL;

        if (nPhraseId != 0)
        {
            pDelPhrase = PYDyn_GetPhrasePtr(pDyn, nPhraseId);
            if (pDelPhrase->nDel == 0)
            {
                nPhraseId = PYDyn_GetNextPhraseId(pDyn, nPhraseId);

                while (nPhraseId != 0)
                {
                    pCurPhrase = PYDyn_GetPhrasePtr(pDyn, nPhraseId);
                    if (pCurPhrase->nDel != 0)
                    {
                        pDelPhrase = pCurPhrase;
                        break;
                    }
                    else
                    {
                        if (pDelPhrase->nFreq > pCurPhrase->nFreq)
                        {
                            pDelPhrase = pCurPhrase;
                        }
                        else
                        {
                            if (pDelPhrase->nFreq == pCurPhrase->nFreq &&
                                pDelPhrase->nLastStamp > pCurPhrase->nLastStamp)
                            {
                                pDelPhrase = pCurPhrase;
                            }
                        }
                    }

                    nPhraseId = PYDyn_GetNextPhraseId(pDyn, nPhraseId);
                }
            }
            
            if (pDelPhrase != PY_NULL)
            {
                pDelPhrase->nFreq = nPhraseFreq;
                pDelPhrase->nDel = 0;
                pDelPhrase->nLastStamp = nPhraseTotal;
                pDelPhrase->nReserved = 0;
                pDelPhrase->nLength = nPhraseLen;

                PY_wcsncpy(pDelPhrase->nPhraseString, pPhraseString, nPhraseLen);
            }
        }

    }

    return pDyn->stData.pSummary->nPhraseTotal;
}

PY_UINT32  PYDyn_IncPhrase(PY_DYN* pDyn, PY_UINT32 nPhraseId)
{
    pDyn->stData.pPhraseItem[nPhraseId & 0xffffff].nFreq++;
    pDyn->stData.pSummary->nFreqTotal++;

    return pDyn->stData.pPhraseItem[nPhraseId & 0xffffff].nFreq;
}

PY_UINT32  PYDyn_GetGroupTotal(PY_DYN* pDyn)
{
    return PYHan_GetPinyinGroup1Total(pDyn->pHan);
}

PY_UINT32  PYDyn_GetFirstPhraseId(PY_DYN* pDyn, PY_UINT32 nGroupId)
{
    PY_UINT32 nPhraseId = pDyn->stData.pGroup[nGroupId].nFirst;

    while (nPhraseId != 0)
    {
        if (!pDyn->stData.pPhraseItem[nPhraseId & 0xffffff].nDel)
        {
            return nPhraseId;
        }

        nPhraseId = PYDyn_GetNextPhraseId(pDyn, nPhraseId);
    }

    return 0;
}

PY_UINT32  PYDyn_GetNextPhraseId(PY_DYN* pDyn, PY_UINT32 nPhraseId)
{
    PY_UINT32 nNextPhraseId = pDyn->stData.pPhraseItem[nPhraseId & 0xffffff].nNext;

    while (nNextPhraseId != 0)
    {
        if (!pDyn->stData.pPhraseItem[nNextPhraseId & 0xffffff].nDel)
        {
            return nNextPhraseId;
        }

        nNextPhraseId = PYDyn_GetNextPhraseId(pDyn, nNextPhraseId);
    }

    return 0;
}

PY_BOOL  PYDyn_DelPhrase(PY_DYN* pDyn, PY_UINT32 nPhraseId)
{
    if (!pDyn->stData.pPhraseItem[nPhraseId & 0xffffff].nDel)
    {
        pDyn->stData.pPhraseItem[nPhraseId & 0xffffff].nDel = PY_TRUE;

        return PY_TRUE;
    }

    return PY_FALSE;
}

PY_UINT32 PYDyn_GetPhraseId(PY_DYN* pDyn, PY_UINT16* pPhraseString, PY_UINT16 nPhraseLen)
{
    PY_UINT32 nGroupId = PYHan_GetPinyinGroup1(pDyn->pHan, pPhraseString, nPhraseLen);
    PY_UINT32 nPhraseId = pDyn->stData.pGroup[nGroupId].nFirst;
    while (nPhraseId != 0)
    {
        if (pDyn->stData.pPhraseItem[nPhraseId & 0xffffff].nLength == nPhraseLen && !pDyn->stData.pPhraseItem[nPhraseId & 0xffffff].nDel)
        {
            if (PY_wcsncmp(pPhraseString, pDyn->stData.pPhraseItem[nPhraseId & 0xffffff].nPhraseString, nPhraseLen) == 0)
            {
                return nPhraseId;
            }
        }

        nPhraseId = PYDyn_GetNextPhraseId(pDyn, nPhraseId);
    }

    return 0;
}

PY_UINT32 PYDyn_GetPhraseTotal(PY_DYN *pDyn)
{
    if (pDyn->stData.pSummary)
    {
        return pDyn->stData.pSummary->nPhraseTotal;
    }
    
    return 0;
}

PY_UINT16* PYDyn_GetPhraseDataPtr(PY_DYN* pDyn, PY_UINT32 nPhraseId)
{
    return pDyn->stData.pPhraseItem[nPhraseId & 0xffffff].nPhraseString;
}

PY_UINT16  PYDyn_GetPhraseWeight(PY_DYN* pDyn, PY_UINT32 nPhraseId)
{
    return (PY_UINT16)(- PY_log10((double)pDyn->stData.pPhraseItem[nPhraseId & 0xffffff].nFreq / pDyn->stData.pSummary->nFreqTotal) * 1000);
}

PY_UINT8   PYDyn_GetPhraseLength(PY_DYN* pDyn, PY_UINT32 nPhraseId)
{
    return pDyn->stData.pPhraseItem[nPhraseId & 0xffffff].nLength;
}

PY_DYN_PHRASE*  PYDyn_GetPhrasePtr(PY_DYN* pDyn, PY_UINT32 nPhraseId)
{
    return &pDyn->stData.pPhraseItem[nPhraseId & 0xffffff];
}

PY_PCSTR PYDyn_GetPhraseDescribe(PY_DYN* pDyn, PY_UINT32 nPhraseId)
{
    static char szLex[256];
    sprintf(szLex, "%s", 
        PYHan_GetHanDescribe(pDyn->pHan, 
            PYDyn_GetPhraseDataPtr(pDyn, nPhraseId),
            PYDyn_GetPhraseLength(pDyn, nPhraseId)));

    return szLex;
}

