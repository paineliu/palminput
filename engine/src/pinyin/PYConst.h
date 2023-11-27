#ifndef __PY_CONST_H__
#define __PY_CONST_H__

#define PY_FUZZY_SM_Z_ZH          0x00000001
#define PY_FUZZY_SM_C_CH          0x00000002
#define PY_FUZZY_SM_S_SH          0x00000004
#define PY_FUZZY_SM_F_H           0x00000008
#define PY_FUZZY_SM_N_L           0x00000010
#define PY_FUZZY_SM_L_R           0x00000020
#define PY_FUZZY_YM_AN_ANG        0x00000040
#define PY_FUZZY_YM_EN_ENG        0x00000080
#define PY_FUZZY_YM_IN_ING        0x00000100
#define PY_FUZZY_YM_IAN_IANG      0x00000200
#define PY_FUZZY_YM_UAN_UANG      0x00000400
#define PY_FUZZY_QP_WANG_HUANG    0x00000800
#define PY_FUZZY_QP_AOU_AOUXX     0x00001000
#define PY_FUZZY_JP_Z_ZH          0x00002000
#define PY_FUZZY_JP_C_CH          0x00004000
#define PY_FUZZY_JP_S_SH          0x00008000
#define PY_FUZZY_JP_ZH_Z          0x00010000
#define PY_FUZZY_JP_CH_C          0x00020000
#define PY_FUZZY_JP_SH_S          0x00040000
#define PY_FUZZY_JP_F_H           0x00080000
#define PY_FUZZY_JP_N_L           0x00100000
#define PY_FUZZY_JP_L_R           0x00200000

#define PY_PINYIN_TOTAL           454
#define PY_PINYIN_MAX_LEN         6
#define PY_HAN_TOTAL              25645
#define PY_NODE_TOTAL             584
#define PY_NODE_PINYIN_TOTAL      1623

#define PY_NODE_TYPE_QUANPIN      0x01
#define PY_NODE_TYPE_JIANPIN      0x02
#define PY_NODE_TYPE_BUQUAN       0x04
#define PY_NODE_TYPE_SPLIT        0x08
#define PY_NODE_TYPE_LETTER       0x10

#endif
