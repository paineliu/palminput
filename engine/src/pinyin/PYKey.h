/**
@file       PYKey.h
@brief      按键到输入节点转换
*/

#ifndef __PY_KEY_H__
#define __PY_KEY_H__

#include "PYPriDef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PY_KEY_MATCH_NODE_MAX_NUM    256

	typedef struct _PY_KEY
	{
		PY_HAN* pHan;
		
		PY_UINT16 nKeyList[PY_INPUT_MAX_LEN + 1];
		PY_CHAR   nKeyMap[PY_INPUT_MAX_LEN + 1][PY_CHAR_MAX_ID + 1];
		PY_UINT8  nKeyLen;

		PY_KEY_NODE stMatchNodeList[PY_KEY_MATCH_NODE_MAX_NUM];
		PY_UINT16   nMatchNodeTotal;

	} PY_KEY;

	/** 初始化函数

	PYLex模需要使用PYHan中的函数，因此在初始化的时候需要传入PYHan指针，目前支持带索引和不带索引的拼音系统词库

	@param pLex     PYLex对象指针
	@param pHan     PYHan对象指针

	@return 词库加载结果
	@retval PY_TRUE     词库加载成功
	@retval PY_FALSE    词库加载失败
	@pre file 必须使用OpenFile的返回值
	*/
	PY_BOOL PYKey_Initialize(PY_KEY* pLex, PY_HAN* pHan);

	/**
	* @brief 释放Key
	*
	* @retval 0     成功
	* @retval -1    失败
	* @pre file 必须使用OpenFile的返回值
	*/
	PY_VOID PYKey_Terminate(PY_KEY* pKey);

	PY_VOID PYKey_Reset(PY_KEY* pKey);

	PY_UINT16  PYKey_AddKey(PY_KEY* pKey, PY_UINT16 nKeyId);

	PY_UINT16  PYKey_DelLastKey(PY_KEY* pKey, PY_UINT16 nDelLen);
	PY_UINT16*  PYKey_GetKeyString(PY_KEY* pKey);

	PY_UINT8  PYKey_GetKeyLen(PY_KEY* pKey);

	PY_KEY_NODE* PYKey_GetMatchNodeList(PY_KEY* pKey);
	
	PY_UINT16 PYKey_GetMatchNodeTotal(PY_KEY* pKey);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PY_KEY_H__ */
