#include "Detours.h"
#include "ImGuiManager.h"
#include "CoDUO.h"
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
			push ebp
			mov	ebp, esp
			sub esp, 0x4
		}

		DWORD moduleAddr;
		_asm
		{
			mov moduleAddr, edx
		}

		if (moduleAddr)
		{
			if (moduleAddr == uo_game_mp_x86)
			{
				uo_game_mp_x86 = 0;
				uo_game_mp_x86_OnDetach();
			}
		}

		_asm
		{
			add esp, 0x4
			pop ebp
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