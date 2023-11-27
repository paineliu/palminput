/** 
@file       PYLexEx.h
@brief      Pinyin lex write inferface
*/

#ifndef __PY_LEX_EX_H__
#define __PY_LEX_EX_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int PYLex_LexMaker(int nLexType, const char* pTxtFileName, const char* pOutPathName, const char* pCppFileName);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PY_LEX_EX_H__ */
