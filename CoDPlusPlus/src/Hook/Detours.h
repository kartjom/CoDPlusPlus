#pragma once
#include "Hook.h"
#include <wtypes.h>

#include <Structs/q_shared.h>
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
	uint16_t __cdecl hkScr_ExecThread(int32_t, uint32_t);
	inline TrampolineHookFrom<hkScr_ExecThread> Scr_ExecThreadHook;

	void __cdecl hkSV_Map();
	inline TrampolineHookFrom<hkSV_Map> SV_MapHook;
}

namespace Hook::Detour /* Loaders.cpp */
{
	void __cdecl hkGScr_LoadGameTypeScript();
	inline TrampolineHookFrom<hkGScr_LoadGameTypeScript> GScr_LoadGameTypeScriptHook;

	void* __cdecl hkScr_GetFunction(const char**, int*);
	inline TrampolineHookFrom<hkScr_GetFunction> Scr_GetFunctionHook;

	void* __cdecl hkScr_GetMethod(const char**, int*);
	inline TrampolineHookFrom<hkScr_GetMethod> Scr_GetMethodHook;

	qboolean __cdecl hkConsoleCommand();
	inline TrampolineHookFrom<hkConsoleCommand> ConsoleCommandHook;

	void __cdecl hkClientCommand();
	inline TrampolineHookFrom<hkClientCommand> ClientCommandHook;
}

namespace Hook::Detour /* CodeCallbacks.cpp */
{
	void __cdecl hkG_InitGame(int, int, int);
	inline TrampolineHookFrom<hkG_InitGame> G_InitGameHook;

	void __cdecl hkFireWeaponAntilag(gentity_t*);
	inline TrampolineHookFrom<hkFireWeaponAntilag> FireWeaponAntilagHook;

	void __cdecl hkWeapon_Melee(gentity_t*);
	inline TrampolineHookFrom<hkWeapon_Melee> Weapon_MeleeHook;

	void __cdecl hkG_Say(gentity_t*, int, char*);
	inline TrampolineHookFrom<hkG_Say> G_SayHook;

	qboolean __cdecl hkClientInactivityTimer();
	inline TrampolineHookFrom<hkClientInactivityTimer> ClientInactivityTimerHook;

	void __cdecl hkCmd_Vote();
	inline TrampolineHookFrom<hkCmd_Vote> Cmd_VoteHook;

	void __cdecl hkCmd_CallVote(gentity_t*);
	inline TrampolineHookFrom<hkCmd_CallVote> Cmd_CallVoteHook;

	void __cdecl hkG_BounceMissile(gentity_t*, trace_t*);
	inline TrampolineHookFrom<hkG_BounceMissile> G_BounceMissileHook;

	void __cdecl hkG_ExplodeMissile(gentity_t*);
	inline TrampolineHookFrom<hkG_ExplodeMissile> G_ExplodeMissileHook;

	void __cdecl hkG_ExplodeSmoke(gentity_t*);
	inline TrampolineHookFrom<hkG_ExplodeSmoke> G_ExplodeSmokeHook;
}

namespace Hook::Detour /* Fixes.cpp */
{
	inline DetourHook VehicleCrashFixHook;
	void VehicleCrashFix_n() noexcept;

	inline DetourHook VEH_UnlinkPlayerFixHook;
	void VEH_UnlinkPlayerFix_n() noexcept;

	int __cdecl hkFUN_00421510();
	inline TrampolineHookFrom<hkFUN_00421510> FUN_00421510Hook;
}

namespace Hook::Detour /* Utility.cpp */
{
	void __cdecl hkG_RunFrame(int);
	inline TrampolineHookFrom<hkG_RunFrame> G_RunFrameHook;
}

namespace Hook::Detour /* WinAPI.cpp */
{
	HMODULE __stdcall hkLoadLibraryA(LPCSTR);
	inline TrampolineHookFrom<hkLoadLibraryA> LoadLibraryAHook;

	NTSTATUS __stdcall hkLdrUnloadDll(PVOID);
	inline TrampolineHookFrom<hkLdrUnloadDll> LdrUnloadDllHook;
}

#ifdef CLIENT
namespace Hook::Detour /* OpenGL.cpp */
{
	void __cdecl hkGLimp_EndFrame();
	inline TrampolineHookFrom<hkGLimp_EndFrame> GLimp_EndFrameHook;
}
#endif