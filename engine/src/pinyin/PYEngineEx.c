#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "PYEngine.h"
#include "PYDynEx.h"
#include "PDLexCore.h"
#include "PDLexBeijing.h"
#include "PDLexName.h"

PY_UINT16 g_nLexTotal = 0;
PY_BLOCK  g_nLexBlock[8];

PY_BOOL PYEngine_LoadFile(PY_BLOCK* pBlock, const char* pFileName)
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


PY_HANDLE PYEngine_New(PY_PCSTR pPathName)
{
	char szFileName[260];
	PY_UINT32 nInstSize = PYEngine_GetInstanceSize();
	PY_BYTE *pBuffer = (PY_BYTE *) malloc(nInstSize);

	sprintf(szFileName, "%s/py_sys.lex", pPathName);

	if (PYEngine_LoadFile(&g_nLexBlock[g_nLexTotal], szFileName))
	{
		g_nLexTotal++;
	}
	else
	{
		g_nLexBlock[g_nLexTotal].pAddress = (PY_UINT8 *) PDLexCore_ptr();
		g_nLexBlock[g_nLexTotal].nSize = PDLexCore_size();
		g_nLexTotal++;

	}

	g_nLexBlock[g_nLexTotal].pAddress = (PY_UINT8*) PDLexName_ptr();
	g_nLexBlock[g_nLexTotal].nSize = PDLexName_size();
	g_nLexTotal++;

	g_nLexBlock[g_nLexTotal].pAddress = (PY_UINT8*)PDLexBeijing_ptr();
	g_nLexBlock[g_nLexTotal].nSize = PDLexBeijing_size();
	g_nLexTotal++;

	sprintf(szFileName, "%s/py_user.lex", pPathName);
	if (PYEngine_LoadFile(&g_nLexBlock[g_nLexTotal], szFileName))
	{
		g_nLexTotal++;
	}
	else
	{
		PYDyn_Create(szFileName, 4 * 1024 * 1024);
		if (PYEngine_LoadFile(&g_nLexBlock[g_nLexTotal], szFileName))
		{
			g_nLexTotal++;
		}
		else
		{
			if (PYDyn_CreateInBlock(4 * 1024 * 1024, &g_nLexBlock[g_nLexTotal]))
			{
				g_nLexTotal++;
			}
		}
	}


	return PYEngine_Initialize(pBuffer, PYEngine_GetInstanceSize(), g_nLexBlock, g_nLexTotal);
}

PY_VOID PYEngine_Free(PY_HANDLE hEngine)
{
	PY_UINT16 i;
	for (i = 0; i < g_nLexTotal; i++)
	{
		if (g_nLexBlock[i].pAddress != PDLexCore_ptr() &&
			g_nLexBlock[i].pAddress != PDLexName_ptr() &&
			g_nLexBlock[i].pAddress != PDLexBeijing_ptr())
		{
			free(g_nLexBlock[i].pAddress);
		}

		g_nLexBlock[i].pAddress = PY_NULL;
	}

	free(hEngine);
	hEngine = PY_NULL;
}