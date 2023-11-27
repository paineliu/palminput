/*!
 * @file PYTypeDef.h
 * @brief Pinyin type define
 */

#ifndef __PY_TYPE_DEF_H__
#define __PY_TYPE_DEF_H__


typedef void*                      PY_PVOID;
typedef const void*                PY_PCVOID;

typedef signed char                PY_INT8;
typedef unsigned char              PY_UINT8;
typedef char                       PY_CHAR;
typedef unsigned char              PY_BYTE;
typedef unsigned short             PY_WCHAR;

typedef signed short               PY_INT16;
typedef unsigned short             PY_UINT16;
typedef float                      PY_FLOAT;
typedef double                     PY_DOUBLE;

typedef signed int                 PY_INT32;
typedef unsigned int               PY_UINT32;
typedef signed long long           PY_INT64;
typedef unsigned long long		   PY_UINT64;

typedef char*			           PY_PSTR;
typedef const char*		           PY_PCSTR;
typedef unsigned short*			   PY_PWSTR;
typedef const unsigned short*	   PY_PCWSTR;

typedef PY_INT32                   PY_BOOL;
typedef void *                     PY_HANDLE;

typedef struct _PY_BLOCK
{
	PY_UINT8     *pAddress;
	PY_UINT32    nSize;

} PY_BLOCK;

typedef struct _PY_OPTION
{
	PY_UINT32    nFuzzyFlags;
    PY_UINT32    bEnablePredict;

} PY_OPTION;

typedef struct _PY_SECTION
{
	PY_UINT16  nBegin;
	PY_UINT16  nEnd;

} PY_SECTION;

typedef struct _PY_RANGE
{
	PY_UINT32  nBegin; //start
	PY_UINT32  nEnd;   // stop

} PY_RANGE;


typedef struct _PY_RANGE16
{
	PY_UINT16  nBegin; // start
	PY_UINT16  nEnd;   // stop

} PY_RANGE16;

typedef struct _PY_RANGE8
{
	PY_UINT8  nBegin;
	PY_UINT8  nLen;
	PY_UINT8  nReserved[2];
} PY_RANGE8;


#define PY_CARET_MOVE_HOME          1
#define PY_CARET_MOVE_END           2
#define PY_CARET_MOVE_LEFT_CHAR     3
#define PY_CARET_MOVE_RIGHT_CHAR    4
#define PY_CARET_MOVE_LEFT_UNIT     5
#define PY_CARET_MOVE_RIGHT_UINT    6

#define PY_FILTER_NONE            0
#define PY_FILTER_SINGLE_WORD     1

#define PY_VOID					   void

#define PY_TRUE                    1
#define PY_FALSE                   0

#define PY_UNREFERENCED_PARAMETER(P)          \
	{ \
	(P) = (P); \
	}


#ifdef  __cplusplus
#define PY_NULL					   0
#else
#define PY_NULL                    ((void *) 0)
#endif

#endif
