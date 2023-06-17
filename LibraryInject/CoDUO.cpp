#include "CoDUO.h"
#include "Hook.h"
#include "Detours.h"

#include <iostream>

namespace CoDUO
{
	int32_t Scr_LoadScript(const char* file)
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

	int32_t Scr_GetFunctionHandle(const char* file, const char* method)
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

	int32_t Scr_RunScript(int32_t scriptHandle, uint32_t argc)
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

	int32_t Scr_GetNumParam()
	{
		_asm
		{
			mov eax, 0x00490640
			call eax
		}
	}

	int32_t Scr_GetType(int param)
	{
		_asm
		{
			push param
			mov eax, 0x00490530
			call eax

			add esp, 0x4
		}
	}

	int32_t Scr_GetInt(int param)
	{
		_asm
		{
			push param
			mov eax, 0x0048FC00
			call eax

			add esp, 0x4
		}
	}

	float Scr_GetFloat(int param)
	{
		_asm
		{
			push param
			mov eax, 0x0048FF00
			call eax

			add esp, 0x4
		}
	}

	void Scr_GetVector(int param, void* destination)
	{
		_asm
		{
			push destination
			push param
			mov eax, 0x004902A0
			call eax

			add esp, 0x8
		}
	}

	vec3_t Scr_GetVector(int param)
	{
		vec3_t vec;
		Scr_GetVector(param, &vec);

		return vec;
	}

	int32_t Scr_GetConstString(int param)
	{
		_asm
		{
			push param
			mov eax, 0x0048FFA0
			call eax

			add esp, 0x4
		}
	}

	const char* Scr_GetString(int param)
	{
		int str_id = Scr_GetConstString(param);
		return SL_ConvertToString(str_id);
	}

