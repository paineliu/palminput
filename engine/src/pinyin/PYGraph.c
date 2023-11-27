#include <stdio.h>
#include "PYGraph.h"
#include "PYHanDef.h"


PY_UINT16 PYGraph_Group(PY_GRAPH* pGraph);

int PYGraph_GetColTotal(PY_GRAPH* pGraph)
{
	return pGraph->nColLen;
}

PY_BOOL PYGraph_Initialize(PY_GRAPH* pGraph, PY_HAN* pHan)
{
	PY_memset(pGraph, 0, sizeof(PY_GRAPH));

	pGraph->pHan = pHan;
	pGraph->nColLen = 0;
	pGraph->nMathItemTotal = 0;
	pGraph->nHanMatchMatrix[0][0].nMatchCol = 0;
	pGraph->nHanMatchMatrix[0][0].nMatchCol = 0;
	return PY_TRUE;
}

PY_VOID PYGraph_Reset(PY_GRAPH* pGraph)
{
	PY_HAN* pHan = pGraph->pHan;
	PY_memset(pGraph, 0, sizeof(PY_GRAPH));
	pGraph->pHan = pHan;
}

// nEndCol 从第一列开始

int PYGraph_AddNode(PY_GRAPH* pGraph, int nCol, PY_KEY_NODE* pKeyNode, const PY_NODE_PINYIN* pNodePinyin)
{
	// 如果当前拼音节点没有匹配过则是第一个匹配的节点

	PY_BOOL bFirstItem = pGraph->stMatrix[nCol].stPinyinList[pNodePinyin->nPinyinId].nNodeType == 0;
	int nNodeCost = pNodePinyin->nNodeType & (PY_NODE_TYPE_QUANPIN) ? 0 : 1;
    
	pGraph->stMatrix[nCol].nRoadType |= pNodePinyin->nNodeType;

	// 更新当前拼音节点的匹配类型
	pGraph->stMatrix[nCol].stPinyinList[pNodePinyin->nPinyinId].nNodeType |= pNodePinyin->nNodeType;

	if (bFirstItem)
	{
		pGraph->stMatrix[nCol].stPinyinList[pNodePinyin->nPinyinId].nMatchItemBegin = pGraph->nMathItemTotal;
		pGraph->stMatrix[nCol].stPinyinList[pNodePinyin->nPinyinId].nMatchItemEnd = pGraph->nMathItemTotal;
	}

	// 当前拼音节点的下一个节点等于当前节点
	pGraph->stMathItem[pGraph->stMatrix[nCol].stPinyinList[pNodePinyin->nPinyinId].nMatchItemEnd].nNextItem = pGraph->nMathItemTotal;

	// 设置当前节点信息

	pGraph->stMathItem[pGraph->nMathItemTotal].nNodeType = pNodePinyin->nNodeType;
	pGraph->stMathItem[pGraph->nMathItemTotal].nNodeCost = nNodeCost;
	pGraph->stMathItem[pGraph->nMathItemTotal].pNodeString = pKeyNode->pMatchString;
	pGraph->stMathItem[pGraph->nMathItemTotal].nNodeLen = pKeyNode->nMatchLen;
	pGraph->stMathItem[pGraph->nMathItemTotal].nNodeId = pKeyNode->nNodeId;
	pGraph->stMathItem[pGraph->nMathItemTotal].nPinyinId = pNodePinyin->nPinyinId;
	pGraph->stMathItem[pGraph->nMathItemTotal].nPrevCol = nCol - pKeyNode->nMatchLen;
	pGraph->stMathItem[pGraph->nMathItemTotal].nNextCol = nCol + 1;
	pGraph->stMathItem[pGraph->nMathItemTotal].nNodeValid = PY_TRUE;

	pGraph->stMathItem[pGraph->nMathItemTotal].nNextItem = 0;

	// 	更新当前拼音节点的End节点为当前节点

	pGraph->stMatrix[nCol].stPinyinList[pNodePinyin->nPinyinId].nMatchItemEnd = pGraph->nMathItemTotal;

	if (nNodeCost + pGraph->stMatrix[nCol - pKeyNode->nMatchLen].nRoadCost < pGraph->stMatrix[nCol].nRoadCost)
	{
		pGraph->stMatrix[nCol].nRoadCost = nNodeCost + pGraph->stMatrix[nCol - pKeyNode->nMatchLen].nRoadCost;
	}

	//	printf("add %d,%d,%d\n", pGraph->nMathItemTotal, pNodePinyin->nRoadType, nMatchLen);

	pGraph->nMathItemTotal++;

	return 0;
}

