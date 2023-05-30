#include "Detours.h"
#include "CoDUO.h"
#include <stdio.h>

using namespace CoDUO;
using namespace CoDUO::Gsc;

void* GetFunctionCallback(const char* value)
{
	if (gsc_functions.find(value) != gsc_functions.end())
	{
		return gsc_functions[value].callback;
	}

	return 0;
}

void* GetMethodCallback(const char* value)
{
	if (gsc_methods.find(value) != gsc_methods.end())
	{
		return gsc_methods[value].callback;
	}

	return 0;
}

namespace Detours
{
	ImplementDetour(GScr_LoadGameTypeScript)
	{
		_asm pushad

		if (Scr_LoadScript("maps/mp/gametypes/_callbacksetup"))
		{
			CodeCallback_PlayerShoot = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerShoot");
		}

		_asm popad

		_restore
		{
			sub esp, 0x44

			mov eax, uo_game_mp_x86
			add eax, 0x00082650

			mov eax,[eax]
		}

		JumpBack(GScr_LoadGameTypeScript)
	}

	ImplementDetour(ShootCallback)
	{
		_asm pushad

		if (CodeCallback_PlayerShoot != 0)
		{
			gentity_t* player = nullptr;
			const char* weaponName = nullptr;

			_asm
			{
				mov player, edi
				push edx
				mov edx, [edx + 4]
				mov weaponName, edx
				pop edx
			}

			if (player && weaponName && player->client)
			{
				Scr_AddString(weaponName);
				Scr_AddVector(player->client->viewangles);
				Scr_AddEntity(player->number);
				Scr_RunScript(CodeCallback_PlayerShoot, 3);
			}
		}

		_asm popad

		_restore
		{
			mov eax, dword ptr ss : [ebp + 0x160]
		}

		JumpBack(ShootCallback)
	}
	
	ImplementDetour(LoadFunctionMP)
	{
		_asm sub esp, 0x4
		_asm pushad

		_asm
		{
			mov eax, [esp + 0x28]
			mov eax, [eax]
			push eax
			call GetFunctionCallback
			add esp, 0x4

			mov [esp + 0x20], eax
		}

		_asm popad

		_asm
		{
			pop eax
			cmp eax, 0
			jne function_found
		}

		_restore
		{
			mov eax, dword ptr ss:[esp + 0x4]
			mov edx, dword ptr ds:[eax]
		}

		JumpBack(LoadFunctionMP)

		_asm
		{
			function_found:
			ret
		}
	}

	ImplementDetour(LoadMethodMP)
	{

		_asm sub esp, 0x4
		_asm pushad
			
		_asm
		{
			mov eax, [esp + 0x28]
			mov eax, [eax]
			push eax
			call GetMethodCallback
			add esp, 0x4

			mov [esp + 0x20], eax
		}

		_asm popad
		
		_asm
		{
			pop eax
			cmp eax, 0
			jne method_found
		}

		_restore
		{
			mov eax, dword ptr ss:[esp + 0x8]
			mov edx, dword ptr ss:[esp + 0x4]
		}

		JumpBack(LoadMethodMP)

		_asm
		{
			method_found:
			ret
		}
	}
}