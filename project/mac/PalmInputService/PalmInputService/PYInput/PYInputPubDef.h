#ifndef __PY_INPUT_PUB_DEF_H__
#define __PY_INPUT_PUB_DEF_H__

#import <Foundation/Foundation.h>

// 配置文件
#define PY_PREF_PATH_NAME                                @"~/Library/Application Support/PalmInput"
#define PY_PREF_PREFERENCE_FILE_NAME                     @"palm_preference.plist"

// 通知消息

#define PY_NOTIFICATION_SWITCH_LANGUAGE_NAME               @"PY_NOTIFICATION_SWITCH_LANGUAGE_NAME"
#define PY_NOTIFICATION_SWITCH_LANGUAGE_KEY                @"PY_Switch_Language_key"
#define PY_NOTIFICATION_SWITCH_LANGUAGE_VALUE_EN           1
#define PY_NOTIFICATION_SWITCH_LANGUAGE_VALUE_CN           0


#define PY_NOTIFICATION_PREFERENCE_CHANGED_NAME                 @"PY_NOTIFICATION_PREFERENCE_CHANGED_NAME"
#define PY_NOTIFICATION_PREFERENCE_CHANGED_KEY                  @"PY_Pref_Changed"
#define PY_NOTIFICATION_PREFERENCE_CHANGED_VALUE_NONE      0
#define PY_NOTIFICATION_PREFERENCE_CHANGED_VALUE_KEY       0x1
#define PY_NOTIFICATION_PREFERENCE_CHANGED_VALUE_DICT      0x2
#define PY_NOTIFICATION_PREFERENCE_CHANGED_VALUE_SKIN      0x4
#define PY_NOTIFICATION_PREFERENCE_CHANGED_VALUE_ENGINE    0x8
#define PY_NOTIFICATION_PREFERENCE_CHANGED_VALUE_ALL       0xF

// 拼音方案 [全拼 双拼]
#define PY_PREF_PINYIN_SCHEME_ID    @"PinyinScheme"
#define PY_PREF_PINYIN_SCHEME_VALUE_QUANPIN    0
#define PY_PREF_PINYIN_SCHEME_VALUE_SHUANGPIN  1

// 双拼方案
#define PY_PREF_SHUANGPIN_SCHEME_ID    @"ShuangpinScheme"
#define PY_PREF_SHUANGPIN_SCHEME_VALUE_NONE                0
#define PY_PREF_SHUANGPIN_SCHEME_VALUE_ZHINENG_ABC         1
#define PY_PREF_SHUANGPIN_SCHEME_VALUE_MICROSOFT           2
#define PY_PREF_SHUANGPIN_SCHEME_VALUE_JIAJIA              3
#define PY_PREF_SHUANGPIN_SCHEME_VALUE_ZIGUANG             4
#define PY_PREF_SHUANGPIN_SCHEME_VALUE_ZIRANMA             5
#define PY_PREF_SHUANGPIN_SCHEME_VALUE_XIAOHE              6
#define PY_PREF_SHUANGPIN_SCHEME_VALUE_SOGOU               7

// 皮肤

#define PY_PREF_SKIN_LIST_ID                 @"SkinList"
#define PY_PREF_SKIN_USED_GUID               @"SkinUsed"

#define PY_PREF_SKIN_GUID                    @"GUID"
#define PY_PREF_SKIN_NAME                    @"Name"

#define PY_PREF_SKIN_BG_COLOR_A              @"BGColorA"
#define PY_PREF_SKIN_BG_COLOR_R              @"BGColorR"
#define PY_PREF_SKIN_BG_COLOR_G              @"BGColorG"
#define PY_PREF_SKIN_BG_COLOR_B              @"BGColorB"
#define PY_PREF_SKIN_HOT_COLOR_R             @"HotColorR"
#define PY_PREF_SKIN_HOT_COLOR_G             @"HotColorG"
#define PY_PREF_SKIN_HOT_COLOR_B             @"HotColorB"
#define PY_PREF_SKIN_NOR_COLOR_R             @"NorColorR"
#define PY_PREF_SKIN_NOR_COLOR_G             @"NorColorG"
#define PY_PREF_SKIN_NOR_COLOR_B             @"NorColorB"


#define PY_PREF_CAND_FONT_ID                  @"CandFont"
#define PY_PREF_CAND_FONT_NAME                @"CandFontName"
#define PY_PREF_CAND_FONT_SIZE                @"CandFontSize"


// 提示文字
#define PY_TIP_MODE_V                                              @"·数字（如：v123）	·日期（如：v2020）"
#define PY_TIP_MODE_U                                              @"·拆字输入，如：u'hspn（木），u'mg（杆）"

// 云输入

#define PY_CLOUD_REPLACE_STRING_LONG                               @"    "
#define PY_CLOUD_REPLACE_STRING_SHORT                              @"   "

#define PY_CLOUD_RES_SAME_FILE_NAME                                      @"CloudStateSame.png"
#define PY_CLOUD_RES_DIFF_FILE_NAME                                      @"CloudStateDiff.png"
#define PY_CLOUD_RES_LOAD_FILE_NAME                                      @"CloudStateLoad.png"
#define PY_CLOUD_RES_FAIL_FILE_NAME                                      @"CloudStateFail.png"

#endif
