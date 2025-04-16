#include "PYCand.h"
#include "PYLex.h"

int PYCand_PhraseCompare(const PY_CAND_ITEM* elem1, const PY_CAND_ITEM* elem2, void* arg)
{
	int nRet = elem1->nMatchLen - elem2->nMatchLen;
	if (nRet == 0)
	{
		return elem1->nPhraseWeight - elem2->nPhraseWeight;
	}
	else
	{
		return -nRet;
	}
}

PY_BOOL   PYCand_Initialize(PY_CAND* pCand, PY_KEY* pKey, PY_HAN *pHan, PY_GRAPH* pGraph, PY_LEX* pCoreLex, PY_DYN *pUserDyn, PY_LEX* pTerm1Lex, PY_UINT16 nTerm1Total, PY_LEX* pTerm2Lex, PY_UINT16 nTerm2Total)
{
	pCand->pCoreLex = pCoreLex;
	pCand->pUserDyn = pUserDyn;
	pCand->pTerm1Lex = pTerm1Lex;
	pCand->nTerm1LexTotal = nTerm1Total;
	pCand->pTerm2Lex = pTerm2Lex;
	pCand->nTerm2LexTotal = nTerm2Total;

	pCand->pHan = pHan;
	pCand->pKey = pKey;
	pCand->pGraph = pGraph;
	PYFix_Initialize(&pCand->stFix);
	PYCand_Reset(pCand);
	
	return PY_TRUE;
}

PY_VOID   PYCand_Terminate(PY_CAND* pCand)
{

}

PY_VOID   PYCand_Reset(PY_CAND* pCand)
{
	pCand->nMatchItemTotal = 0;
	pCand->nSelItemTotal = 0;
	pCand->nSelLen = 0;
	pCand->nCandItemTotal = 0;
	pCand->stMatchItemRange[0].nEnd = 0;
	pCand->bSingleWord = PY_FALSE;
	PYFix_Reset(&pCand->stFix);
	PY_memset(pCand->stSenItem, 0, sizeof(pCand->stSenItem));
}

PY_UINT16 PYCand_GetPhraseBigramWeight(PY_CAND* pCand, PY_UINT32 nPhraseId1, PY_UINT32 nPhraseId2, PY_UINT16 nPhraseWeight2)
{
	return nPhraseWeight2;
}

PY_VOID PYCand_GenSen(PY_CAND* pCand, PY_UINT16 nKeyLen);

PY_BOOL PYCand_AddCandItem(PY_CAND* pCand, PY_CAND_ITEM* pItem)
{
	PY_UINT32 i;
	if (pCand->nCandItemTotal < PY_CAND_ITEM_MAX_NUM) {
		
		// 优化过滤机制，考虑用第一个字的unicode值判断是否需要查重
		PY_UINT16 nHanUnicode = PYHan_GetHanUnicode(pCand->pHan, pItem->pPhraseString[0]);

		if (pCand->byCandUnicode[nHanUnicode] != 0)
		{
			for (i = 0; i < pCand->nCandItemTotal; i++) {
				if (pCand->nCandItem[i].nPhraseLen == pItem->nPhraseLen && PYHan_IsEqual(pCand->pGraph->pHan, pCand->nCandItem[i].pPhraseString, pItem->pPhraseString, pItem->nPhraseLen))
				{
					if (pCand->nCandItem[i].nPhraseWeight > pItem->nPhraseWeight ||
                        pCand->nCandItem[i].nMatchLen < pItem->nMatchLen) {
						pCand->nCandItem[i] = *pItem;
					}

					return PY_FALSE;
				}
			}

		}

		pCand->byCandUnicode[nHanUnicode] = 1;

		pCand->nCandItem[pCand->nCandItemTotal++] = *pItem;

		return PY_TRUE;
	}

	return PY_FALSE;

}

