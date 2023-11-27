#ifndef __PY_ENGINE_H__
#define __PY_ENGINE_H__

#include "PYTypeDef.h"

//-------------------------------------------------------------------
// API Export/Import Macros
//-------------------------------------------------------------------
/** Indicates an exported and imported shared library function. */
#define PY_DLL_EXPORT

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief 获取引擎版本
*
* @return 返回版本描述
*/
PY_PCSTR  PYEngine_GetVersion(PY_VOID);

PY_UINT32 PYEngine_GetInstanceSize(PY_VOID);

PY_HANDLE PYEngine_Initialize(PY_VOID *pInstBuffer, PY_UINT32 nInstSize, const PY_BLOCK *pDictList, PY_UINT16 nDictTotal);

PY_VOID PYEngine_Terminate(PY_HANDLE hEngine);

PY_VOID PYEngine_Reset(PY_HANDLE hEngine);

// 选项

PY_BOOL PYEngine_SetOption(PY_HANDLE hEngine, const PY_OPTION *pOption);

const PY_OPTION* PYEngine_GetOption(PY_HANDLE hEngine);

// 按键

PY_BOOL PYEngine_KeyEvent(PY_HANDLE hEngine, PY_UINT16 nKeyId, PY_UINT16 nFlags);

PY_BOOL PYEngine_SetKeys(PY_HANDLE hEngine, PY_PCSTR pKeyString, PY_UINT16 nKeyLen);

// 输入串

PY_UINT16 PYEngine_GetKeyStringUtf8(PY_HANDLE hEngine, PY_CHAR* pKeyString, PY_UINT16 nKeyMaxLen);

PY_UINT16 PYEngine_GetKeyString(PY_HANDLE hEngine, PY_PWSTR pKeyString, PY_UINT16 nKeyMaxLen);

PY_UINT16 PYEngine_GetKeyStringLen(PY_HANDLE hEngine);

// 写作串

PY_UINT16 PYEngine_GetCompStringUtf8(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PSTR pCompString, PY_UINT16 nCompMaxLen, PY_UINT16 *pCaretPos, PY_BOOL bSplit);

PY_UINT16 PYEngine_GetCompString(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PWSTR pCompString, PY_UINT16 nCompMaxLen, PY_UINT16 *pCaretPos, PY_BOOL bSplit);

// 结果串

PY_UINT16 PYEngine_GetConvertedStringUtf8(PY_HANDLE hEngine, PY_PSTR pConvertedString, PY_UINT16 nConvertedMaxLen);

PY_UINT16 PYEngine_GetConvertedString(PY_HANDLE hEngine, PY_PWSTR pConvertedString, PY_UINT16 nConvertedMaxLen);

// 汉字候选

PY_UINT16 PYEngine_GetCandTotal(PY_HANDLE hEngine);

PY_UINT16 PYEngine_GetCandItemStringUtf8(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PSTR pCandString, PY_UINT16 nCandMaxLen);

PY_UINT16 PYEngine_GetCandItemString(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PWSTR pCandString, PY_UINT16 nCandMaxLen);

PY_UINT16 PYEngine_SelCandItem(PY_HANDLE hEngine, PY_UINT16 nItemId);

// Page

PY_UINT16 PYEngine_SetPageMaxItem(PY_HANDLE hEngine, PY_UINT16 nPageMaxNum);

PY_UINT16 PYEngine_GetPageMaxItem(PY_HANDLE hEngine);

PY_UINT16 PYEngine_GetPageTotal(PY_HANDLE hEngine);

PY_UINT16 PYEngine_GetPageItemTotal(PY_HANDLE hEngine, PY_UINT16 nPageId);

PY_UINT16 PYEngine_GetPageItemStringUtf8(PY_HANDLE hEngine, PY_UINT16 nPageId, PY_UINT16 nItemId, PY_PSTR pPhraseString, PY_UINT16 nPhraseMaxLen);

PY_UINT16 PYEngine_GetPageItemString(PY_HANDLE hEngine, PY_UINT16 nPageId, PY_UINT16 nItemId, PY_PWSTR pPhraseString, PY_UINT16 nPhraseMaxLen);

PY_UINT16 PYEngine_SelPageItem(PY_HANDLE hEngine, PY_UINT16 nPageId, PY_UINT16 nItemId);

PY_BOOL PYEngine_HasPrevPage(PY_HANDLE hEngine);
PY_BOOL PYEngine_HasNextPage(PY_HANDLE hEngine);
PY_BOOL PYEngine_TurnPrevPage(PY_HANDLE hEngine);
PY_BOOL PYEngine_TurnNextPage(PY_HANDLE hEngine);
PY_UINT16 PYEngine_GetCurrPageItemTotal(PY_HANDLE hEngine);
PY_UINT16 PYEngine_GetCurrPageItemStringUtf8(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PSTR pPhraseString, PY_UINT16 nPhraseMaxLen);
PY_UINT16 PYEngine_GetCurrPageItemString(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PWSTR pPhraseString, PY_UINT16 nPhraseMaxLen);

PY_UINT16 PYEngine_SelCurrPageItem(PY_HANDLE hEngine, PY_UINT16 nItemId);

// 拼音候选

PY_UINT16 PYEngine_GetPinyinTotal(PY_HANDLE hEngine);

PY_UINT16 PYEngine_GetPinyinItemStringUtf8(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PSTR pPinyinString, PY_UINT16 nPinyinMaxLen);

PY_UINT16 PYEngine_GetPinyinItemString(PY_HANDLE hEngine, PY_UINT16 nItemId, PY_PWSTR pPinyinString, PY_UINT16 nPinyinMaxLen);

PY_UINT16 PYEngine_SelPinyinItem(PY_HANDLE hEngine, PY_UINT16 nItemId);

// 移动光标

PY_UINT16 PYEngine_MoveCaret(PY_HANDLE hEngine, PY_UINT16 nCaretPos);

// 候选过滤-单字模式

PY_UINT16 PYEngine_GetFilterType(PY_HANDLE hEngine);

PY_UINT16 PYEngine_SetFilterType(PY_HANDLE hEngine, PY_UINT16 nFilterType);


#ifdef __cplusplus
}
#endif

#endif
