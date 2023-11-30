/***************************************************************************
 *   Copyright (C) 2010~2011 by CSSlayer                                   *
 *   wengxt@gmail.com                                                      *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <libintl.h>

#include <fcitx/ime.h>
#include <fcitx-utils/log.h>
#include <fcitx/keys.h>
#include <fcitx/instance.h>
#include <fcitx/context.h>
#include <fcitx-config/fcitx-config.h>
#include <fcitx-config/hotkey.h>
#include <fcitx-config/xdg.h>
#include <fcitx-utils/utils.h>
#include <fcitx/candidate.h>

#include "PYEngine.h"
#include "PDLexCore.h"
#include "PYDynEx.h"

#define _(x) dgettext("fcitx-palminput", (x))

#include "eim.h"


#ifdef __cplusplus
extern "C" {
#endif
    FCITX_EXPORT_API
    FcitxIMClass ime = {
        FcitxPalmInputCreate,
        FcitxPalmInputDestroy
    };

    FCITX_EXPORT_API
    int ABI_VERSION = FCITX_ABI_VERSION;
#ifdef __cplusplus
}
#endif

CONFIG_DESC_DEFINE(GetPalmInputConfigDesc, "fcitx-palminput.desc")


typedef struct _PalmInputCandWord {
    int index;
} PalmInputCandWord;


typedef struct _FcitxPalmInput
{
    FcitxInstance* owner;

    PY_HANDLE handle;
    PY_BLOCK user_data;
} FcitxPalmInput;


#define PY_LEX_PATH          "/usr/local/share/fcitx/palminput/"
#define PY_LEX_PINYIN_SYS    "py_sys.lex"
#define PY_LEX_PINYIN_USR    "py_usr.lex"

bool LoadDict(char *pFileName, PY_BLOCK *pBlock)
{
    FILE	*fp;
    long	nSize;

    fp = fopen(pFileName, "rb");
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        nSize = ftell(fp);

        char *pData = (char *) malloc(nSize);
        fseek(fp, 0, SEEK_SET);
        fread(pData, 1, nSize, fp);

        pBlock->nSize		= nSize;
        pBlock->pAddress	= (PY_UINT8 *) pData;

        fclose(fp);

        return true;
    }

    return false;
}

bool LoadUserDict(PY_BLOCK *pBlock)
{
    long	nSize;
    FILE* fp = FcitxXDGGetFileUserWithPrefix("palminput", "py_usr.lex", "rb", NULL);

    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        nSize = ftell(fp);

        char *pData = (char *) malloc(nSize);
        fseek(fp, 0, SEEK_SET);
        fread(pData, 1, nSize, fp);

        pBlock->nSize		= nSize;
        pBlock->pAddress	= (PY_UINT8 *) pData;

        fclose(fp);

        return true;
    }

    return false;
}

void ReleaseEngine(PY_HANDLE hEngine)
{
    if (NULL != hEngine)
    {
        free(hEngine);
        hEngine = PY_NULL;
    }
}

PY_HANDLE CreateEngine(PY_BLOCK *pLexBlock, PY_UINT32 nLexTotal)
{
    PY_HANDLE			hEngine = PY_NULL;
    PY_BYTE				*pInstance = PY_NULL;
    PY_UINT32			nInstanceSize = 0;
   

    nInstanceSize = PYEngine_GetInstanceSize();
    pInstance = (PY_BYTE *) malloc(nInstanceSize);
    if (pInstance)
    {
        hEngine = PYEngine_Initialize(pInstance, nInstanceSize, pLexBlock, nLexTotal);           
    }

    return hEngine;
}


void GetCandString(FcitxPalmInput *palmInput, int index, char *pCandStr, int candMaxLen)
{
    int retlen = PYEngine_GetCandItemStringUtf8(palmInput->handle, index, (PY_CHAR *)pCandStr, candMaxLen);
    pCandStr[retlen] = 0;
}

bool SelCandString(FcitxPalmInput *palmInput, int index, char *pCommitStr, int commitMaxLen)
{
    if (PYEngine_SelCandItem(palmInput->handle, index) == 1)
    {
        int len = PYEngine_GetConvertedStringUtf8(palmInput->handle, (PY_CHAR *) pCommitStr, commitMaxLen);
        pCommitStr[len] = 0;
        return len > 0;
    }

    return false;
}


int GetInputKeyLen(FcitxPalmInput *palmInput)
{
    PY_UINT16 szBuf[256];
    PY_UINT16 nCaretPos;
    int retlen = PYEngine_GetCompString(palmInput->handle, 0, (PY_PWSTR) szBuf, 256, &nCaretPos, PY_FALSE);
    szBuf[retlen] = 0;

    return retlen;
}


void UpdateCand(FcitxPalmInput *palmInput)
{
    FcitxInstance* instance = palmInput->owner;
    FcitxInputState* input = FcitxInstanceGetInputState(instance);

    size_t len = 0;
    FcitxLog(DEBUG, "len: %lu", len);

    FcitxInstanceCleanInputWindowUp(instance);

    int candTotal = PYEngine_GetCandTotal(palmInput->handle);

    PY_UINT16 nCaretPos = 0;

    {
        char szBuf[256];

        int retlen = PYEngine_GetCompStringUtf8(palmInput->handle, 0, (PY_CHAR *) szBuf, 256, &nCaretPos, 1);
        szBuf[retlen] = 0;


        FcitxMessagesAddMessageAtLast(FcitxInputStateGetPreedit(input), MSG_INPUT, "%s", szBuf);
        FcitxInputStateSetCursorPos(input, nCaretPos);
    }

    {
        char szBuf[256];

        int retlen = PYEngine_GetCompStringUtf8(palmInput->handle, 0, (PY_CHAR *) szBuf, 256, &nCaretPos, 0);
        szBuf[retlen] = 0;

        strcpy(FcitxInputStateGetRawInputBuffer(input), szBuf);
        FcitxInputStateSetRawInputBufferSize(input,  retlen);

    }

    FcitxInputStateSetShowCursor(input, true);
    FcitxInstanceCleanInputWindowDown(instance);

//    FcitxMessagesAddMessageAtLast(FcitxInputStateGetClientPreedit(input), MSG_INPUT, "%s", palmInput->buf);
//    FcitxInputStateSetClientCursorPos(input, 0);

    int index = 0;
    char szCandStr[256];

    for (int i = 0 ;i < candTotal; i ++, index ++)
    {
        GetCandString(palmInput, i, szCandStr, 256);

        PalmInputCandWord *palmInputCandWord = (PalmInputCandWord *) fcitx_utils_malloc0(sizeof(PalmInputCandWord));
        palmInputCandWord->index = i;

        FcitxCandidateWord candWord;
        candWord.callback = FcitxPalmInputSelCandWord;
        candWord.owner = palmInput;
        candWord.priv = palmInputCandWord;
        candWord.strExtra = NULL;
        candWord.strWord = strdup(szCandStr);
        candWord.wordType = MSG_OTHER;

        FcitxCandidateWordAppend(FcitxInputStateGetCandidateList(input), &candWord);
    }
}


void im_destory(void *arg)
{
    FcitxPalmInput* palmInput = (FcitxPalmInput*) arg;
    ReleaseEngine(palmInput->handle);
    free(palmInput->user_data.pAddress);
    free(arg);
}

boolean im_init(void *arg)
{
    return true;
}

void im_reset(void *arg)
{
    FcitxPalmInput* palmInput = (FcitxPalmInput*) arg;

    PYEngine_Reset(palmInput->handle);
}


void im_save(void *arg)
{
    FcitxPalmInput* palmInput = (FcitxPalmInput*) arg;
    if (palmInput->user_data.pAddress != NULL)
    {
        FILE* fp = FcitxXDGGetFileUserWithPrefix("palminput", "py_usr.lex", "wb", NULL);
        if (fp)
        {
             fwrite(palmInput->user_data.pAddress, 1, palmInput->user_data.nSize, fp);
             fclose(fp);
        }
    }
}

/**
 * @brief initialize the extra input method
 *
 * @param arg
 * @return successful or not
 **/
