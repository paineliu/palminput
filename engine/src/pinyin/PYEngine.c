#include <stdio.h>
#include <stdlib.h>
#include "PYHan.h"
#include "PYGraph.h"
#include "PYCand.h"
#include "PYKey.h"
#include "PYDyn.h"
#include "PYPredict.h"


#include "PYEngine.h"

#define PY_INPUT_MODE_PINYIN  1
#define PY_INPUT_MODE_PREDICT 2

typedef struct _PY_DEV_OPT
{
	PY_UINT32    nLogLevel;
	PY_UINT32    bNeedPrepareCand;
} PY_DEV_OPT;

typedef struct _PY_ENGINE
{
	PY_UINT16 szKeys[PY_INPUT_MAX_LEN];
	PY_UINT16 nInsPos;
	PY_UINT16 nKeyLen;
	PY_UINT16 nTerm1LexTotal;
	PY_UINT16 nTerm2LexTotal;
	PY_OPTION stOption;
	PY_DEV_OPT stDevOpt;
	PY_HAN stHan;
	PY_GRAPH stGraph;
	PY_CAND stCand;
	PY_PREDICT stPredict;
	PY_LEX stCoreLex;
	PY_LEX stTerm1Lex[PY_TERM_LEX_MAX_NUM];
	PY_LEX stTerm2Lex[PY_TERM_LEX_MAX_NUM];
	PY_DYN stUserDyn;
	PY_KEY stKey;
	PY_UINT16 nPageMaxNum;
	PY_UINT16 nCurrPageId;

	PY_UINT16 nInputMode;
	PY_UINT16 nFilterFlags;

} PY_ENGINE;

PY_UINT16 PYEngine_PrepareCand(PY_HANDLE hEngine);
PY_BOOL PYEngine_InsertKey(PY_HANDLE hEngine, PY_UINT16 nKeyId);

PY_PCSTR  PYEngine_GetVersion(PY_VOID)
{
	static char szVersion[256];

    const char *szEnglishMonth[12]= {
            "Jan","Feb","Mar","Apr","May","Jun",
            "Jul","Aug","Sep","Oct","Nov","Dec",
    };
    char szTmpDate[100]={0};
    char szTmpTime[100]={0};
    char szMonth[10]={0};
    char szDateTime[100];
    int iYear, iMonth, iDay;
    int iHour,iMin,iSec;

    sprintf(szTmpDate,"%s", __DATE__); //"Sep 18 2010"
    sprintf(szTmpTime,"%s", __TIME__); //"10:59:19"

    memcpy(szMonth, szTmpDate, 3);
    for (int i=0; i < 12; i++)
    {
        if (strncmp(szMonth, szEnglishMonth[i], 3) == 0)
        {
            iMonth=i+1;
            break;
        }
    }

    memset(szMonth, 0, 10);
    memcpy(szMonth, szTmpDate+4, 2);
    iDay = atoi(szMonth);

    memset(szMonth, 0, 10);
    memcpy(szMonth, szTmpDate+7, 4);
    iYear = atoi(szMonth);

    memset(szMonth, 0, 10);
    memcpy(szMonth, szTmpTime, 2);
    iHour = atoi(szMonth);

    memset(szMonth, 0, 10);
    memcpy(szMonth, szTmpTime+3, 2);
    iMin = atoi(szMonth);

    memset(szMonth, 0, 10);
    memcpy(szMonth, szTmpTime+6, 2);
    iSec = atoi(szMonth);

    sprintf(szDateTime,"%4d-%02d-%02d %02d:%02d:%02d", iYear,iMonth,iDay,iHour,iMin,iSec);

    sprintf(szVersion,"PYEngine v%d.%d build %s", PY_ENGINE_VERSION_MAJOR, PY_ENGINE_VERSION_MINOR, szDateTime);

    return szVersion;
}

PY_UINT32 PYEngine_GetInstanceSize()
{
	return sizeof(PY_ENGINE);
}

PY_VOID PYEngine_Reset(PY_HANDLE hEngine)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	pEngine->nKeyLen = 0;
	pEngine->nInsPos = 0;

	PYKey_Reset(&pEngine->stKey);
	PYGraph_Reset(&pEngine->stGraph);
	PYCand_Reset(&pEngine->stCand);
	PYPredict_Reset(&pEngine->stPredict);
}

