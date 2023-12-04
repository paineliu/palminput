#include "PYAdapter.h"
#include <android/asset_manager_jni.h>
#include <malloc.h>
#include <string.h>
#include <PYDynEx.h>
#include <PDLexCore.h>

#define FT_DICT_SYS_LEX		"py_sys.lex"
#define FT_DICT_USR_LEX		"py_usr_v1.lex"

#define PY_MAX_PATH 260
#define PY_MAX_DICT 64

typedef struct _PY_ADAPTER {
    AAssetManager *assetManager;
    char szDictPathname[PY_MAX_PATH];
    char szUserPathname[PY_MAX_PATH];
    char szCompString[260];
    char szCommitString[260];
    char szCandString[260];
    PY_HANDLE hEngine;
    AAsset *assetFile[PY_MAX_DICT];
    PY_BLOCK stUserLex;
    int nFileTotal;

} PY_ADAPTER;


PY_BOOL LoadDict(AAssetManager* assetManager, const char *pFileName, PY_BLOCK *pBlock, AAsset **assetFile)
{
    *assetFile = AAssetManager_open(assetManager, pFileName, AASSET_MODE_BUFFER);
    if (*assetFile)
    {
        pBlock->nSize = AAsset_getLength(*assetFile);
        pBlock->pAddress = (PY_BYTE *) AAsset_getBuffer(*assetFile);
        return PY_TRUE;
    }

    return PY_FALSE;
}

PY_BOOL PYAdapter_LoadFile(PY_BLOCK* pBlock, const char* pFileName)
{
    FILE* fp = fopen(pFileName, "rb");
    if (fp != NULL)
    {
        fseek(fp, 0L, SEEK_END);

        int flen = ftell(fp);
        pBlock->pAddress = (PY_BYTE*)malloc(flen);
        pBlock->nSize = flen;

        fseek(fp, 0l, SEEK_SET);

        if (pBlock->pAddress)
        {
            fread(pBlock->pAddress, 1, pBlock->nSize, fp);
        }

        fclose(fp);

        return PY_TRUE;
    }

    return PY_FALSE;
}

PY_BOOL PYAdapter_SaveFile(PY_BLOCK* pBlock, const char* pFileName)
{
    FILE* fp = fopen(pFileName, "wb");
    if (fp != NULL)
    {
        fwrite(pBlock->pAddress, sizeof(char), pBlock->nSize, fp);
        fclose(fp);

        return PY_TRUE;
    }

    return PY_FALSE;
}

PY_VOID PYAdapter_TrimSep(char *pPathName)
{
    int nLen = strlen(pPathName);
    if (nLen > 0)
    {
        if (pPathName[nLen-1] == '\\' || pPathName[nLen-1] == '/')
        {
            pPathName[nLen-1] = 0;
        }
    }
}

PY_BOOL PYAdapter_Init(PY_ADAPTER *pAdapter, AAssetManager* assetManager, const char* dictPathname, const char* userPathname)
{
    PY_BLOCK stDictList[PY_MAX_DICT];
    PY_UINT32 nInstSize = PYEngine_GetInstanceSize();
    PY_BYTE *pInstBuf = malloc(nInstSize);
    char szFileName[PY_MAX_PATH];

    if (pInstBuf)
    {
        pAdapter->nFileTotal = 0;
        pAdapter->assetManager = assetManager;
        strncpy(pAdapter->szDictPathname, dictPathname, PY_MAX_PATH);
        strncpy(pAdapter->szUserPathname, userPathname, PY_MAX_PATH);
        PYAdapter_TrimSep(pAdapter->szDictPathname);
        PYAdapter_TrimSep(pAdapter->szUserPathname);
        sprintf(szFileName, "%s/%s", pAdapter->szDictPathname, FT_DICT_SYS_LEX);

        if (LoadDict(assetManager, szFileName, &stDictList[pAdapter->nFileTotal], &pAdapter->assetFile[pAdapter->nFileTotal]))
        {
            pAdapter->nFileTotal ++;
        }
        else
        {
            stDictList[pAdapter->nFileTotal].nSize = PDLexCore_size();
            stDictList[pAdapter->nFileTotal].pAddress = (PY_UINT8 *) PDLexCore_ptr();
            pAdapter->nFileTotal ++;
        }

        sprintf(szFileName, "%s/%s", pAdapter->szUserPathname, FT_DICT_USR_LEX);

        // Demo - PYDyn_Create(szFileName, 4 * 1024 * 1024);

        if (PYAdapter_LoadFile(&stDictList[pAdapter->nFileTotal], szFileName))
        {
            pAdapter->stUserLex = stDictList[pAdapter->nFileTotal];

            pAdapter->nFileTotal ++;
        }
        else
        {
            PYDyn_Create(szFileName, 4 * 1024 * 1024);
            if (PYAdapter_LoadFile(&stDictList[pAdapter->nFileTotal], szFileName))
            {
                pAdapter->stUserLex = stDictList[pAdapter->nFileTotal];

                pAdapter->nFileTotal ++;
            }
            else
            {
                if (PYDyn_CreateInBlock(4 * 1024 * 1024, &stDictList[pAdapter->nFileTotal]))
                {
                    pAdapter->stUserLex = stDictList[pAdapter->nFileTotal];

                    pAdapter->nFileTotal ++;
                }
            }
        }


        pAdapter->hEngine = PYEngine_Initialize(pInstBuf, nInstSize, stDictList, pAdapter->nFileTotal);

        return PY_TRUE;
    }

    return PY_FALSE;
}

