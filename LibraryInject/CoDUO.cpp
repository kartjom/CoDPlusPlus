#include "CoDUO.h"
#include "Hook.h"
#include "Detours.h"

#include <iostream>

namespace CoDUO
{
	uint32_t Scr_LoadScript(const char* file)
	{
		_asm
		{
			push[file]
			mov eax, [file]
			mov edi, 0x00480150
			call edi
			add esp, 0x4
		}
	}

	uint32_t Scr_GetFunctionHandle(const char* file, const char* method)
	{
		_asm
		{
			push[method]
			push[file]
			mov eax, 0x0047FE50
			call eax
			add esp, 0x8
		}
	}

	uint32_t Scr_RunScript(uint32_t scriptHandle, uint32_t argc)
	{
		_asm
		{
			push argc
			push scriptHandle
			mov eax, 0x0048f3e0
			call eax // Scr_ExecThread

			push eax
			mov eax, 0x0048f640
			call eax // Scr_FreeThread

			add esp, 0xC
		}
	}

	void Scr_AddUndefined()
	{
		_asm
		{
			mov eax, 0x004906d0
			call eax
		}
	}

	void Scr_AddBool(bool value)
	{
		_asm
		{
			push value
			mov eax, 0x00490650
			call eax

			add esp, 0x4
		}
	}

	void Scr_AddInt(int value)
	{
		_asm
		{
			push value
			mov eax, 0x00490670
			call eax

			add esp, 0x4
		}
	}

	void Scr_AddFloat(float value)
	{
		_asm
		{
			push value
			mov eax, 0x00490690
			call eax

			add esp, 0x4
		}
	}

	void Scr_AddVector(void* value)
	{
		_asm
		{
			push value
			mov eax, 0x004908C0
			call eax

			add esp, 0x4
		}
	}

	void Scr_AddString(const char* string)
	{
		_asm
		{
			push[string]
			mov eax, 0x004907F0
			call eax

			add esp, 0x4
		}
	}

	void Scr_AddEntity(int index)
	{
		_asm
		{
			push 0x0
			push index
			mov eax, 0x00490720
			call eax

			add esp, 0x8
		}
	}

	const char* SL_ConvertToString(int32_t index)
	{
		_asm
		{
			push index
			mov eax, 0x00482290
			call eax

			add esp, 0x4
		}
	}

	cvar_t* Cvar_Get(const char* var_name, const char* var_value, int flags)
	{
		_asm
		{
			push flags
			push var_value
			push var_name
			mov eax, 0x0043D9E0
			call eax

			add esp, 0xC
		}
	}

	cvar_t* Cvar_Set(const char* var_name, const char* value, qboolean force)
	{
		_asm
		{
			push force
			push value
			push var_name
			mov eax, 0x0043DC50
			call eax

			add esp, 0xC
		}
	}

	void BaseAttach()
	{
		BYTE cheatBuffer[] = { 0xEB }; // Console cvars
		Hook::Patch(0x0043DD86, cheatBuffer, 1); // Read Only
		Hook::Patch(0x0043DDA3, cheatBuffer, 1); // Write Protected
		Hook::Patch(0x0043DDC1, cheatBuffer, 1); // Cheat Protected

		cvar_indexes = (cvar_t*)(0x009987A0);
		cvar_indexes = cvar_indexes->next; // first one is junk, remove if something's broken
	}

	void uo_game_mp_x86_OnAttach()
	{
		g_entities = (gentity_t*)(uo_game_mp_x86 + 0x00118d40);
		gameCvarTable = (cvarTable_t*)(uo_game_mp_x86 + 0x00086A58);

		DetourRet(uo_game_mp_x86 + 0x000361c0, Detours::GScr_LoadGameTypeScript, 8);
		DetourRet(uo_game_mp_x86 + 0x0005689d, Detours::ShootCallback, 6);

		std::cout << "[uo_game_mp_x86] - OnAttach" << std::endl;
	}

	void uo_game_mp_x86_OnDetach()
	{
		g_entities = nullptr;
		gameCvarTable = nullptr;

		std::cout << "[uo_game_mp_x86] - OnDetach" << std::endl;
	}
}