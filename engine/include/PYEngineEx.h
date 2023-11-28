#ifndef __PY_ENGINE_EX_H__
#define __PY_ENGINE_EX_H__

#include "PYEngine.h"

#ifdef __cplusplus
extern "C" {
#endif

PY_HANDLE PYEngine_New(PY_PCSTR pPathName);

PY_VOID PYEngine_Free(PY_HANDLE hEngine);

PY_UINT16 PYEngine_PrepareCand(PY_HANDLE hEngine);


#ifdef __cplusplus
}
#endif

#endif
