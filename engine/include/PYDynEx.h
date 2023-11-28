/**
@file       PYDynEx.h
@brief      用户词库创建接口
*/

#ifndef __PY_DYN_EX_H__
#define __PY_DYN_EX_H__

#include "PYTypeDef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


PY_UINT32  PYDyn_Create(PY_PCSTR pFileName, PY_UINT32 nFileSize);
PY_BOOL    PYDyn_CreateInBlock(PY_UINT32 nLexSize, PY_BLOCK* pBlock);
PY_UINT32  PYDyn_Resize(PY_PCSTR pFileName, PY_PCSTR pNewFileName, PY_UINT32 nNewFileSize);
PY_BOOL    PYDyn_DumpFile(PY_PCSTR pLexFileName, PY_PCSTR pTxtFileName);
PY_BOOL    PYDyn_SaveLex(PY_BLOCK* pBlock, const char* pFileName);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PY_DYN_EX_H__ */
