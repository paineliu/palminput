#ifndef __PY_PRI_DEF_H__
#define __PY_PRI_DEF_H__

#include "PYTypeDef.h"
#include "PYConst.h"
#include "PYUtils.h"
#include "PYHan.h"

#define PY_ENGINE_VERSION_MAJOR 1
#define PY_ENGINE_VERSION_MINOR 4

#define PY_INPUT_MAX_LEN      255
#define PY_TERM_LEX_MAX_NUM   64
#define PY_PINYIN_ID_INVALID  (-1)
#define PY_NODE_ID_INVALID    (0xFFFF)
#define PY_HAN_ID_INVALID     (0xFFFF)
#define PY_MATCH_ITEM_MAX_NUM  809600
#define PY_CAND_ITEM_MAX_NUM   2048
#define PY_PREDICT_ITEM_MAX_NUM   32
#define PY_SEN_MAX_NUM         8
#define PY_DYN_PHRASE_MAX_LEN  24
#define PY_CHAR_MAX_ID         127

#define PY_MAX_PATH 260

#define PY_LEX_UNKNOWN 0
#define PY_LEX_CORE    1
#define PY_LEX_USER    2
#define PY_LEX_TERM1   3
#define PY_LEX_TERM2   4

#define PY_CAND_FIX_ITEM    5
/**
@brief 词库文件头
*/
typedef struct _PY_FILE_HEAD
{
	PY_UINT32		nFileMask;  /**<标识符 */
	PY_UINT32       nVersion;   /**<版本 */
	PY_UINT32		nFileSize;  /**<文件大小 */
	PY_UINT32		nTimeStamp; /**<时间戳 */

	PY_UINT32		nBlockTotal;  /**<文件块总数 */
	PY_UINT32		nBlockOffset; /**<块数据偏移位置 */
	PY_UINT32		nDataOffset;  /**<数据偏移位置 */
	PY_UINT32		nReserved;    /**<保留 */

} PY_FILE_HEAD;

typedef struct _PY_FILE_BLOCK
{
	PY_UINT32		nBlockId;
	PY_UINT32		nOffset;
	PY_UINT32		nSize;
	PY_UINT32       nReserved;

} PY_FILE_BLOCK;

#define PY_FILE_PY_SYSTEM_MASK   (('P' << 0) | ('Y' << 8) | ('S' << 16) | ('1' << 24))
#define PY_FILE_PY_SYSTEM_VER           1

#define PY_FILE_PY_TERM1_MASK   (('P' << 0) | ('Y' << 8) | ('T' << 16) | ('1' << 24))
#define PY_FILE_PY_TERM1_VER           1

#define PY_FILE_PY_TERM2_MASK   (('P' << 0) | ('Y' << 8) | ('T' << 16) | ('2' << 24))
#define PY_FILE_PY_TERM2_VER           1

#define PY_FILE_PY_DYNAMIC_MASK   (('P' << 0) | ('Y' << 8) | ('D' << 16) | ('1' << 24))
#define PY_FILE_PY_DYNAMIC_VER          1

#define PY_FILE_BLOCK_EXTEND_INFO       0xC7022076
#define PY_FILE_BLOCK_PINYIN_GROUP_L1   0x70C517DC
#define PY_FILE_BLOCK_PINYIN_GROUP_L2   0x878D68B9
#define PY_FILE_BLOCK_HASH_GROUP_L1     0x912B9237
#define PY_FILE_BLOCK_HASH_GROUP_L2     0x08E453D5
#define PY_FILE_BLOCK_PHRASE_ID         0x2EEFDCBA
#define PY_FILE_BLOCK_PHRASE_DATA       0x942AEE43
#define PY_FILE_BLOCK_SORTED_PHRASE_ID  0x675ADD34
#define PY_FILE_BLOCK_BIGRAM_PRE_WORD   0x86730934
#define PY_FILE_BLOCK_BIGRAM_POST_DATA  0x56734584


#define PY_FILE_BLOCK_DYN_SUMMARY       0x67022456
#define PY_FILE_BLOCK_DYN_GROUP         0x66787585
#define PY_FILE_BLOCK_DYN_BUFFER        0x96834985

typedef struct _PY_KEY_NODE
{
	PY_UINT16 nNodeId;
	PY_UINT8  nMatchLen;
	PY_PCSTR  pMatchString;

} PY_KEY_NODE;

#endif
