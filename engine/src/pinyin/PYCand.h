#ifndef __PY_CAND_H__
#define __PY_CAND_H__

#include "PYPriDef.h"
#include "PYGraph.h"
#include "PYLex.h"
#include "PYDyn.h"
#include "PYKey.h"
#include "PYFix.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _PY_CAND_ITEM
{
	PY_UINT8   nPhraseType;
	PY_UINT32  nPhraseId;
	PY_UINT16* pPhraseString;
	PY_UINT16  nPhraseLen;
	PY_UINT16  nPhraseWeight;
	PY_UINT16  nLastPinyinId;
	PY_UINT8   nMatchBegin;
	PY_UINT8   nMatchLen;

} PY_CAND_ITEM;

typedef struct _PY_SEN_ITEM
{
	
	PY_UINT32  nMatchItemId;
	PY_UINT32  nPhraseId;
	struct _PY_SEN_ITEM *pPrevItem;
	PY_UINT32  nRoadWeight;

} PY_SEN_ITEM;

typedef struct _PY_SEL_ITEM
{

	PY_UINT32  nMatchItemId;

} PY_SEL_ITEM;


typedef struct _PY_CAND
{
	PY_LEX *pCoreLex;
	PY_LEX* pTerm1Lex;
	PY_UINT16 nTerm1LexTotal;
	PY_LEX* pTerm2Lex;
	PY_UINT16 nTerm2LexTotal;
	PY_DYN* pUserDyn;
	PY_HAN* pHan;
	PY_KEY* pKey;
	PY_GRAPH *pGraph;

	PY_UINT8 byCandUnicode[65536];
	PY_FIX stFix;
	PY_CAND_ITEM nCandItem[PY_CAND_ITEM_MAX_NUM];  // 存储候选最大项目 
	PY_UINT32 nCandItemTotal;
	PY_BOOL bSingleWord;

	PY_UINT8   nSelLen;

	PY_CAND_ITEM stSelItem[PY_INPUT_MAX_LEN];
	PY_UINT16   nSelItemTotal;

	PY_UINT16   nSenHanLen;
	PY_UINT16   nSenHanString[PY_INPUT_MAX_LEN + 1];

	PY_UINT32   nSenItemTotal[PY_INPUT_MAX_LEN];
	PY_SEN_ITEM stSenItem[PY_INPUT_MAX_LEN][PY_SEN_MAX_NUM];  // 存储候选最大项目

	PY_UINT32 nMatchItemTotal;
	PY_RANGE  stMatchItemRange[PY_INPUT_MAX_LEN];
	PY_CAND_ITEM stMatchItem[PY_MATCH_ITEM_MAX_NUM]; // 存储匹配最大项目

	PY_UINT16 nSelItemType;

} PY_CAND;

PY_BOOL   PYCand_Initialize(PY_CAND* pCand, PY_KEY* pKey, PY_HAN* pHan, PY_GRAPH* pGraph, PY_LEX* pCoreLex, PY_DYN* pUserDyn, PY_LEX* pTerm1Lex, PY_UINT16 nTerm1Total, PY_LEX* pTerm2Lex, PY_UINT16 nTerm2Total);

PY_VOID   PYCand_Terminate(PY_CAND* pCand);

PY_VOID   PYCand_Reset(PY_CAND* pCand);

PY_BOOL   PYCand_AddKey(PY_CAND* pCand, PY_UINT32 nKey, PY_UINT32 nKeyFlags);
PY_BOOL   PYCand_SetSingleWord(PY_CAND* pCand, PY_BOOL bSingleWord);

PY_UINT16 PYCand_PrepareCand(PY_CAND* pCand);
PY_BOOL   PYCand_SetColTotal(PY_CAND* pCand, PY_UINT16 nCol);
PY_UINT16 PYCand_GetSelLen(PY_CAND* pCand);
PY_UINT16 PYCand_GetSelFixLen(PY_CAND* pCand);
PY_UINT16 PYCand_GetSelString(PY_CAND* pCand, PY_PWSTR pSelString, PY_UINT16 nSelMaxLen);

PY_UINT16 PYCand_GetSelHanString(PY_CAND* pCand, PY_PWSTR pSelString, PY_UINT16 nSelMaxLen);

PY_UINT32 PYCand_GetTotal(PY_CAND* pCand);

PY_UINT16 PYCand_UnselLastItem(PY_CAND* pCand);

PY_UINT16 PYCand_GetItemCompString(PY_CAND* pCand, PY_UINT16 nItemId, PY_PWSTR pCandString, PY_UINT16 nCandMaxLen, PY_BOOL bSplit);

PY_UINT16 PYCand_GetSelItemType(PY_CAND* pCand);

PY_UINT16 PYCand_GetItemString(PY_CAND* pCand, PY_UINT16 nItemId, PY_PWSTR pCandString, PY_UINT16 nCandMaxLen);

// PY_UINT16 PYCand_GetItemStringUtf8(PY_CAND* pCand, PY_UINT16 nItemId, PY_PSTR pCandString, PY_UINT16 nCandMaxLen);

PY_UINT16 PYCand_SelItem(PY_CAND* pCand, PY_UINT16 nItemId);

PY_VOID   PYCand_Match(PY_CAND* pCand, PY_UINT8 nLexId, PY_UINT32 nPhraseId, PY_UINT16 *pPhraseString, PY_UINT16 nPhraseLen, PY_UINT16 nPhraseWeight);

#ifdef __cplusplus
}
#endif

#endif