PY_VOID PYCand_InsertFixItemByPos(PY_CAND* pCand, PY_INT16 nPos, PY_UINT16 nFixItemId)
{
	if (pCand->nCandItemTotal < PY_CAND_ITEM_MAX_NUM)
	{
		PY_memmove(&pCand->nCandItem[nPos + 1], &pCand->nCandItem[nPos], sizeof(PY_CAND_ITEM) * (pCand->nCandItemTotal - nPos));

		pCand->nCandItem[nPos].pPhraseString = PYFix_GetPhraseDataPtr(&pCand->stFix, nFixItemId);
		pCand->nCandItem[nPos].nPhraseLen = PYFix_GetPhraseLength(&pCand->stFix, nFixItemId);
		pCand->nCandItem[nPos].nPhraseType = PY_CAND_FIX_ITEM;
		pCand->nCandItem[nPos].nLastPinyinId = 0;
		pCand->nCandItem[nPos].nMatchBegin = 0;
		pCand->nCandItem[nPos].nMatchLen = PYKey_GetKeyLen(pCand->pKey);
		pCand->nCandItem[nPos].nPhraseWeight = 0;
		pCand->nCandItem[nPos].nPhraseId = 0;

		pCand->nCandItemTotal++;
	}
}

PY_VOID PYCand_InsertFixItemByString(PY_CAND* pCand)
{

}

PY_VOID PYCand_GenFixCand(PY_CAND* pCand)
{
	PY_UINT32 i;
	PY_UINT32 nFixCandTotal = PYFix_Search(&pCand->stFix, PYKey_GetKeyString(pCand->pKey), PYKey_GetKeyLen(pCand->pKey));

	for (i = 0; i < nFixCandTotal; i++)
	{
		PYCand_InsertFixItemByPos(pCand, PYFix_GetPhrasePos(&pCand->stFix, i), i);
	}

}