void* PYAdapter_Create(AAssetManager* assetManager,
                       const char* dictPathname,
                       const char* userPathname)
{
    PY_ADAPTER *pAdapter = malloc(sizeof(PY_ADAPTER));
    if (pAdapter!= NULL && PYAdapter_Init(pAdapter, assetManager, dictPathname, userPathname))
    {
        return pAdapter;
    }

    return NULL;
}

const char * PYAdapter_GetCompStringUTF8(void *adapter) {
    PY_ADAPTER *pAdapter = adapter;
    PY_HANDLE hEngine = pAdapter->hEngine;

    PY_UINT16 nCaretPos = 0;
    PY_UINT16 nCompLen = PYEngine_GetCompStringUtf8(hEngine, 0, (PY_CHAR *) pAdapter->szCompString, 260, &nCaretPos, PY_TRUE);
    pAdapter->szCompString[nCompLen] = 0;

    return pAdapter->szCompString;
}

const char * PYAdapter_GetCommitStringUTF8(void *adapter) {
    PY_ADAPTER *pAdapter = adapter;
    PY_HANDLE hEngine = pAdapter->hEngine;

    int nCommitLen = 0;
    nCommitLen = PYEngine_GetConvertedStringUtf8(hEngine, pAdapter->szCommitString, 260);
    pAdapter->szCommitString[nCommitLen] = 0;

    return pAdapter->szCommitString;
}

const char * PYAdapter_GetCandStringUTF8(void *adapter, int id)
{
    PY_ADAPTER *pAdapter = adapter;
    PY_HANDLE hEngine = pAdapter->hEngine;

    PY_UINT16 nCandLen = PYEngine_GetCandItemStringUtf8(hEngine, id, (PY_CHAR *) pAdapter->szCandString, 260);
    pAdapter->szCandString[nCandLen] = 0;

    return pAdapter->szCandString;
}

int PYAdapter_SelCandItem(void *adapter, int id)
{
    PY_ADAPTER *pAdapter = adapter;
    PY_HANDLE hEngine = pAdapter->hEngine;
    if (id < PYAdapter_GetCandItemTotal(adapter))
    {
        if (PYEngine_SelCandItem(hEngine, (PY_UINT16) id) != 1)
        {
            return 0;
        }

        return 1;
    }

    return -1;
}

int PYAdapter_GetCandItemTotal(void *adapter)
{
    PY_ADAPTER *pAdapter = adapter;
    PY_HANDLE hEngine = pAdapter->hEngine;

    return PYEngine_GetCandTotal(hEngine);
}

const char * PYAdapter_GetPinyinStringUTF8(void *adapter, int id)
{
    return PY_NULL;
}

int PYAdapter_SelPinyinItem(void *adapter, int id)
{
    return 0;
}

int PYAdapter_GetPinyinItemTotal(void *adapter)
{
    return 0;
}

int PYAdapter_SetOptions(void *adapter, const PY_OPTION *pOption)
{
    PY_ADAPTER *pAdapter = adapter;
    PY_HANDLE hEngine = pAdapter->hEngine;

    PYEngine_SetOption(hEngine, pOption);

    return 0;
}

const PY_OPTION * PYAdapter_GetOptions(void *adapter)
{
    PY_ADAPTER *pAdapter = adapter;
    PY_HANDLE hEngine = pAdapter->hEngine;

    return PYEngine_GetOption(hEngine);
}

void PYAdapter_Reset(void *adapter)
{
    PY_ADAPTER *pAdapter = adapter;
    PY_HANDLE hEngine = pAdapter->hEngine;

    PYEngine_Reset(hEngine);
}

 int PYAdapter_AddKey(void *adapter, int key) {
    PY_ADAPTER *pAdapter = adapter;
    PY_HANDLE hEngine = pAdapter->hEngine;
    if (PYEngine_KeyEvent(hEngine, key, 0) != PY_FALSE)
    {
        return 0;
    }

    return -1;
}

int PYAdapter_Save(void *adapter)
{
    PY_ADAPTER *pAdapter = adapter;
    char szFileName[PY_MAX_PATH];
    sprintf(szFileName, "%s/%s", pAdapter->szUserPathname, FT_DICT_USR_LEX);
    if (PYAdapter_SaveFile(&pAdapter->stUserLex, szFileName))
    {
        return 0;
    }

    return -1;
}

void PYAdapter_Destroy(void *adapter)
{
    PY_ADAPTER *pAdapter = adapter;
    PY_INT32 i;

    for (i=0; i<pAdapter->nFileTotal; i++)
    {
        if (pAdapter->assetFile[i] != NULL)
        {
            AAsset_close(pAdapter->assetFile[i]);
            pAdapter->assetFile[i] = NULL;
        }
    }

    if (pAdapter->hEngine != NULL)
    {
        free(pAdapter->hEngine);
        pAdapter->hEngine = NULL;
    }

    if (adapter != NULL)
    {
        free(adapter);
        adapter = NULL;
    }
}
