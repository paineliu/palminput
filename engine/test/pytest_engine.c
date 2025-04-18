#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "PYEngineEx.h"
#include "PDLexCore.h"
int phrase_output_test()
{
    PY_HANDLE hEngine = PYEngine_New("");

    FILE *fp = fopen("../../../Test/Data/phrase_output.txt", "rb");
    if (fp) 
    {
        char szLine[1024];
        char szPinyin[1024];
        char szPinyinNoSplit[1024];
        char szPhrase[1024];
        char szCand[1024];
        int  line = 0;
        while (fgets(szLine, 1024, fp) != NULL)
        {
            if (sscanf(szLine, "%s %s", szPhrase, szPinyin) == 2)
            {
                int l = 0;
                for (int i = 0; i <= strlen(szPinyin); i++)
                {
                    if (szPinyin[i] != '\'') 
                    {
                        szPinyinNoSplit[l++] = szPinyin[i];
                    }
                }
                PYEngine_SetKeys(hEngine, szPinyinNoSplit, strlen(szPinyinNoSplit));
                
                int nCandTotal = PYEngine_GetCandTotal(hEngine);
                PY_BOOL bFind = PY_FALSE;
                for(int i=0; i<nCandTotal; i++)
                {
                    PYEngine_GetCandItemStringUtf8(hEngine, i, szCand, 1024);
                    if (strcmp(szPhrase, szCand) == 0)
                    {
                        bFind = PY_TRUE;
                        break;
                    }                    
                }
                line++;

                if (bFind) {
                    if (line % 1000 == 0) {
                        printf("%s\t%s\tok\n", szPhrase, szPinyinNoSplit);
                    }
                }else
                {
                    printf("%s\t%s\tfail\n", szPhrase, szPinyinNoSplit);
                    /*for (int i = 0; i < nCandTotal; i++)
                    {
                        PYEngine_GetCandItemStringUtf8(hEngine, i, szCand, 1024);
                        printf("%d %s\n", i+1, szCand);
                    }
                    printf("%s\t%s\tfail\n", szPhrase, szPinyin);*/
                }
            }
        }
        fclose(fp);
    }

    PYEngine_Free(hEngine);

    return 0;
}

int load_lex(PY_BLOCK *pBlock, const char *pFileName)
{
    PY_BLOCK stBlock;
    FILE* fp = fopen(pFileName, "rb");
    if (fp != NULL)
    {
        fseek(fp, 0L, SEEK_END); /* ��λ���ļ�ĩβ */

        int flen = ftell(fp); /* �õ��ļ���С */
        pBlock->pAddress = (PY_BYTE *) malloc(flen);
        pBlock->nSize = flen;

        fseek(fp, 0l, SEEK_SET);

        if (pBlock->pAddress)
        {
            fread(pBlock->pAddress, 1, pBlock->nSize, fp);
        }

        fclose(fp);

        return 0;
    }

    return -1;
}

int test_engine()
{
    PY_BLOCK stBlock={0};
    load_lex(&stBlock, "../../../Data/Phrase/sys_lex.dic");

    if (stBlock.pAddress == PY_NULL)
    {
        stBlock.pAddress = PDLexCore_ptr();
        stBlock.nSize = PDLexCore_size();
    }
    PY_PCSTR nVersion = PYEngine_GetVersion();

    PY_UINT32 nInstSize = PYEngine_GetInstanceSize();
    PY_BYTE* pInstObj = (PY_BYTE*)malloc(nInstSize);
    memset(pInstObj, 0, nInstSize);
    PY_HANDLE hEngine = PYEngine_Initialize(pInstObj, nInstSize, &stBlock, 1);
    
    printf("%s mem = %d\n", PYEngine_GetVersion(), nInstSize);

    //char szKeys[] = "a'''ggupiao";
    char szKeys[] = "abbbbaa";
    //char szKeys[] = "''agu''gupiao'''";
    for (int i = 0; i < strlen(szKeys); i++)
    {
        PYEngine_KeyEvent(hEngine, szKeys[i], 0);

        PY_CHAR szCompString[256];
        PY_UINT16 nCompLen = 256;
        PYEngine_GetCompStringUtf8(hEngine, 0, szCompString, nCompLen, 0, 0);
        PY_UINT16 nCandTotal = PYEngine_GetCandTotal(hEngine);
        PY_CHAR szKeyString[256];
        PY_UINT16 nKeyLen = 256;
        PYEngine_GetKeyStringUtf8(hEngine, szKeyString, 256);
        printf("\nkeys = %s\n", szKeyString);
        printf("\ncomp = %s\ncand total = %d\n", szCompString, nCandTotal);
        for (int i = 0; i < nCandTotal; i++) 
        {
            PY_CHAR szCand[256];
            PYEngine_GetCandItemStringUtf8(hEngine, i, szCand, 256);
            printf("cand item[%d] = %s\n", i + 1, szCand);
        }
    }

    PYEngine_Terminate(hEngine);

    return 0;
}
// fi'''ao error