PY_VOID PYCand_GenCand(PY_CAND* pCand, PY_UINT16 nKeyLen)
{
	PY_UINT32 i;

	// 生成候选
	PYCand_GenSen(pCand, nKeyLen);

	PY_BOOL bGenSencence = PY_TRUE;
	int nMatchMaxLen = PYGraph_GetColTotal(pCand->pGraph);
	while (pCand->pGraph->stMatrix[nMatchMaxLen].nRoadType == PY_NODE_TYPE_SPLIT)
	{
		nMatchMaxLen--;
	}

	// 添加候选到候选列表并且标记是否需要生成整句候选

	pCand->nCandItemTotal = 0;
	PY_memzero(pCand->byCandUnicode, 65536 * sizeof(PY_BYTE));

	for (i = 0; i < pCand->nMatchItemTotal; i++)
	{
		if (pCand->stMatchItem[i].nMatchBegin == pCand->nSelLen &&
			PYGraph_IsValidPinyin(pCand->pGraph, pCand->stMatchItem[i].nMatchBegin + pCand->stMatchItem[i].nMatchLen,
				pCand->stMatchItem[i].nLastPinyinId))
		{
			if (pCand->nCandItemTotal < PY_CAND_ITEM_MAX_NUM - 1)
			{
				if (pCand->bSingleWord)
				{
					if (pCand->stMatchItem[i].nPhraseLen == 1)
					{
						if (bGenSencence && pCand->stMatchItem[i].nMatchLen == nMatchMaxLen)
						{
							bGenSencence = PY_FALSE;
						}

						PYCand_AddCandItem(pCand, &pCand->stMatchItem[i]);
					}
				}
				else
				{
					if (bGenSencence && pCand->stMatchItem[i].nMatchLen == nMatchMaxLen)
					{
						bGenSencence = PY_FALSE;
					}

					PYCand_AddCandItem(pCand, &pCand->stMatchItem[i]);
				}
			}
		}
	}

	// 需要添加整句候选

	if (nKeyLen > 0 && bGenSencence)
	{
		// 权重最低的路径

		PY_UINT32 nMinRoadWeight = pCand->stSenItem[nKeyLen][0].nRoadWeight;
		PY_UINT32 nMinRoadItem = 0;
		PY_UINT32 k;
		for (k = 0; k < pCand->nSenItemTotal[nKeyLen]; k++)
		{
			if (nMinRoadWeight > pCand->stSenItem[nKeyLen][k].nRoadWeight)
			{
				nMinRoadWeight = pCand->stSenItem[nKeyLen][k].nRoadWeight;
				nMinRoadItem = k;
			}
		}

		// 生成候选句子HanString

		PY_UINT16 nEndLen = PY_INPUT_MAX_LEN;
		PY_CAND_ITEM stSenCandItem;

		PY_SEN_ITEM* cur = &pCand->stSenItem[nKeyLen][nMinRoadItem];
		while (cur)
		{
			if (cur->nPhraseId != 0)
			{
				int nDataLen = pCand->stMatchItem[cur->nMatchItemId].nPhraseLen;
                
				PY_wcsncpy(&pCand->nSenHanString[nEndLen - nDataLen],
					pCand->stMatchItem[cur->nMatchItemId].pPhraseString, nDataLen);
                
                // 这里还是hanid无效
                if (PY_ishanletter(pCand->stMatchItem[cur->nMatchItemId].pPhraseString, nDataLen))
                {
                    for (PY_UINT8 j = pCand->stMatchItem[cur->nMatchItemId].nMatchBegin;
                         j< pCand->stMatchItem[cur->nMatchItemId].nMatchBegin + pCand->stMatchItem[cur->nMatchItemId].nMatchLen; j++)
                    {
                        if (PY_ISUPPER(PYGraph_GetColVal(pCand->pGraph, j + 1)))
                        {
                            if (PY_ISHANLOWER(pCand->stMatchItem[cur->nMatchItemId].pPhraseString[j - pCand->stSelItem[i].nMatchBegin]))
                            {
                                pCand->nSenHanString[nEndLen - nDataLen + j - pCand->stSelItem[i].nMatchBegin] = PY_TOHANUPPER(pCand->nSenHanString[nEndLen - nDataLen + j - pCand->stSelItem[i].nMatchBegin]);
                            }
                        }
                    }
                }
                
				nEndLen -= nDataLen;
			}
			cur = cur->pPrevItem;
		}
		pCand->nSenHanLen = PY_INPUT_MAX_LEN - nEndLen;

		// 添加到候选项
		
		stSenCandItem.nMatchBegin = pCand->nSelLen;
		stSenCandItem.nMatchLen = nKeyLen - pCand->nSelLen;
		stSenCandItem.nPhraseId = 0;
		stSenCandItem.nPhraseLen = pCand->nSenHanLen;
		stSenCandItem.nPhraseType = 0;
		stSenCandItem.nPhraseWeight = 0;
		stSenCandItem.pPhraseString = &pCand->nSenHanString[nEndLen];

		PYCand_AddCandItem(pCand, &stSenCandItem);
	}

	// 候选排序
	if (pCand->nCandItemTotal > 0)
	{
		PY_quicksort(pCand->nCandItem, pCand->nCandItemTotal, sizeof(PY_CAND_ITEM), (PY_compare) PYCand_PhraseCompare, PY_NULL);
		// 添加fix候选
		PYCand_GenFixCand(pCand);

	}



}

PY_BOOL   PYCand_SetColTotal(PY_CAND* pCand, PY_UINT16 nCol)
{
	pCand->nMatchItemTotal = pCand->stMatchItemRange[nCol].nEnd;
	return PY_TRUE;
}