void* FcitxPalmInputCreate(FcitxInstance *instance)
{

    FcitxPalmInput* palmInput = (FcitxPalmInput*) fcitx_utils_malloc0(sizeof(FcitxPalmInput));
    bindtextdomain("fcitx-palminput", LOCALEDIR);

    palmInput->owner = instance;
    
    PY_BLOCK            stLexBlock[64];
    PY_UINT16           nLexTotal = 0;
    char szFileName[260];

	sprintf(szFileName, "%s%s", PY_LEX_PATH, PY_LEX_PINYIN_SYS);
	if (LoadDict(szFileName, &stLexBlock[nLexTotal]))
	{
	    nLexTotal++;
	}
	else
	{
        stLexBlock[nLexTotal].pAddress = (PY_UINT8 *) PDLexCore_ptr();
        stLexBlock[nLexTotal].nSize = PDLexCore_size();
        nLexTotal++;
    }
    
	if (LoadUserDict(&stLexBlock[nLexTotal]))
	{
        palmInput->user_data = stLexBlock[nLexTotal];

		nLexTotal++;
	}
	else
	{
		if (PYDyn_CreateInBlock(4 * 1024 * 1024, &stLexBlock[nLexTotal]))
		{
            palmInput->user_data = stLexBlock[nLexTotal];
    		nLexTotal++;
		}
		else
		{
		    palmInput->user_data.pAddress = NULL;
		    palmInput->user_data.nSize = 0;
		}
	}
	
    palmInput->handle = CreateEngine(stLexBlock, nLexTotal);


    FcitxInstanceRegisterIM(instance,
                            palmInput,
                            "palminput",
                            _("Palm Input"),
                            "palminput",
                            FcitxPalmInputInit,
                            FcitxPalmInputReset,
                            FcitxPalmInputDoInput,
                            FcitxPalmInputGetCandWords,
                            NULL,
                            FcitxPalmInputSave,
                            FcitxPalmInputReloadConfig,
                            NULL,
                            5,
                            "zh_CN"
    );

    return palmInput;
}

