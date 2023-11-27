#ifndef __PY_GRAPH_H__
#define __PY_GRAPH_H__

#include "PYPriDef.h"
#include "PYHan.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _PY_GRAPH_MATH_ITEM
{
	PY_UINT16 nNodeId;
	PY_UINT8  nNodeType;
	PY_UINT8  nNodeLen;
	PY_PCSTR  pNodeString;
	PY_UINT16 nNodeCost;
	PY_UINT8  nNodeValid;
	PY_UINT16 nPinyinId;
	PY_UINT8  nPrevCol;
	PY_UINT8  nNextCol;
	PY_UINT16 nNextItem;

} PY_GRAPH_MATH_ITEM;

typedef struct _PY_GRAPH_PINYIN_ITEM
{
	PY_UINT8  nNodeType;       // 拼音节点类型

	PY_UINT16 nMatchItemBegin; /* 匹配节点开始位置 */
	PY_UINT16 nMatchItemEnd;   /* 匹配节点结束位置 */

} PY_GRAPH_PINYIN_ITEM;

typedef struct _PY_GRAPH_ITEM
{
	PY_UINT16  nKeyId;
	PY_UINT8 nRoadType; // 路径节点类型 
	PY_UINT8 nRoadCost; // 路径上最少简拼数量 

	PY_UINT16 nMatchItemBegin; // 当前节点下全部有效拼音数量 

	PY_GRAPH_PINYIN_ITEM stPinyinList[PY_PINYIN_TOTAL];  // 拼音节点数据

} PY_GRAPH_ITEM;

typedef struct _PY_GRAPH_HAN_MATCH
{
	PY_UINT16 nMatchCol;
	PY_UINT16 nMatchNodeId;
} PY_GRAPH_HAN_MATCH;

typedef struct _PY_GRAPH
{
	PY_HAN	*pHan;

	PY_UINT16 nMathItemTotal;
	PY_GRAPH_MATH_ITEM stMathItem[40960]; /* 匹配的Node节点 */

	PY_UINT16 nGroup1Total;
	PY_UINT32 nGroup1[PY_PINYIN_TOTAL + 1]; /* 有效分组ID */

	PY_UINT16 nGroup2Total;
	PY_UINT32 nGroup2[40960]; /* 有效分组ID */

	PY_UINT8 nValidMartix[PY_INPUT_MAX_LEN + 1][8];
	PY_UINT8 nValidTotal[PY_INPUT_MAX_LEN + 1];

	PY_UINT16 nHanMatchTotal[PY_INPUT_MAX_LEN + 1];
	PY_UINT16 nHanSplitTotal[PY_INPUT_MAX_LEN + 1];
	PY_GRAPH_HAN_MATCH nHanMatchMatrix[PY_INPUT_MAX_LEN][4];

	PY_UINT8 nColLen;
	PY_GRAPH_ITEM	stMatrix[PY_INPUT_MAX_LEN + 1];

} PY_GRAPH;

PY_BOOL PYGraph_Initialize(PY_GRAPH *pGraph, PY_HAN *pHan);
PY_VOID PYGraph_Reset(PY_GRAPH* pGraph);

PY_BOOL PYGraph_IsValidPinyin(PY_GRAPH* pGraph, PY_UINT16 nCol, PY_UINT16 nPinyinId);

int PYGraph_AddCol(PY_GRAPH* pGraph, PY_UINT16 nKeyId, PY_KEY_NODE* pNodeIdList, PY_UINT16 nNodeIdTotal);

PY_BOOL PYGraph_SetColTotal(PY_GRAPH* pGraph, PY_UINT8 nColTotal);

int PYGraph_GetColTotal(PY_GRAPH* pGraph);
int PYGraph_GetColType(PY_GRAPH* pGraph, PY_UINT16 nColId);
PY_UINT16 PYGraph_GetColVal(PY_GRAPH* pGraph, PY_UINT16 nColId);

int PYGraph_getColTrimSplitLen(PY_GRAPH* pGraph);
// 取得最后一列匹配的的分组信息

PY_UINT32* PYGraph_GetLastColMatchGroup1Ptr(PY_GRAPH* pGraph);
PY_UINT16  PYGraph_GetLastColMatchGroup1Total(PY_GRAPH* pGraph);

PY_UINT32* PYGraph_GetLastColMatchGroup2Ptr(PY_GRAPH* pGraph);
PY_UINT16  PYGraph_GetLastColMatchGroup2Total(PY_GRAPH* pGraph);

PY_INT16 PYGraph_Match(PY_GRAPH *pGraph, PY_UINT16 nBeginCol, PY_UINT16 nEndCol, const PY_UINT16* pHanString, PY_UINT16 nHanLen, PY_PWSTR pCompString, PY_UINT16 *pCompLen, PY_BOOL bSplit);

int PYGraph_Dump(PY_GRAPH *pGraph);

#ifdef __cplusplus
}
#endif

#endif
