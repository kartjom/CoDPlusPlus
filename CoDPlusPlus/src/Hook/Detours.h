#pragma once
#include "Detours_defines.h"
#include <wtypes.h>

DeclareType(__stdcall, HMODULE, LoadLibraryA)(LPCSTR lpLibFileName);

namespace Detours
{
	DeclareOverrideArg1(__stdcall, HMODULE, LoadLibraryA, LPCSTR lpLibFileName);
	
	inline DWORD FreeLibrary_kernelbase = 0;
	DeclareDetour(FreeLibrary);

	DeclareDetour(GScr_LoadGameTypeScript);
	DeclareDetour(ServerTick);
	DeclareDetour(PostInitGame);
	DeclareDetour(ConsoleCommand);
	DeclareDetour(ClientCommand);

	DeclareDetour(ShootCallback);
	DeclareDetour(MeleeCallback);
	DeclareDetour(PlayerSayCallback);
	DeclareDetour(VoteCallCallback);
	DeclareDetour(PlayerVoteCallback);
	DeclareDetour(PlayerInactivity);
	DeclareDetour(ProjectileBounceCallback);
	DeclareDetour(ProjectileExplodeCallback);
	DeclareDetour(SmokeExplodeCallback);

	DeclareDetour(LoadFunctionMP);
	DeclareDetour(LoadMethodMP);
	DeclareDetour(Scr_ExecThread_GscReturnValue);

	DeclareDetour(SV_Map_LoadConfig);

	// Various fixes for UO bugs/exploits
	DeclareDetour(VehicleCrashFix);
	DeclareDetour(VEH_UnlinkPlayerFix);
};

#ifdef CLIENT

DeclareType(__stdcall, BOOL, SetPhysicalCursorPos)(int x, int y);
DeclareType(__stdcall, BOOL, wglSwapBuffers)(HDC hDc);

namespace Detours
{
	DeclareOverrideArg2(__stdcall, BOOL, SetPhysicalCursorPos, int x, int y);
	DeclareOverrideArg1(__stdcall, BOOL, wglSwapBuffers, HDC hDc);
}

#endif