PY_UINT16 PYGraph_AddColNode(PY_GRAPH* pGraph, int nCol, PY_KEY_NODE *pKeyNode)
{
	PY_UINT16 i;
	const PY_NODE_PINYIN* pNodePinyin;
	const PY_RANGE16* pRange = PYHan_GetNodePinyinRange(pGraph->pHan, pKeyNode->nNodeId);

	for (i = pRange->nBegin; i < pRange->nEnd; i++)
	{
		pNodePinyin = PYHan_GetNodePinyinItemValid(pGraph->pHan, i);
		if (pNodePinyin != PY_NULL)
		{
			PYGraph_AddNode(pGraph, nCol, pKeyNode, pNodePinyin);
		}
	}

	return 0;
}

int PYGraph_BeginCol(PY_GRAPH* pGraph, PY_UINT8 nCol, PY_UINT16 nKeyId)
{
	if (nCol < PY_INPUT_MAX_LEN)
	{
		pGraph->nColLen = nCol;
	}

	pGraph->stMatrix[nCol].nMatchItemBegin = pGraph->nMathItemTotal;
	pGraph->stMatrix[nCol].nRoadCost = pGraph->stMatrix[nCol - 1].nRoadCost + 1;
	pGraph->stMatrix[nCol].nKeyId = nKeyId;
	pGraph->stMatrix[nCol].nRoadType = 0;

	return 0;
}

int PYGraph_FinishCol(PY_GRAPH* pGraph, PY_UINT16 nCol1)
{
	PY_INT16 i;
	PY_INT16 j;
	PY_UINT8 nKeyLen = pGraph->nColLen;

	// 去掉无效的节点

	PY_memzero(pGraph->nValidTotal, (nKeyLen + 1) * sizeof(PY_UINT8));

	// 将所有节点设置为无效节点

	for (i = 0; i < pGraph->nMathItemTotal; i++)
	{
		if (pGraph->stMathItem[i].nNodeType != PY_NODE_TYPE_LETTER)
		{
			pGraph->stMathItem[i].nNodeValid = PY_FALSE;
		}
		else
		{
			pGraph->stMathItem[i].nNodeValid = PY_TRUE;
		}
	}

	pGraph->nValidTotal[nKeyLen] = 1;
	pGraph->nValidMartix[nKeyLen][0] = nKeyLen;

	// 生成有效拼音矩阵

	for (i = nKeyLen; i > 0; i--)
	{
		if (pGraph->nValidTotal[i] == 1 && pGraph->nValidMartix[i][0] == 0)
		{
//			printf("-----%d %d\n", nKeyLen - i, nRoadNodeMax);
			break;
		}

		for (j = 0; j < pGraph->nValidTotal[i]; j++)
		{
			int nCol = pGraph->nValidMartix[i][j];

			// 当前节点下没有拼音节点，则跳到前一节点，这个节点下只有LETTER时就会产生
			

			if ((pGraph->stMatrix[nCol].nRoadType & 0xF) == 0)
			{
				pGraph->nValidTotal[i - 1] = 1;
				pGraph->nValidMartix[i - 1][0] = nCol - 1;
				continue;
			}

			for (int l = 0; l < PY_PINYIN_TOTAL; l++)
			{
				if (pGraph->stMatrix[nCol].stPinyinList[l].nNodeType != 0)
				{
                    //printf("col = %d, py = %d, type = %x\n", nCol, l, pGraph->stMatrix[nCol].stPinyinList[l].nNodeType);
					int cur = pGraph->stMatrix[nCol].stPinyinList[l].nMatchItemBegin;
					do
					{
						// 有效节点判断条件

						// 上一节点的路径花费+当前节点花费 等于 当前节点路径花费 并且
						// 排除 fi 这种情况 当前节点和上一节点同时为补全拼音时，当前节点的拼音无效 fia  fi'a

						if (pGraph->stMathItem[cur].nNodeCost + pGraph->stMatrix[pGraph->stMathItem[cur].nPrevCol].nRoadCost == pGraph->stMatrix[nCol].nRoadCost && !(pGraph->stMatrix[pGraph->stMathItem[cur].nPrevCol].nRoadType & pGraph->stMatrix[nCol].nRoadType & PY_NODE_TYPE_BUQUAN))
						{
							// 下一个循环中是否已经把节点的前项节点位置加入

							PY_BOOL bExist = PY_FALSE;
							for (int k = 0; k < pGraph->nValidTotal[i - 1]; k++)
							{
								if (pGraph->nValidMartix[i - 1][k] == pGraph->stMathItem[cur].nPrevCol)
								{
									bExist = PY_TRUE;
								}
							}

							// 没有加入则加入到其中（不包含Letter节点）

							if (!bExist)
							{
								pGraph->nValidMartix[i - 1][pGraph->nValidTotal[i - 1]] = pGraph->stMathItem[cur].nPrevCol;
								pGraph->nValidTotal[i - 1]++;
							}

							// 当前节点有效

							pGraph->stMathItem[cur].nNodeValid = PY_TRUE;
						}
						else
						{
							// 否则设置其他节点为无效（所有的Letter节点保留）

							if (pGraph->stMathItem[cur].nNodeType != PY_NODE_TYPE_LETTER && pGraph->stMathItem[cur].nNodeType != PY_NODE_TYPE_QUANPIN)
							{
                                // 最后一个节点下简拼有效
                                
                                if (nCol == nKeyLen && pGraph->stMathItem[cur].nNodeType == PY_NODE_TYPE_JIANPIN)
                                {
                                    pGraph->stMathItem[cur].nNodeValid = PY_TRUE;
                                }
                                else
                                {
                                    pGraph->stMathItem[cur].nNodeValid = PY_FALSE;
                                }
							}
							else
							{
								pGraph->stMathItem[cur].nNodeValid = PY_TRUE;
							}
						}

						cur = pGraph->stMathItem[cur].nNextItem;

					} while (cur != 0);
				}
			}
		}
	}

	// PYGraph_Dump(pGraph);

	PYGraph_Group(pGraph);

	return 0;
}

