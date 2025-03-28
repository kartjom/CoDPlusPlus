#include "CoDUO.h"
#include <Hook/Hook.h>
#include <Hook/Detours.h>
#include <Engine/Async/Task/Task.h>
#include <Utils/ImGuiManager.h>
#include <print>

using namespace CoDUO::Gsc;
using namespace CoDUO::Gsc::Async;

namespace CoDUO
{
	void BaseAttach()
	{
		using namespace Hook::Patch;
		{
			ReadOnlyPatch.Inject(0x0043dd86, { 0xEB });
			WriteProtectedPatch.Inject(0x0043dda3, { 0xEB });
			CheatProtectedPatch.Inject(0x0043ddc1, { 0xEB });
		}

		using namespace Hook::Detour;
		{
			// Core functionality
			ScrThread_ReturnValueHook.Inject(0x0048f40e, ScrThread_ReturnValue_n, 5);
			MapBindingsHook.Inject(0x00457702, MapBindings_n, 8);
		}

		#ifdef CLIENT
			Cmd_AddCommand("devgui", ImGuiManager::Toggle);
		#endif

		std::println("[CoDUOMP] - BaseAttach");
	}

	void uo_game_mp_x86_OnAttach()
	{
		svs = (serverStatic_t*)(0x4907BC0);
		level = (level_locals_t*)(uo_game_mp_x86 + 0x0030fac0);
		g_entities = (gentity_t*)(uo_game_mp_x86 + 0x00118d40);
		gameCvarTable = (cvarTable_t*)(uo_game_mp_x86 + 0x00086a58);
		bg_iNumWeapons = (int32_t*)(uo_game_mp_x86 + 0x0010ed3c);

		CodeCallback = {};

		uo_game_mp_x86_Cleanup();

		using namespace Hook::Detour;
		{
			// Loaders
			LoadGameTypeScriptHook.Inject(uo_game_mp_x86 + 0x000361c0, LoadGameTypeScript_n, 8);

			LookupFunctionHook.Inject(uo_game_mp_x86 + 0x0003D230, LookupFunction_n, 6);
			LookupMethodHook.Inject(uo_game_mp_x86 + 0x0003D330, LookupMethod_n, 8);

			LookupCommandHook.Inject(uo_game_mp_x86 + 0x0004f1d2, LookupCommand_n, 5);
			LookupClientCommandHook.Inject(uo_game_mp_x86 + 0x0002556a, LookupClientCommand_n, 5);

			// Gsc Callbacks
			InitializeHook.Inject(uo_game_mp_x86 + 0x0002c46f, Initialize_n, 5);

			PlayerShootHook.Inject(uo_game_mp_x86 + 0x0005689d, PlayerShoot_n, 6);
			PlayerMeleeHook.Inject(uo_game_mp_x86 + 0x00055727, PlayerMelee_n, 5);
			PlayerSayHook.Inject(uo_game_mp_x86 + 0x00022b5d, PlayerSay_n, 7);
			PlayerInactivityHook.Inject(uo_game_mp_x86 + 0x0001aaa0, PlayerInactivity_n, 6);

			PlayerVoteHook.Inject(uo_game_mp_x86 + 0x0002444c, PlayerVote_n, 9);
			VoteCalledHook.Inject(uo_game_mp_x86 + 0x00023698, VoteCalled_n, 5);
		
			ProjectileBounceHook.Inject(uo_game_mp_x86 + 0x0002ff58, ProjectileBounce_n, 5);
			ProjectileExplodeHook.Inject(uo_game_mp_x86 + 0x00030448, ProjectileExplode_n, 6);
			SmokeExplodeHook.Inject(uo_game_mp_x86 + 0x00030ac5, SmokeExplode_n, 5);

			// Fixes
			VehicleCrashFixHook.Inject(uo_game_mp_x86 + 0x0001b482, VehicleCrashFix_n, 6);
			VEH_UnlinkPlayerFixHook.Inject(uo_game_mp_x86 + 0x0004804a, VEH_UnlinkPlayerFix_n, 5);

			// Utility
			// ServerTickHook.Inject(uo_game_mp_x86 + 0x0001b1e6, ServerTick_n, 6);
		}

		FlushInstructionCache(GetCurrentProcess(), NULL, NULL);

		std::println("[uo_game_mp_x86] - OnAttach");
	}

	void uo_game_mp_x86_OnDetach()
	{
		svs = nullptr;
		level = nullptr;
		g_entities = nullptr;
		gameCvarTable = nullptr;
		bg_iNumWeapons = nullptr;

		CodeCallback = {};

		uo_game_mp_x86_Cleanup();

		std::println("[uo_game_mp_x86] - OnDetach");
	}

	void uo_game_mp_x86_Cleanup()
	{
		{
			std::unique_lock<std::mutex> lock(TaskListMutex);
			AllocatedTasks.clear();
		}

		gsc_commands.clear();
		gsc_clientcommands.clear();
	}
}