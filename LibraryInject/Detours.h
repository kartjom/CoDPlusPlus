#pragma once
#include "Detours_defines.h"
#include <wtypes.h>

DeclareTypeArg1(HMODULE, __stdcall, LoadLibraryA, LPCSTR lpLibFileName);
DeclareTypeArg2(BOOL, __stdcall, SetPhysicalCursorPos, int x, int y);

DeclareTypeArg1(BOOL, __stdcall, wglSwapBuffers, HDC hDc);

class Detours
{
public:
	DeclareOverrideArg1(HMODULE, __stdcall, LoadLibraryA, LPCSTR lpLibFileName);
	DeclareOverrideArg2(BOOL, __stdcall, SetPhysicalCursorPos, int x, int y);

	DeclareOverrideArg1(BOOL, __stdcall, wglSwapBuffers, HDC hDc);

	DeclareDetour(GScr_LoadGameTypeScript);
	DeclareDetour(ShootCallback);
};