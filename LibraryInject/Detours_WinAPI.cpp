#include "Detours.h"
#include "ImGuiManager.h"
#include "CoDUO.h"
#include <string>

#pragma region LoadLibraryA
ImplementOverride(HMODULE, __stdcall, LoadLibraryA)(LPCSTR lpLibFileName)
{
	std::string dllName(lpLibFileName);

	HMODULE handle = Original(LoadLibraryA)(lpLibFileName);

	if (dllName.find("uo_game_mp_x86.dll") != std::string::npos)
	{
		CoDUO::uo_game_mp_x86 = (DWORD)handle;
		CoDUO::uo_game_mp_x86_OnAttach();
	}

	return handle;
}

ImplementDetour(LoadLibraryA)
{
	_asm
	{
		mov edi, dword ptr[Wrapper(LoadLibraryA)]
	}

	JumpBack(LoadLibraryA)
}
#pragma endregion

#pragma region SetPhysicalCursorPos
ImplementOverride(BOOL, __stdcall, SetPhysicalCursorPos)(int x, int y)
{
	if (ImGuiManager::ShouldShow)
	{
		return false;
	}

	return Original(SetPhysicalCursorPos)(x, y);
}

ImplementDetour(SetPhysicalCursorPos)
{
	_asm
	{
		call dword ptr[Wrapper(SetPhysicalCursorPos)]
	}

	JumpBack(SetPhysicalCursorPos)
}
#pragma endregion