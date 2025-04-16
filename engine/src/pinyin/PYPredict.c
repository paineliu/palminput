#include "PYPredict.h"


PY_BOOL PYPredict_Initialize(PY_PREDICT* pPredict, PY_HAN* pHan, PY_LEX* pCoreLex, PY_DYN* pUserDyn, PY_LEX* pTerm1Lex, PY_UINT16 nTerm1Total, PY_LEX* pTerm2Lex, PY_UINT16 nTerm2Total)
{
	pPredict->pHan = pHan;
	
	pPredict->pCoreLex = pCoreLex;
	pPredict->pTerm1Lex = pTerm1Lex;
	pPredict->nTerm1LexTotal = nTerm1Total;
	pPredict->pTerm2Lex = pTerm2Lex;
	pPredict->nTerm2LexTotal = nTerm2Total;
	pPredict->pUserDyn = pUserDyn;
	PYPredict_Reset(pPredict);
	return PY_TRUE;
}

PY_VOID PYPredict_Reset(PY_PREDICT* pPredict)
{
	pPredict->nCandItemTotal = 0;
	pPredict->nSelItemStr[0] = 0;
	pPredict->nSelItemLen = 0;
}

PY_VOID PYPredict_AddCandItem(PY_PREDICT* pPredict, PY_UINT16* pPhraseString, PY_UINT16 nPhraseLen, PY_UINT16 nPhraseWeight)
{
	if (pPredict->nCandItemTotal < PY_PREDICT_ITEM_MAX_NUM)
	{
		pPredict->nCandItem[pPredict->nCandItemTotal].nPhraseLen = nPhraseLen;
		pPredict->nCandItem[pPredict->nCandItemTotal].pPhraseString = pPhraseString;
		pPredict->nCandItem[pPredict->nCandItemTotal].nPhraseWeight = nPhraseWeight;
		pPredict->nCandItem[pPredict->nCandItemTotal].nPhraseId = 0;
		pPredict->nCandItem[pPredict->nCandItemTotal].nPhraseType = 0;
		pPredict->nCandItem[pPredict->nCandItemTotal].nMatchBegin = 0;
		pPredict->nCandItem[pPredict->nCandItemTotal].nMatchLen = 0;

		pPredict->nCandItemTotal++;
	}
	else
	{
		PY_UINT32 i;
		PY_UINT16 nMaxWeight = pPredict->nCandItem[0].nPhraseWeight;
		PY_UINT16 nMaxIndex = 0;

		for (i = 1; i < PY_PREDICT_ITEM_MAX_NUM; i++)
		{
			if (nMaxWeight < pPredict->nCandItem[i].nPhraseWeight)
			{
				nMaxIndex = i;
				nMaxWeight = pPredict->nCandItem[i].nPhraseWeight;
			}
		}

		if (nMaxWeight > nPhraseWeight)
		{
			pPredict->nCandItem[nMaxIndex].nPhraseLen = nPhraseLen;
			pPredict->nCandItem[nMaxIndex].pPhraseString = pPhraseString;
			pPredict->nCandItem[nMaxIndex].nPhraseWeight = nPhraseWeight;
			pPredict->nCandItem[nMaxIndex].nPhraseId = 0;
			pPredict->nCandItem[nMaxIndex].nPhraseType = 0;
			pPredict->nCandItem[nMaxIndex].nMatchBegin = 0;
			pPredict->nCandItem[nMaxIndex].nMatchLen = 0;
		}
	}
}

int PYPredict_PhraseCompare(const PY_CAND_ITEM* elem1, const PY_CAND_ITEM* elem2, void* arg)
{
	return elem1->nPhraseWeight - elem2->nPhraseWeight;
}

