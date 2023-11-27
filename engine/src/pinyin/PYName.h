/**
@file       PYName.h
@brief      人名候选
*/

#ifndef __PY_NAME_H__
#define __PY_NAME_H__

#include "PYPriDef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	typedef struct _PY_NAME
	{
		PY_HAN* pHan;

	} PY_NAME;

	PY_BOOL PYName_Initialize(PY_NAME* pName, PY_HAN* pHan);

	PY_VOID PYName_Terminate(PY_NAME* pName);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PY_NAME_H__ */
