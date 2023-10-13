#include <Windows.h>

#include <Utils/WinApiHelper.h>
#include <Utils/OpenGLHelper.h>
#include <Utils/ImGuiManager.h>
#include <Engine/CoDUO.h>

using namespace CoDUO;

DWORD WINAPI MainThread(LPVOID param)
{
	if ( !WinApiHelper::CheckGame() )
	{
		FreeLibraryAndExitThread((HMODULE)param, 0);
		return 0;
	}

	WinApiHelper::CreateConsole("Console");
	WinApiHelper::InjectDetours();

	#ifdef CLIENT
		OpenGLHelper::InjectDetours();
	#endif

	CoDUO::BaseAttach();
	FlushInstructionCache(GetCurrentProcess(), NULL, NULL);

	while (true)
	{
		Sleep(1);
	}

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		CreateThread(0, 0, MainThread, hModule, 0, 0);
		DisableThreadLibraryCalls(hModule);
	}

	return TRUE;
}