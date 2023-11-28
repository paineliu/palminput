//
//	Copyright (C) 2013 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
//
//	This library is free software; you can redistribute it and/or
//	modify it under the terms of the GNU Library General Public
//	License as published by the Free Software Foundation; either
//	version 2 of the License, or (at your option) any later version.
//
//	This library is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//	Library General Public License for more details.
//
//	You should have received a copy of the GNU Library General Public
//	License along with this library; if not, write to the
//	Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
//	Boston, MA  02110-1301, USA.
//

#include "PalmInput.h"
#include <assert.h>
#include <string>
#include <Utils.h> // libIME2
#include <LangBarButton.h> // libIME2
#include "PalmInputModule.h"
#include "resource.h"
#include <Shellapi.h>
#include <sys/stat.h>

#define CHINESE_MODE 1
#define FULLSHAPE_MODE 1
#define HALFSHAPE_MODE 0


using namespace std;

namespace Chewing {

// English/Chinses mode switch
// {E26088B0-F16C-4FD4-ADD2-768950E4B501}
static const GUID g_modeButtonGuid =
{ 0xe26088b0, 0xf16c, 0x4fd4, { 0xad, 0xd2, 0x76, 0x89, 0x50, 0xe4, 0xb5, 0x1 } };

// half shape/full shape switch
// {DA2202C8-1247-49CA-A13D-556FCB4624F2}
static const GUID g_shapeTypeButtonGuid =
{ 0xda2202c8, 0x1247, 0x49ca, { 0xa1, 0x3d, 0x55, 0x6f, 0xcb, 0x46, 0x24, 0xf2 } };

// settings button/menu
// {0285F137-1E4C-41B9-994A-B281C7B94914}
static const GUID g_settingsButtonGuid =
{ 0x285f137, 0x1e4c, 0x41b9, { 0x99, 0x4a, 0xb2, 0x81, 0xc7, 0xb9, 0x49, 0x14 } };

// shift + space
// {5981895B-FEC8-4C85-B31E-343AAC3F548B}
static const GUID g_shiftSpaceGuid =
{ 0x5981895b, 0xfec8, 0x4c85, { 0xb3, 0x1e, 0x34, 0x3a, 0xac, 0x3f, 0x54, 0x8b } };

// global compartment GUID for config change notification
// {F2F54E33-DAE4-468E-9DA4-674D95B5BEDD}
static const GUID g_configChangedGuid =
{ 0xf2f54e33, 0xdae4, 0x468e, { 0x9d, 0xa4, 0x67, 0x4d, 0x95, 0xb5, 0xbe, 0xdd } };

// this is the GUID of the IME mode icon in Windows 8
// the value is not available in older SDK versions, so let's define it ourselves.
static const GUID _GUID_LBI_INPUTMODE =
{ 0x2C77A81E, 0x41CC, 0x4178, { 0xA3, 0xA7, 0x5F, 0x8A, 0x98, 0x75, 0x68, 0xE6 } };

TextService::TextService(ImeModule* module):
	m_imeModule(module),
	Ime::TextService(module),
	showingCandidates_(false),
	langMode_(-1),
	shapeMode_(-1),
	langModeNew_(CHINESE_MODE),
	shapeModeNew_(HALFSHAPE_MODE),
	outputSimpChinese_(false),
	lastKeyDownCode_(0),
	messageWindow_(NULL),
	messageTimerId_(0),
	candidateWindow_(NULL),
	imeModeIcon_(NULL),
	symbolsFileTime_(0),
	m_hEngine(NULL) {

	// add preserved keys
	addPreservedKey(VK_SPACE, TF_MOD_SHIFT, g_shiftSpaceGuid); // shift + space

	// add language bar buttons
	// siwtch Chinese/English modes
	switchLangButton_ = new Ime::LangBarButton(this, g_modeButtonGuid, ID_SWITCH_LANG);
	switchLangButton_->setTooltip(IDS_SWITCH_LANG);
	addButton(switchLangButton_);

	// toggle full shape/half shape
	switchShapeButton_ = new Ime::LangBarButton(this, g_shapeTypeButtonGuid, ID_SWITCH_SHAPE);
	switchShapeButton_->setTooltip(IDS_SWITCH_SHAPE);
	addButton(switchShapeButton_);

	// settings and others, may open a popup menu
	Ime::LangBarButton* button = new Ime::LangBarButton(this, g_settingsButtonGuid);
	button->setTooltip(IDS_SETTINGS);
	button->setIcon(IDI_CONFIG);
	HMENU menu = ::LoadMenuW(this->imeModule()->hInstance(), LPCTSTR(IDR_MENU));
	popupMenu_ = ::GetSubMenu(menu, 0);
	button->setMenu(popupMenu_);
	addButton(button);
	button->Release();

	// Windows 8 systray IME mode icon
	imeModeIcon_ = new Ime::LangBarButton(this, _GUID_LBI_INPUTMODE, ID_MODE_ICON);
	imeModeIcon_->setIcon(IDI_ENG);
	addButton(imeModeIcon_);

	// global compartment stuff
	// addCompartmentMonitor(g_configChangedGuid, true);

	// font for candidate and mesasge windows
	font_ = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT lf;
	GetObject(font_, sizeof(lf), &lf);
	lf.lfHeight = config().fontSize;
	lf.lfWeight = FW_NORMAL;
	font_ = CreateFontIndirect(&lf);
}

TextService::~TextService(void) {
	if(popupMenu_)
		::DestroyMenu(popupMenu_);

	if(candidateWindow_)
		candidateWindow_->Release();

	if(messageWindow_)
		hideMessage();

	if(font_)
		::DeleteObject(font_);

	if(switchLangButton_)
		switchLangButton_->Release();
	if(switchShapeButton_)
		switchShapeButton_->Release();
	if(imeModeIcon_)
		imeModeIcon_->Release();

	freeChewingContext();

	if (m_hEngine) {
		PYEngine_Free(m_hEngine);
		m_hEngine = NULL;
	}
}

// virtual
void TextService::onActivate() {
	DWORD configStamp = globalCompartmentValue(g_configChangedGuid);
	config().reloadIfNeeded(configStamp);
	//initChewingContext();
	char szUserPath[MAX_PATH] = "";
	WideCharToMultiByte(CP_ACP, 0, imeModule()->userDir().c_str(), -1, szUserPath, MAX_PATH, NULL, NULL);
	m_hEngine = PYEngine_New(szUserPath);
	updateLangButtons();
	if(imeModeIcon_) // windows 8 IME mode icon
		imeModeIcon_->setEnabled(isKeyboardOpened());
}

// virtual
void TextService::onDeactivate() {
	lastKeyDownCode_ = 0;
	freeChewingContext();

	hideMessage();

	if(candidateWindow_) {
		showingCandidates_ = false;
		candidateWindow_->Release();
		candidateWindow_ = NULL;
	}
}

// virtual
void TextService::onFocus() {
}

// virtual
bool TextService::filterKeyDown(Ime::KeyEvent& keyEvent) {

	//return false;

	Config& cfg = config();
	lastKeyDownCode_ = keyEvent.keyCode();
	// return false if we don't need this key
	//assert(chewingContext_);
	if(!isComposing()) { // we're not composing now
		// don't do further handling in English + half shape mode
		if(langMode_ != CHINESE_MODE && shapeMode_ != FULLSHAPE_MODE)
			return false;

		// if Ctrl or Alt key is down
		if(keyEvent.isKeyDown(VK_CONTROL) || keyEvent.isKeyDown(VK_MENU)) {
			// bypass IME. This might be a shortcut key used in the application
			// FIXME: we only need Ctrl in composition mode for adding user phrases.
			// However, if we turn on easy symbol input with Ctrl support later,
			// we'll need th Ctrl key then.
			return false;
		}

		// we always need further processing in full shape mode since all English chars,
		// numbers, and symbols need to be converted to full shape Chinese chars.
		if(shapeMode_ != FULLSHAPE_MODE) {
			// Caps lock is on => English mode
			if(cfg.enableCapsLock && keyEvent.isKeyToggled(VK_CAPITAL)) {
				// We need to handle this key because in onKeyDown(),
				// the upper case chars need to be converted to lower case
				// before doing output to the applications.
				if(keyEvent.isChar() && isalpha(keyEvent.charCode()))
					return true; // this is an English alphabet
				else
					return false;
			}

			if(keyEvent.isKeyToggled(VK_NUMLOCK)) { // NumLock is on
				// if this key is Num pad 0-9, +, -, *, /, pass it back to the system
				if(keyEvent.keyCode() >= VK_NUMPAD0 && keyEvent.keyCode() <= VK_DIVIDE)
					return false; // bypass IME
			}
		}
		else { // full shape mode
			if(keyEvent.keyCode() == VK_SPACE) // we need to convert space to fullshape.
				return true;
		}

		// when not composing, we only cares about Bopomofo
		// FIXME: we should check if the key is mapped to a phonetic symbol instead
		if(keyEvent.isChar() && isalpha(keyEvent.charCode())) {
			// this is a key mapped to a printable char. we want it!
			return true;
		}
		if (isgraph(keyEvent.charCode())) {
			return true;
		}
		return false;
	}
	return true;
}

void TextService::commitString(Ime::EditSession* session, const wchar_t *message)
{
	if (!isComposing()) // start the composition
		startComposition(session->context());
	setCompositionString(session, message, wcslen(message));
	endComposition(session->context());
}

// virtual
bool TextService::onKeyDown(Ime::KeyEvent& keyEvent, Ime::EditSession* session) {
//	assert(chewingContext_);
	Config& cfg = config();
#if 0 // What's easy symbol input??
	// set this to true or false according to the status of Shift key
	// alternatively, should we set this when onKeyDown and onKeyUp receive VK_SHIFT or VK_CONTROL?
	bool easySymbols = false;
	if(cfg.easySymbolsWithShift)
		easySymbols = keyEvent.isKeyDown(VK_SHIFT);
	if(!easySymbols && cfg.easySymbolsWithCtrl)
		easySymbols = keyEvent.isKeyDown(VK_CONTROL);
	::chewing_set_easySymbolInput(chewingContext_, easySymbols);
#endif

	UINT charCode = keyEvent.charCode();
	
	if (isalpha(charCode)) {

		PYEngine_KeyEvent(m_hEngine, isupper(charCode) ? tolower(charCode) : charCode, 0);

		wchar_t szCommit[256];
	
		if (!isComposing()) // start the composition
			startComposition(session->context());
		PY_UINT16 nCaretPos = 0;
		PYEngine_GetCompString(m_hEngine, 0, (PY_PWSTR) szCommit, 256, &nCaretPos, PY_TRUE);

		std::wstring wbuf = szCommit;
		setCompositionString(session, wbuf.c_str(), wbuf.length());
		setCompositionCursor(session, nCaretPos);
		//if (isComposing())
		//	endComposition(session->context());

		if (true) {
			if (!showingCandidates())
				showCandidates(session);
			else
				updateCandidates(session);
		}
		else {
			if (showingCandidates())
				hideCandidates();
		}

		return true;
	}
	else if (isdigit(charCode))
	{
		if (PYEngine_GetKeyStringLen(m_hEngine) > 0 &&
			charCode - '1' < PYEngine_GetCurrPageItemTotal(m_hEngine))
		{
			if (PYEngine_SelCurrPageItem(m_hEngine, charCode - '1') == 1)
			{
				wchar_t szCommit[256];
				PYEngine_GetConvertedString(m_hEngine, (PY_PWSTR)szCommit, 256);
				std::wstring wbuf = szCommit;
				setCompositionString(session, wbuf.c_str(), wbuf.length());
				endComposition(session->context());
				PYEngine_Reset(m_hEngine);
				hideCandidates();
			}
			else
			{
				wchar_t szCommit[256];
				szCommit[0] = charCode;
				szCommit[1] = 0;

				if (!isComposing()) // start the composition
					startComposition(session->context());
				PY_UINT16 nCaretPos = 0;
				PYEngine_GetCompString(m_hEngine, 0, (PY_PWSTR)szCommit, 256, &nCaretPos, PY_TRUE);

				std::wstring wbuf = szCommit;
				setCompositionString(session, wbuf.c_str(), wbuf.length());
				setCompositionCursor(session, nCaretPos);
				//if (isComposing())
				//	endComposition(session->context());

				if (true) {
					if (!showingCandidates())
						showCandidates(session);
					else
						updateCandidates(session);
				}
				else {
					if (showingCandidates())
						hideCandidates();
				}
			}
		}
		else
		{
			if (PYEngine_GetKeyStringLen(m_hEngine) == 0)
			{
				wchar_t szCommit[2] = L"";
				szCommit[0] = charCode;
				szCommit[1] = 0;
				commitString(session, szCommit);
			}
		}
		return true;
	}
	else if (charCode == '-') {
		if (PYEngine_GetKeyStringLen(m_hEngine) > 0)
		{
			PYEngine_TurnPrevPage(m_hEngine);
			if (!showingCandidates())
				showCandidates(session);
			else
				updateCandidates(session);
		}
		else
		{
			commitString(session, L"-");
		}
		return true;
	}
	else if (charCode == '=') {
		if (PYEngine_GetKeyStringLen(m_hEngine) > 0)
		{
			PYEngine_TurnNextPage(m_hEngine);
			if (!showingCandidates())
				showCandidates(session);
			else
				updateCandidates(session);
		}
		else
		{
			commitString(session, L"=");
		}

		return true;
	}
	else {
		charCode = keyEvent.charCode();
		if (isgraph(charCode))
		{
			if (PYEngine_GetKeyStringLen(m_hEngine) > 0) {
				PYEngine_SelCandItem(m_hEngine, 0);
				wchar_t szCommit[256];
				PYEngine_GetConvertedString(m_hEngine, (PY_PWSTR)szCommit, 256);
				std::wstring wbuf = szCommit;
				setCompositionString(session, wbuf.c_str(), wbuf.length());
				endComposition(session->context());
				PYEngine_Reset(m_hEngine);
				hideCandidates();
			}
			if (charCode == '.') {
				commitString(session, L"��");
			}
			else if (charCode == ',') {
				commitString(session, L"��");
			}
			else if (charCode == '?') {
				commitString(session, L"��");
			}
			else if (charCode == '/') {
				commitString(session, L"��");
			}
			else if (charCode == '\\') {
				commitString(session, L"��");
			}
			else if (charCode == '!') {
				commitString(session, L"��");
			}
			else if (charCode == '$') {
				commitString(session, L"��");
			}
			else if (charCode == '^') {
				commitString(session, L"����");
			}
			else if (charCode == '(') {
				commitString(session, L"��");
			}
			else if (charCode == ')') {
				commitString(session, L"��");
			}
			else if (charCode == '[') {
				commitString(session, L"��");
			}
			else if (charCode == ']') {
				commitString(session, L"��");
			}
			else if (charCode == '`') {
				commitString(session, L"��");
			}
			else if (charCode == ':') {
				commitString(session, L"��");
			}
			else if (charCode == ';') {
				commitString(session, L"��");
			}
			else if (charCode == '<') {
				commitString(session, L"��");
			}
			else if (charCode == '>') {
				commitString(session, L"��");
			}
			else {
				wchar_t szCommit[2] = L"";
				szCommit[0] = charCode;
				szCommit[1] = 0;
				commitString(session, szCommit);
			}
			return true;
		}

		charCode = keyEvent.keyCode();

		if (charCode == VK_ESCAPE)
		{
			PYEngine_Reset(m_hEngine);
			setCompositionString(session, L"", 0);
			endComposition(session->context());
			hideCandidates();
		}
		else if (charCode == VK_RETURN)
		{
			wchar_t szCommit[256];
			PYEngine_GetConvertedString(m_hEngine, (PY_PWSTR)szCommit, 256);
			std::wstring wbuf = szCommit;
			setCompositionString(session, wbuf.c_str(), wbuf.length());
			setCompositionCursor(session, wbuf.length());
			endComposition(session->context());
			PYEngine_Reset(m_hEngine);
			hideCandidates();
		}
		else if (charCode == VK_SPACE)
		{
			if (PYEngine_SelCurrPageItem(m_hEngine, 0) == 1)
			{
				wchar_t szCommit[256];
				PYEngine_GetConvertedString(m_hEngine, (PY_PWSTR)szCommit, 256);
				std::wstring wbuf = szCommit;
				setCompositionString(session, wbuf.c_str(), wbuf.length());
				endComposition(session->context());
				PYEngine_Reset(m_hEngine);
				hideCandidates();
			}
			else
			{
				wchar_t szCommit[256];
				szCommit[0] = charCode;
				szCommit[1] = 0;

				if (!isComposing()) // start the composition
					startComposition(session->context());
				PY_UINT16 nCaretPos = 0;
				PYEngine_GetCompString(m_hEngine, 0, (PY_PWSTR)szCommit, 256, &nCaretPos, PY_TRUE);

				std::wstring wbuf = szCommit;
				setCompositionString(session, wbuf.c_str(), wbuf.length());
				setCompositionCursor(session, nCaretPos);
				//if (isComposing())
				//	endComposition(session->context());

				if (true) {
					if (!showingCandidates())
						showCandidates(session);
					else
						updateCandidates(session);
				}
				else {
					if (showingCandidates())
						hideCandidates();
				}
			}
			return true;
		}
		else if (charCode == VK_BACK)
		{
			PYEngine_KeyEvent(m_hEngine, '\b', 0);
			if (PYEngine_GetCandTotal(m_hEngine) > 0)
			{
				wchar_t szCommit[256];
				szCommit[0] = charCode;
				szCommit[1] = 0;

				if (!isComposing()) // start the composition
					startComposition(session->context());
				PY_UINT16 nCaretPos = 0;
				PYEngine_GetCompString(m_hEngine, 0, (PY_PWSTR)szCommit, 256, &nCaretPos, PY_TRUE);

				std::wstring wbuf = szCommit;
				setCompositionString(session, wbuf.c_str(), wbuf.length());
				setCompositionCursor(session, nCaretPos);
				//if (isComposing())
				//	endComposition(session->context());

				if (true) {
					if (!showingCandidates())
						showCandidates(session);
					else
						updateCandidates(session);
				}
				else {
					if (showingCandidates())
						hideCandidates();
				}
			}
			else
			{
				if (isComposing())
				{
					setCompositionString(session, L"", 0);
					setCompositionCursor(session, 0);
					endComposition(session->context());
				}

				if (showingCandidates())
					hideCandidates();
			}

			return true;
		}
		else if (charCode == VK_LEFT)
		{
			PYEngine_MoveCaret(m_hEngine, PY_CARET_MOVE_LEFT_UNIT);

			PY_UINT16 nCaretPos = 0;
			wchar_t szCommit[256];
			PYEngine_GetCompString(m_hEngine, 0, (PY_PWSTR)szCommit, 256, &nCaretPos, PY_TRUE);

			std::wstring wbuf = szCommit;
			setCompositionString(session, wbuf.c_str(), wbuf.length());
			setCompositionCursor(session, nCaretPos);
			return true;
		}
		else if (charCode == VK_RIGHT)
		{
			PYEngine_MoveCaret(m_hEngine, PY_CARET_MOVE_RIGHT_CHAR);

			PY_UINT16 nCaretPos = 0;
			wchar_t szCommit[256];
			PYEngine_GetCompString(m_hEngine, 0, (PY_PWSTR)szCommit, 256, &nCaretPos, PY_TRUE);

			std::wstring wbuf = szCommit;
			setCompositionString(session, wbuf.c_str(), wbuf.length());
			setCompositionCursor(session, nCaretPos);
			return true;
		}
		else if (charCode == VK_HOME)
		{
			PYEngine_MoveCaret(m_hEngine, PY_CARET_MOVE_HOME);

			PY_UINT16 nCaretPos = 0;
			wchar_t szCommit[256];
			PYEngine_GetCompString(m_hEngine, 0, (PY_PWSTR)szCommit, 256, &nCaretPos, PY_TRUE);

			std::wstring wbuf = szCommit;
			setCompositionString(session, wbuf.c_str(), wbuf.length());
			setCompositionCursor(session, nCaretPos);
			return true;
		}
		else if (charCode == VK_END)
		{
			PYEngine_MoveCaret(m_hEngine, PY_CARET_MOVE_END);

			PY_UINT16 nCaretPos = 0;
			wchar_t szCommit[256];
			PYEngine_GetCompString(m_hEngine, 0, (PY_PWSTR)szCommit, 256, &nCaretPos, PY_TRUE);

			std::wstring wbuf = szCommit;
			setCompositionString(session, wbuf.c_str(), wbuf.length());
			setCompositionCursor(session, nCaretPos);
			return true;
		}
		else if (charCode == VK_UP) {
			PYEngine_TurnPrevPage(m_hEngine);
			if (!showingCandidates())
				showCandidates(session);
			else
				updateCandidates(session);
			return true;
		}
		else if (charCode == VK_DOWN) {
			PYEngine_TurnNextPage(m_hEngine);
			if (!showingCandidates())
				showCandidates(session);
			else
				updateCandidates(session);
			return true;
		}
		else {
			PY_UINT16 nCaretPos = 0;
			wchar_t szCommit[256];
			wsprintf(szCommit, L"charcode = %x keycode = %x", keyEvent.charCode(), keyEvent.keyCode());

			std::wstring wbuf = szCommit;
			showMessage(session, szCommit, 3);
			return true;
		}
	}

	return true;
}

// virtual
bool TextService::filterKeyUp(Ime::KeyEvent& keyEvent) {
	if(config().switchLangWithShift) {
		if(lastKeyDownCode_ == VK_SHIFT && keyEvent.keyCode() == VK_SHIFT) {
			// last key down event is also shift key
			// a <Shift> key down + key up pair was detected
			// switch language
			toggleLanguageMode();
		}
	}
	lastKeyDownCode_ = 0;
	return false;
}

// virtual
bool TextService::onKeyUp(Ime::KeyEvent& keyEvent, Ime::EditSession* session) {
	return true;
}

// virtual
bool TextService::onPreservedKey(const GUID& guid) {
	lastKeyDownCode_ = 0;
	// some preserved keys registered in ctor are pressed
	if(::IsEqualIID(guid, g_shiftSpaceGuid)) { // shift + space is pressed
		toggleShapeMode();
		return true;
	}
	return false;
}

// virtual
bool TextService::onCommand(UINT id, CommandType type) {
	// assert(chewingContext_);
	if(type == COMMAND_RIGHT_CLICK) {
		if(id == ID_MODE_ICON) { // Windows 8 IME mode icon
			Ime::Window window; // TrackPopupMenu requires a window to work, so let's build a transient one.
			window.create(HWND_DESKTOP, 0);
			POINT pos = {0};
			::GetCursorPos(&pos);
			UINT ret = ::TrackPopupMenu(popupMenu_, TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTALIGN | TPM_BOTTOMALIGN, pos.x, pos.y, 0, window.hwnd(), NULL);
			if(ret > 0)
				onCommand(ret, COMMAND_MENU);
		}
		else {
			// we only handle right click in Windows 8 for the IME mode icon
			return false;
		}
	}
	else {
		switch(id) {
		case ID_SWITCH_LANG:
			toggleLanguageMode();
			break;
		case ID_SWITCH_SHAPE:
			toggleShapeMode();
			break;
		case ID_MODE_ICON: // Windows 8 IME mode icon
			toggleLanguageMode();
			break;
		case ID_CONFIG: // show config dialog
			if(!isImmersive()) { // only do this in desktop app mode
				ImeModule* module = imeModule();
				module->onConfigure(HWND_DESKTOP);
			}
			break;
		case ID_OUTPUT_SIMP_CHINESE: // toggle output traditional or simplified Chinese
			toggleSimplifiedChinese();
			break;
		case ID_ABOUT: // show about dialog
			if(!isImmersive()) { // only do this in desktop app mode
				// show about dialog
				std::wstring path = imeModule()->programDir();
				path += L"\\ChewingPreferences.exe";
				::ShellExecuteW(NULL, L"open", path.c_str(), L"/about", NULL, SW_SHOWNORMAL);
			}
			break;
		case ID_WEBSITE: // visit chewing website
			::ShellExecuteW(NULL, NULL, L"http://chewing.im/", NULL, NULL, SW_SHOWNORMAL);
			break;
		case ID_GROUP: // visit chewing google groups website
			::ShellExecuteW(NULL, NULL, L"http://groups.google.com/group/chewing-devel", NULL, NULL, SW_SHOWNORMAL);
			break;
		case ID_BUGREPORT: // visit bug tracker page
			::ShellExecuteW(NULL, NULL, L"https://github.com/chewing/windows-chewing-/issues?state=open", NULL, NULL, SW_SHOWNORMAL);
			break;
		case ID_DICT_BUGREPORT:
			::ShellExecuteW(NULL, NULL, L"https://github.com/chewing/libchewing-data/issues", NULL, NULL, SW_SHOWNORMAL);
			break;
		case ID_MOEDICT: // a very awesome online Chinese dictionary
			::ShellExecuteW(NULL, NULL, L"https://www.moedict.tw/", NULL, NULL, SW_SHOWNORMAL);
			break;
		case ID_DICT: // online Chinese dictonary
			::ShellExecuteW(NULL, NULL, L"http://dict.revised.moe.edu.tw/", NULL, NULL, SW_SHOWNORMAL);
			break;
		case ID_SIMPDICT: // a simplified version of the online dictonary
			::ShellExecuteW(NULL, NULL, L"http://dict.concised.moe.edu.tw/main/cover/main.htm", NULL, NULL, SW_SHOWNORMAL);
			break;
		case ID_LITTLEDICT: // a simplified dictionary for little children
			::ShellExecuteW(NULL, NULL, L"http://dict.mini.moe.edu.tw/cgi-bin/gdic/gsweb.cgi?o=ddictionary", NULL, NULL, SW_SHOWNORMAL);
			break;
		case ID_PROVERBDICT: // a dictionary for proverbs (seems to be broken at the moment?)
			::ShellExecuteW(NULL, NULL, L"http://dict.idioms.moe.edu.tw/?", NULL, NULL, SW_SHOWNORMAL);
			break;
		case ID_CHEWING_HELP:
			// TODO: open help file here
			// Need to update the old ChewingIME docs
			break;
		default:
			return false;
		}
	}
	return true;
}

// virtual
void TextService::onCompartmentChanged(const GUID& key) {
	if(::IsEqualGUID(key, g_configChangedGuid)) {
		// changes of configuration are detected
		return;
	}
	Ime::TextService::onCompartmentChanged(key);
}

// called when the keyboard is opened or closed
// virtual
void TextService::onKeyboardStatusChanged(bool opened) {
	Ime::TextService::onKeyboardStatusChanged(opened);
	if(opened) { // keyboard is opened
		initChewingContext();
	}
	else { // keyboard is closed
		if(isComposing()) {
			// end current composition if needed
			ITfContext* context = currentContext();
			if(context) {
				endComposition(context);
				context->Release();
			}
		}
		if(showingCandidates()) // disable candidate window if it's opened
			hideCandidates();
		hideMessage(); // hide message window, if there's any
		freeChewingContext(); // IME is closed, chewingContext is not needed
	}

	if(imeModeIcon_)
		imeModeIcon_->setEnabled(opened);
	// FIXME: should we also disable other language bar buttons as well?
}

// called just before current composition is terminated for doing cleanup.
// if forced is true, the composition is terminated by others, such as
// the input focus is grabbed by another application.
// if forced is false, the composition is terminated gracefully by endComposition().
// virtual
void TextService::onCompositionTerminated(bool forced) {
	// we do special handling here for forced composition termination.
	if(forced) {
		// we're still editing our composition and have something in the preedit buffer.
		// however, some other applications grabs the focus and force us to terminate
		// our composition.
	}
}

void TextService::initChewingContext() {


	
}

void TextService::freeChewingContext() {

}

void TextService::applyConfig() {
	Config& cfg = config();

	// apply the new configurations
	
	// font for candidate and mesasge windows
	LOGFONT lf;
	GetObject(font_, sizeof(lf), &lf);
	if(lf.lfHeight != cfg.fontSize) { // font size is changed
		::DeleteObject(font_); // delete old font
		lf.lfHeight = cfg.fontSize; // apply the new size
		font_ = CreateFontIndirect(&lf); // create new font
		if(messageWindow_)
			messageWindow_->setFont(font_);
		if(candidateWindow_)
			candidateWindow_->setFont(font_);
	}
}

// toggle between English and Chinese
void TextService::toggleLanguageMode() {
	// switch between Chinses and English modes
	langModeNew_ = !langModeNew_;
	updateLangButtons();
	return;
}

// toggle between full shape and half shape
void TextService::toggleShapeMode() {
	shapeModeNew_ = !shapeModeNew_;
	updateLangButtons();
	return;
}

// toggle output traditional or simplified Chinese
void TextService::toggleSimplifiedChinese() {
	outputSimpChinese_ = !outputSimpChinese_;
	// update popup menu to check/uncheck the simplified Chinese item
	DWORD checkFlags = outputSimpChinese_ ?  MF_CHECKED : MF_UNCHECKED;
	::CheckMenuItem(popupMenu_, ID_OUTPUT_SIMP_CHINESE, MF_BYCOMMAND|checkFlags);
}

void TextService::updateCandidates(Ime::EditSession* session) {
	assert(candidateWindow_);
	candidateWindow_->clear();

	wchar_t szCand[256] = L"";
	int nCandTotal = PYEngine_GetCurrPageItemTotal(m_hEngine);
	if (nCandTotal > 9) {
		nCandTotal = 9;
	}
	for (int i = 0; i < nCandTotal; i++) {

		PYEngine_GetCurrPageItemString(m_hEngine, i, (PY_PWSTR) szCand, 256);
		std::wstring wstr = szCand;

		candidateWindow_->add(wstr, (wchar_t)'1' + i);
	}
//	candidateWindow_->setCurrentSel(0);

	/*
	::chewing_cand_Enumerate(chewingContext_);
	int* selKeys = ::chewing_get_selKey(chewingContext_); // keys used to select candidates
	int n = ::chewing_cand_ChoicePerPage(chewingContext_); // candidate string shown per page
	int i;
	for(i = 0; i < n && ::chewing_cand_hasNext(chewingContext_); ++i) {
		char* str = ::chewing_cand_String(chewingContext_);
		std::wstring wstr = utf8ToUtf16(str);
		::chewing_free(str);
		candidateWindow_->add(wstr, (wchar_t)selKeys[i]);
	}
	::chewing_free(selKeys);
	*/
	candidateWindow_->recalculateSize();
	candidateWindow_->refresh();

	RECT textRect;
	// get the position of composition area from 
	if(selectionRect(session, &textRect)) {
		// FIXME: where should we put the candidate window?
		candidateWindow_->move(textRect.left, textRect.bottom);
	}
}

// show candidate list window
void TextService::showCandidates(Ime::EditSession* session) {
	// TODO: implement ITfCandidateListUIElement interface to support UI less mode
	// Great reference: http://msdn.microsoft.com/en-us/library/windows/desktop/aa966970(v=vs.85).aspx

	// NOTE: in Windows 8 store apps, candidate window should be owned by
	// composition window, which can be returned by TextService::compositionWindow().
	// Otherwise, the candidate window cannot be shown.
	// Ime::CandidateWindow handles this internally. If you create your own
	// candidate window, you need to call TextService::isImmersive() to check
	// if we're in a Windows store app. If isImmersive() returns true,
	// The candidate window created should be a child window of the composition window.
	// Please see Ime::CandidateWindow::CandidateWindow() for an example.
	if(!candidateWindow_) {
		candidateWindow_ = new Ime::CandidateWindow(this, session);
		candidateWindow_->setFont(font_);
	}
	candidateWindow_->setUseCursor(config().cursorCandList);
	candidateWindow_->setCandPerRow(9);
	updateCandidates(session);
	candidateWindow_->show();
	showingCandidates_ = true;
}

// hide candidate list window
void TextService::hideCandidates() {
	assert(candidateWindow_);
	if(candidateWindow_) {
		candidateWindow_->Release();
		candidateWindow_ = NULL;
	}
	showingCandidates_ = false;
}

// message window
void TextService::showMessage(Ime::EditSession* session, std::wstring message, int duration) {
	// remove previous message if there's any
	hideMessage();
	// FIXME: reuse the window whenever possible
	messageWindow_ = new Ime::MessageWindow(this, session);
	messageWindow_->setFont(font_);
	messageWindow_->setText(message);
	
	int x = 0, y = 0;
	if(isComposing()) {
		RECT rc;
		if(selectionRect(session, &rc)) {
			x = rc.left;
			y = rc.bottom + 50;
		}
	}
	messageWindow_->move(x, y);
	messageWindow_->show();

	messageTimerId_ = ::SetTimer(messageWindow_->hwnd(), 1, duration * 1000, (TIMERPROC)TextService::onMessageTimeout);
}

void TextService::hideMessage() {
	if(messageTimerId_) {
		::KillTimer(messageWindow_->hwnd(), messageTimerId_);
		messageTimerId_ = 0;
	}
	if(messageWindow_) {
		delete messageWindow_;
		messageWindow_ = NULL;
	}
}

// called when the message window timeout
void TextService::onMessageTimeout() {
	hideMessage();
}

// static
void CALLBACK TextService::onMessageTimeout(HWND hwnd, UINT msg, UINT_PTR id, DWORD time) {
	Ime::MessageWindow* messageWindow = (Ime::MessageWindow*)Ime::Window::fromHwnd(hwnd);
	assert(messageWindow);
	if(messageWindow) {
		TextService* pThis = (Chewing::TextService*)messageWindow->textService();
		pThis->onMessageTimeout();
	}
}


void TextService::updateLangButtons() {

	//int langMode = ::chewing_get_ChiEngMode(chewingContext_);
	
	if(langMode_ != langModeNew_) {
		langMode_ = langModeNew_;
		switchLangButton_->setIcon(langMode_ == CHINESE_MODE ? IDI_CHI : IDI_ENG);
		if(imeModeIcon_) {
			// FIXME: we need a better set of icons to meet the 
			//        WIndows 8 IME guideline and UX guidelines.
			imeModeIcon_->setIcon(langMode_ == CHINESE_MODE ? IDI_CHI : IDI_ENG);
		}
	}

	//int shapeMode = ::chewing_get_ShapeMode(chewingContext_);
	if(shapeMode_ != shapeModeNew_) {
		shapeMode_ = shapeModeNew_;
		switchShapeButton_->setIcon(shapeMode_ == FULLSHAPE_MODE ? IDI_FULL_SHAPE : IDI_HALF_SHAPE);
	}

}


} // namespace Chewing
