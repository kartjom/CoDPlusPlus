#pragma once
#include "Hook.h"
#include <wtypes.h>

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
	inline Hook::DetourHook LoadGameTypeScriptHook;
	void LoadGameTypeScript_n() noexcept;

	inline Hook::DetourHook LookupFunctionHook;
	void LookupFunction_n() noexcept;

	inline Hook::DetourHook LookupMethodHook;
	void LookupMethod_n() noexcept;

	inline Hook::DetourHook LookupCommandHook;
	void LookupCommand_n() noexcept;

	inline Hook::DetourHook LookupClientCommandHook;
	void LookupClientCommand_n() noexcept;
}

namespace Hook::Detour /* CodeCallbacks.cpp */
{
	inline Hook::DetourHook InitializeHook;
	void Initialize_n() noexcept;

	inline Hook::DetourHook PlayerShootHook;
	void PlayerShoot_n() noexcept;

	inline Hook::DetourHook PlayerMeleeHook;
	void PlayerMelee_n() noexcept;

	inline Hook::DetourHook PlayerSayHook;
	void PlayerSay_n() noexcept;

	inline Hook::DetourHook PlayerInactivityHook;
	void PlayerInactivity_n() noexcept;

	inline Hook::DetourHook PlayerVoteHook;
	void PlayerVote_n() noexcept;

	inline Hook::DetourHook VoteCalledHook;
	void VoteCalled_n() noexcept;

	inline Hook::DetourHook ProjectileBounceHook;
	void ProjectileBounce_n() noexcept;

	inline Hook::DetourHook ProjectileExplodeHook;
	void ProjectileExplode_n() noexcept;

	inline Hook::DetourHook SmokeExplodeHook;
	void SmokeExplode_n() noexcept;
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
	typedef HMODULE(__stdcall* LoadLibraryA_t)(LPCSTR lpLibFileName);
	inline Hook::TrampolineHook<LoadLibraryA_t> LoadLibraryAHook;
	HMODULE __stdcall hkLoadLibraryA(LPCSTR lpLibFileName);

	typedef NTSTATUS(__stdcall* LdrUnloadDll_t)(PVOID BaseAddress);
	inline Hook::TrampolineHook<LdrUnloadDll_t> LdrUnloadDllHook;
	NTSTATUS __stdcall hkLdrUnloadDll(PVOID BaseAddress);
}

#ifdef CLIENT
namespace Hook::Detour /* OpenGL.cpp */
{
	typedef void(__stdcall* GLimp_EndFrame_t)();
	inline Hook::TrampolineHook<GLimp_EndFrame_t> GLimp_EndFrameHook;
	void __stdcall hkGLimp_EndFrame();
}
#endif