/**
 * @brief Destroy the input method while unload it.
 *
 **/
void FcitxPalmInputDestroy(void *arg)
{
    im_destory(arg);
}

/**
 * @brief init the status.
 *
 **/
boolean FcitxPalmInputInit(void *arg)
{
    FcitxPalmInput *palmInput = (FcitxPalmInput *) arg;
    FcitxInstanceSetContext(palmInput->owner, CONTEXT_IM_KEYBOARD_LAYOUT, "us");

    im_init(arg);

    return true;
}

/**
 * @brief Reset the status.
 *
 **/
void FcitxPalmInputReset(void *arg)
{
    im_reset(arg);
}

/**
 * @brief Process Key Input and return the status
 *
 * @param keycode keycode from XKeyEvent
 * @param state state from XKeyEvent
 * @param count count from XKeyEvent
 * @return INPUT_RETURN_VALUE
 **/
INPUT_RETURN_VALUE FcitxPalmInputDoInput(void *arg, FcitxKeySym sym, unsigned int state)
{
    FcitxPalmInput *palmInput = (FcitxPalmInput *) arg;
    FcitxInstance* instance = palmInput->owner;
    FcitxInputState* input = FcitxInstanceGetInputState(instance);

    if (FcitxHotkeyIsHotKeyDigit(sym, state))
    {
        if (GetInputKeyLen(palmInput) > 0 && (sym & 0xff) == '0')
        {
            if (PYEngine_GetFilterType(palmInput->handle) == PY_FILTER_NONE)
            {
                PYEngine_SetFilterType(palmInput->handle, PY_FILTER_SINGLE_WORD);
            }
            else
            {
                FcitxCandidateWordGoNextPage(FcitxInputStateGetCandidateList(input));
                return IRV_DISPLAY_MESSAGE;
            }

            return IRV_DISPLAY_CANDWORDS;

        }
        return IRV_TO_PROCESS;
    }

    if (FcitxHotkeyIsHotKeySimple(sym, state))
    {

        if (FcitxHotkeyIsHotKeyLAZ(sym, state) || sym == '\'')
        {
            PYEngine_KeyEvent(palmInput->handle, sym & 0xff, 0);
            return IRV_DISPLAY_CANDWORDS;
        }
        else if (FcitxHotkeyIsHotKey(sym, state, FCITX_SPACE))
        {
            if (GetInputKeyLen(palmInput) == 0)
            {
                return IRV_TO_PROCESS;
            }

            return FcitxCandidateWordChooseByIndex(FcitxInputStateGetCandidateList(input), 0);
        }
    }

    if (FcitxHotkeyIsHotKey(sym, state, FCITX_BACKSPACE) || FcitxHotkeyIsHotKey(sym, state, FCITX_DELETE))
    {

        if (GetInputKeyLen(palmInput) > 0)
        {
            PYEngine_KeyEvent(palmInput->handle, 0x08, 0);

            return IRV_DISPLAY_CANDWORDS;
        }
        else
            return IRV_TO_PROCESS;
    }
    else
    {
        if (GetInputKeyLen(palmInput) > 0)
        {
            if (FcitxHotkeyIsHotKey(sym, state, FCITX_LEFT))
            {
                PYEngine_MoveCaret(palmInput->handle, PY_CARET_MOVE_LEFT_UNIT);
                return IRV_DISPLAY_CANDWORDS;
            }
            else if (FcitxHotkeyIsHotKey(sym, state, FCITX_RIGHT))
            {
                PYEngine_MoveCaret(palmInput->handle, PY_CARET_MOVE_RIGHT_CHAR);
                return IRV_DISPLAY_CANDWORDS;
            }
            else if (FcitxHotkeyIsHotKey(sym, state, FCITX_HOME))
            {
                PYEngine_MoveCaret(palmInput->handle, PY_CARET_MOVE_HOME);
                return IRV_DISPLAY_CANDWORDS;
            }
            else if (FcitxHotkeyIsHotKey(sym, state, FCITX_END))
            {
                PYEngine_MoveCaret(palmInput->handle, PY_CARET_MOVE_END);
                return IRV_DISPLAY_CANDWORDS;
            }
        }
        else
        {
            return IRV_TO_PROCESS;
        }
    }

    return IRV_TO_PROCESS;
}

