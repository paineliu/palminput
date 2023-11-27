#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include "PYHan.h"
#include "PYDyn.h"
#include "PYDynEx.h"

#define PY_DYN_WRITE_BUF_SIZE (1024 * 64)

PY_UINT32  PYDyn_Create(PY_PCSTR pFileName, PY_UINT32 nFileSize)
{
	PY_HAN stHan;
	PY_UINT32 nDataSize = nFileSize;
	PY_UINT32 nNeedWriteSize;
	PY_UINT32 nRealWriteSize;
	PY_BYTE *pBuf = calloc(PY_DYN_WRITE_BUF_SIZE, sizeof(PY_BYTE));
	
	PYHan_Initialize(&stHan, PYHan_GetHanData());

	FILE* fp = fopen(pFileName, "wb");
	if (fp && pBuf)
	{
		PY_FILE_HEAD stFileHead;
		PY_UINT32 nFileHeadSize = sizeof(PY_FILE_HEAD);
		PY_UINT32 nBlockTotal = 3;
		PY_UINT32 nFileBlockSize = nBlockTotal * sizeof(PY_FILE_BLOCK);
		PY_UINT32 nDataOffset = 0;
		PY_UINT32 nDataSize = nFileSize - nFileHeadSize - nFileBlockSize;

		PY_UINT32 nDataSummarySize = sizeof(PY_DYN_SUMMARY);
		PY_UINT32 nDataGroupSize = sizeof(PY_DYN_GROUP) * PYHan_GetPinyinGroup1Total(&stHan);
		PY_UINT32 nDataBufferSize = nDataSize - nDataSummarySize - nDataGroupSize;

		time_t t = time(NULL);
		struct tm* p = localtime(&t);
		char szDay[9];
		sprintf(szDay, "%04d%02d%02d", 1900 + p->tm_year, (1 + p->tm_mon), p->tm_mday);

		stFileHead.nFileSize = nFileSize;
		stFileHead.nFileMask = PY_FILE_PY_DYNAMIC_MASK;
		stFileHead.nVersion = PY_FILE_PY_DYNAMIC_VER;
		stFileHead.nTimeStamp = (szDay[0] - '0') << 4 | (szDay[1] - '0') << 0 | (szDay[2] - '0') << 12 | (szDay[3] - '0') << 8 | (szDay[4] - '0') << 20 | (szDay[5] - '0') << 16 | (szDay[6] - '0') << 28 | (szDay[7] - '0') << 24;

		stFileHead.nBlockTotal = nBlockTotal;
		stFileHead.nBlockOffset = nFileHeadSize;
		stFileHead.nDataOffset = nFileHeadSize + nFileBlockSize;
		stFileHead.nReserved = 0;
		
		fwrite(&stFileHead, sizeof(PY_FILE_HEAD), 1, fp);
		nDataOffset = 0;

		PY_FILE_BLOCK stSummaryBlock;

		stSummaryBlock.nBlockId = PY_FILE_BLOCK_DYN_SUMMARY;
		stSummaryBlock.nOffset = nDataOffset;
		stSummaryBlock.nSize = sizeof(PY_DYN_SUMMARY);
		stSummaryBlock.nReserved = 0;

		fwrite(&stSummaryBlock, sizeof(PY_FILE_BLOCK), 1, fp);

		nDataOffset += stSummaryBlock.nSize;


		PY_FILE_BLOCK stGroupBlock;

		stGroupBlock.nBlockId = PY_FILE_BLOCK_DYN_GROUP;
		stGroupBlock.nOffset = nDataOffset;
		stGroupBlock.nSize = sizeof(PY_DYN_GROUP) * PYHan_GetPinyinGroup1Total(&stHan);
		stGroupBlock.nReserved = 0;

		fwrite(&stGroupBlock, sizeof(PY_FILE_BLOCK), 1, fp);
		nDataOffset += stGroupBlock.nSize;

		PY_FILE_BLOCK stDataBlock;
		stDataBlock.nBlockId = PY_FILE_BLOCK_DYN_BUFFER;
		stDataBlock.nOffset = nDataOffset;
		stDataBlock.nSize = nDataBufferSize;
		stDataBlock.nReserved = 0;

		fwrite(&stDataBlock, sizeof(PY_FILE_BLOCK), 1, fp);

		PY_DYN_SUMMARY stDynHead;
		memset(&stDynHead, 0, sizeof(PY_DYN_SUMMARY));

		stDynHead.nPhraseTotal = 0;
		stDynHead.nFreqTotal = 0;
		stDynHead.nPhraseCapacity = nDataBufferSize / sizeof(PY_DYN_PHRASE);
		stDynHead.nBufferSize = nDataBufferSize;

		fwrite(&stDynHead, sizeof(PY_DYN_SUMMARY), 1, fp);
		
		PY_DYN_GROUP* pDynGroup = calloc(PYHan_GetPinyinGroup1Total(&stHan), sizeof(PY_DYN_GROUP));
		if (pDynGroup)
		{
			fwrite(pDynGroup, sizeof(PY_DYN_GROUP), PYHan_GetPinyinGroup1Total(&stHan), fp);
			free(pDynGroup);
		}

		nDataSize = nDataBufferSize;

		do 
		{
			nNeedWriteSize = nDataSize > PY_DYN_WRITE_BUF_SIZE ? PY_DYN_WRITE_BUF_SIZE : nDataSize;
			if (nNeedWriteSize > 0)
			{
				nRealWriteSize = (PY_UINT32) fwrite(pBuf, sizeof(PY_BYTE), nNeedWriteSize, fp);
				nDataSize -= nNeedWriteSize;
			}
			else
			{
				nRealWriteSize = 0;
			}
		} while (nNeedWriteSize > 0 && nRealWriteSize == nNeedWriteSize);
	}

	if (fp)
	{
		fclose(fp);
	}

	if (pBuf != PY_NULL)
	{
		free(pBuf);
	}

	PYHan_Terminate(&stHan);

	return 0;
}

