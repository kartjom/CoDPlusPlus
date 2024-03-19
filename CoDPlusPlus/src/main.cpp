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

		Detours::InjectEntryPoint_addr = 0x0046c79a;
		Hook::Detour(Detours::InjectEntryPoint_addr, Detours::InjectEntryPoint_n, 5, &Detours::InjectEntryPoint_r, Detours::InjectEntryPoint_bytes);

		DisableThreadLibraryCalls(hModule);
	}

	return TRUE;
}