PY_INT16 PYEngine_GetBlockType(const PY_BLOCK* pBlock)
{
	PY_FILE_HEAD* pHead = (PY_FILE_HEAD*)pBlock->pAddress;

	if (pHead->nFileMask == PY_FILE_PY_SYSTEM_MASK) {
		return PY_LEX_CORE;
	}
	else if (pHead->nFileMask == PY_FILE_PY_TERM1_MASK) {
		return PY_LEX_TERM1;
	}
	else if (pHead->nFileMask == PY_FILE_PY_TERM2_MASK) {
		return PY_LEX_TERM2;
	}
	else if (pHead->nFileMask == PY_FILE_PY_DYNAMIC_MASK) {
		return PY_LEX_USER;
	}

	return PY_LEX_UNKNOWN;
}


PY_HANDLE  PYEngine_Initialize(PY_VOID* pInstBuffer, PY_UINT32 nInstSize, const PY_BLOCK* pDictBlockList, PY_UINT16 nDictBlockTotal)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)pInstBuffer;
	PY_UINT16 i;
	PY_INT16 nLexType;
	PYHan_Initialize(&pEngine->stHan, PYHan_GetHanData());

	pEngine->nTerm1LexTotal = 0;
	pEngine->nTerm2LexTotal = 0;
	pEngine->nPageMaxNum = 5;

	for (i = 0; i < nDictBlockTotal; i++)
	{
		nLexType = PYEngine_GetBlockType(&pDictBlockList[i]);

		if (nLexType == PY_LEX_CORE)
		{
			PYLex_Initialize(&pEngine->stCoreLex, &pEngine->stHan, &pDictBlockList[i]);
		}
		else if (nLexType == PY_LEX_USER) 
		{
			PYDyn_Initialize(&pEngine->stUserDyn, &pEngine->stHan, &pDictBlockList[i]);
		}
		else if (nLexType == PY_LEX_TERM1) 
		{
			if (PYLex_Initialize(&pEngine->stTerm1Lex[pEngine->nTerm1LexTotal], &pEngine->stHan, &pDictBlockList[i])) {
				pEngine->nTerm1LexTotal++;
			}
		}
		else if (nLexType == PY_LEX_TERM2)
		{
			if (PYLex_Initialize(&pEngine->stTerm2Lex[pEngine->nTerm2LexTotal], &pEngine->stHan, &pDictBlockList[i])) {
				pEngine->nTerm2LexTotal++;
			}
		}
	}

	PYKey_Initialize(&pEngine->stKey, &pEngine->stHan);
	PYGraph_Initialize(&pEngine->stGraph, &pEngine->stHan);
	
	PYCand_Initialize(&pEngine->stCand, &pEngine->stKey, &pEngine->stHan, &pEngine->stGraph, &pEngine->stCoreLex, &pEngine->stUserDyn, pEngine->stTerm1Lex, pEngine->nTerm1LexTotal, pEngine->stTerm2Lex, pEngine->nTerm2LexTotal);
	
	PYPredict_Initialize(&pEngine->stPredict, &pEngine->stHan, &pEngine->stCoreLex, &pEngine->stUserDyn, pEngine->stTerm1Lex, pEngine->nTerm1LexTotal, pEngine->stTerm2Lex, pEngine->nTerm2LexTotal);

	PYEngine_Reset(pEngine);
	pEngine->stOption.nFuzzyFlags = 0;
	pEngine->stOption.bEnablePredict = PY_FALSE;
	pEngine->stDevOpt.bNeedPrepareCand = PY_TRUE;
	pEngine->stDevOpt.nLogLevel = 0;
	PYHan_SetFuzzy(&pEngine->stHan, pEngine->stOption.nFuzzyFlags | PY_FUZZY_JP_ZH_Z | PY_FUZZY_JP_CH_C | PY_FUZZY_JP_SH_S);

	pEngine->nInputMode = PY_INPUT_MODE_PINYIN;

	return pEngine;
}

PY_VOID PYEngine_Terminate(PY_HANDLE hEngine)
{

}

PY_BOOL PYEngine_SetOption(PY_HANDLE hEngine, const PY_OPTION* pOption)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	
	if (pOption != PY_NULL) {
		pEngine->stOption = *pOption;

		PYHan_SetFuzzy(&pEngine->stHan, pEngine->stOption.nFuzzyFlags | PY_FUZZY_JP_ZH_Z | PY_FUZZY_JP_CH_C | PY_FUZZY_JP_SH_S);
	}

	return PY_TRUE;
}