PY_VOID PYCand_GenSen(PY_CAND* pCand, PY_UINT16 nKeyLen)
{
	// 计算整句候选待选列表
	PY_UINT32 i;
	PY_UINT32 j;
	PY_UINT32 c;

	if (nKeyLen == 3)
	{
		pCand->nSenItemTotal[pCand->nSelLen] = 1;

	}
	pCand->nSenItemTotal[pCand->nSelLen] = 1;
	PY_memzero(pCand->stSenItem, sizeof(pCand->stSenItem));
	for (c = pCand->nSelLen + 1; c <= nKeyLen; c++)
	{
		if (PYGraph_GetColType(pCand->pGraph, c) == PY_NODE_TYPE_SPLIT)
		{
			pCand->nSenItemTotal[c] = 1;
			pCand->stSenItem[c][0].nRoadWeight = 0;
			pCand->stSenItem[c][0].nPhraseId = 0;
			pCand->stSenItem[c][0].nMatchItemId = 0;

			PY_UINT32 nPhraseWeight = pCand->stSenItem[c - 1][0].nRoadWeight;
			PY_UINT32 nId = 0;
			PY_UINT32 i;

			for (i = 1; i < pCand->nSenItemTotal[c - 1]; i++)
			{
				if (pCand->stSenItem[c - 1][i].nRoadWeight < nPhraseWeight)
				{
					nPhraseWeight = pCand->stSenItem[c - 1][i].nRoadWeight;
					nId = i;
				}
			}

			pCand->stSenItem[c][0].pPrevItem = &pCand->stSenItem[c - 1][nId];

			continue;
		}

		pCand->nSenItemTotal[c] = 0;

		for (i = pCand->stMatchItemRange[c].nBegin; i < pCand->stMatchItemRange[c].nEnd; i++)
		{
			PY_UINT32 nPhraseId2 = pCand->stMatchItem[i].nPhraseId;

			if (pCand->stMatchItem[i].nMatchBegin >= pCand->nSelLen && PYGraph_IsValidPinyin(pCand->pGraph, c, pCand->stMatchItem[i].nLastPinyinId))
			{
				// 计算和前项节点的路径权重

				for (j = 0; j < pCand->nSenItemTotal[pCand->stMatchItem[i].nMatchBegin]; j++)
				{
					PY_UINT32 nPhraseId1 = pCand->stSenItem[pCand->stMatchItem[i].nMatchBegin][j].nPhraseId;
					PY_UINT16 nPhraseBigramWeight = PYCand_GetPhraseBigramWeight(pCand, nPhraseId1, nPhraseId2, pCand->stMatchItem[i].nPhraseWeight);
					PY_UINT32 nRoadWeight = pCand->stSenItem[pCand->stMatchItem[i].nMatchBegin][j].nRoadWeight + nPhraseBigramWeight;

					if (pCand->nSenItemTotal[c] < PY_SEN_MAX_NUM)
					{
						pCand->stSenItem[c][pCand->nSenItemTotal[c]].nRoadWeight = nRoadWeight;
						pCand->stSenItem[c][pCand->nSenItemTotal[c]].nPhraseId = nPhraseId2;
						pCand->stSenItem[c][pCand->nSenItemTotal[c]].nMatchItemId = i;
						pCand->stSenItem[c][pCand->nSenItemTotal[c]].pPrevItem = &pCand->stSenItem[pCand->stMatchItem[i].nMatchBegin][j];
						pCand->nSenItemTotal[c] ++;
					}
					else
					{
						PY_UINT32 nMaxRoadWeight = 0;
						PY_UINT32 nMaxRoadItem = 0;
						PY_UINT32 k;

						for (k = 0; k < pCand->nSenItemTotal[c]; k++)
						{
							if (nMaxRoadWeight < pCand->stSenItem[c][k].nRoadWeight)
							{
								nMaxRoadWeight = pCand->stSenItem[c][k].nRoadWeight;
								nMaxRoadItem = k;
							}
						}

						if (nMaxRoadWeight > nRoadWeight)
						{
							pCand->stSenItem[c][nMaxRoadItem].nRoadWeight = nRoadWeight;
							pCand->stSenItem[c][nMaxRoadItem].nPhraseId = nPhraseId2;
							pCand->stSenItem[c][nMaxRoadItem].nMatchItemId = i;
							pCand->stSenItem[c][nMaxRoadItem].pPrevItem = &pCand->stSenItem[pCand->stMatchItem[i].nMatchBegin][j];
						}
					}
				}
			}
		}
	}
}

