#include "PYFix.h"

PY_FIX_ITEM g_szFix[] =
{
    {"dayu", 5, "", {'>', 0}},
    {"xiaoyu", 5, "", {'<', 0}},
    {"dengyu", 5, "", {'=', 0}},
    {"budengyu", 5, "", {'=', '=', 0}}
};

PY_BOOL PYFix_Initialize(PY_FIX* pFix)
{
	pFix->nCandTotal = 0;

	return PY_TRUE;
}

PY_VOID PYFix_Reset(PY_FIX* pFix)
{
	pFix->nCandTotal = 0;
}

PY_UINT32 PYFix_Search(PY_FIX* pFix, PY_UINT16* pKeyString, PY_UINT16 nKeyLen)
{
	PY_UINT32 i;
	pFix->nCandTotal = 0;
	for (i = 0; i < 4; i++)
	{
		if (PY_wcsstrcmp(pKeyString, g_szFix[i].szKey) == 0)
		{
			pFix->nCandItem[pFix->nCandTotal++] = i;
		}
	}

	return pFix->nCandTotal;
}

PY_UINT32 PYFix_GetTotal(PY_FIX* pFix)
{
	return pFix->nCandTotal;
}

PY_UINT16* PYFix_GetPhraseDataPtr(PY_FIX* pFix, PY_UINT32 nItemId)
{
	return g_szFix[pFix->nCandItem[nItemId]].szValue;
}

PY_UINT16  PYFix_GetPhrasePos(PY_FIX* pFix, PY_UINT32 nItemId)
{
	return g_szFix[pFix->nCandItem[nItemId]].nPos;
}

PY_UINT8   PYFix_GetPhraseLength(PY_FIX* pFix, PY_UINT32 nItemId)
{
	return PY_wcslen(g_szFix[pFix->nCandItem[nItemId]].szValue);
}

PY_VOID PYFix_Terminate(PY_FIX* pFix)
{

}
