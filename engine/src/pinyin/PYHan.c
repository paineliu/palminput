#include <stdio.h>
#include "PYHan.h"

PY_BOOL   PYHan_Initialize(PY_HAN* pHan, const PY_HAN_DATA* pData)
{
	pHan->pData = pData;

	PYHan_SetFuzzy(pHan, 0x00000000);

	return PY_TRUE;
}

PY_VOID   PYHan_Terminate(PY_HAN* pHan)
{
	PY_UNREFERENCED_PARAMETER(pHan);
}

PY_UINT16 PYHan_GetPinyinMaxLen(PY_HAN* pHan)
{
	return  pHan->pData->nPinyinMaxLen;
}

PY_UINT16 PYHan_GetHashGroupTotal(PY_HAN* pHan)
{
	return  pHan->pData->nHashGroupTotal;
}

PY_UINT16 PYHan_GetPinyinTotal(PY_HAN* pHan)
{
	return  pHan->pData->nPinyinItemTotal;
}

PY_PCSTR PYHan_GetPinyinString(PY_HAN* pHan, PY_UINT16 nPinyinId)
{
	return &pHan->pData->pPinyinStr[pHan->pData->pPinyinItem[nPinyinId].nOffset];
}

PY_UINT16 PYHan_GetPinyinStringLen(PY_HAN* pHan, PY_UINT16 nPinyinId)
{
	return pHan->pData->pPinyinItem[nPinyinId].nLen;
}

PY_UINT16 PYHan_GetPinyinGroupId(PY_HAN* pHan, PY_UINT16 nPinyinId)
{
	return pHan->pData->pPinyinItem[nPinyinId].nHash;
}

PY_INT32 PYHan_Compare_PinyinItem(PY_PCSTR pPinyinString, const PY_PINYIN_ITEM* pPinyinItem, PY_PCSTR pPinyinBaseStr)
{
	return PY_strcmp(pPinyinString, pPinyinBaseStr + pPinyinItem->nOffset);
}

PY_UINT16 PYHan_GetPinyinId(PY_HAN* pHan, PY_PCSTR pPinyinString, PY_UINT16 nPinyinLen)
{
	if (pPinyinString)
	{
		PY_PINYIN_ITEM* pItemFound = (PY_PINYIN_ITEM*)PY_binsearch(pPinyinString,
			pHan->pData->pPinyinItem,
			pHan->pData->nPinyinItemTotal,
			(PY_UINT32)sizeof(PY_PINYIN_ITEM),
			(PY_compare)PYHan_Compare_PinyinItem,
			(PY_PVOID)pHan->pData->pPinyinStr);
		if (pItemFound)
		{
			return (PY_UINT16)(pItemFound - pHan->pData->pPinyinItem);
		}
	}

	return PY_PINYIN_ID_INVALID;
}

// Han - Pinyin

//inline PY_UINT16 PYHan_GetHanPinyinId(PY_HAN* pHan, PY_UINT16 nHanId)
//{
//	return  pHan->pData->pHanItem[nHanId].nPinyinId;
//}

PY_UINT16 PYHan_GetHanUnicode(PY_HAN* pHan, PY_UINT16 nHanId)
{
	return  pHan->pData->pHanItem[nHanId].nUnicode;
}

PY_UINT16 PYHan_GetHanUtf8(PY_HAN* pHan, PY_UINT16 nHanId, PY_PSTR pUtf8String, PY_UINT16 nUtf8MaxLen)
{
	return PY_utf16_to_utf8_one(PYHan_GetHanUnicode(pHan, nHanId), pUtf8String, nUtf8MaxLen);
}

PY_UINT16 PYHan_GetHanTotal(PY_HAN* pHan)
{
	return  pHan->pData->nHanItemTotal;
}