PY_BOOL   PYCand_SetSingleWord(PY_CAND* pCand, PY_BOOL bSingleWord)
{
	pCand->bSingleWord = bSingleWord;
    return PY_TRUE;
}

PY_BOOL   PYCand_AddKey(PY_CAND* pCand, PY_UINT32 nKey1, PY_UINT32 nKeyFlags)
{
	PY_UINT32 i;
	PY_UINT32 j;
	PY_UINT32 k;

	PY_UINT16 nKeyLen = PYGraph_GetColTotal(pCand->pGraph);

	pCand->stMatchItemRange[nKeyLen].nBegin = pCand->nMatchItemTotal;
	pCand->nSenItemTotal[0] = 1;

	pCand->stSenItem[0][0].nPhraseId = 0;
	PY_memzero(pCand->stSenItem[0], sizeof(PY_SEN_ITEM) * PY_SEN_MAX_NUM);
	// 生成前N个句子
	pCand->nSenItemTotal[nKeyLen] = 0;
	
	if (pCand->pGraph->stMatrix[nKeyLen].nRoadType != PY_NODE_TYPE_SPLIT)
	{
		// 获取当前节点有效候选项

		PY_UINT16 nGroup2Total = PYGraph_GetLastColMatchGroup2Total(pCand->pGraph);
		PY_UINT32* pGroup2Item = PYGraph_GetLastColMatchGroup2Ptr(pCand->pGraph);
		PY_UINT32  nGPhrase2Total;
		PY_UINT32  nPhraseId;

		if (nGroup2Total > 0)
		{
			// 核心词库中匹配候选

			for (i = 0; i < nGroup2Total; i++)
			{
				nGPhrase2Total = PYLex_GetGroupItemTotal(pCand->pCoreLex, pGroup2Item[i]);

				for (j = 0; j < nGPhrase2Total; j++)
				{
					PYCand_Match(pCand, PY_LEX_CORE, 
						PYLex_GetGroupItemPhraseId(pCand->pCoreLex, pGroup2Item[i], j),
						PYLex_GetGroupItemDataPtr(pCand->pCoreLex, pGroup2Item[i], j),
						PYLex_GetGroupItemLength(pCand->pCoreLex, pGroup2Item[i], j),
						PYLex_GetGroupItemWeight(pCand->pCoreLex, pGroup2Item[i], j));
				}
			}

			// 用户词库中匹配候选
            if (PYDyn_GetPhraseTotal(pCand->pUserDyn) > 0)
            {
                for (i = 0; i < nGroup2Total; i++)
                {
                    nPhraseId = PYDyn_GetFirstPhraseId(pCand->pUserDyn, pGroup2Item[i]);
                    while (nPhraseId != 0)
                    {
                        PYCand_Match(pCand, PY_LEX_USER, nPhraseId,
                            PYDyn_GetPhraseDataPtr(pCand->pUserDyn, nPhraseId),
                            PYDyn_GetPhraseLength(pCand->pUserDyn, nPhraseId),
                            PYDyn_GetPhraseWeight(pCand->pUserDyn, nPhraseId));

                        nPhraseId = PYDyn_GetNextPhraseId(pCand->pUserDyn, nPhraseId);
                    }
                }
                
            }
			

			if (pCand->nTerm2LexTotal > 0)
			{
				for (k = 0; k < pCand->nTerm2LexTotal; k++)
				{
					for (i = 0; i < nGroup2Total; i++)
					{
						nGPhrase2Total = PYLex_GetGroupItemTotal(&pCand->pTerm2Lex[k], pGroup2Item[i]);

						for (j = 0; j < nGPhrase2Total; j++)
						{
							PYCand_Match(pCand, PY_LEX_TERM2,
								PYLex_GetGroupItemPhraseId(&pCand->pTerm2Lex[k], pGroup2Item[i], j),
								PYLex_GetGroupItemDataPtr(&pCand->pTerm2Lex[k], pGroup2Item[i], j),
								PYLex_GetGroupItemLength(&pCand->pTerm2Lex[k], pGroup2Item[i], j),
								PYLex_GetGroupItemWeight(&pCand->pTerm2Lex[k], pGroup2Item[i], j));
						}
					}
				}
			}

			if (pCand->nTerm1LexTotal > 0)
			{
				PY_UINT16 nGroup1Total = PYGraph_GetLastColMatchGroup1Total(pCand->pGraph);
				PY_UINT32* pGroup1Item = PYGraph_GetLastColMatchGroup1Ptr(pCand->pGraph);
				PY_UINT32  nGPhrase1Total;

				for (k = 0; k < pCand->nTerm1LexTotal; k++)
				{
					for (i = 0; i < nGroup1Total; i++)
					{
						nGPhrase1Total = PYLex_GetGroupItemTotal(&pCand->pTerm1Lex[k], pGroup1Item[i]);

						for (j = 0; j < nGPhrase1Total; j++)
						{
							PYCand_Match(pCand, PY_LEX_TERM1,
								PYLex_GetGroupItemPhraseId(&pCand->pTerm1Lex[k], pGroup1Item[i], j),
								PYLex_GetGroupItemDataPtr(&pCand->pTerm1Lex[k], pGroup1Item[i], j),
								PYLex_GetGroupItemLength(&pCand->pTerm1Lex[k], pGroup1Item[i], j),
								PYLex_GetGroupItemWeight(&pCand->pTerm1Lex[k], pGroup1Item[i], j));
						}
					}
				}
			}
		}
	}

	pCand->stMatchItemRange[nKeyLen].nEnd = pCand->nMatchItemTotal;
	
	return PY_TRUE;
}