PY_BOOL PYDyn_CreateInBlock(PY_UINT32 nLexSize, PY_BLOCK *pBlock)
{
	PY_BOOL bRet = PY_FALSE;
	PY_HAN stHan;
	
	PY_BYTE* pBuffer = calloc(nLexSize, sizeof(PY_BYTE));
	PY_UINT32 nOffset = 0;

	PYHan_Initialize(&stHan, PYHan_GetHanData());

	if (pBuffer != PY_NULL)
	{
		PY_FILE_HEAD stFileHead;
		PY_UINT32 nFileHeadSize = sizeof(PY_FILE_HEAD);
		PY_UINT32 nBlockTotal = 3;
		PY_UINT32 nFileBlockSize = nBlockTotal * sizeof(PY_FILE_BLOCK);
		PY_UINT32 nDataOffset = 0;
		PY_UINT32 nDataSize = nLexSize - nFileHeadSize - nFileBlockSize;

		PY_UINT32 nDataSummarySize = sizeof(PY_DYN_SUMMARY);
		PY_UINT32 nDataGroupSize = sizeof(PY_DYN_GROUP) * PYHan_GetPinyinGroup1Total(&stHan);
		PY_UINT32 nDataBufferSize = nDataSize - nDataSummarySize - nDataGroupSize;

		time_t t = time(NULL);
		struct tm* p = localtime(&t);
		char szDay[9];
		sprintf(szDay, "%04d%02d%02d", 1900 + p->tm_year, (1 + p->tm_mon), p->tm_mday);

		stFileHead.nFileSize = nLexSize;
		stFileHead.nFileMask = PY_FILE_PY_DYNAMIC_MASK;
		stFileHead.nVersion = PY_FILE_PY_DYNAMIC_VER;
		stFileHead.nTimeStamp = (szDay[0] - '0') << 4 | (szDay[1] - '0') << 0 | (szDay[2] - '0') << 12 | (szDay[3] - '0') << 8 | (szDay[4] - '0') << 20 | (szDay[5] - '0') << 16 | (szDay[6] - '0') << 28 | (szDay[7] - '0') << 24;

		stFileHead.nBlockTotal = nBlockTotal;
		stFileHead.nBlockOffset = nFileHeadSize;
		stFileHead.nDataOffset = nFileHeadSize + nFileBlockSize;
		stFileHead.nReserved = 0;

		memcpy(&pBuffer[nOffset], &stFileHead, sizeof(PY_FILE_HEAD));
		nOffset += sizeof(PY_FILE_HEAD);

		nDataOffset = 0;

		PY_FILE_BLOCK stSummaryBlock;

		stSummaryBlock.nBlockId = PY_FILE_BLOCK_DYN_SUMMARY;
		stSummaryBlock.nOffset = nDataOffset;
		stSummaryBlock.nSize = sizeof(PY_DYN_SUMMARY);
		stSummaryBlock.nReserved = 0;

		memcpy(&pBuffer[nOffset], &stSummaryBlock, sizeof(PY_FILE_BLOCK));
		nOffset += sizeof(PY_FILE_BLOCK);

		nDataOffset += stSummaryBlock.nSize;


		PY_FILE_BLOCK stGroupBlock;

		stGroupBlock.nBlockId = PY_FILE_BLOCK_DYN_GROUP;
		stGroupBlock.nOffset = nDataOffset;
		stGroupBlock.nSize = sizeof(PY_DYN_GROUP) * PYHan_GetPinyinGroup1Total(&stHan);
		stGroupBlock.nReserved = 0;

		memcpy(&pBuffer[nOffset], &stGroupBlock, sizeof(PY_FILE_BLOCK));
		nOffset += sizeof(PY_FILE_BLOCK);

		nDataOffset += stGroupBlock.nSize;

		PY_FILE_BLOCK stDataBlock;
		stDataBlock.nBlockId = PY_FILE_BLOCK_DYN_BUFFER;
		stDataBlock.nOffset = nDataOffset;
		stDataBlock.nSize = nDataBufferSize;
		stDataBlock.nReserved = 0;

		memcpy(&pBuffer[nOffset], &stDataBlock, sizeof(PY_FILE_BLOCK));
		nOffset += sizeof(PY_FILE_BLOCK);

		PY_DYN_SUMMARY stDynHead;
		stDynHead.nPhraseTotal = 0;
		stDynHead.nFreqTotal = 0;
        stDynHead.nPhraseCapacity = nDataBufferSize / sizeof(PY_DYN_PHRASE);
        stDynHead.nBufferSize = nDataBufferSize;

		memcpy(&pBuffer[nOffset], &stDynHead, sizeof(PY_DYN_SUMMARY));
		nOffset += sizeof(PY_DYN_SUMMARY);

		pBlock->nSize = nLexSize;
		pBlock->pAddress = pBuffer;
		bRet = PY_TRUE;
	}

	PYHan_Terminate(&stHan);

	return bRet;
}

