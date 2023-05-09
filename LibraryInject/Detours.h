#pragma once
#include "Detours_defines.h"
#include <wtypes.h>

DeclareType(__stdcall, HMODULE, LoadLibraryA)(LPCSTR lpLibFileName);
DeclareType(__stdcall, BOOL, SetPhysicalCursorPos)(int x, int y);

DeclareType(__stdcall, BOOL, wglSwapBuffers)(HDC hDc);

class Detours
{
public:
	DeclareOverrideArg1(__stdcall, HMODULE, LoadLibraryA, LPCSTR lpLibFileName);
	DeclareOverrideArg2(__stdcall, BOOL, SetPhysicalCursorPos, int x, int y);

	DeclareOverrideArg1(__stdcall, BOOL, wglSwapBuffers, HDC hDc);

	DeclareDetour(GScr_LoadGameTypeScript);
	DeclareDetour(ShootCallback);
};