PY_UINT16 PYCand_GetSelFixLen(PY_CAND* pCand)
{
	PY_UINT16 nSelFixLen = 0;
	if (pCand->nSelLen > 0)
	{
		PY_UINT16 i;
		nSelFixLen = pCand->nSelLen;

		for (i = pCand->nSelLen + 1; i < PYGraph_GetColTotal(pCand->pGraph); i++)
		{
			if (PYGraph_GetColType(pCand->pGraph, i) != PY_NODE_TYPE_SPLIT)
			{
				nSelFixLen = i - 1;
				break;
			}
		}
	}

	return nSelFixLen;
}

PY_UINT16 PYCand_GetSelLen(PY_CAND* pCand)
{
	return pCand->nSelLen;
}

PY_UINT16 PYCand_PrepareCand(PY_CAND* pCand)
{
	PYCand_GenCand(pCand, PYGraph_GetColTotal(pCand->pGraph));
	return PYCand_GetTotal(pCand);
}

PY_UINT16 PYCand_UnselLastItem(PY_CAND* pCand)
{
	if (pCand->nSelItemTotal > 0)
	{
		pCand->nSelItemTotal--;
		pCand->nSelLen -= pCand->stSelItem[pCand->nSelItemTotal].nMatchLen;
	}

	return pCand->nSelLen;

}

PY_UINT16 PYCand_GetSelHanString(PY_CAND* pCand, PY_PWSTR pSelString, PY_UINT16 nSelMaxLen)
{
	PY_UINT16 i;
	PY_UINT16 nUsedLen = 0;

	for (i = 0; i < pCand->nSelItemTotal; i++)
	{
		PY_wcsncpy(&pSelString[nUsedLen], pCand->stSelItem[i].pPhraseString, pCand->stSelItem[i].nPhraseLen);
		nUsedLen += pCand->stSelItem[i].nPhraseLen;
	}

	if (nSelMaxLen > nUsedLen)
	{
		pSelString[nUsedLen] = 0;
	}

	return nUsedLen;
}


