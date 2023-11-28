#include "PalmInputModule.h"
#include "PalmInputConfig.h"
#include "resource.h"

Chewing::ImeModule* g_imeModule = NULL;

// GUID of our language profile - palminput
// {40942709-09D0-4D23-8696-74D9783707C8}
static const GUID g_profileGuid =
{ 0x40942709, 0x9d0, 0x4d23, { 0x86, 0x96, 0x74, 0xd9, 0x78, 0x37, 0x7, 0xc8 } };

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		::DisableThreadLibraryCalls(hModule); // disable DllMain calls due to new thread creation
		g_imeModule = new Chewing::ImeModule(hModule);
		break;
	case DLL_PROCESS_DETACH:
		if(g_imeModule) {
			g_imeModule->Release();
			g_imeModule = NULL;
		}
		break;
	}
	return TRUE;
}

STDAPI DllCanUnloadNow(void) {
	return g_imeModule->canUnloadNow();
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppvObj) {
	return g_imeModule->getClassObject(rclsid, riid, ppvObj);
}

STDAPI DllUnregisterServer(void) {
	return g_imeModule->unregisterServer();
}

STDAPI DllRegisterServer(void) {
	wchar_t name[32];
	::LoadStringW(g_imeModule->hInstance(), IDS_CHEWING, name, 32);

	// get path of our module
	wchar_t modulePath[MAX_PATH];
	DWORD modulePathLen = GetModuleFileNameW(g_imeModule->hInstance(), modulePath, MAX_PATH);

	int iconIndex = 1; // use Windows 8 style IME icon

	Ime::LangProfileInfo info;
	info.name = name;
	info.profileGuid = g_profileGuid;
	info.locale = L"zh-Hans-CN";
	info.fallbackLocale = L"zh-CN";
	info.iconIndex = iconIndex;
	info.iconFile = modulePath;

	return g_imeModule->registerServer(name, &info, 1);
}

STDAPI PalmInputSetup() {
	// The directory is already created when the ImeModule object is constructed.
	// Grant permission to app containers
	Chewing::Config::grantAppContainerAccess(g_imeModule->userDir().c_str(), SE_FILE_OBJECT, GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE|DELETE);
	
	return S_OK;
}