const PY_OPTION* PYEngine_GetOption(PY_HANDLE hEngine)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	
	return &pEngine->stOption;
}

PY_BOOL PYEngine_SetKeys(PY_HANDLE hEngine, PY_PCSTR pKeyString, PY_UINT16 nKeyLen)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	PY_UINT16 i;

	pEngine->nInputMode = PY_INPUT_MODE_PINYIN;
	PYEngine_Reset(pEngine);
	
	for (i = 0; i < nKeyLen; i++)
	{
		PYEngine_InsertKey(pEngine, pKeyString[i]);
	}

	if (pEngine->stDevOpt.bNeedPrepareCand) {
		PYEngine_PrepareCand(hEngine);
	}

	return PY_TRUE;
}

PY_UINT16 PYEngine_GetKeyStringLen(PY_HANDLE hEngine)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;

	return pEngine->nKeyLen;
}

PY_BOOL PYEngine_AppendKey(PY_HANDLE hEngine, PY_UINT16 nKeyId)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	pEngine->szKeys[pEngine->nKeyLen++] = nKeyId;

	PYKey_AddKey(&pEngine->stKey, nKeyId);

	PYGraph_AddCol(&pEngine->stGraph, nKeyId, PYKey_GetMatchNodeList(&pEngine->stKey), PYKey_GetMatchNodeTotal(&pEngine->stKey));

	return PYCand_AddKey(&pEngine->stCand, nKeyId, 0);
}

PY_BOOL PYEngine_InsertKey(PY_HANDLE hEngine, PY_UINT16 nKeyId)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	PY_UINT16 i;

	if (nKeyId == '\b')
	{
		if (PYCand_GetSelLen(&pEngine->stCand) > 0)
		{
			PYCand_UnselLastItem(&pEngine->stCand);
			return PY_TRUE;
		}

		if (pEngine->nKeyLen > 0 && pEngine->nInsPos > 0)
		{
			if (pEngine->nKeyLen == pEngine->nInsPos)
			{
				pEngine->nKeyLen--;
				pEngine->nInsPos--;
				PYKey_DelLastKey(&pEngine->stKey, 1);
				PYGraph_SetColTotal(&pEngine->stGraph, PYKey_GetKeyLen(&pEngine->stKey));
				PYCand_SetColTotal(&pEngine->stCand, PYKey_GetKeyLen(&pEngine->stKey));
			}
			else
			{
				PY_UINT16 nKeyList[PY_INPUT_MAX_LEN];
				PY_UINT16 nMoveLen = pEngine->nKeyLen - pEngine->nInsPos;
				PY_UINT16 nReminLen = pEngine->nInsPos - 1;
				PY_memcpy(nKeyList, &pEngine->szKeys[pEngine->nInsPos], nMoveLen * sizeof(PY_UINT16));
				pEngine->nKeyLen = nReminLen;
				pEngine->nInsPos = nReminLen;

				PYKey_DelLastKey(&pEngine->stKey, nMoveLen + 1);
				PYGraph_SetColTotal(&pEngine->stGraph, PYKey_GetKeyLen(&pEngine->stKey));
				PYCand_SetColTotal(&pEngine->stCand, PYKey_GetKeyLen(&pEngine->stKey));

				for (i = 0; i < nMoveLen; i++)
				{
					PYEngine_AppendKey(pEngine, nKeyList[i]);
				}
			}

			if (pEngine->nKeyLen == 0)
			{
				PYEngine_Reset(hEngine);
			}

			return PY_TRUE;
		}
	}
	else
	{
		if (pEngine->nKeyLen < 80)
		{
			if (pEngine->nKeyLen == pEngine->nInsPos)
			{
				PYEngine_AppendKey(pEngine, nKeyId);
				pEngine->nInsPos++;
			}
			else
			{
				PY_UINT16 nKeyList[PY_INPUT_MAX_LEN];
				PY_UINT16 nMoveLen = pEngine->nKeyLen - pEngine->nInsPos;
				PY_UINT16 nReminLen = pEngine->nInsPos;
				PY_memcpy(nKeyList, &pEngine->szKeys[pEngine->nInsPos], nMoveLen * sizeof(PY_UINT16));
				pEngine->nKeyLen = nReminLen;
				pEngine->nInsPos = nReminLen + 1;

				PYKey_DelLastKey(&pEngine->stKey, nMoveLen);
				PYGraph_SetColTotal(&pEngine->stGraph, PYKey_GetKeyLen(&pEngine->stKey));
				PYCand_SetColTotal(&pEngine->stCand, PYKey_GetKeyLen(&pEngine->stKey));

				PYEngine_AppendKey(pEngine, nKeyId);

				for (i = 0; i < nMoveLen; i++)
				{
					PYEngine_AppendKey(pEngine, nKeyList[i]);
				}
			}
		}

		return PY_TRUE;
	}

	return PY_FALSE;
}