PY_INT32 PYHan_Compare_UnicodeItem(const PY_HAN_ITEM* pHanItem, const PY_HAN_R_ITEM* pUnicodeItem, PY_VOID* pArg)
{
	PY_UNREFERENCED_PARAMETER(pArg);

	if (pHanItem->nUnicode == pUnicodeItem->nUnicode)
	{
		return pHanItem->nPinyinId - pUnicodeItem->nPinyinId;
	}
	else
	{
		return pHanItem->nUnicode - pUnicodeItem->nUnicode;
	}
}

PY_UINT16  PYHan_GetHanId(PY_HAN* pHan, PY_UINT16 nUnicode, PY_UINT16 nPinyinId)
{
	PY_HAN_ITEM stEncodeItem;
	stEncodeItem.nUnicode = nUnicode;
	stEncodeItem.nPinyinId = nPinyinId;

	PY_HAN_R_ITEM* pItemFound = (PY_HAN_R_ITEM*)PY_binsearch(&stEncodeItem,
		pHan->pData->pHanRItem,
		pHan->pData->nHanItemTotal,
		(PY_UINT32)sizeof(PY_HAN_R_ITEM),
		(PY_compare)PYHan_Compare_UnicodeItem,
		PY_NULL);

	if (pItemFound)
	{
		return (PY_UINT16)(pItemFound->nHanId);
	}

	return PY_HAN_ID_INVALID;
}

PY_BOOL   PYHan_IsAscii(PY_HAN* pHan, PY_WCHAR cUnicode)
{
	PY_UNREFERENCED_PARAMETER(pHan);

	return (cUnicode >= 'A' && cUnicode <= 'Z') || (cUnicode >= 'a' && cUnicode <= 'z') || (cUnicode >= '0' && cUnicode <= '9');
}

PY_BOOL   PYHan_IsUpper(PY_HAN* pHan, PY_WCHAR cUnicode)
{
	return cUnicode >= 'A' && cUnicode <= 'Z';
}

PY_BOOL PYHan_IsEqual(PY_HAN* pHan, const PY_UINT16* pSrcString, const PY_UINT16* pDstString, PY_UINT16 nHanLen)
{
	PY_UINT16		i;

	for (i = 0; i < nHanLen; i++)
	{
		if (pHan->pData->pHanItem[pSrcString[i]].nUnicode != pHan->pData->pHanItem[pDstString[i]].nUnicode)
		{
			return PY_FALSE;
		}
	}

	return PY_TRUE;
}

PY_UINT16 PYHan_DecodePhrase(PY_HAN* pHan, const PY_UINT16* pHanString, PY_UINT16 nHanLen, PY_PWSTR pUnicodeString, PY_UINT16 nUnicodeMaxLen)
{
	PY_UINT16		nIndex;

	if (pHanString && nHanLen > 0 && pUnicodeString)
	{
		for (nIndex = 0; nIndex < nHanLen; nIndex++)
		{
			pUnicodeString[nIndex] = pHan->pData->pHanItem[pHanString[nIndex]].nUnicode;
		}

		if (nUnicodeMaxLen > nHanLen)
		{
			pUnicodeString[nHanLen] = 0;
		}
	}

	return nHanLen;

}

PY_BOOL PYHan_Decode(PY_HAN* pHan, const PY_UINT16* pHanString, PY_UINT16 nHanLen, PY_PWSTR pUnicodeString,
	PY_UINT16* pUnicodeLen, PY_PWSTR pPinyinString, PY_UINT16* pPinyinLen, PY_WCHAR cPinyinSplit)
{
	PY_UINT16		nIndex;
	PY_PCSTR        pPinyinItem;
	PY_UINT16       nPinyinItemLen;
	PY_UINT16       nPinyinBegin = 0;
	PY_UINT16 i;

	if (pHanString && nHanLen > 0 && pUnicodeString)
	{
		for (nIndex = 0; nIndex < nHanLen; nIndex++)
		{
			pUnicodeString[nIndex] = pHan->pData->pHanItem[pHanString[nIndex]].nUnicode;
			pPinyinItem = PYHan_GetPinyinString(pHan, pHan->pData->pHanItem[pHanString[nIndex]].nPinyinId);
			nPinyinItemLen = PYHan_GetPinyinStringLen(pHan, pHan->pData->pHanItem[pHanString[nIndex]].nPinyinId);

			if (nIndex > 0 && !(PYHan_IsAscii(pHan, pUnicodeString[nIndex]) && PYHan_IsAscii(pHan, pUnicodeString[nIndex - 1])))
			{
				pPinyinString[nPinyinBegin] = cPinyinSplit;
				nPinyinBegin++;
			}

			for (i = 0; i < nPinyinItemLen; i++)
			{
				pPinyinString[nPinyinBegin] = pPinyinItem[i];
				nPinyinBegin++;
			}
		}
	}

	*pPinyinLen = nPinyinBegin;
	*pUnicodeLen = nHanLen;

	return 0;
}

