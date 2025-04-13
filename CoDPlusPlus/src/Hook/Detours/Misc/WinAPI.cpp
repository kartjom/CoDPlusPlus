#include <Engine/CoDUO.h>
#include <Hook/Detours.h>
#include <Utils/ImGuiManager.h>
#include <string>
#include <print>

using namespace CoDUO;

namespace Hook::Detour
{
	HMODULE __stdcall hkLoadLibraryA(LPCSTR lpLibFileName)
	{
		std::string dllName(lpLibFileName);
		HMODULE handle = LoadLibraryAHook.Invoke(lpLibFileName);

		if ((DWORD)handle != 0 && dllName.find("uo_game_mp_x86") != std::string::npos)
		{
			uo_game_mp_x86 = (DWORD)handle;
			OnServerInitialize();
		}

		return handle;
	}

	NTSTATUS __stdcall hkLdrUnloadDll(PVOID BaseAddress)
	{
		if ((DWORD)BaseAddress == uo_game_mp_x86)
		{
			uo_game_mp_x86 = 0;
			OnServerShutdown();
		}

		return LdrUnloadDllHook.Invoke(BaseAddress);
	}
}