PY_BOOL PYEngine_KeyEvent(PY_HANDLE hEngine, PY_UINT16 nKeyId, PY_UINT16 nKeyFlags)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;

	if (pEngine->nInputMode != PY_INPUT_MODE_PINYIN)
	{
		pEngine->nInputMode = PY_INPUT_MODE_PINYIN;
		PYEngine_Reset(pEngine);
	}

	if (PYEngine_InsertKey(hEngine, nKeyId)) {
		if (pEngine->stDevOpt.bNeedPrepareCand) {
			PYEngine_PrepareCand(hEngine);
		}

		return PY_TRUE;

	}
	return PY_FALSE;
}

PY_UINT16 PYEngine_GetKeyStringUtf8(PY_HANDLE hEngine, PY_CHAR* pKeyString, PY_UINT16 nKeyMaxLen)
{
	PY_WCHAR szKeyString[PY_INPUT_MAX_LEN + 1];
	PY_UINT16 nKeyLen = PY_INPUT_MAX_LEN;
	nKeyLen = PYEngine_GetKeyString(hEngine, szKeyString, PY_INPUT_MAX_LEN);
	return PY_utf16_to_utf8(szKeyString, nKeyLen, pKeyString, nKeyMaxLen);
}

PY_UINT16  PYEngine_GetKeyString(PY_HANDLE hEngine, PY_PWSTR pKeyString, PY_UINT16 nKeyMaxLen)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	
	
	if (nKeyMaxLen >= pEngine->nKeyLen)
	{
		PY_wcsncpy(pKeyString, pEngine->szKeys, pEngine->nKeyLen);
		if (nKeyMaxLen > pEngine->nKeyLen)
		{
			pKeyString[pEngine->nKeyLen] = 0;
		}
	}

	return pEngine->nKeyLen;
}

PY_UINT16  PYEngine_GetCompStringUtf8(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PSTR pCompString, PY_UINT16 nCompMaxLen, PY_UINT16* pCaretPos, PY_BOOL bSplit)
{
	PY_WCHAR szCompString[PY_INPUT_MAX_LEN + 1];
	PY_UINT16 nCompLen = PY_INPUT_MAX_LEN;
	PY_UINT16 nCaretUtf16 = 0;
	nCompLen = PYEngine_GetCompString(hEngine, nItemId, szCompString, PY_INPUT_MAX_LEN, &nCaretUtf16, bSplit);
	PY_UINT16 nCaretPosUtf8 = PY_utf16_to_utf8(szCompString, nCaretUtf16, pCompString, nCompMaxLen);
	
	if (pCaretPos)
	{
		*pCaretPos = nCaretPosUtf8;
	}
	
	return PY_utf16_to_utf8(szCompString, nCompLen, pCompString, nCompMaxLen);
}