PY_UINT16 PYHan_DecodePhraseUtf8(PY_HAN* pHan, const PY_UINT16* pHanString, PY_UINT16 nHanLen, PY_PSTR pUtf8String, PY_UINT16 nUtf8MaxLen)
{
	PY_UINT16		nIndex;
	PY_UINT16       nUtf8Begin = 0;
	PY_UINT16       nUtf8CharLen = 0;

	if (pHanString && nHanLen > 0 && pUtf8String)
	{
		for (nIndex = 0; nIndex < nHanLen; nIndex++)
		{
			PY_UINT16 nUnicode = pHan->pData->pHanItem[pHanString[nIndex]].nUnicode;

			nUtf8CharLen = PY_utf16_to_utf8_one(nUnicode, &pUtf8String[nUtf8Begin], nUtf8MaxLen - nUtf8Begin);
			nUtf8Begin += nUtf8CharLen;

		}

		if (nUtf8MaxLen > nUtf8Begin)
		{
			pUtf8String[nUtf8Begin] = 0;
		}
	}

	return nUtf8Begin;
}

PY_BOOL PYHan_DecodeUtf8(PY_HAN* pHan, const PY_UINT16* pHanString, PY_UINT16 nHanLen, PY_PSTR pUtf8String,
	PY_UINT16* pUtf8Len, PY_PSTR pPinyinString, PY_UINT16* pPinyinLen, PY_CHAR cPinyinSplit)
{
	PY_UINT16		nIndex;
	PY_PCSTR        pPinyinItem;
	PY_UINT16       nPinyinItemLen;
	PY_UINT16       nPinyinBegin = 0;
	PY_UINT16       nUtf8Begin = 0;
	PY_UINT16       nUtf8CharLen = 0;
	PY_UINT16 i;
	PY_BOOL bLastAscii = PY_FALSE;

	if (pHanString && nHanLen > 0 && pUtf8String)
	{
		for (nIndex = 0; nIndex < nHanLen; nIndex++)
		{
			PY_UINT16 nUnicode = pHan->pData->pHanItem[pHanString[nIndex]].nUnicode;

			nUtf8CharLen = PY_utf16_to_utf8_one(nUnicode, &pUtf8String[nUtf8Begin], *pUtf8Len - nUtf8Begin);
			nUtf8Begin += nUtf8CharLen;

			pPinyinItem = PYHan_GetPinyinString(pHan, pHan->pData->pHanItem[pHanString[nIndex]].nPinyinId);
			nPinyinItemLen = PYHan_GetPinyinStringLen(pHan, pHan->pData->pHanItem[pHanString[nIndex]].nPinyinId);

			if (nIndex > 0 && !(PYHan_IsAscii(pHan, nUnicode) && bLastAscii))
			{
				if (pPinyinString != PY_NULL && pPinyinLen != PY_NULL && *pPinyinLen > nPinyinBegin)
				{
					pPinyinString[nPinyinBegin] = cPinyinSplit;
				}

				nPinyinBegin++;
			}

			for (i = 0; i < nPinyinItemLen; i++)
			{

				if (pPinyinString != PY_NULL && pPinyinLen != PY_NULL && *pPinyinLen > nPinyinBegin)
				{
					pPinyinString[nPinyinBegin] = pPinyinItem[i];
				}

				nPinyinBegin++;
			}

			bLastAscii = PYHan_IsAscii(pHan, nUnicode);
		}

		if (*pUtf8Len > nUtf8Begin)
		{
			pUtf8String[nUtf8Begin] = 0;
		}

		*pUtf8Len = nUtf8Begin;
	}

	if (pPinyinString != PY_NULL && pPinyinLen != PY_NULL && *pPinyinLen > nPinyinBegin)
	{
		pPinyinString[nPinyinBegin] = 0;
	}

	if (pPinyinLen != PY_NULL)
	{
		*pPinyinLen = nPinyinBegin;
	}

	return nUtf8Begin;

}