PY_VOID  PYPredict_GetLexCands(PY_PREDICT* pPredict, PY_LEX* pLex, PY_UINT16* pHanString, PY_UINT16 nHanLen)
{
	PY_UINT32 j;
	PY_UINT32 nBeginPos;
	PY_UINT32 nTotal;
	PY_UINT32 nPhraseId;
	PY_PCWSTR pPhraseStr;
	PY_UINT8 nPhraseLen;

	nBeginPos = PYLex_GetSortedPhraseIdBeginPos(pLex, pHanString, nHanLen);
	nTotal = PYLex_GetSortedTotal(pLex);
	for (j = nBeginPos; j < nTotal; j++)
	{
		nPhraseId = PYLex_GetSortedItemPhraseId(pLex, j);
		pPhraseStr = PYLex_GetPhraseDataPtr(pLex, nPhraseId);
		nPhraseLen = PYLex_GetPhraseLength(pLex, nPhraseId);

		if (PY_wcsncmp(pHanString, pPhraseStr, nHanLen) == 0)
		{
			if (nPhraseLen > nHanLen)
			{
				PYPredict_AddCandItem(pPredict, &pPhraseStr[nHanLen], nPhraseLen - nHanLen, PYLex_GetPhraseWeight(pLex, nPhraseId));
			}
		}
		else
		{
			break;
		}
	}
}

PY_UINT16* PYPredict_GetSelString(PY_PREDICT* pPredict)
{
	return pPredict->nSelItemStr;
}

PY_UINT16 PYPredict_GetSelStringLenght(PY_PREDICT* pPredict)
{
	return pPredict->nSelItemLen;
}

PY_UINT16 PYPredict_Search(PY_PREDICT* pPredict, PY_UINT16* pHanString, PY_UINT16 nHanLen)
{
	PY_UINT16 i;

	pPredict->nCandItemTotal = 0;

	
	PYPredict_GetLexCands(pPredict, pPredict->pCoreLex, pHanString, nHanLen);

	for (i = 0; i < pPredict->nTerm1LexTotal; i++)
	{
		PYPredict_GetLexCands(pPredict, &pPredict->pTerm1Lex[i], pHanString, nHanLen);
	}

	for (i = 0; i < pPredict->nTerm2LexTotal; i++)
	{
		PYPredict_GetLexCands(pPredict, &pPredict->pTerm2Lex[i], pHanString, nHanLen);
	}

	if (pPredict->nCandItemTotal > 0)
	{
		PY_quicksort(pPredict->nCandItem, pPredict->nCandItemTotal, sizeof(PY_CAND_ITEM), (PY_compare) PYPredict_PhraseCompare, PY_NULL);
	}


	return pPredict->nCandItemTotal;
}

PY_UINT32 PYPredict_GetTotal(PY_PREDICT* pPredict)
{
	return pPredict->nCandItemTotal;
}

PY_UINT16 PYPredict_GetItemString(PY_PREDICT* pPredict, PY_UINT16 nItemId, PY_PWSTR pCandString, PY_UINT16 nCandMaxLen)
{
	return PYHan_DecodePhrase(pPredict->pHan, pPredict->nCandItem[nItemId].pPhraseString,
		pPredict->nCandItem[nItemId].nPhraseLen, pCandString, nCandMaxLen);

}

PY_UINT16 PYPredict_GetItemStringUtf8(PY_PREDICT* pPredict, PY_UINT16 nItemId, PY_PSTR pCandString, PY_UINT16 nCandMaxLen)
{
	return PYHan_DecodePhraseUtf8(pPredict->pHan, pPredict->nCandItem[nItemId].pPhraseString,
		pPredict->nCandItem[nItemId].nPhraseLen, pCandString, nCandMaxLen);
}

PY_UINT16 PYPredict_SelItem(PY_PREDICT* pPredict, PY_UINT16 nItemId)
{
	pPredict->nSelItemLen = pPredict->nCandItem[nItemId].nPhraseLen;
	PY_wcsncpy(pPredict->nSelItemStr, pPredict->nCandItem[nItemId].pPhraseString,
		pPredict->nSelItemLen);

	pPredict->nSelItemStr[pPredict->nSelItemLen] = 0;

	return PYPredict_Search(pPredict, pPredict->nSelItemStr, pPredict->nSelItemLen);
}

PY_VOID PYPredict_Terminate(PY_PREDICT* pPredict)
{

}