PY_UINT16  PYEngine_GetCompString_Pinyin(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PWSTR pCompString, PY_UINT16 nCompMaxLen, PY_UINT16* pCaretPos, PY_BOOL bSplit)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	PY_UINT16 i;
	PY_UINT16 nCompLen = 0;
	if (PYCand_GetSelLen(&pEngine->stCand) > 0)
	{
		PY_UINT16 szCompString[PY_INPUT_MAX_LEN];

		PY_UINT16 nSelStringLen = PYCand_GetSelString(&pEngine->stCand, pCompString, nCompMaxLen);
		if (PYCand_GetSelFixLen(&pEngine->stCand) >= pEngine->nKeyLen)
		{
			*pCaretPos = nSelStringLen;
			return  nSelStringLen;
		}
		PY_UINT16 nCompLen = PYCand_GetItemCompString(&pEngine->stCand, nItemId, szCompString, PY_INPUT_MAX_LEN, bSplit);
		PY_UINT16 nCompBegin = PYCand_GetSelFixLen(&pEngine->stCand);
		PY_UINT16 nCompFixNum = PYCand_GetSelFixLen(&pEngine->stCand) - PYCand_GetSelLen(&pEngine->stCand);

		if (nCompBegin < pEngine->nKeyLen)
		{
			PY_wcsncpy(&pCompString[nSelStringLen], &szCompString[nCompFixNum], nCompLen - nCompFixNum);
			if (pEngine->nInsPos <= nCompBegin)
			{
				pEngine->nInsPos = PYCand_GetSelFixLen(&pEngine->stCand);
				*pCaretPos = nSelStringLen;
			}
			else
			{
				PY_UINT16 nValidLen = PYCand_GetSelLen(&pEngine->stCand);
				*pCaretPos = nCompLen - nCompFixNum + nSelStringLen;

				for (i = 0; i < nCompLen; i++)
				{
					if (szCompString[i] != '|')
					{
						if (nValidLen == pEngine->nInsPos)
						{
							*pCaretPos = i - nCompFixNum + nSelStringLen;
							break;
						}

						nValidLen++;
					}
					else
					{
						if (nValidLen == pEngine->nInsPos)
						{
							*pCaretPos = i - nCompFixNum + nSelStringLen;
							break;
						}

					}
				}
			}
		}
		else
		{
			*pCaretPos = nSelStringLen;
		}

		for (i = nSelStringLen; i < nSelStringLen + nCompLen - nCompFixNum; i++)
		{
			if (pCompString[i] == '|') {
				pCompString[i] = '\'';
			}

			if (pCompString[i] >= 'A' && pCompString[i] <= 'Z') {
				pCompString[i] += 'a' - 'A';
			}
		}

		nCompLen = nSelStringLen + nCompLen - nCompFixNum;

		if (nCompMaxLen > nCompLen) {
			pCompString[nCompLen] = 0;
		}

		return nCompLen;
	}
	else
	{
		PY_UINT16 nCompLen = PYCand_GetItemCompString(&pEngine->stCand, nItemId, pCompString, nCompMaxLen, bSplit);
		PY_UINT16 nValidLen = 0;
		*pCaretPos = nCompLen;

		for (i = 0; i < nCompLen; i++)
		{
			if (pCompString[i] != '|')
			{

				if (nValidLen == pEngine->nInsPos)
				{
					*pCaretPos = i;
					break;
				}

				nValidLen++;
			}
			else
			{
				if (nValidLen == pEngine->nInsPos)
				{
					*pCaretPos = i;
					break;
				}

			}
		}
        
        int nKeyPos = 0;

		for (i = 0; i < nCompLen; i++)
		{
			if (pCompString[i] == '|') {
				pCompString[i] = '\'';
			}
            else
            {
                if (!PY_ISUPPER(PYGraph_GetColVal(&pEngine->stGraph, nKeyPos + 1)))
                {
                    if (pCompString[i] >= 'A' && pCompString[i] <= 'Z') {
                        pCompString[i] += 'a' - 'A';
                    }
                }
                nKeyPos ++;
            }
		}

		return nCompLen;
	}
}

PY_UINT16  PYEngine_GetCompString(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PWSTR pCompString, PY_UINT16 nCompMaxLen, PY_UINT16* pCaretPos, PY_BOOL bSplit)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	if (pEngine->nInputMode == PY_INPUT_MODE_PREDICT)
	{
			pCompString[0] = 0;
			return 0;
	}

	return PYEngine_GetCompString_Pinyin(hEngine, nItemId, pCompString, nCompMaxLen, pCaretPos, bSplit);
}

PY_UINT16 PYEngine_PrepareCand(PY_HANDLE hEngine)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	
	pEngine->nCurrPageId = 0;
	
	if (pEngine->nInputMode == PY_INPUT_MODE_PREDICT)
	{
		return PYPredict_GetTotal(&pEngine->stPredict);
	}

	return PYCand_PrepareCand(&pEngine->stCand);
}

PY_UINT16  PYEngine_GetCandTotal(PY_HANDLE hEngine)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;

	if (pEngine->nInputMode == PY_INPUT_MODE_PREDICT)
	{
		return PYPredict_GetTotal(&pEngine->stPredict);
	}

	return PYCand_GetTotal(&pEngine->stCand);
}