PY_BOOL PYHan_IsLower(PY_HAN* pHan, PY_CHAR cChar)
{
	return cChar >= 'a' && cChar <= 'z';
}

PY_CHAR PYHan_ToUpper(PY_HAN* pHan, PY_CHAR cChar)
{
	if (PYHan_IsLower(pHan, cChar))
	{
		return cChar - 'a' + 'A';
	}

	return cChar;
}

PY_UINT16 PYHan_Encode(PY_HAN* pHan, PY_UINT16* pHanString, PY_UINT16 nHanMaxLen, PY_PCWSTR pUnicodeString,
	PY_UINT16 nUnicodeLen, PY_PCWSTR pPinyinString, PY_UINT16 nPinyinLen, PY_WCHAR cPinyinSplit)
{
	PY_UINT16 nUnicodeBegin = 0;
	PY_UINT16 nPinyinItemBegin = 0;
	PY_PSTR pPinyinItemString = PY_NULL;
	PY_UINT16 nPinyinItemLen = 0;
	PY_UINT16 nPinyinTotalLen = 0;

	PY_UINT16 i;
	PY_UINT16 j;
	PY_CHAR szPinyinString[64 * 8];
	PY_UINT16 nHancodeId;
	PY_UINT16 nPinyinId;
	PY_CHAR szAsciiString[2];

	if (pHanString && pUnicodeString && nUnicodeLen > 0 && pPinyinString && nPinyinLen > 0)
	{
		szAsciiString[1] = 0;
		for (i = 0; i < nPinyinLen; i++)
		{
			if (pPinyinString[i] == cPinyinSplit)
			{
				if (nPinyinTotalLen > 0 && szPinyinString[nPinyinTotalLen - 1] != '\0')
				{
					szPinyinString[nPinyinTotalLen] = '\0';
					nPinyinTotalLen++;
				}
			}
			else
			{
				szPinyinString[nPinyinTotalLen] = (PY_CHAR)pPinyinString[i];
				nPinyinTotalLen++;
				if (PYHan_IsUpper(pHan, pPinyinString[i]))
				{
					szPinyinString[nPinyinTotalLen] = '\0';
					nPinyinTotalLen++;
				}
			}
		}
		szPinyinString[nPinyinTotalLen] = '\0';

		for (i = 0; i < nPinyinTotalLen; i++)
		{
			if (szPinyinString[i] == '\0' || i + 1 == nPinyinTotalLen)
			{
				if (szPinyinString[i] == '\0')
				{
					nPinyinItemLen = i - nPinyinItemBegin;
				}
				else
				{
					nPinyinItemLen = i - nPinyinItemBegin + 1;
				}

				if (nPinyinItemLen > 0)
				{
					pPinyinItemString = &szPinyinString[nPinyinItemBegin];

					if (PYHan_IsAscii(pHan, pUnicodeString[nUnicodeBegin]))
					{
						for (j = 0; j < nPinyinItemLen; j++)
						{
							szAsciiString[0] = PYHan_ToUpper(pHan, szPinyinString[nPinyinItemBegin + j]);
							nPinyinId = PYHan_GetPinyinId(pHan, szAsciiString, 1);
							nHancodeId = PYHan_GetHanId(pHan, pUnicodeString[nUnicodeBegin + j], nPinyinId);
							if (nHancodeId == PY_HAN_ID_INVALID)
							{
								return 0;
							}

							if (nUnicodeBegin + j < nHanMaxLen)
							{
								pHanString[nUnicodeBegin + j] = nHancodeId;
							}
						}

						nUnicodeBegin += nPinyinItemLen;
					}
					else
					{
						nPinyinId = PYHan_GetPinyinId(pHan, pPinyinItemString, nPinyinItemLen);
						nHancodeId = PYHan_GetHanId(pHan, pUnicodeString[nUnicodeBegin], nPinyinId);
						if (nHancodeId == PY_HAN_ID_INVALID)
						{
							return 0;
						}

						if (nUnicodeBegin < nHanMaxLen) {
							pHanString[nUnicodeBegin] = nHancodeId;
						}
						nUnicodeBegin += 1;
					}

					nPinyinItemBegin = i + 1;
				}
			}
		}
	}

	return nUnicodeBegin;
}

