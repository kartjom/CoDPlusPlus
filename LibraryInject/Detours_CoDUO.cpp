#include "Detours.h"
#include "CoDUO.h"
#include <stdio.h>

using namespace CoDUO;

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

			if (player && weaponName)
			{
				Scr_AddString(weaponName);
				Scr_AddVector(&player->viewangles);
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
}