int PYGraph_AddCol(PY_GRAPH* pGraph, PY_UINT16 nKeyId, PY_KEY_NODE* pNodeIdList, PY_UINT16 nNodeIdTotal)
{
	PY_INT16 i;
	
	PYGraph_BeginCol(pGraph, pGraph->nColLen + 1, nKeyId);

	for (i = 0; i < nNodeIdTotal; i++)
	{
		PYGraph_AddColNode(pGraph, pGraph->nColLen, &pNodeIdList[i]);
	}

	PYGraph_FinishCol(pGraph, 0);


	return 0;
}

PY_UINT16 PYGraph_Group(PY_GRAPH* pGraph)
{
	PY_UINT16 i;
	PY_UINT16 j;
	PY_UINT16 nLastPinyinCol = 0;
	PY_UINT8 nPrevCol;
	PY_UINT16 nCur;
	PY_UINT16 nCur2;

	pGraph->nGroup1Total = 0;
	pGraph->nGroup2Total = 0;

	// 跳过切分节点

	for (i = pGraph->nColLen; i > 0; i--)
	{
		if (pGraph->stMatrix[i].nRoadType != PY_NODE_TYPE_SPLIT)
		{
			nLastPinyinCol = i;
			break;
		}
	}

	if (nLastPinyinCol > 0)
	{
		for (i = 0; i < PY_PINYIN_TOTAL; i++)
		{
			if (pGraph->stMatrix[nLastPinyinCol].stPinyinList[i].nNodeType != 0)
			{
				nCur = pGraph->stMatrix[nLastPinyinCol].stPinyinList[i].nMatchItemBegin;
				do
				{
					// 一级有效节点

					if (pGraph->stMathItem[nCur].nNodeValid)
					{
						pGraph->nGroup2[pGraph->nGroup2Total] = PYHan_GetPinyinGroup2ByPinyinId(pGraph->pHan, 0, i);
						pGraph->nGroup2Total++;

						pGraph->nGroup1[pGraph->nGroup1Total] = i;
						pGraph->nGroup1Total++;

						/*printf("group1 - %s(%d), gid = 0x%04x\n",
							PYHan_GetPinyinString(pGraph->pHan, i),
							i,
							PYHan_GetPinyinGroup2ByPinyinId(pGraph->pHan, 0, i));*/
					}

					// 跳过split

					nPrevCol = pGraph->stMathItem[nCur].nPrevCol;
					while (pGraph->stMatrix[nPrevCol].nRoadType == PY_NODE_TYPE_SPLIT)
					{
						nPrevCol--;
					}

					if (nPrevCol > 0)
					{
						for (j = 0; j < PY_PINYIN_TOTAL; j++)
						{
							if (pGraph->stMatrix[nPrevCol].stPinyinList[j].nNodeType != 0)
							{
								// 二级有效节点

								nCur2 = pGraph->stMatrix[nPrevCol].stPinyinList[j].nMatchItemBegin;
								do
								{
									if (pGraph->stMathItem[nCur2].nNodeValid)
									{
										/*printf("group2 - %s(%d), %s(%d), gid = 0x%04x\n",
											PYHan_GetPinyinString(pGraph->pHan, j),
											j,
											PYHan_GetPinyinString(pGraph->pHan, i),
											i,
											PYHan_GetPinyinGroup2ByPinyinId(pGraph->pHan, j, i));*/
										pGraph->nGroup2[pGraph->nGroup2Total] = PYHan_GetPinyinGroup2ByPinyinId(pGraph->pHan, j, i);
										pGraph->nGroup2Total++;
									}

									nCur2 = pGraph->stMathItem[nCur2].nNextItem;
								} while (nCur2 != 0);
							}
						}
					}

					nCur = pGraph->stMathItem[nCur].nNextItem;
				} while (nCur != 0);
			}
		}
	}

	return pGraph->nGroup2Total;
}

