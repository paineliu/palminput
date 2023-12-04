#ifndef __PY_ADAPTER_H__
#define __PY_ADAPTER_H__

#include <jni.h>
#include <android/asset_manager.h>
#include <PYEngine.h>


#ifdef __cplusplus
extern "C"{
#endif

void* PYAdapter_Create(AAssetManager* assetManager, const char* dictPathname, const char* userPathname);

int PYAdapter_SetOptions(void *adapter, const PY_OPTION *pOption);

const PY_OPTION * PYAdapter_GetOptions(void *adapter);

void PYAdapter_Reset(void *adapter);
int PYAdapter_AddKey(void *adapter, int key);

const char * PYAdapter_GetCommitStringUTF8(void *adapter);
const char * PYAdapter_GetCompStringUTF8(void *adapter);

int PYAdapter_GetCandItemTotal(void *adapter);
const char * PYAdapter_GetCandStringUTF8(void *adapter, int id);
int PYAdapter_SelCandItem(void *adapter, int id);

int PYAdapter_GetPinyinItemTotal(void *adapter);
const char * PYAdapter_GetPinyinStringUTF8(void *adapter, int id);
int PYAdapter_SelPinyinItem(void *adapter, int id);

int PYAdapter_Save(void *adapter);

void PYAdapter_Destroy(void *adapter);

#ifdef __cplusplus
}
#endif
#endif
