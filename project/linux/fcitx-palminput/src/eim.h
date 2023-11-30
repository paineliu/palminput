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

#ifndef EIM_H
#define EIM_H

#ifdef __cplusplus
#define __EXPORT_API extern "C"
#else
#define __EXPORT_API
#endif

#include <fcitx/ime.h>
#include <fcitx/instance.h>
#include <fcitx/candidate.h>



__EXPORT_API void* FcitxPalmInputCreate(FcitxInstance *instance);
__EXPORT_API void FcitxPalmInputDestroy(void *arg);
__EXPORT_API boolean FcitxPalmInputInit(void *arg);
__EXPORT_API void FcitxPalmInputReset(void *arg);
__EXPORT_API INPUT_RETURN_VALUE FcitxPalmInputDoInput(void *arg, FcitxKeySym sym, unsigned int state);
__EXPORT_API INPUT_RETURN_VALUE FcitxPalmInputGetCandWords(void *arg);
__EXPORT_API void FcitxPalmInputReloadConfig(void *arg);
__EXPORT_API void FcitxPalmInputSave(void *arg);;

__EXPORT_API INPUT_RETURN_VALUE FcitxPalmInputSelCandWord(void *arg, FcitxCandidateWord *candWord);

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 0;
