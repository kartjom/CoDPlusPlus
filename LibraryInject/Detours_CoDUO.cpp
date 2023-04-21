#include "Detours.h"
#include "CoDUO.h"

#pragma region GScr_LoadGameTypeScript
ImplementDetour(GScr_LoadGameTypeScript)
{
	_asm pushad

	if (CoDUO::Scr_LoadScript("maps/mp/gametypes/_callbacksetup"))
	{
		CoDUO::CodeCallback_PlayerShoot = CoDUO::Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerShoot");
	}

	_asm popad

	_restore
	{
		sub esp, 0x44

		mov eax, CoDUO::uo_game_mp_x86
		add eax, 0x00082650

		mov eax, [eax]
	}

	JumpBack(GScr_LoadGameTypeScript)
}
#pragma endregion

#pragma region ShootCallback
ImplementDetour(ShootCallback)
{
	_asm pushad

	if (CoDUO::CodeCallback_PlayerShoot != 0)
	{
		gentity_t* player;
		const char* weaponName;

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
			CoDUO::Scr_AddString(weaponName);
			CoDUO::Scr_AddVector(&player->viewangles);
			CoDUO::Scr_AddEntity(player->number);
			CoDUO::Scr_RunScript(CoDUO::CodeCallback_PlayerShoot, 3);
		}
	}

	_asm popad

	_restore
	{
		mov eax, dword ptr ss:[ebp + 0x160]
	}

	JumpBack(ShootCallback)
}
#pragma endregion