PY_UINT16  PYEngine_GetCandItemStringUtf8(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PSTR pPhraseString,
	PY_UINT16 nPhraseMaxLen)
{
    PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
    PY_WCHAR szOutString[PY_INPUT_MAX_LEN + 1];
    PY_UINT16 nOutLen = PY_INPUT_MAX_LEN;
    nOutLen = PYEngine_GetCandItemString(hEngine, nItemId, szOutString, PY_INPUT_MAX_LEN);
    return PY_utf16_to_utf8(szOutString, nOutLen, pPhraseString, nPhraseMaxLen);
    
//	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
//
//	if (pEngine->nInputMode == PY_INPUT_MODE_PREDICT)
//	{
//		return PYPredict_GetItemStringUtf8(&pEngine->stPredict, nItemId, pPhraseString, nPhraseMaxLen);
//	}
//
//	return PYCand_GetItemStringUtf8(&pEngine->stCand, nItemId, pPhraseString, nPhraseMaxLen);
}

PY_UINT16  PYEngine_GetCandItemString(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PWSTR pPhraseString,
	PY_UINT16 nPhraseMaxLen)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;

	if (pEngine->nInputMode == PY_INPUT_MODE_PREDICT)
	{
		return PYPredict_GetItemString(&pEngine->stPredict, nItemId, pPhraseString, nPhraseMaxLen);
	}

	return PYCand_GetItemString(&pEngine->stCand, nItemId, pPhraseString, nPhraseMaxLen);
}

PY_UINT16 PYEngine_GetConvertedStringUtf8(PY_HANDLE hEngine, PY_PSTR pOutputString, PY_UINT16 nOutputMaxLen)
{
	PY_WCHAR szOutString[PY_INPUT_MAX_LEN + 1];
	PY_UINT16 nOutLen = PY_INPUT_MAX_LEN;
	nOutLen = PYEngine_GetConvertedString(hEngine, szOutString, PY_INPUT_MAX_LEN);
	return PY_utf16_to_utf8(szOutString, nOutLen, pOutputString, nOutputMaxLen);
}

PY_UINT16 PYEngine_GetConvertedString(PY_HANDLE hEngine, PY_PWSTR pOutputString, PY_UINT16 nOutputMaxLen)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	PY_UINT16 nCaretPos = 0;
	PY_UINT16 nRet = 0;

	if (PYPredict_GetSelStringLenght(&pEngine->stPredict) > 0)
	{
		return PYHan_DecodePhrase(&pEngine->stHan, PYPredict_GetSelString(&pEngine->stPredict),PYPredict_GetSelStringLenght(&pEngine->stPredict), pOutputString, nOutputMaxLen);
	}

	if (PYCand_GetSelItemType(&pEngine->stCand) == PY_CAND_FIX_ITEM)
	{
		nRet = PYCand_GetSelHanString(&pEngine->stCand, pOutputString, nOutputMaxLen);
	}
	else 
	{
		nRet = PYEngine_GetCompString_Pinyin(hEngine, 0, pOutputString, nOutputMaxLen, &nCaretPos, PY_FALSE);
	}

	return nRet;
}

PY_UINT16 PYEngine_SelCandItem(PY_HANDLE hEngine, PY_UINT16 nItemId)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;

	if (pEngine->nInputMode == PY_INPUT_MODE_PREDICT)
	{
		if (PYPredict_SelItem(&pEngine->stPredict, nItemId) > 0)
		{
			return 1;
		}
		else
		{
//			pEngine->nInputMode = PY_INPUT_MODE_PINYIN;
			pEngine->nKeyLen = 0;
			pEngine->nInsPos = 0;
			pEngine->nCurrPageId = 0;
			PYKey_Reset(&pEngine->stKey);
			PYGraph_Reset(&pEngine->stGraph);
			PYCand_Reset(&pEngine->stCand);
			return 1;
		}
	}

	PY_UINT16 nRet = PYCand_SelItem(&pEngine->stCand, nItemId);
	if (nRet != 1)
	{
		if (pEngine->stDevOpt.bNeedPrepareCand)
		{
			PYEngine_PrepareCand(hEngine);
		}
	}
	else
	{

		if (PYCand_GetSelItemType(&pEngine->stCand) != PY_CAND_FIX_ITEM)
		{
			PY_UINT16 nHanString[256];
			PY_UINT16 nHanLen = PYCand_GetSelHanString(&pEngine->stCand, nHanString, 256);
            
            if (PYDyn_IsInit(&pEngine->stUserDyn))
            {
                PYDyn_AddPhrase(&pEngine->stUserDyn, nHanString, nHanLen, 1);
            }
            if (pEngine->stOption.bEnablePredict)
            {
                if (PYPredict_Search(&pEngine->stPredict, nHanString, nHanLen) > 0)
                {
                    pEngine->nInputMode = PY_INPUT_MODE_PREDICT;
                }
                else
                {
                    PYEngine_Reset(pEngine);
                    pEngine->nInputMode = PY_INPUT_MODE_PINYIN;
                }
            }
			
		}
	}

	return nRet;
}