PY_UINT32  PYDyn_Resize(PY_PCSTR pFileName, PY_PCSTR pNewFileName, PY_UINT32 nNewFileSize)
{
	return 0;
}

PY_BOOL PYDyn_SaveLex(PY_BLOCK* pBlock, const char* pFileName)
{
	FILE* fp = fopen(pFileName, "wb");
	if (fp != NULL)
	{
		if (pBlock->pAddress)
		{
			fwrite(pBlock->pAddress, 1, pBlock->nSize, fp);
		}

		fclose(fp);

		return PY_TRUE;
	}

	return PY_FALSE;
}


PY_BOOL PYDyn_LoadLex(PY_BLOCK* pBlock, const char* pFileName)
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

PY_BOOL PYDyn_DumpFile(PY_PCSTR pLexFileName, PY_PCSTR pTxtFileName)
{
	PY_BLOCK stBlock;
	PY_HAN stHan;
	PY_DYN stDyn;
	PY_UINT32 i;

	if (PYDyn_LoadLex(&stBlock, pLexFileName))
	{
		FILE* fp = fopen(pTxtFileName, "w");

		PYHan_Initialize(&stHan, PYHan_GetHanData());
		PYDyn_Initialize(&stDyn, &stHan, &stBlock);
		PYDyn_GetGroupTotal(&stDyn);

		for (i = 0; i < PYDyn_GetGroupTotal(&stDyn); i++)
		{
			if (stDyn.stData.pGroup[i].nFirst != 0)
			{
				printf("%d, 0x%08x, 0x%08x\n", i, stDyn.stData.pGroup[i].nFirst, stDyn.stData.pGroup[i].nLast);
			}
		}

		for (i = 0; i < PYDyn_GetGroupTotal(&stDyn); i++)
		{
			PY_UINT32 nPhraseId = stDyn.stData.pGroup[i].nFirst;
			while (nPhraseId != 0)
			{
				printf("%d, 0x%08x\n", i, nPhraseId);
				nPhraseId = PYDyn_GetNextPhraseId(&stDyn, nPhraseId);
			}
		}

		fclose(fp);

		free(stBlock.pAddress);
	}

	return PY_FALSE;
}