PY_UINT16 PYHan_EncodeUtf8(PY_HAN* pHan, PY_UINT16* pHanString, PY_UINT16 nHanMaxLen, PY_PCSTR pUtf8String,
	PY_UINT16 nUtf8Len, PY_PCSTR pPinyinString, PY_UINT16 nPinyinLen, PY_CHAR cPinyinSplit)
{
	PY_UINT16 szPhraseW[256];
	PY_UINT16 szPinyinW[215];
	PY_UINT16 nPhraseWLen;
	PY_UINT16 nPinyinWLen;

	nPhraseWLen = PY_utf8_to_utf16(pUtf8String, nUtf8Len, szPhraseW, 256);
	nPinyinWLen = PY_utf8_to_utf16(pPinyinString, nPinyinLen, szPinyinW, 512);
	szPinyinW[nPinyinWLen] = 0;

	return PYHan_Encode(pHan, pHanString, nHanMaxLen, szPhraseW, nPhraseWLen, szPinyinW, nPinyinWLen, cPinyinSplit);

}

PY_BOOL PYHan_SetFuzzy(PY_HAN* pHan, PY_UINT32 nFuzzyFlags)
{
	PY_UINT16 i;
	pHan->nFuzzyFlags = nFuzzyFlags;

	for (i = 0; i < pHan->pData->nNodePinyinTotal; i++)
	{
		pHan->nNodeEnabled[i] = ((pHan->pData->pNodePinyin[i].nFuzzyFlags & nFuzzyFlags) == pHan->pData->pNodePinyin[i].nFuzzyFlags);
	}

	return PY_TRUE;
}


PY_UINT32 PYHan_GetFuzzy(PY_HAN* pHan)
{
	return pHan->nFuzzyFlags;
}


PY_UINT16 PYHan_GetNodeSegmentTotal(PY_HAN* pHan)
{
	return pHan->pData->nNodeSegmentTotal;
}

const PY_RANGE16* PYHan_GetNodeSegmentRange(PY_HAN* pHan, PY_UINT16 nNodeLen)
{
	if (nNodeLen < pHan->pData->nNodeSegmentTotal)
	{
		return &pHan->pData->pNodeSegment[nNodeLen];
	}

	return PY_NULL;
}

PY_UINT16 PYHan_GetNodeTotal(PY_HAN* pHan)
{
	return pHan->pData->nNodeItemTotal;
}

const PY_NODE_ITEM* PYHan_GetNodeItem(PY_HAN* pHan, PY_UINT16 nNodeId)
{
	return &pHan->pData->pNodeItem[nNodeId];
}

PY_PCSTR  PYHan_GetNodeString(PY_HAN* pHan, PY_UINT16 nNodeId)
{
	return &pHan->pData->pNodeStr[pHan->pData->pNodeItem[nNodeId].stString.nOffset];
}