	gentity_t* Scr_GetEntity(int param)
	{
		_asm
		{
			push param
			mov esi, param
			mov eax, uo_game_mp_x86
			add eax, 0x4E1E0
			call eax

			add esp, 0x4
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

	void Scr_AddBool(int value)
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

	void Scr_AddVector(float* value)
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

	void Scr_AddEntityNum(int index)
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

	void Scr_MakeArray()
	{
		_asm
		{
			mov eax, 0x00490910
			call eax
		}
	}

	void Scr_AddArray()
	{
		_asm
		{
			mov eax, 0x00490960
			call eax
		}
	}

	void Scr_AddArrayStringIndexed(int32_t str_index)
	{
		_asm
		{
			push str_index
			mov eax, 0x004909E0
			call eax

			add esp, 0x4
		}
	}

	weapondef_t* G_GetWeaponDef(int32_t index)
	{
		weapondef_t* weaponinfo = nullptr;

		if (index > 0 && index <= *weaponDefCount && uo_game_mp_x86 != 0)
		{
			uintptr_t ptr = *(uintptr_t*)(uo_game_mp_x86 + 0x0010ed40);
			weaponinfo = *(weapondef_t**)(ptr + index * 4);
		}

		return weaponinfo;
	}

	weaponslot_t G_GetWeaponSlot(gentity_t* player, int32_t weaponIndex)
	{
		weaponslot_t slot = {};

		if (weaponIndex > 0 && weaponIndex <= *weaponDefCount && player->client != nullptr && uo_game_mp_x86 > 0)
		{
			weapondef_t* def = G_GetWeaponDef(weaponIndex);
			if (def->clientIndex > 0)
			{
				int clipAddr = (DWORD)(player->client) + (def->clientIndex * 4) + 0x334;
				int reserveAddr = (DWORD)(player->client) + (def->clientIndex * 4) + 0x134;

				slot.weapondef = def;
				slot.clip = *(DWORD*)clipAddr;
				slot.reserve = *(DWORD*)reserveAddr;
			}
		}

		return slot;
	}

	int32_t G_NewString(const char* string)
	{
		_asm
		{
			mov edi, string
			mov eax, uo_game_mp_x86
			add eax, 0x0004dbc0
			call eax
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

	void G_GetPlayerViewOrigin(gentity_t* ent, float* destination)
	{
		_asm
		{
			mov esi, destination
			mov edi, ent
			mov eax, uo_game_mp_x86
			add eax, 0x000563b0
			call eax
		}
	}

	void trap_GetUserinfo(int num, char* buffer, int bufferSize)
	{
		_asm
		{
			push bufferSize
			push buffer
			push num
			push 0x21
			mov eax, syscall
			call eax

			add esp, 0x10
		}
	}

	void trap_SetUserinfo(int num, const char* buffer)
	{
		_asm
		{
			push buffer
			push num
			push 0x22
			mov eax, syscall
			call eax

			add esp, 0xC
		}
	}

	const char* Info_ValueForKey(const char* buffer, const char* key)
	{
		_asm
		{
			mov ebx, key
			mov ecx, buffer

			mov eax, uo_game_mp_x86
			add eax, 0x00058180
			call eax
		}
	}

	void Info_SetValueForKey(char* buffer, const char* key, const char* value)
	{
		_asm
		{
			mov edx, buffer
			push value
			push buffer
			mov ecx, key

			mov eax, uo_game_mp_x86
			add eax, 0x00058560
			call eax

			add esp, 0x8
		}
	}

	void trap_SendConsoleCommand(int exec_when, const char* text)
	{
		_asm
		{
			push text
			push exec_when
			push 0x17
			mov eax, syscall
			call eax

			add esp, 0xC
		}
	}

	void Cmd_AddCommand(const char* cmd_name, void* function)
	{
		_asm
		{
			push function
			push cmd_name
			mov eax, 0x0042C3B0
			call eax

			add esp, 0x8
		}
	}

	cvar_t* Cvar_FindVar(const char* var_name)
	{
		_asm
		{
			push var_name
			mov eax, 0x0043D8F0
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
		BYTE payloadBuffer[] = { 0xEB }; // Console cvars
		Hook::Patch(0x0043DD86, payloadBuffer, 1); // Read Only
		Hook::Patch(0x0043DDA3, payloadBuffer, 1); // Write Protected
		Hook::Patch(0x0043DDC1, payloadBuffer, 1); // Cheat Protected

		refdef = (refdef_t*)(0x0489A100);
		cvar_indexes = (cvar_t*)(0x009987A0);
		cvar_indexes = cvar_indexes->next; // first one is junk, remove if something's broken

		std::cout << "[CoDUOMP] - BaseAttach" << std::endl;
	}

	void uo_game_mp_x86_OnAttach()
	{
		g_entities = (gentity_t*)(uo_game_mp_x86 + 0x00118d40);
		gameCvarTable = (cvarTable_t*)(uo_game_mp_x86 + 0x00086A58);
		weaponDefCount = (int32_t*)(uo_game_mp_x86 + 0x0010ED3C);

		DetourRet(uo_game_mp_x86 + 0x000361c0, Detours::GScr_LoadGameTypeScript, 8);
		DetourRet(uo_game_mp_x86 + 0x0005689d, Detours::ShootCallback, 6);
		DetourRet(uo_game_mp_x86 + 0x00055727, Detours::MeleeCallback, 5);
		DetourRet(uo_game_mp_x86 + 0x0002ff58, Detours::ProjectileBounceCallback, 5);
		DetourRet(uo_game_mp_x86 + 0x00030420, Detours::ProjectileExplodeCallback, 5);

		DetourRet(uo_game_mp_x86 + 0x0003D230, Detours::LoadFunctionMP, 6);
		DetourRet(uo_game_mp_x86 + 0x0003D330, Detours::LoadMethodMP, 8);

		std::cout << "[uo_game_mp_x86] - OnAttach" << std::endl;
	}

	void uo_game_mp_x86_OnDetach()
	{
		g_entities = nullptr;
		gameCvarTable = nullptr;
		weaponDefCount = nullptr;

		CodeCallback = {}; // Clear all callbacks

		std::cout << "[uo_game_mp_x86] - OnDetach" << std::endl;
	}
}