PY_UINT16 PYCand_GetSelString(PY_CAND* pCand, PY_PWSTR pSelString, PY_UINT16 nSelMaxLen)
{
	PY_UINT16 i;
	PY_UINT16 nUsedLen = 0;
	PY_UINT16 nItemLen = 0;
	
	for (i = 0; i < pCand->nSelItemTotal; i++)
	{
		nItemLen = PYHan_DecodePhrase(pCand->pGraph->pHan,
			pCand->stSelItem[i].pPhraseString,
			pCand->stSelItem[i].nPhraseLen,
			&pSelString[nUsedLen], nSelMaxLen - nUsedLen);
        
        if (PY_isletter(&pSelString[nUsedLen], nItemLen))
        {
            for (PY_UINT8 j = pCand->stSelItem[i].nMatchBegin; j< pCand->stSelItem[i].nMatchBegin + pCand->stSelItem[i].nMatchLen; j++)
            {
                if (PY_ISUPPER(PYGraph_GetColVal(pCand->pGraph, j + 1)))
                {
                    if (PY_ISLOWER(pSelString[nUsedLen + j - pCand->stSelItem[i].nMatchBegin]))
                    {
                        pSelString[nUsedLen + j - pCand->stSelItem[i].nMatchBegin] = PY_TOUPPER(pSelString[nUsedLen + j - pCand->stSelItem[i].nMatchBegin]);
                    }
                }
            }
        }
        
		nUsedLen += nItemLen;
	}

	if (nSelMaxLen > nUsedLen)
	{
		pSelString[nUsedLen] = 0;
	}

	return nUsedLen;
}

PY_UINT16 PYCand_GetSelItemType(PY_CAND* pCand)
{
	return pCand->nSelItemType;
}

PY_UINT16 PYCand_SelItem(PY_CAND* pCand, PY_UINT16 nItemId)
{
	PY_UINT16 nSelLen = pCand->nCandItem[nItemId].nMatchBegin + pCand->nCandItem[nItemId].nMatchLen;
	pCand->stSelItem[pCand->nSelItemTotal++] = pCand->nCandItem[nItemId];
	pCand->nSelLen += pCand->nCandItem[nItemId].nMatchLen;
	
	pCand->nSelItemType = pCand->nCandItem[nItemId].nPhraseType;

	if (pCand->nSelLen >= PYGraph_getColTrimSplitLen(pCand->pGraph))
	{
		return 1;
	}

	return 0;
}

PY_VOID   PYCand_Match(PY_CAND* pCand, PY_UINT8 nLexId, PY_UINT32 nPhraseId, PY_UINT16 *pPhraseString, PY_UINT16 nPhraseLen, PY_UINT16 nPhraseWeight)
{	
	int nMatchBegin = PYGraph_Match(pCand->pGraph,
		0,
		PYGraph_GetColTotal(pCand->pGraph),
		pPhraseString,
		nPhraseLen, PY_NULL, PY_NULL, PY_FALSE);

	if (nMatchBegin != -1 && pCand->nMatchItemTotal < PY_MATCH_ITEM_MAX_NUM)
	{
		pCand->stMatchItem[pCand->nMatchItemTotal].nPhraseLen = nPhraseLen;
		pCand->stMatchItem[pCand->nMatchItemTotal].pPhraseString = pPhraseString;
		pCand->stMatchItem[pCand->nMatchItemTotal].nPhraseWeight = nPhraseWeight;
		pCand->stMatchItem[pCand->nMatchItemTotal].nPhraseId = nPhraseId;
		pCand->stMatchItem[pCand->nMatchItemTotal].nPhraseType = nLexId;
		pCand->stMatchItem[pCand->nMatchItemTotal].nMatchBegin = nMatchBegin;
		pCand->stMatchItem[pCand->nMatchItemTotal].nMatchLen = PYGraph_GetColTotal(pCand->pGraph) - nMatchBegin;

		PY_UINT16 nHanId = pCand->stMatchItem[pCand->nMatchItemTotal].pPhraseString[pCand->stMatchItem[pCand->nMatchItemTotal].nPhraseLen - 1];
		PY_UINT16 nLastPinyinId = PYHan_GetHanPinyinId(pCand->pGraph->pHan, nHanId);
		pCand->stMatchItem[pCand->nMatchItemTotal].nLastPinyinId = nLastPinyinId;

		pCand->nMatchItemTotal++;
	}
}

