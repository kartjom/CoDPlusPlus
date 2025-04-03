#pragma once
#include "Hook.h"
#include <wtypes.h>

#include <Structs/Engine/gentity.h>
#include <Structs/Engine/trace.h>

namespace Hook::Patch
{
	inline Hook::MemoryPatch ReadOnlyPatch;
	inline Hook::MemoryPatch WriteProtectedPatch;
	inline Hook::MemoryPatch CheatProtectedPatch;
}

namespace Hook::Detour /* Core.cpp */
{
	inline Hook::DetourHook ScrThread_ReturnValueHook;
	void ScrThread_ReturnValue_n() noexcept;

	inline Hook::DetourHook MapBindingsHook;
	void MapBindings_n() noexcept;
}

namespace Hook::Detour /* Loaders.cpp */
{
	typedef void(__cdecl* GScr_LoadGameTypeScript_t)();
	inline Hook::TrampolineHook<GScr_LoadGameTypeScript_t> GScr_LoadGameTypeScriptHook;
	void __cdecl hkGScr_LoadGameTypeScript();

	typedef void*(__cdecl* Scr_GetFunction_t)(const char**, int*);
	inline Hook::TrampolineHook<Scr_GetFunction_t> Scr_GetFunctionHook;
	void* __cdecl hkScr_GetFunction(const char**, int*);

	typedef void* (__cdecl* Scr_GetMethod_t)(const char**, int*);
	inline Hook::TrampolineHook<Scr_GetMethod_t> Scr_GetMethodHook;
	void* __cdecl hkScr_GetMethod(const char**, int*);

	typedef int(__cdecl* ConsoleCommand_t)();
	inline Hook::TrampolineHook<ConsoleCommand_t> ConsoleCommandHook;
	int __cdecl hkConsoleCommand();

	typedef void(__cdecl* ClientCommand_t)();
	inline Hook::TrampolineHook<ClientCommand_t> ClientCommandHook;
	void __cdecl hkClientCommand();
}

namespace Hook::Detour /* CodeCallbacks.cpp */
{
	typedef void(__cdecl* G_InitGame_t)(int, int, int);
	inline Hook::TrampolineHook<G_InitGame_t> G_InitGameHook;
	void __cdecl hkG_InitGame(int, int, int);

	typedef void(__cdecl* FireWeaponAntilag_t)(gentity_t*);
	inline Hook::TrampolineHook<FireWeaponAntilag_t> FireWeaponAntilagHook;
	void __cdecl hkFireWeaponAntilag(gentity_t*);

	inline Hook::DetourHook PlayerMeleeHook;
	void PlayerMelee_n() noexcept;

	typedef void(__cdecl* G_Say_t)(gentity_t*, int, char*);
	inline Hook::TrampolineHook<G_Say_t> G_SayHook;
	void __cdecl hkG_Say(gentity_t*, int, char*);

	inline Hook::DetourHook PlayerInactivityHook;
	void PlayerInactivity_n() noexcept;

	inline Hook::DetourHook PlayerVoteHook;
	void PlayerVote_n() noexcept;

	inline Hook::DetourHook VoteCalledHook;
	void VoteCalled_n() noexcept;

	typedef void(__cdecl* G_BounceMissile_t)(gentity_t*, trace_t*);
	inline Hook::TrampolineHook<G_BounceMissile_t> G_BounceMissileHook;
	void __cdecl hkG_BounceMissile(gentity_t*, trace_t*);

	typedef void(__cdecl* G_ExplodeMissile_t)(gentity_t*);
	inline Hook::TrampolineHook<G_ExplodeMissile_t> G_ExplodeMissileHook;
	void __cdecl hkG_ExplodeMissile(gentity_t*);

	typedef void(__cdecl* G_ExplodeSmoke_t)(gentity_t*);
	inline Hook::TrampolineHook<G_ExplodeSmoke_t> G_ExplodeSmokeHook;
	void __cdecl hkG_ExplodeSmoke(gentity_t*);
}

namespace Hook::Detour /* Fixes.cpp */
{
	inline Hook::DetourHook VehicleCrashFixHook;
	void VehicleCrashFix_n() noexcept;

	inline Hook::DetourHook VEH_UnlinkPlayerFixHook;
	void VEH_UnlinkPlayerFix_n() noexcept;
}

namespace Hook::Detour /* Utility */
{
	inline Hook::DetourHook ServerTickHook;
	void ServerTick_n() noexcept;
}

namespace Hook::Detour /* WinAPI.cpp */
{
	typedef HMODULE(__stdcall* LoadLibraryA_t)(LPCSTR);
	inline Hook::TrampolineHook<LoadLibraryA_t> LoadLibraryAHook;
	HMODULE __stdcall hkLoadLibraryA(LPCSTR);

	typedef NTSTATUS(__stdcall* LdrUnloadDll_t)(PVOID);
	inline Hook::TrampolineHook<LdrUnloadDll_t> LdrUnloadDllHook;
	NTSTATUS __stdcall hkLdrUnloadDll(PVOID);
}

#ifdef CLIENT
namespace Hook::Detour /* OpenGL.cpp */
{
	typedef void(__cdecl* GLimp_EndFrame_t)();
	inline Hook::TrampolineHook<GLimp_EndFrame_t> GLimp_EndFrameHook;
	void __cdecl hkGLimp_EndFrame();
}
#endif