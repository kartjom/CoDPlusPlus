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
		CoDUO::Scr_AddEntity(0);
		CoDUO::Scr_AddVector((float*)&CoDUO::g_entities[0].viewangles);
		CoDUO::Scr_RunScript(CoDUO::CodeCallback_PlayerShoot, 2);
	}

	_asm popad

	_restore
	{
		lea ecx, dword ptr ds : [ecx + eax * 0x4 + 0x334]
		sub dword ptr ds : [ecx] , esi
	}

	JumpBack(ShootCallback)
}
#pragma endregion