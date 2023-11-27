/**
@file       PYFix.h
@brief      固定短语
*/

#ifndef __PY_FIX_H__
#define __PY_FIX_H__

#include "PYPriDef.h"
#include "PYKey.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	typedef struct _PY_FIX_ITEM
	{
		char szKey[40];
		int nPos;
		char szCand[40];
		PY_UINT16 szValue[40];

	} PY_FIX_ITEM;

	#define PY_FIX_MAX_ITEM 4

	typedef struct _PY_FIX
	{
		PY_KEY* pKey;
		PY_UINT16 nCandTotal;
		PY_UINT32 nCandItem[PY_FIX_MAX_ITEM];

	} PY_FIX;

	PY_BOOL PYFix_Initialize(PY_FIX* pFix);
	
	PY_VOID PYFix_Reset(PY_FIX* pFix);

	PY_UINT32 PYFix_Search(PY_FIX* pFix, PY_UINT16* pKeyString, PY_UINT16 nKeyLen);

	PY_UINT32 PYFix_GetTotal(PY_FIX* pFix);

	PY_UINT16* PYFix_GetPhraseDataPtr(PY_FIX* pFix, PY_UINT32 nItemId);
	PY_UINT16  PYFix_GetPhrasePos(PY_FIX* pFix, PY_UINT32 nItemId);
	PY_UINT8   PYFix_GetPhraseLength(PY_FIX* pFix, PY_UINT32 nItemId);


	PY_VOID PYFix_Terminate(PY_FIX* pFix);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PY_FIX_H__ */