PY_UINT32 PYCand_GetTotal(PY_CAND* pCand)
{
	return pCand->nCandItemTotal;
}

PY_UINT16 PYCand_GetItemCompString(PY_CAND* pCand, PY_UINT16 nItemId, PY_PWSTR pCandString, PY_UINT16 nCandMaxLen, PY_BOOL bSplit)
{
	PY_UINT16 i;
	PY_UINT16 nCandLen = nCandMaxLen;

	if (PYCand_GetTotal(pCand) > nItemId)
	{
		PYGraph_Match(pCand->pGraph,
			pCand->nCandItem[nItemId].nMatchBegin,
			pCand->nCandItem[nItemId].nMatchBegin + pCand->nCandItem[nItemId].nMatchLen,
			pCand->nCandItem[nItemId].pPhraseString,
			pCand->nCandItem[nItemId].nPhraseLen, pCandString, &nCandLen, bSplit);

		PY_UINT16 nEnd = pCand->nCandItem[nItemId].nMatchBegin + pCand->nCandItem[nItemId].nMatchLen;
		
		if (nEnd < PYGraph_GetColTotal(pCand->pGraph))
		{
			for (i = nEnd + 1; i <= PYGraph_GetColTotal(pCand->pGraph); i++)
			{
				pCandString[nCandLen++] = '\'';
			}

			pCandString[nCandLen] = 0;
		}
	}
	else 
	{
		nCandLen = 0;

		for (i = 1; i <= PYGraph_GetColTotal(pCand->pGraph); i++)
		{
			pCandString[nCandLen++] = '\'';
		}

		pCandString[nCandLen] = 0;
	}

	return nCandLen;
}


PY_UINT16 PYCand_GetItemString(PY_CAND* pCand, PY_UINT16 nItemId, PY_PWSTR pCandString, PY_UINT16 nCandMaxLen)
{
    PY_UINT16 nRet = 0;
	if (pCand->nCandItem[nItemId].nPhraseType == PY_CAND_FIX_ITEM)
	{
		PY_wcsncpy(pCandString, pCand->nCandItem[nItemId].pPhraseString,
			pCand->nCandItem[nItemId].nPhraseLen);
		pCandString[pCand->nCandItem[nItemId].nPhraseLen] = 0;

		return pCand->nCandItem[nItemId].nPhraseLen;
	}
//    如果需要，在这里处理中英混合词语大小写问题，
//    if (PY_existhanletter(pCand->nCandItem[nItemId].pPhraseString, pCand->nCandItem[nItemId].nPhraseLen) &&
//        !PY_ishanletter(pCand->nCandItem[nItemId].pPhraseString, pCand->nCandItem[nItemId].nPhraseLen))
//    {
//    }
//    else
    {
        nRet = PYHan_DecodePhrase(pCand->pGraph->pHan, pCand->nCandItem[nItemId].pPhraseString,
            pCand->nCandItem[nItemId].nPhraseLen, pCandString, nCandMaxLen);

        if (PY_isletter(pCandString, nRet))
        {
            for (PY_UINT8 i = pCand->nCandItem[nItemId].nMatchBegin; i< pCand->nCandItem[nItemId].nMatchBegin + pCand->nCandItem[nItemId].nMatchLen; i++)
            {
                if (PY_ISUPPER(PYGraph_GetColVal(pCand->pGraph, i + 1)))
                {
                    if (PY_ISLOWER(pCandString[i - pCand->nCandItem[nItemId].nMatchBegin]))
                    {
                        pCandString[i - pCand->nCandItem[nItemId].nMatchBegin] = PY_TOUPPER(pCandString[i - pCand->nCandItem[nItemId].nMatchBegin]);
                    }
                }
            }
        }
    }

    return nRet;
}