PY_UINT16 PYHan_GetNodeStringLen(PY_HAN* pHan, PY_UINT16 nEncodeId)
{
	if (nEncodeId < pHan->pData->nNodeItemTotal)
	{
		return pHan->pData->pNodeItem[nEncodeId].stString.nLen;
	}

	return 0;
}

PY_INT32 PYHan_Compare_EncodeItem(PY_PCSTR pEncodeString, const PY_NODE_ITEM* pEncodeItem, PY_PCSTR pEncodeBaseStr)
{
	return PY_strncmp(pEncodeString, pEncodeBaseStr + pEncodeItem->stString.nOffset, pEncodeItem->stString.nLen);
}

const PY_RANGE16* PYHan_GetNodePinyinRange(PY_HAN* pHan, PY_UINT16 nNodeId)
{
	if (nNodeId < pHan->pData->nNodeItemTotal)
	{
		return &pHan->pData->pNodeItem[nNodeId].stPinyin;
	}

	return PY_NULL;
}

PY_UINT16 PYHan_GetNodeId(PY_HAN* pHan, PY_PCSTR pNodeString, PY_UINT16 nNodeLen)
{
	if (pNodeString && nNodeLen < PYHan_GetNodeSegmentTotal(pHan))
	{
		PY_NODE_ITEM* pItemFound = (PY_NODE_ITEM*)PY_binsearch(pNodeString,
			&pHan->pData->pNodeItem[pHan->pData->pNodeSegment[nNodeLen].nBegin],
			pHan->pData->pNodeSegment[nNodeLen].nEnd - pHan->pData->pNodeSegment[nNodeLen].nBegin,
			(PY_UINT32)sizeof(PY_NODE_ITEM),
			(PY_compare)PYHan_Compare_EncodeItem,
			(PY_PVOID)pHan->pData->pNodeStr);

		if (pItemFound)
		{
			return (PY_UINT16)(pItemFound - pHan->pData->pNodeItem);
		}
	}

	return PY_NODE_ID_INVALID;
}

const PY_NODE_PINYIN* PYHan_GetNodePinyinItemValid(PY_HAN* pHan, PY_UINT16 nEncodePinyinId)
{
	if (pHan->nNodeEnabled[nEncodePinyinId])
	{
		return &pHan->pData->pNodePinyin[nEncodePinyinId];
	}

	return PY_NULL;
}

const PY_NODE_PINYIN* PYHan_GetNodePinyinItem(PY_HAN* pHan, PY_UINT16 nEncodePinyinId)
{
	return &pHan->pData->pNodePinyin[nEncodePinyinId];
}

PY_UINT16  PYHan_GetKeyTotal(PY_HAN* pHan)
{
	return pHan->pData->nKeyItemTotal;
}

const PY_KEY_ITEM* PYHan_GetKeyItem(PY_HAN* pHan, PY_UINT16 nIndex)
{
	return &pHan->pData->pKeyItem[nIndex];
}

PY_INT32 PYHan_Compare_KeyItem(PY_UINT16* pKeyId, const PY_KEY_ITEM* pKeyItem, PY_VOID* pVoid)
{
	return *pKeyId - pKeyItem->nKey;
}
PY_BOOL PYHan_GetKeyRange(PY_HAN* pHan, PY_INT16 nKeyId, PY_RANGE16* pRange)
{

	PY_UINT16 i;
	pRange->nBegin = PY_binsearchmin(&nKeyId, pHan->pData->pKeyItem, pHan->pData->nKeyItemTotal, sizeof(PY_KEY_ITEM), (PY_compare)PYHan_Compare_KeyItem, PY_NULL);
	pRange->nEnd = pHan->pData->nKeyItemTotal;
	for (i = pRange->nBegin; i < pHan->pData->nKeyItemTotal; i++)
	{
		if (pHan->pData->pKeyItem[i].nKey != nKeyId)
		{
			pRange->nEnd = i;
			break;
		}
	}

	return PY_TRUE;
}

