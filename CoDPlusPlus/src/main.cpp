#include <Windows.h>
#include <Hook/Hook.h>
#include <Hook/Detours.h>
#include <Utils/WinApiHelper.h>

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (!WinApiHelper::CheckGame())
		{
			return FALSE;
		}

		DetourRet(0x0046c79a, Detours::InjectEntryPoint, 5);
		DisableThreadLibraryCalls(hModule);
	}

	return TRUE;
}