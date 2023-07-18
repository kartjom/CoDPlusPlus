#include <Engine/CoDUO.h>
#include <Hook/Detours.h>
#include <Utils/ImGuiManager.h>
#include <string>

using namespace CoDUO;

namespace Detours
{
	ImplementOverride(HMODULE, __stdcall, LoadLibraryA)(LPCSTR lpLibFileName)
	{
		std::string dllName(lpLibFileName);

		HMODULE handle = Original(LoadLibraryA)(lpLibFileName);

		if (dllName.find("uo_game_mp_x86.dll") != std::string::npos)
		{
			uo_game_mp_x86 = (DWORD)handle;
			uo_game_mp_x86_OnAttach();
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

	ImplementDetour(FreeLibrary)
	{
		_asm pushad

		_asm
		{
			mov	ebp, esp
		}

		DWORD moduleAddr_edx;
		DWORD moduleAddr_eax;
		_asm
		{
			mov moduleAddr_edx, edx
			mov moduleAddr_eax, eax
		}

		// Depending on the file (cracked, original or Steam), addr of module is in edx or eax
		if ((uo_game_mp_x86 && moduleAddr_edx == uo_game_mp_x86) || (uo_game_mp_x86 && moduleAddr_eax == uo_game_mp_x86))
		{
			uo_game_mp_x86 = 0;
			uo_game_mp_x86_OnDetach();
		}

		_asm popad

		_restore
		{
			jmp [Detours::FreeLibrary_kernelbase]
		}
	}

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
}