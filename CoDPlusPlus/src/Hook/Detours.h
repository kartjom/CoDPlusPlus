#pragma once
#include "Detours_defines.h"
#include <wtypes.h>

DeclareType(__stdcall, HMODULE, LoadLibraryA)(LPCSTR lpLibFileName);
DeclareType(__stdcall, BOOL, SetPhysicalCursorPos)(int x, int y);

DeclareType(__stdcall, BOOL, wglSwapBuffers)(HDC hDc);

namespace Detours
{
	DeclareOverrideArg1(__stdcall, HMODULE, LoadLibraryA, LPCSTR lpLibFileName);
	DeclareOverrideArg2(__stdcall, BOOL, SetPhysicalCursorPos, int x, int y);

	DeclareOverrideArg1(__stdcall, BOOL, wglSwapBuffers, HDC hDc);

	inline DWORD FreeLibrary_kernelbase = 0;
	DeclareDetour(FreeLibrary);

	DeclareDetour(GScr_LoadGameTypeScript);
	DeclareDetour(Tick);
	DeclareDetour(ShootCallback);
	DeclareDetour(MeleeCallback);
	DeclareDetour(PlayerSayCallback);
	DeclareDetour(ProjectileBounceCallback);
	DeclareDetour(ProjectileExplodeCallback);

	DeclareDetour(LoadFunctionMP);
	DeclareDetour(LoadMethodMP);
};