#include "Detours.h"
#include "CoDUO.h"

#pragma region GScr_LoadGameTypeScript
ImplementDetour(GScr_LoadGameTypeScript)
{
	SAFE_CALL
	(
		if (CoDUO::Scr_LoadScript("maps/mp/gametypes/_callbacksetup"))
		{
			CoDUO::CodeCallback_Custom = CoDUO::Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_Custom");
		}
	)

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
	SAFE_CALL
	(
		if (CoDUO::CodeCallback_Custom != 0)
		{
			CoDUO::Scr_RunScript(CoDUO::CodeCallback_Custom, 0);
		}
	)

	_restore
	{
		lea ecx, dword ptr ds : [ecx + eax * 0x4 + 0x334]
		sub dword ptr ds : [ecx] , esi
	}

	JumpBack(ShootCallback)
}
#pragma endregion