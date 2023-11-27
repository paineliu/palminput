/**
@file       PYDyn.h
@brief      用户词库读取接口
*/

#ifndef __PY_DYN_H__
#define __PY_DYN_H__

#include "PYPriDef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	typedef struct _PY_DYN_SUMMARY
	{
		PY_UINT32  nPhraseTotal;
		PY_UINT32  nFreqTotal;
		PY_UINT32  nPhraseCapacity;
		PY_UINT32  nBufferSize;

	} PY_DYN_SUMMARY;

	typedef struct _PY_DYN_GROUP
	{
		PY_UINT32  nFirst;
		PY_UINT32  nLast;

	} PY_DYN_GROUP;

	typedef struct _PY_DYN_PHRASE
	{
		PY_UINT32  nFreq;
		PY_UINT32  nLastStamp;
		PY_UINT8   nLength;
		PY_UINT8   nDel;
		PY_UINT16  nReserved;
		PY_UINT32  nNext;
		PY_UINT16  nPhraseString[PY_DYN_PHRASE_MAX_LEN];
	} PY_DYN_PHRASE;

	typedef struct _PY_DYN_DATA
	{
		PY_DYN_SUMMARY *pSummary;
		PY_DYN_GROUP *pGroup;
		PY_BYTE* pBuffer;
		PY_UINT16* pPhraseData;
		PY_DYN_PHRASE* pPhraseItem;

	} PY_DYN_DATA;

	typedef struct _PY_DYN
	{
		PY_HAN* pHan;
		PY_DYN_DATA stData;
		
	} PY_DYN;

	/** 初始化函数

	PYLex模需要使用PYHan中的函数，因此在初始化的时候需要传入PYHan指针，目前支持带索引和不带索引的拼音系统词库

	@param pUserDyn     PYLex对象指针
	@param pHan     PYHan对象指针
	@param pBlock   词库地址和大小

	@return 词库加载结果
	@retval PY_TRUE     词库加载成功
	@retval PY_FALSE    词库加载失败
	@pre file 必须使用OpenFile的返回值
	*/
	PY_BOOL PYDyn_Initialize(PY_DYN* pDyn, PY_HAN* pHan, const PY_BLOCK* pBlock);

	PY_UINT32  PYDyn_Create(PY_PCSTR pFileName, PY_UINT32 nFileSize);

	/**
	* @brief 释放Lex
	*
	* @retval 0     成功
	* @retval -1    失败
	* @pre file 必须使用OpenFile的返回值
	*/
	PY_VOID PYDyn_Terminate(PY_DYN* pDyn);
    
    PY_BOOL PYDyn_IsInit(PY_DYN *pDyn);

	PY_UINT32  PYDyn_AddPhrase(PY_DYN* pDyn, PY_UINT16* pPhraseString, PY_UINT16 nPhraseLen, PY_UINT32 nPhraseFreq);
	
	PY_UINT32  PYDyn_IncPhrase(PY_DYN* pDyn, PY_UINT32 nPhraseId);
	
	PY_BOOL    PYDyn_DelPhrase(PY_DYN* pDyn, PY_UINT32 nPhraseId);

	PY_UINT32  PYDyn_GetPhraseId(PY_DYN* pDyn, PY_UINT16* pPhraseString, PY_UINT16 nPhraseLen);

	PY_UINT32  PYDyn_GetGroupTotal(PY_DYN* pDyn);

	PY_UINT32  PYDyn_GetFirstPhraseId(PY_DYN* pDyn, PY_UINT32 nGroupId);

	PY_UINT32  PYDyn_GetNextPhraseId(PY_DYN* pDyn, PY_UINT32 nPhraseId);

	PY_DYN_PHRASE* PYDyn_GetPhrasePtr(PY_DYN* pDyn, PY_UINT32 nPhraseId);

	PY_UINT16* PYDyn_GetPhraseDataPtr(PY_DYN* pDyn, PY_UINT32 nPhraseId);

	PY_UINT16  PYDyn_GetPhraseWeight(PY_DYN* pDyn, PY_UINT32 nPhraseId);
	
	PY_UINT8   PYDyn_GetPhraseLength(PY_DYN* pDyn, PY_UINT32 nPhraseId);

	// for debug
	
	/** 获取词条总数

	@param pUserDyn     PYLex对象指针

	@return 词条总数
	*/
	PY_UINT32 PYDyn_GetPhraseTotal(PY_DYN* pDyn);
	PY_DYN_PHRASE* PYDyn_GetPhrasePtr(PY_DYN* pDyn, PY_UINT32 nPhraseId);

	PY_PCSTR   PYDyn_GetPhraseDescribe(PY_DYN* pDyn, PY_UINT32 nPhraseId);
	PY_BOOL PYDyn_GetDumpFile(PY_DYN* pDyn, PY_PCSTR pFileName);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PY_DYN_H__ */