PY_UINT16 PYHan_GetHashGroup(PY_HAN* pHan, PY_UINT16* pHanString, PY_UINT16 nHanLen)
{
	PY_UINT16 nPinyin1Id = 0;
	PY_UINT16 nPinyin2Id = 0;

	if (nHanLen == 1)
	{
		nPinyin1Id = PYHan_GetHanPinyinId(pHan, pHanString[0]);
		return PYHan_GetPinyinGroupId(pHan, nPinyin1Id);
	}

	if (nHanLen > 1)
	{
		nPinyin1Id = PYHan_GetHanPinyinId(pHan, pHanString[0]);
		nPinyin2Id = PYHan_GetHanPinyinId(pHan, pHanString[1]);
		return (PYHan_GetPinyinGroupId(pHan, nPinyin1Id) << 8) | PYHan_GetPinyinGroupId(pHan, nPinyin2Id);
	}

	return 0;
}

PY_UINT32 PYHan_GetPinyinGroup2(PY_HAN* pHan, PY_UINT16* pHanString, PY_UINT16 nHanLen)
{
	PY_UINT16 nPinyin1Id = 0;
	PY_UINT16 nPinyin2Id = 0;

	if (nHanLen == 1)
	{
		nPinyin1Id = PYHan_GetHanPinyinId(pHan, pHanString[0]);
		return nPinyin1Id;
	}

	if (nHanLen > 1)
	{
		nPinyin1Id = PYHan_GetHanPinyinId(pHan, pHanString[nHanLen - 2]);
		nPinyin2Id = PYHan_GetHanPinyinId(pHan, pHanString[nHanLen - 1]);

		return PYHan_GetPinyinGroup2ByPinyinId(pHan, nPinyin1Id, nPinyin2Id);
	}

	return 0;
}

PY_UINT32 PYHan_GetPinyinGroup1(PY_HAN* pHan, PY_UINT16* pHanString, PY_UINT16 nHanLen)
{
	PY_UINT16 nPinyin1Id = 0;

	if (nHanLen > 0)
	{
		nPinyin1Id = PYHan_GetHanPinyinId(pHan, pHanString[nHanLen - 1]);
		return nPinyin1Id;
	}

	return 0;
}

PY_UINT32 PYHan_GetPinyinGroup1Total(PY_HAN* pHan)
{
	return PYHan_GetPinyinTotal(pHan);
}

PY_UINT32 PYHan_GetPinyinGroup2Total(PY_HAN* pHan)
{
	return PYHan_GetPinyinTotal(pHan) * PYHan_GetPinyinTotal(pHan);
}

PY_UINT32 PYHan_GetPinyinGroup2ByPinyinId(PY_HAN* pHan, PY_UINT16 nPinyin1Id, PY_UINT16 nPinyin2Id)
{
	return nPinyin1Id * PYHan_GetPinyinTotal(pHan) + nPinyin2Id;
}

PY_PCSTR  PYHan_GetNodeTypeDescribe(PY_UINT16 eNodeType)
{
	const static char szDescribe[][10] = { "", "qunapin", "jianpin" , "", "buquan" };

	return szDescribe[eNodeType];
}

PY_PCSTR PYHan_GetHanDescribe(PY_HAN* pHan, PY_UINT16* pHanString, PY_UINT8 nHanLen)
{
	static char szLex[256];

	char szDUtf8[64] = "";
	char szDPinyin[256] = "";
	PY_UINT16 nUtf8Len = 64;
	PY_UINT16 nPinyinLen = 256;

	PYHan_DecodeUtf8(pHan, pHanString, nHanLen, (PY_PSTR)szDUtf8, &nUtf8Len, (PY_PSTR)szDPinyin, &nPinyinLen, '\'');
	sprintf(szLex, "%s|%s", szDUtf8, szDPinyin);

	return szLex;
}