PY_BOOL PYGraph_SetColTotal(PY_GRAPH* pGraph, PY_UINT8 nColTotal)
{
	PY_INT16 nDelCol = pGraph->nColLen - nColTotal;

	pGraph->nColLen = nColTotal;
	pGraph->nMathItemTotal = pGraph->stMatrix[pGraph->nColLen + 1].nMatchItemBegin;

	if (nDelCol > 0)
	{
		PY_memset(&pGraph->stMatrix[pGraph->nColLen + 1], 0, sizeof(PY_GRAPH_ITEM) * nDelCol);
	}

	PYGraph_FinishCol(pGraph, nColTotal);

	return PY_TRUE;
}
PY_UINT32* PYGraph_GetLastColMatchGroup1Ptr(PY_GRAPH* pGraph)
{
	return pGraph->nGroup1;
}

PY_UINT16  PYGraph_GetLastColMatchGroup1Total(PY_GRAPH* pGraph)
{
	return pGraph->nGroup1Total;
}


PY_UINT32* PYGraph_GetLastColMatchGroup2Ptr(PY_GRAPH* pGraph)
{
	return pGraph->nGroup2;
}

PY_UINT16 PYGraph_GetLastColMatchGroup2Total(PY_GRAPH* pGraph)
{
	return pGraph->nGroup2Total;
}


