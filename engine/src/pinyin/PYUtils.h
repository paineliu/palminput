/**
@file       PYUtils.h
@brief      ���ߺ���
*/

#ifndef __PY_UTILS_H__
#define __PY_UTILS_H__

#include <string.h>
#include "PYTypeDef.h"

#define PY_ASSERT(x)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

PY_UINT32 PY_wcslen(PY_PCWSTR pSrc);

PY_PWSTR PY_wcscpy(PY_PWSTR pDst, PY_PCWSTR pSrc);

PY_PWSTR PY_wcscat(PY_PWSTR pDst, PY_PCWSTR pSrc);

PY_INT32 PY_wcscmp(PY_PCWSTR pSrc, PY_PCWSTR pDst);

PY_INT32 PY_wcsstrcmp(PY_PCWSTR pSrc, PY_PCSTR pDst);

PY_INT32 PY_strcmp(PY_PCSTR pSrc, PY_PCSTR pDst);

PY_PWSTR PY_swncpy(PY_PWSTR pDst, PY_PCSTR pSrc, PY_UINT32 nCount);
PY_PSTR PY_strcpy(PY_PSTR pDst, PY_PCSTR pSrc);
PY_PSTR PY_strncpy(PY_PSTR pDst, PY_PCSTR pSrc, PY_UINT32 nCount);

PY_PWSTR PY_wcsncpy(PY_PWSTR pDst, PY_PCWSTR pSrc, PY_UINT32 nCount);

PY_INT32 PY_strncmp(PY_PCSTR pSrc, PY_PCSTR pDst, PY_UINT32 nCount);

PY_INT32 PY_wcsncmp(PY_PCWSTR pSrc, PY_PCWSTR pDst, PY_UINT32 nCount);

PY_PSTR PY_stoupper(PY_PSTR pStr);

PY_PWSTR PY_tolower(PY_PWSTR pStr);

PY_PWSTR PY_toupper(PY_PWSTR pStr);

#define PY_ISUPPER(c) ((c)>='A'&&(c)<='Z')
#define PY_TOUPPER(c) ((c) - 0x20)

#define PY_ISLOWER(c) ((c)>='a'&&(c)<='z')

#define PY_ISLETTER(c) (PY_ISUPPER(c) || PY_ISLOWER(c))

PY_BOOL PY_isletter(PY_PWSTR pStr, PY_UINT32 nLen);

#define PY_ISHANUPPER(c) ((c)>=0x25&&(c)<=0x3e)
#define PY_TOHANUPPER(c) ((c) + (0x25 - 0x0b))

#define PY_ISHANLOWER(c) ((c)>=0x0b&&(c)<=0x24)
#define PY_ISHANLETTER(c) (PY_ISHANUPPER(c) || PY_ISHANLOWER(c))

PY_BOOL PY_ishanletter(PY_PWSTR pStr, PY_UINT32 nLen);

PY_BOOL PY_existhanletter(PY_PWSTR pStr, PY_UINT32 nLen);


#define PY_memzero(pDst, nCount) memset((pDst), 0, (nCount))

// PY_PVOID PY_memzero(PY_PVOID pDst, PY_UINT32 nCount);

PY_PVOID PY_memcpy(PY_PVOID pDst, PY_PCVOID pSrc, PY_UINT32 nCount);

PY_PVOID PY_memmove(PY_PVOID pDst, PY_PCVOID pSrc, PY_UINT32 nCount);
        
PY_PVOID PY_memset(PY_PVOID pDst, PY_INT32 nVal, PY_UINT32 nCount);

PY_INT32 PY_memcmp(PY_PVOID pBuffer1, PY_PCVOID pBuffer2, PY_UINT32 nCount);

double   PY_log10(double x);

typedef int (* PY_compare)(const void* elem1, const void* elem2, void *arg);

void PY_quicksort(void *const pbase, PY_UINT32 total_elems, PY_UINT32  size, PY_compare cmp, void *arg);

PY_PVOID PY_binsearch(const PY_VOID *key, const PY_VOID *base, PY_UINT32 num, PY_UINT32 width, PY_compare cmp, void *arg);

PY_PVOID PY_binsearch_nocmp(const PY_VOID *key, const PY_VOID *base, PY_UINT32 num, PY_UINT32 width);

PY_UINT32 PY_binsearchmin(const PY_VOID *key, const PY_VOID *base, PY_UINT32 num, PY_UINT32 width, PY_compare cmp, void *arg);

PY_UINT32 PY_binsearchmin_nocmp(const PY_VOID *key, const PY_VOID *base, PY_UINT32 num, PY_UINT32 width);

PY_UINT32 PY_binsearchmax(const PY_VOID *key, const PY_VOID *base, PY_UINT32 num, PY_UINT32 width, PY_compare cmp, void *arg);

PY_INT32 PY_utf16_to_utf8_one(PY_UINT32 nUniCode, PY_CHAR *pUTF8Char, PY_INT32 nMaxSize);
PY_INT32 PY_utf16_to_utf8(PY_UINT16* utf16, PY_INT32 utf16_len, PY_CHAR* utf8, PY_INT32 utf8_max_len);
PY_INT32 PY_utf8_to_utf16(PY_PCSTR utf8, PY_INT32 utf8_len, PY_UINT16 *utf16, PY_INT32 utf16_max_len);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __PY_UTILS_H__