PY_UINT16 PYEngine_MoveCaretLeftUnit(PY_HANDLE hEngine)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	PY_WCHAR szCompString[PY_INPUT_MAX_LEN + 1];
	PY_UINT16 nCompLen = PY_INPUT_MAX_LEN;
	PY_UINT16 nCaretPos = 0;
	PY_UINT16 i = 0;
	PY_UINT16 nSelStringLen = PYCand_GetSelString(&pEngine->stCand, szCompString, PY_INPUT_MAX_LEN);
	nCompLen = PYEngine_GetCompString(hEngine, 0, szCompString, PY_INPUT_MAX_LEN, &nCaretPos, PY_TRUE);
	int nMoveLen = nSelStringLen - nCaretPos;

	if (nCaretPos > 0 && nSelStringLen < nCompLen)
	{
		for (i = nCaretPos - 1; i > nSelStringLen; i--)
		{
			if (szCompString[i] == '\'')
			{
				if (nCaretPos == i + 1)
				{
					nMoveLen = i - nCaretPos;
				}
				else
				{
					nMoveLen = i + 1 - nCaretPos;
				}
				break;
			}
			if (szCompString[i] == '|')
			{
				nMoveLen = i + 1 - nCaretPos;
				break;
			}
		}
	}
	pEngine->nInsPos += nMoveLen;
	if (pEngine->nInsPos < PYCand_GetSelFixLen(&pEngine->stCand))
	{
		pEngine->nInsPos = PYCand_GetSelFixLen(&pEngine->stCand);
	}
	return pEngine->nInsPos;

}

PY_UINT16 PYEngine_MoveCaret(PY_HANDLE hEngine, PY_UINT16 nCaretPos)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	
	if (pEngine->nInputMode == PY_INPUT_MODE_PREDICT)
	{
		return 0;
	}

	switch (nCaretPos)
	{
	case PY_CARET_MOVE_HOME:
		pEngine->nInsPos = PYCand_GetSelFixLen(&pEngine->stCand);
		break;
	case PY_CARET_MOVE_END:
		pEngine->nInsPos = pEngine->nKeyLen;
		break;
	case PY_CARET_MOVE_LEFT_CHAR:
		if (pEngine->nInsPos > PYCand_GetSelFixLen(&pEngine->stCand))
		{
			pEngine->nInsPos--;
		}
		break;
	case PY_CARET_MOVE_RIGHT_CHAR:
		if (pEngine->nInsPos < pEngine->nKeyLen)
		{
			pEngine->nInsPos++;
		}
		break;
	case PY_CARET_MOVE_LEFT_UNIT:
		PYEngine_MoveCaretLeftUnit(hEngine);
		break;

	case PY_CARET_MOVE_RIGHT_UINT:
		if (pEngine->nInsPos < pEngine->nKeyLen)
		{
			pEngine->nInsPos++;
		}
		break;
	default:
		return 0;
	}

	return 0;
}

PY_UINT16 PYEngine_GetFilterType(PY_HANDLE hEngine)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;

	return pEngine->nFilterFlags;
}

PY_UINT16 PYEngine_SetFilterType(PY_HANDLE hEngine, PY_UINT16 nFilterType)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;

	if (pEngine->nInputMode == PY_INPUT_MODE_PREDICT)
	{
		return 0;
	}

	pEngine->nFilterFlags = nFilterType;
	if (nFilterType == PY_FILTER_SINGLE_WORD)
	{
		PYCand_SetSingleWord(&pEngine->stCand, PY_TRUE);
	}
	else
	{
		PYCand_SetSingleWord(&pEngine->stCand, PY_FALSE);
	}

	if (pEngine->stDevOpt.bNeedPrepareCand) {
		PYEngine_PrepareCand(hEngine);
	}

	return PY_TRUE;
}

