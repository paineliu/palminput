/**
@file       PYLex.h
@brief      词库读取接口
*/

#ifndef __PY_LEX_H__
#define __PY_LEX_H__

#include "PYPriDef.h"
#include "PYHan.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	typedef struct _PY_LEX_BIGRAM_PRE_WORD {
		PY_UINT32  nPhraseId;
		PY_UINT32  nPostWordEnd;
	} PY_LEX_BIGRAM_PRE_WORD;

	typedef struct _PY_LEX_BIGRAM_POST_WORD {
		PY_UINT32  nPhraseId;
		PY_UINT16  nWeight;
	} PY_LEX_BIGRAM_POST_WORD;

	typedef struct _PY_LEX_DATA
	{
		PY_UINT32  nGroupType;
		PY_UINT32  nPhraseTotal;

		PY_UINT16* pPhraseData;
		PY_UINT16* pPhraseWeight;
		PY_UINT32* pPyGroupHead;
		PY_UINT32* pPhraseId;

		// 排序

		PY_UINT32  nSortedPhraseTotal;
		PY_UINT32* pSortedPhraseId;

		// 二元

		PY_UINT32  nBiPrePhraseTotal;
		PY_UINT32  nBiPostPhraseTotal;
		PY_LEX_BIGRAM_PRE_WORD* pBiPrePhrase;
		PY_UINT32* pBiPostPhraseId;
		PY_UINT16* pBiPostPhraseWeight;

	} PY_LEX_DATA;

	typedef struct _PY_LEX
	{
		PY_HAN* pHan;
		PY_LEX_DATA stData;

	} PY_LEX;

	/** 初始化函数

	PYLex模需要使用PYHan中的函数，因此在初始化的时候需要传入PYHan指针，目前支持带索引和不带索引的拼音系统词库

	@param pCoreLex     PYLex对象指针
	@param pHan     PYHan对象指针
	@param pBlock   词库地址和大小

	@return 词库加载结果
	@retval PY_TRUE     词库加载成功
	@retval PY_FALSE    词库加载失败
	@pre file 必须使用OpenFile的返回值
	*/
	PY_BOOL PYLex_Initialize(PY_LEX* pLex, PY_HAN* pHan, const PY_BLOCK* pBlock);

	/**
	* @brief 释放Lex
	*
	* @retval 0     成功
	* @retval -1    失败
	* @pre file 必须使用OpenFile的返回值
	*/
	PY_VOID PYLex_Terminate(PY_LEX* pLex);

	/** 获取词条总数

	@param pCoreLex     PYLex对象指针

	@return 词条总数
	*/
	PY_UINT32  PYLex_GetTotal(PY_LEX* pLex);
	PY_UINT32  PYLex_GetItemPhraseId(PY_LEX* pLex, PY_INT32 nIndex);
	PY_UINT16  PYLex_GetItemWeight(PY_LEX* pLex, PY_INT32 nIndex);
	PY_UINT8   PYLex_GetItemLength(PY_LEX* pLex, PY_INT32 nIndex);
	PY_UINT16* PYLex_GetItemDataPtr(PY_LEX* pLex, PY_INT32 nIndex);


	PY_UINT32  PYLex_GetGroupTotal(PY_LEX* pLex);

	//PY_UINT32  PYLex_GetGroupItemTotal(PY_LEX* pLex, PY_INT32 nGroupId);
	//PY_UINT16  PYLex_GetGroupItemWeight(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex);
	//PY_UINT8   PYLex_GetGroupItemLength(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex);
	//PY_UINT16* PYLex_GetGroupItemDataPtr(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex);
	//PY_UINT32  PYLex_GetGroupItemPhraseId(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex);

	//PY_UINT32 PYLex_GetGroupItemTotal(PY_LEX* pLex, PY_INT32 nGroupId);
	#define PYLex_GetGroupItemTotal(pLex, nGroupId) \
((pLex)->stData.pPyGroupHead[(nGroupId) + 1] - (pLex)->stData.pPyGroupHead[(nGroupId)])


	//PY_UINT32 PYLex_GetGroupItemPhraseId(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex);
	#define PYLex_GetGroupItemPhraseId(pLex, nGroupId, nIndex) \
(pLex)->stData.pPhraseId[(pLex)->stData.pPyGroupHead[(nGroupId)] + (nIndex)]

//	inline PY_UINT32 PYLex_GetGroupItemPhraseId(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex)
//	{
//		return pLex->stData.pPhraseId[pLex->stData.pPyGroupHead[nGroupId] + nIndex];
//	}

	//PY_UINT16 PYLex_GetGroupItemWeight(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex);
#define PYLex_GetGroupItemWeight(pLex, nGroupId, nIndex) \
(pLex)->stData.pPhraseWeight[(pLex)->stData.pPhraseId[(pLex)->stData.pPyGroupHead[(nGroupId)] + (nIndex)] & 0x00FFFFFF]
//	inline PY_UINT16 PYLex_GetGroupItemWeight(PY_LEX* pLex, PY_UINT32 nGroupId, PY_INT32 nIndex)
//	{
//		PY_UINT32 nPhraseId = pLex->stData.pPhraseId[pLex->stData.pPyGroupHead[nGroupId] + nIndex];
//		return pLex->stData.pPhraseWeight[nPhraseId & 0x00FFFFFF];
//	}
#define PYLex_GetGroupItemLength(pLex, nGroupId, nIndex) \
((pLex)->stData.pPhraseId[(pLex)->stData.pPyGroupHead[(nGroupId)] + (nIndex)] >> 24)
//	PY_UINT8 PYLex_GetGroupItemLength(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex);
	//	inline PY_UINT8 PYLex_GetGroupItemLength(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex)
//	{
//		PY_UINT32 nPhraseId = pLex->stData.pPhraseId[pLex->stData.pPyGroupHead[nGroupId] + nIndex];
//		return nPhraseId >> 24;
//	}

#define PYLex_GetGroupItemDataPtr(pLex, nGroupId, nIndex) \
(&(pLex)->stData.pPhraseData[(pLex)->stData.pPhraseId[(pLex)->stData.pPyGroupHead[(nGroupId)] + (nIndex)] & 0x00FFFFFF])
	//PY_UINT16* PYLex_GetGroupItemDataPtr(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex);

//	inline PY_UINT16* PYLex_GetGroupItemDataPtr(PY_LEX* pLex, PY_INT32 nGroupId, PY_INT32 nIndex)
//	{
//		PY_UINT32 nPhraseId = pLex->stData.pPhraseId[pLex->stData.pPyGroupHead[nGroupId] + nIndex];
//		return &pLex->stData.pPhraseData[nPhraseId & 0x00FFFFFF];
//	}

	PY_UINT32 PYLex_GetSortedTotal(PY_LEX* pLex);
	PY_UINT32 PYLex_GetSortedItemPhraseId(PY_LEX* pLex, PY_INT32 nIndex);

	PY_UINT16* PYLex_GetPhraseDataPtr(PY_LEX* pLex, PY_UINT32 nPhraseId);
	PY_UINT16  PYLex_GetPhraseWeight(PY_LEX* pLex, PY_UINT32 nPhraseId);
	PY_UINT8   PYLex_GetPhraseLength(PY_LEX* pLex, PY_UINT32 nPhraseId);

	PY_UINT32 PYLex_GetSortedPhraseIdBeginPos(PY_LEX* pLex, PY_UINT16* pHanString, PY_UINT16 nHanLen);

	PY_PCSTR   PYLex_GetPhraseDescribe(PY_LEX* pLex, PY_UINT32 nPhraseId);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PY_LEX_H__ */
