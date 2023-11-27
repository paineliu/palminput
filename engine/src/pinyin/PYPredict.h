/**
@file       PYPredict.h
@brief      联想候选
*/

#ifndef __PY_PREDICT_H__
#define __PY_PREDICT_H__

#include "PYPriDef.h"
#include "PYDyn.h"
#include "PYLex.h"
#include "PYCand.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	typedef struct _PY_PREDICT
	{
		PY_HAN* pHan;
		PY_LEX* pCoreLex;
		PY_LEX* pTerm1Lex;
		PY_UINT16 nTerm1LexTotal;
		PY_LEX* pTerm2Lex;
		PY_UINT16 nTerm2LexTotal;
		PY_DYN* pUserDyn;

		PY_CAND_ITEM nCandItem[PY_PREDICT_ITEM_MAX_NUM];
		PY_UINT32 nCandItemTotal;
		PY_UINT16 nSelItemStr[64];
		PY_UINT16 nSelItemLen;
	} PY_PREDICT;

	PY_BOOL PYPredict_Initialize(PY_PREDICT* pPredict, PY_HAN* pHan, PY_LEX* pCoreLex, PY_DYN* pUserDyn, PY_LEX* pTerm1Lex, PY_UINT16 nTerm1Total, PY_LEX* pTerm2Lex, PY_UINT16 nTerm2Total);

	PY_VOID PYPredict_Reset(PY_PREDICT* pPredict);

	PY_UINT16 PYPredict_Search(PY_PREDICT* pPredict, PY_UINT16* pHanString, PY_UINT16 nHanLen);

	PY_UINT32 PYPredict_GetTotal(PY_PREDICT* pPredict);

	PY_UINT16 PYPredict_GetItemString(PY_PREDICT* pPredict, PY_UINT16 nItemId, PY_PWSTR pCandString, PY_UINT16 nCandMaxLen);

	PY_UINT16 PYPredict_GetItemStringUtf8(PY_PREDICT* pPredict, PY_UINT16 nItemId, PY_PSTR pCandString, PY_UINT16 nCandMaxLen);

	PY_UINT16 PYPredict_SelItem(PY_PREDICT* pPredict, PY_UINT16 nItemId);
	PY_UINT16* PYPredict_GetSelString(PY_PREDICT* pPredict);
	PY_UINT16 PYPredict_GetSelStringLenght(PY_PREDICT* pPredict);

	PY_VOID PYPredict_Terminate(PY_PREDICT* pPredict);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PY_NAME_H__ */
