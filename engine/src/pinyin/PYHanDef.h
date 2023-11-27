#ifndef __PY_HAN_DEF_H__
#define __PY_HAN_DEF_H__

#include "PYTypeDef.h"

typedef struct _PY_PINYIN_ITEM
{
    PY_UINT16 nOffset;
    PY_UINT8  nLen;
    PY_UINT8  nHash;

} PY_PINYIN_ITEM;

typedef struct _PY_OFFSET
{
    PY_UINT16 nOffset;
    PY_UINT16 nLen;
    
} PY_OFFSET;

typedef PY_RANGE16 PY_NODE_SEGMENT;

typedef struct _PY_NODE_ITEM
{
    PY_OFFSET   stString;
    PY_RANGE16  stPinyin;

} PY_NODE_ITEM;

typedef struct _PY_NODE_PINYIN
{
    PY_UINT32 nFuzzyFlags;
    PY_UINT16 nNodeType;
    PY_UINT16 nPinyinId;

} PY_NODE_PINYIN;

typedef struct _PY_ENCODE_ITEM
{
    PY_UINT16 nUnicode;
    PY_UINT16 nPinyinId;
    
} PY_HAN_ITEM;

typedef struct _PY_HAN_R_ITEM
{
    PY_UINT16 nUnicode;
    PY_UINT16 nPinyinId;
    PY_UINT16 nHanId;

} PY_HAN_R_ITEM;

typedef struct _PY_KEY_ITEM
{
    PY_UINT16 nKey;
    PY_UINT16 nNode;

} PY_KEY_ITEM;

typedef struct _PY_HAN_DATA
{
    PY_UINT16 nPinyinItemTotal;
    PY_UINT16 nPinyinStrSize;
    PY_UINT16 nPinyinMaxLen;
    PY_UINT16 nHashGroupTotal;

    PY_UINT16 nHanItemTotal;
    
    PY_UINT16 nNodeSegmentTotal;
    PY_UINT16 nNodeItemTotal;
    PY_UINT16 nNodeStrSize;
    PY_UINT16 nNodePinyinTotal;
    
    PY_UINT16 nKeyItemTotal;

    const PY_PINYIN_ITEM    *pPinyinItem;
    const PY_CHAR           *pPinyinStr;
    
    const PY_HAN_ITEM       *pHanItem;
    const PY_HAN_R_ITEM     *pHanRItem;
    
    const PY_NODE_SEGMENT   *pNodeSegment;
    const PY_NODE_ITEM      *pNodeItem;
    const PY_CHAR           *pNodeStr;
    const PY_NODE_PINYIN    *pNodePinyin;
    
    const PY_KEY_ITEM       *pKeyItem;

} PY_HAN_DATA;

#endif