/**
 * @brief function DoInput has done everything for us.
 *
 * @param searchMode
 * @return INPUT_RETURN_VALUE
 **/
INPUT_RETURN_VALUE FcitxPalmInputGetCandWords(void *arg)
{
    FcitxPalmInput* palmInput = (FcitxPalmInput *) arg;
    FcitxInstance* instance = palmInput->owner;
    FcitxInputState* input = FcitxInstanceGetInputState(instance);
    FcitxGlobalConfig* config = FcitxInstanceGetGlobalConfig(palmInput->owner);

    FcitxCandidateWordSetPageSize(FcitxInputStateGetCandidateList(input), config->iMaxCandWord);
    FcitxCandidateWordSetChoose(FcitxInputStateGetCandidateList(input), DIGIT_STR_CHOOSE);

//    if (DecodeIsDone(palmInput))
//    {
//        GetCandString(palmInput, 0);
//        size_t len;
//        ime_pinyin::im_get_sps_str(&len);
//
//        strcpy(FcitxInputStateGetOutputString(input), palmInput->ubuf);
//        strcat(FcitxInputStateGetOutputString(input), palmInput->buf + len);
//        if (strlen(FcitxInputStateGetOutputString(input)) == 0)
//            return IRV_CLEAN;
//        else
//            return IRV_COMMIT_STRING;
//    }

    UpdateCand(palmInput);

    if (GetInputKeyLen(palmInput) == 0)
        return IRV_CLEAN;

    return IRV_DISPLAY_CANDWORDS;
}

/**
 * @brief get the candidate word by index
 *
 * @param iIndex index of candidate word
 * @return the string of canidate word
 **/
INPUT_RETURN_VALUE FcitxPalmInputSelCandWord(void *arg, FcitxCandidateWord *candWord)
{
    FcitxPalmInput* palmInput = (FcitxPalmInput*) arg;
    FcitxInstance* instance = palmInput->owner;
    FcitxInputState* input = FcitxInstanceGetInputState(instance);
    PalmInputCandWord* ggCand = (PalmInputCandWord*) candWord->priv;

    char szCommitStr[256] = "";

    if (SelCandString(palmInput, ggCand->index, szCommitStr, 256))
    {
        strcpy(FcitxInputStateGetOutputString(input), szCommitStr);
        if (strlen(FcitxInputStateGetOutputString(input)) == 0)
            return IRV_CLEAN;
        else
            return IRV_COMMIT_STRING;
    }
    else
        return IRV_DISPLAY_CANDWORDS;
}

/**
 * @brief load config.
 *
 **/
void FcitxPalmInputReloadConfig(void *arg)
{
}

/**
 * @brief save dict.
 *
 **/
void FcitxPalmInputSave(void *arg)
{
    im_save(arg);
}

// kate: indent-mode cstyle; space-indent on; indent-width 0;
