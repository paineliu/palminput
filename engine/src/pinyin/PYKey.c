#include "PYKey.h"

char g_szCor[][2][12] =
{
	{"qv", "qu"},
	{"jv", "ju"},
	{"xv", "xu"}
};

PY_BOOL PYKey_Initialize(PY_KEY* pKey, PY_HAN* pHan)
{
	pKey->pHan = pHan;

	PYKey_Reset(pKey);

	return PY_TRUE;
}

PY_VOID PYKey_Terminate(PY_KEY* pKey)
{

}

PY_VOID PYKey_Reset(PY_KEY* pKey)
{
	pKey->nKeyLen = 0;
	pKey->nMatchNodeTotal = 0;
}

PY_BOOL PYKey_IsMatch(PY_KEY* pKey, PY_UINT16 nLastCol, PY_PCSTR pNodeString, PY_UINT16 nNodeLen)
{
	PY_UINT16 i;
	
	if (nNodeLen <= nLastCol)
	{
		for (i = 0; i < nNodeLen; i++)
		{
			if (pKey->nKeyMap[nLastCol - nNodeLen + i + 1][pNodeString[i]] != 1)
			{
				return PY_FALSE;
			}
		}

		return PY_TRUE;
	}

	return PY_FALSE;
}

PY_UINT16  PYKey_GenNodeList(PY_KEY* pKey, PY_UINT16 nCol)
{
	PY_INT16 i;
	PY_RANGE16 stRange;

	PY_HAN* pHan = pKey->pHan;
	PY_INT16 nPinyinMaxLen = PYHan_GetPinyinMaxLen(pHan);
	PY_UINT16 nNodeTotal = PYHan_GetNodeTotal(pHan);
	PY_memset(pKey->nKeyMap[nCol], 0, PY_CHAR_MAX_ID);
	pKey->nMatchNodeTotal = 0;

	// 标记当前列下有效节点（列从1开始）

	if (PYHan_GetKeyRange(pHan, pKey->nKeyList[nCol], &stRange))
	{
		for (i = stRange.nBegin; i < stRange.nEnd; i++)
		{
			pKey->nKeyMap[nCol][PYHan_GetKeyItem(pHan, i)->nNode] = 1;
		}
	}

	// 添加有效节点到当前列

	for (i = 0; i < nNodeTotal; i++)
	{
		if (PYKey_IsMatch(pKey, nCol, PYHan_GetNodeString(pHan, i), PYHan_GetNodeStringLen(pHan, i)))
		{
			pKey->stMatchNodeList[pKey->nMatchNodeTotal].nNodeId = i;
			pKey->stMatchNodeList[pKey->nMatchNodeTotal].pMatchString = PYHan_GetNodeString(pHan, i);
			pKey->stMatchNodeList[pKey->nMatchNodeTotal].nMatchLen = PYHan_GetNodeStringLen(pHan, i);

			pKey->nMatchNodeTotal++;
		}
	}

	// 添加纠错拼音节点到当前列

	for (i = 0; i < 3; i++)
	{
		if (PYKey_IsMatch(pKey, nCol, g_szCor[i][0], (PY_UINT16) strlen(g_szCor[i][0])))
		{
			pKey->stMatchNodeList[pKey->nMatchNodeTotal].nNodeId = PYHan_GetNodeId(pHan, g_szCor[i][1], (PY_UINT16) strlen(g_szCor[i][1]));
			pKey->stMatchNodeList[pKey->nMatchNodeTotal].pMatchString = g_szCor[i][0];
			pKey->stMatchNodeList[pKey->nMatchNodeTotal].nMatchLen = (PY_UINT8) strlen(g_szCor[i][0]);

			pKey->nMatchNodeTotal++;
		}

	}
	return pKey->nMatchNodeTotal;
}

PY_UINT16 PYKey_AddKey(PY_KEY* pKey, PY_UINT16 nKeyId)
{
	if (pKey->nKeyLen < PY_INPUT_MAX_LEN)
	{
		// 为了计算方便，有效数据列从1开始

		pKey->nKeyLen++;
		pKey->nKeyList[pKey->nKeyLen] = nKeyId;
		pKey->nKeyList[pKey->nKeyLen + 1] = 0;
		PYKey_GenNodeList(pKey, pKey->nKeyLen);
	}
	else
	{
		pKey->nMatchNodeTotal = 0;
	}

	return pKey->nKeyLen;
}

PY_UINT16  PYKey_DelLastKey(PY_KEY* pKey, PY_UINT16 nDelLen)
{
	if (pKey->nKeyLen > nDelLen)
	{
		pKey->nKeyLen -= nDelLen;
		pKey->nKeyList[pKey->nKeyLen + 1] = 0;
	}
	else
	{
		pKey->nKeyLen = 0;
	}

	return pKey->nKeyLen;
}

PY_UINT16* PYKey_GetKeyString(PY_KEY* pKey)
{
	return &pKey->nKeyList[1];
}

PY_UINT8  PYKey_GetKeyLen(PY_KEY* pKey)
{
	return pKey->nKeyLen;
}

PY_KEY_NODE* PYKey_GetMatchNodeList(PY_KEY* pKey)
{
	return pKey->stMatchNodeList;
}

PY_UINT16 PYKey_GetMatchNodeTotal(PY_KEY* pKey)
{
	return pKey->nMatchNodeTotal;
}