PY_INT16 PYGraph_Match(PY_GRAPH* pGraph, PY_UINT16 nBeginCol, PY_UINT16 nEndCol, const PY_UINT16* pHanString, PY_UINT16 nHanLen, PY_PWSTR pCompString, PY_UINT16* pCompLen, PY_BOOL bSplit)
{
	PY_UINT16 i;
	PY_UINT16 j;
	PY_UINT16 nMatchCol;
	PY_UINT16 nPinyinId;
//    PY_BOOL bPrint = PY_FALSE;
	PY_memzero(pGraph->nHanMatchTotal, sizeof(PY_UINT16) * (nHanLen + 1));
	PY_memzero(pGraph->nHanSplitTotal, sizeof(PY_UINT16) * (nHanLen + 1));
	
//	if (pCompString)
//	{
//        bPrint = PY_TRUE;
//		printf("%d %s\n", nBeginCol, PYHan_GetHanDescribe(pGraph->pHan, pHanString, nHanLen));
//	}

	pGraph->nHanMatchTotal[nHanLen] = 1;
	pGraph->nHanMatchMatrix[nHanLen][0].nMatchCol = nEndCol;

	while (pGraph->stMatrix[pGraph->nHanMatchMatrix[nHanLen][0].nMatchCol].nRoadType == PY_NODE_TYPE_SPLIT)
	{
		pGraph->nHanSplitTotal[nHanLen] ++;
		pGraph->nHanMatchMatrix[nHanLen][0].nMatchCol --;
	}

	for (i = nHanLen; i > 0; i--)
	{
		nPinyinId = PYHan_GetHanPinyinId(pGraph->pHan, pHanString[i-1]);
		
		for (j = 0; j < pGraph->nHanMatchTotal[i]; j++)
		{
			nMatchCol = pGraph->nHanMatchMatrix[i][j].nMatchCol;

			if (pGraph->stMatrix[nMatchCol].stPinyinList[nPinyinId].nNodeType != 0)
			{
				int cur = pGraph->stMatrix[nMatchCol].stPinyinList[nPinyinId].nMatchItemBegin;
				do
				{
					if (pGraph->stMathItem[cur].nNodeValid)
					{
						pGraph->nHanMatchMatrix[i - 1][pGraph->nHanMatchTotal[i - 1]].nMatchCol = pGraph->stMathItem[cur].nPrevCol;
						pGraph->nHanMatchMatrix[i - 1][pGraph->nHanMatchTotal[i - 1]].nMatchNodeId = cur;
//                        if (bPrint)
//                        {
//                            printf("%d %d %d\n", i - 1, pGraph->nHanMatchTotal[i - 1], pGraph->stMathItem[cur].nPrevCol);
//                        }
						pGraph->nHanMatchTotal[i - 1] ++;

					}
					cur = pGraph->stMathItem[cur].nNextItem;
				} while (cur != 0);
			}
		}

		while (pGraph->nHanMatchTotal[i - 1] == 1 && pGraph->stMatrix[pGraph->nHanMatchMatrix[i - 1][0].nMatchCol].nRoadType == PY_NODE_TYPE_SPLIT)
		{
			pGraph->nHanSplitTotal[i - 1] ++;
			pGraph->nHanMatchMatrix[i - 1][0].nMatchCol --;
		}

		if (pGraph->nHanMatchTotal[i - 1] == 0)
		{
			break;
		}
	}
	
	if (i == 0 && pGraph->nHanMatchTotal[0] > 0)
	{
		if (pCompString != PY_NULL)
		{
			PY_UINT16 nCompLen = 0;
			PY_UINT16 k;

			nMatchCol = nBeginCol;

			for (i = 0; i < nHanLen; i++)
			{
				for (j = 0; j < pGraph->nHanSplitTotal[i]; j++)
				{
					pCompString[nCompLen++] = '\'';
				}

				for (j = 0; j < pGraph->nHanMatchTotal[i]; j++)
				{
					if (pGraph->nHanMatchMatrix[i][j].nMatchCol == nMatchCol)
					{
						nMatchCol = pGraph->stMathItem[pGraph->nHanMatchMatrix[i][j].nMatchNodeId].nNextCol - 1;
						if (pGraph->stMathItem[pGraph->nHanMatchMatrix[i][j].nMatchNodeId].nNodeLen == 1 &&
                            PY_ISUPPER(pGraph->stMathItem[pGraph->nHanMatchMatrix[i][j].nMatchNodeId].pNodeString[0]))
						{
							// 处理G'O'O'D' 这种情况

							if (nCompLen > 1 && pCompString[nCompLen-1] == '|' && PY_ISUPPER(pCompString[nCompLen - 2]))
							{
								pCompString[nCompLen-1] = pGraph->stMathItem[pGraph->nHanMatchMatrix[i][j].nMatchNodeId].pNodeString[0];
							}
							else
							{
								pCompString[nCompLen++] = pGraph->stMathItem[pGraph->nHanMatchMatrix[i][j].nMatchNodeId].pNodeString[0];

							}
						}
						else
						{
							for (k = 0; k < pGraph->stMathItem[pGraph->nHanMatchMatrix[i][j].nMatchNodeId].nNodeLen; k++)
							{
								pCompString[nCompLen++] = pGraph->stMathItem[pGraph->nHanMatchMatrix[i][j].nMatchNodeId].pNodeString[k];

							}
						}
						continue;
					}
				}
				if (i + 1 != nHanLen)
				{
					if (bSplit)
					{
						if (pGraph->nHanSplitTotal[i+1] == 0)
						{
							pCompString[nCompLen++] = '|';
						}
					}
				}
			}

			for (j = 0; j < pGraph->nHanSplitTotal[nHanLen]; j++)
			{
				pCompString[nCompLen++] = '\'';
			}
			
			pCompString[nCompLen] = 0;

			*pCompLen = nCompLen;
		}

        if (pGraph->nHanMatchTotal[0] > 1)
        {
            int nMinCol = pGraph->nHanMatchMatrix[0][0].nMatchCol;
            for (i=1; i<pGraph->nHanMatchTotal[0]; i++)
            {
                if (nMinCol > pGraph->nHanMatchMatrix[0][i].nMatchCol)
                {
                    nMinCol = pGraph->nHanMatchMatrix[0][i].nMatchCol;
                }
            }
            
            return nMinCol;
            
        }
        else
        {
            return pGraph->nHanMatchMatrix[0][0].nMatchCol;
        }
	}

	return -1;
}