PY_UINT16 PYEngine_SetPageMaxItem(PY_HANDLE hEngine, PY_UINT16 nPageMaxNum) {
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	pEngine->nPageMaxNum = nPageMaxNum;

	return pEngine->nPageMaxNum;
}

PY_UINT16 PYEngine_GetPageMaxItem(PY_HANDLE hEngine) {
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	
	return pEngine->nPageMaxNum;
}

PY_UINT16 PYEngine_GetPageTotal(PY_HANDLE hEngine) {
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	PY_UINT16 nCandTotal = PYEngine_GetCandTotal(hEngine);

	return (nCandTotal - 1) / PYEngine_GetPageMaxItem(hEngine) + 1;
}

PY_UINT16 PYEngine_GetPageItemTotal(PY_HANDLE hEngine, PY_UINT16 nPageId)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	PY_UINT16 nRemainTotal = PYEngine_GetCandTotal(hEngine) - nPageId * pEngine->nPageMaxNum;
	
	if (nRemainTotal > pEngine->nPageMaxNum)
	{
		nRemainTotal = pEngine->nPageMaxNum;
	}

	return nRemainTotal;
}

PY_UINT16 PYEngine_GetPageItemStringUtf8(PY_HANDLE hEngine, PY_UINT16 nPageId, PY_UINT16 nItemId, PY_PSTR pPhraseString, PY_UINT16 nPhraseMaxLen)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	PY_UINT16 nCandItemId = nPageId * pEngine->nPageMaxNum + nItemId;

	return PYEngine_GetCandItemStringUtf8(hEngine, nCandItemId, pPhraseString, nPhraseMaxLen);
}

PY_UINT16 PYEngine_GetPageItemString(PY_HANDLE hEngine, PY_UINT16 nPageId, PY_UINT16 nItemId,  PY_PWSTR pPhraseString, PY_UINT16 nPhraseMaxLen)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	PY_UINT16 nCandItemId = nPageId * pEngine->nPageMaxNum + nItemId;

	return PYEngine_GetCandItemString(hEngine, nCandItemId, pPhraseString, nPhraseMaxLen);

}

PY_UINT16 PYEngine_SelPageItem(PY_HANDLE hEngine, PY_UINT16 nPageId, PY_UINT16 nItemId)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	PY_UINT16 nCandItemId = nPageId * pEngine->nPageMaxNum + nItemId;

	return PYEngine_SelCandItem(hEngine, nCandItemId);
}

PY_BOOL PYEngine_HasPrevPage(PY_HANDLE hEngine)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	
	return (pEngine->nCurrPageId > 0);
}

PY_BOOL PYEngine_HasNextPage(PY_HANDLE hEngine) 
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;

	return (pEngine->nCurrPageId + 1) < PYEngine_GetPageTotal(hEngine);
}
PY_BOOL PYEngine_TurnPrevPage(PY_HANDLE hEngine) 
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	if (PYEngine_HasPrevPage(hEngine))
	{
		pEngine->nCurrPageId--;
	}
	return PY_FALSE;
}
PY_BOOL PYEngine_TurnNextPage(PY_HANDLE hEngine) 
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;
	if (PYEngine_HasNextPage(hEngine))
	{
		pEngine->nCurrPageId++;
	}
	return PY_FALSE;

}
PY_UINT16 PYEngine_GetCurrPageItemTotal(PY_HANDLE hEngine)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;

	return PYEngine_GetPageItemTotal(hEngine, pEngine->nCurrPageId);
}
PY_UINT16 PYEngine_GetCurrPageItemStringUtf8(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PSTR pPhraseString, PY_UINT16 nPhraseMaxLen)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;

	return PYEngine_GetPageItemStringUtf8(hEngine, pEngine->nCurrPageId, nItemId, pPhraseString, nPhraseMaxLen);
}

PY_UINT16 PYEngine_GetCurrPageItemString(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PWSTR pPhraseString, PY_UINT16 nPhraseMaxLen)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;

	return PYEngine_GetPageItemString(hEngine, pEngine->nCurrPageId, nItemId, pPhraseString, nPhraseMaxLen);
}

PY_UINT16 PYEngine_SelCurrPageItem(PY_HANDLE hEngine, PY_UINT16 nItemId)
{
	PY_ENGINE* pEngine = (PY_ENGINE*)hEngine;

	return PYEngine_SelPageItem(hEngine, pEngine->nCurrPageId, nItemId);
}
