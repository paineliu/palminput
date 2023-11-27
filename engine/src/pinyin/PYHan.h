#ifndef __PY_HAN_H__
#define __PY_HAN_H__

#include "PYPriDef.h"
#include "PYHanData.h"

typedef struct _PY_HAN
{
	const PY_HAN_DATA* pData;
	PY_UINT32 nFuzzyFlags;
	PY_UINT8  nNodeEnabled[PY_NODE_PINYIN_TOTAL];

} PY_HAN;


#ifdef __cplusplus
extern "C" {
#endif

	/** 初始化函数

	PYHan初始化的时需要传入汉字拼音编码数据

	@param pHan     PYHan对象指针
	@param pBlock   汉字拼音编码地址和大小

	@return 初始化结果
	@retval PY_TRUE     初始化成功
	@retval PY_FALSE    初始化失败
	*/
	PY_BOOL   PYHan_Initialize(PY_HAN* pHan, const PY_HAN_DATA* pData);

	PY_VOID   PYHan_Terminate(PY_HAN* pHan);

	/**取得拼音最大长度

	@param pHan     PYHan对象指针

	@return 拼音最大长度
	*/
	PY_UINT16 PYHan_GetPinyinMaxLen(PY_HAN* pHan);

	/**取得拼音总数

	@param pHan     PYHan对象指针

	@return 拼音总数
	*/
	PY_UINT16 PYHan_GetPinyinTotal(PY_HAN* pHan);

	PY_PCSTR  PYHan_GetPinyinString(PY_HAN* pHan, PY_UINT16 nPinyinId);
	PY_UINT16 PYHan_GetPinyinStringLen(PY_HAN* pHan, PY_UINT16 nPinyinId);
	PY_UINT16 PYHan_GetPinyinId(PY_HAN* pHan, PY_PCSTR pPinyinString, PY_UINT16 nPinyinLen);


	// Han - Pinyin

	PY_UINT16 PYHan_GetHanTotal(PY_HAN* pHan);
	//PY_UINT16 PYHan_GetHanPinyinId(PY_HAN* pHan, PY_UINT16 nHanId);
#define PYHan_GetHanPinyinId(pHan, nHanId) ((pHan)->pData->pHanItem[(nHanId)].nPinyinId)

//	inline PY_UINT16 PYHan_GetHanPinyinId(PY_HAN* pHan, PY_UINT16 nHanId)
//	{
//		return  pHan->pData->pHanItem[nHanId].nPinyinId;
//	}

	PY_UINT16 PYHan_GetHanUnicode(PY_HAN* pHan, PY_UINT16 nHanId);
	PY_UINT16 PYHan_GetHanUtf8(PY_HAN* pHan, PY_UINT16 nHanId, PY_PSTR pUtf8String, PY_UINT16 nUtf8MaxLen);

	// Encode

	PY_UINT16 PYHan_GetNodeSegmentTotal(PY_HAN* pHan);

	const PY_RANGE16* PYHan_GetNodeSegmentRange(PY_HAN* pHan, PY_UINT16 nNodeLen);

	PY_UINT16 PYHan_GetNodeTotal(PY_HAN* pHan);

	
	PY_UINT16 PYHan_GetNodeId(PY_HAN* pHan, PY_PCSTR pNodeString, PY_UINT16 nNodeLen);

	const PY_NODE_ITEM* PYHan_GetNodeItem(PY_HAN* pHan, PY_UINT16 nNodeId);

	//PY_PCSTR  PYHan_GetNodeString(PY_HAN* pHan, PY_UINT16 nNodeId);

//	inline const PY_NODE_ITEM* PYHan_GetNodeItem(PY_HAN* pHan, PY_UINT16 nNodeId)
//	{
//		return &pHan->pData->pNodeItem[nNodeId];
//	}

    PY_PCSTR  PYHan_GetNodeString(PY_HAN* pHan, PY_UINT16 nNodeId);
//#define PYHan_GetNodeString(pHan, nNodeId) \ 
//	(&((pHan)->pData->pNodeStr[(pHan)->pData->pNodeItem[(nNodeId)].stString.nOffset]))

//	inline PY_PCSTR  PYHan_GetNodeString(PY_HAN* pHan, PY_UINT16 nNodeId)
//	{
//		return &pHan->pData->pNodeStr[pHan->pData->pNodeItem[nNodeId].stString.nOffset];
//	}

	PY_UINT16 PYHan_GetNodeStringLen(PY_HAN* pHan, PY_UINT16 nNodeId);

	const PY_RANGE16* PYHan_GetNodePinyinRange(PY_HAN* pHan, PY_UINT16 nNodeLen);


	const PY_NODE_PINYIN* PYHan_GetNodePinyinItemValid(PY_HAN* pHan, PY_UINT16 nNodePinyinId);

	const PY_NODE_PINYIN* PYHan_GetNodePinyinItem(PY_HAN* pHan, PY_UINT16 nNodePinyinId);

	PY_BOOL PYHan_IsEqual(PY_HAN* pHan, const PY_UINT16* pSrcString, const PY_UINT16* pDstString, PY_UINT16 nHanLen);

	PY_UINT16 PYHan_DecodePhrase(PY_HAN* pHan, const PY_UINT16* pHanString, PY_UINT16 nHanLen, PY_PWSTR pUnicodeString, PY_UINT16 nUnicodeMaxLen);
	PY_UINT16 PYHan_DecodePhraseUtf8(PY_HAN* pHan, const PY_UINT16* pHanString, PY_UINT16 nHanLen, PY_PSTR pUtf8String, PY_UINT16 nUtf8MaxLen);

	PY_BOOL PYHan_Decode(PY_HAN* pHan, const PY_UINT16* pHanString, PY_UINT16 nHanLen, PY_PWSTR pUnicodeString,
		PY_UINT16* pUnicodeLen, PY_PWSTR pPinyinString, PY_UINT16* pPinyinLen, PY_WCHAR cPinyinSplit);


	PY_BOOL PYHan_DecodeUtf8(PY_HAN* pHan, const PY_UINT16* pHanString, PY_UINT16 nHanLen, PY_PSTR pUtf8String,
		PY_UINT16* pUtf8Len, PY_PSTR pPinyinString, PY_UINT16* pPinyinLen, PY_CHAR cPinyinSplit);

	PY_UINT16 PYHan_Encode(PY_HAN* pHan, PY_UINT16* pHanString, PY_UINT16 nHanMaxLen, PY_PCWSTR pUnicodeString,
		PY_UINT16 nUnicodeLen, PY_PCWSTR pPinyinString, PY_UINT16 nPinyinLen, PY_WCHAR cPinyinSplit);

	PY_UINT16 PYHan_EncodeUtf8(PY_HAN* pHan, PY_UINT16* pHanString, PY_UINT16 nHanMaxLen, PY_PCSTR pUtf8String,
		PY_UINT16 nUtf8Len, PY_PCSTR pPinyinString, PY_UINT16 nPinyinLen, PY_CHAR cPinyinSplit);

	// Group

	PY_UINT16 PYHan_GetHashGroupTotal(PY_HAN* pHan);
	PY_UINT16 PYHan_GetHashGroup(PY_HAN* pHan, PY_UINT16* pHanString, PY_UINT16 nHanLen);

	PY_UINT32 PYHan_GetPinyinGroup1(PY_HAN* pHan, PY_UINT16* pHanString, PY_UINT16 nHanLen);
	PY_UINT32 PYHan_GetPinyinGroup1Total(PY_HAN* pHan);

	PY_UINT32 PYHan_GetPinyinGroup2(PY_HAN* pHan, PY_UINT16* pHanString, PY_UINT16 nHanLen);
	PY_UINT32 PYHan_GetPinyinGroup2ByPinyinId(PY_HAN* pHan, PY_UINT16 nPinyin1Id, PY_UINT16 nPinyin2Id);
	PY_UINT32 PYHan_GetPinyinGroup2Total(PY_HAN* pHan);

	// Fuzzy

	PY_BOOL   PYHan_SetFuzzy(PY_HAN* pHan, PY_UINT32 nFuzzyFlags);
	PY_UINT32 PYHan_GetFuzzy(PY_HAN* pHan);

	// Key

	PY_UINT16    PYHan_GetKeyTotal(PY_HAN* pHan);
	const PY_KEY_ITEM* PYHan_GetKeyItem(PY_HAN* pHan, PY_UINT16 nIndex);

	PY_BOOL      PYHan_GetKeyRange(PY_HAN* pHan, PY_INT16 nKeyId, PY_RANGE16* pRange);
	// Debug

	PY_PCSTR  PYHan_GetNodeTypeDescribe(PY_UINT16 eNodeType);

	PY_PCSTR  PYHan_GetHanDescribe(PY_HAN* pHan, PY_UINT16* pHanString, PY_UINT8 nHanLen);
	PY_VOID PYHan_Print(PY_HAN* pHan, PY_UINT16* pHanString, PY_UINT8 nHanLen);

#ifdef __cplusplus
}
#endif

#endif