PY_UINT16 PYGraph_GetColVal(PY_GRAPH* pGraph, PY_UINT16 nColId)
{
    return pGraph->stMatrix[nColId].nKeyId;
}


int PYGraph_GetColType(PY_GRAPH* pGraph, PY_UINT16 nColId)
{
	return pGraph->stMatrix[nColId].nRoadType;
}

int PYGraph_getColTrimSplitLen(PY_GRAPH* pGraph)
{
	int i;
	for (i = pGraph->nColLen; i > 0; i--)
	{
		if (pGraph->stMatrix[i].nRoadType != PY_NODE_TYPE_SPLIT)
		{
			return i;
		}
	}

	return 0;
}


PY_BOOL PYGraph_IsValidPinyin(PY_GRAPH* pGraph, PY_UINT16 nCol, PY_UINT16 nPinyinId)
{
	if (pGraph->stMatrix[nCol].stPinyinList[nPinyinId].nNodeType != 0)
	{
		int cur = pGraph->stMatrix[nCol].stPinyinList[nPinyinId].nMatchItemBegin;
		do
		{
			if (pGraph->stMathItem[cur].nNodeValid)
			{
				return PY_TRUE;
			}
			cur = pGraph->stMathItem[cur].nNextItem;
		} while (cur != 0);
		
	}

	return PY_FALSE;
}

int PYGraph_Dump(PY_GRAPH* pGraph)
{
	int i;
	int j;
	int nNodeTotal = pGraph->nColLen;
	PY_GRAPH_ITEM* pMartix = pGraph->stMatrix;
	
	printf("\n==========");
	for (j = 1; j <= nNodeTotal; j++)
	{
		printf("==========");
	}
	printf("\n");

	printf("ID    ,   ");
	for (j = 1; j <= nNodeTotal; j++)
	{
		printf("       %2d|", j);
	}
	printf("\n");

	printf("CHAR  ,   ");
	for (j = 1; j <= nNodeTotal; j++)
	{
		printf("        %c|", pMartix[j].nKeyId);
	}
	printf("\n");
	printf("COST  ,   ");
	for (j = 1; j <= nNodeTotal; j++)
	{
		printf("       %2d|", pMartix[j].nRoadCost);
	}
	printf("\n");

	printf("----------");
	for (j = 1; j <= nNodeTotal; j++)
	{
		printf("----------");
	}
	printf("\n");

	int nMartrixCol[PY_INPUT_MAX_LEN] = { 0 };

	pMartix[nNodeTotal+1].nRoadCost = pMartix[nNodeTotal].nRoadCost ;
	int nTotal = 0;
	for (i = 0; i < PY_PINYIN_TOTAL; i++)
	{
		nTotal = 0;
		for (j = 0; j <= nNodeTotal; j++)
		{
			nTotal += pMartix[j].stPinyinList[i].nNodeType;
		}

		if (nTotal > 0)
		{
			printf("%-6s,   ", PYHan_GetPinyinString(pGraph->pHan, i));
			for (j = 1; j <=nNodeTotal; j++)
			{
				if (pMartix[j].stPinyinList[i].nNodeType != 0)
				{
					int cur = pMartix[j].stPinyinList[i].nMatchItemBegin;
					do{
						if (!pGraph->stMathItem[cur].nNodeValid)
						{
							printf("         ");
						}
						else 
						{
							printf("<%2d%6s", pGraph->stMathItem[cur].nPrevCol, 
								PYHan_GetNodeString(pGraph->pHan, pGraph->stMathItem[cur].nNodeId));
						}

						cur = pGraph->stMathItem[cur].nNextItem;
					} while (cur != 0);
				}
				else
				{
					printf("         ");
				}

				printf("|");
			}
			printf("\n");
		}
	}
	printf("\n==========");
	for (j = 1; j <= nNodeTotal; j++)
	{
		printf("==========");
	}
	printf("\n");

	printf("\n");
    return 0;
}
