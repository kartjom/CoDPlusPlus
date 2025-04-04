﻿#pragma once
#include "Hook.h"
#include <wtypes.h>

#include <Structs/Engine/gentity.h>
#include <Structs/Engine/trace.h>

namespace Hook::Patch
{
	inline MemoryPatch ReadOnlyPatch;
	inline MemoryPatch WriteProtectedPatch;
	inline MemoryPatch CheatProtectedPatch;
}

namespace Hook::Detour /* Core.cpp */
{
	inline DetourHook ScrThread_ReturnValueHook;
	void ScrThread_ReturnValue_n() noexcept;

	inline DetourHook MapBindingsHook;
	void MapBindings_n() noexcept;
}

namespace Hook::Detour /* Loaders.cpp */
{
	inline TrampolineHook<void(__cdecl*)()> GScr_LoadGameTypeScriptHook;
	void __cdecl hkGScr_LoadGameTypeScript();

	inline TrampolineHook<void*(__cdecl*)(const char**, int*)> Scr_GetFunctionHook;
	void* __cdecl hkScr_GetFunction(const char**, int*);

	inline TrampolineHook<void*(__cdecl*)(const char**, int*)> Scr_GetMethodHook;
	void* __cdecl hkScr_GetMethod(const char**, int*);

	inline TrampolineHook<int(__cdecl*)()> ConsoleCommandHook;
	int __cdecl hkConsoleCommand();

	inline TrampolineHook<void(__cdecl*)()> ClientCommandHook;
	void __cdecl hkClientCommand();
}

namespace Hook::Detour /* CodeCallbacks.cpp */
{
	inline TrampolineHook<void(__cdecl*)(int, int, int)> G_InitGameHook;
	void __cdecl hkG_InitGame(int, int, int);

	inline TrampolineHook<void(__cdecl*)(gentity_t*)> FireWeaponAntilagHook;
	void __cdecl hkFireWeaponAntilag(gentity_t*);

	inline DetourHook PlayerMeleeHook;
	void PlayerMelee_n() noexcept;

	inline TrampolineHook<void(__cdecl*)(gentity_t*, int, char*)> G_SayHook;
	void __cdecl hkG_Say(gentity_t*, int, char*);

	inline DetourHook PlayerInactivityHook;
	void PlayerInactivity_n() noexcept;

	inline DetourHook PlayerVoteHook;
	void PlayerVote_n() noexcept;

	inline DetourHook VoteCalledHook;
	void VoteCalled_n() noexcept;

	inline TrampolineHook<void(__cdecl*)(gentity_t*, trace_t*)> G_BounceMissileHook;
	void __cdecl hkG_BounceMissile(gentity_t*, trace_t*);

	inline TrampolineHook<void(__cdecl*)(gentity_t*)> G_ExplodeMissileHook;
	void __cdecl hkG_ExplodeMissile(gentity_t*);

	inline TrampolineHook<void(__cdecl*)(gentity_t*)> G_ExplodeSmokeHook;
	void __cdecl hkG_ExplodeSmoke(gentity_t*);
}

namespace Hook::Detour /* Fixes.cpp */
{
	inline DetourHook VehicleCrashFixHook;
	void VehicleCrashFix_n() noexcept;

	inline DetourHook VEH_UnlinkPlayerFixHook;
	void VEH_UnlinkPlayerFix_n() noexcept;
}

namespace Hook::Detour /* Utility */
{
	inline TrampolineHook<void(__cdecl*)(int)> G_RunFrameHook;
	void __cdecl hkG_RunFrame(int);
}

namespace Hook::Detour /* WinAPI.cpp */
{
	inline TrampolineHook<HMODULE(__stdcall*)(LPCSTR)> LoadLibraryAHook;
	HMODULE __stdcall hkLoadLibraryA(LPCSTR);

	inline TrampolineHook<NTSTATUS(__stdcall*)(PVOID)> LdrUnloadDllHook;
	NTSTATUS __stdcall hkLdrUnloadDll(PVOID);
}

#ifdef CLIENT
namespace Hook::Detour /* OpenGL.cpp */
{
	inline TrampolineHook<void(__cdecl*)()> GLimp_EndFrameHook;
	void __cdecl hkGLimp_EndFrame();
}
#endif