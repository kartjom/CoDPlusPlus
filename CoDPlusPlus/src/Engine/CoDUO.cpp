#include "CoDUO.h"
#include <Hook/Hook.h>
#include <Hook/Detours.h>
#include <Engine/Async/Task/Task.h>
#include <Engine/MapBindings/MapBindings.h>
#include <Utils/ImGuiManager.h>
#include <Utils/ThreadPool/ThreadPool.h>
#include <print>

using namespace Utils;
using namespace CoDUO::Gsc;
using namespace CoDUO::Gsc::Async;

namespace CoDUO
{
	void BaseInitialize()
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
			Scr_ExecThreadHook.Inject(0x0048f3e0, hkScr_ExecThread);
			SV_MapHook.Inject(0x00457650, hkSV_Map);
		}

		AddConsoleCommands();

		std::println("[CoDPlusPlus] - Core Initialized");
	}

	void OnServerInitialize()
	{
		ServerCleanup();

		CodeCallback = {};
		svs = (serverStatic_t*)(0x4907BC0);
		level = (level_locals_t*)(uo_game_mp_x86 + 0x0030fac0);
		g_entities = (gentity_t*)(uo_game_mp_x86 + 0x00118d40);
		gameCvarTable = (cvarTable_t*)(uo_game_mp_x86 + 0x00086a58);

		using namespace Hook::Detour;
		{
			// Loaders
			GScr_LoadGameTypeScriptHook.Inject(uo_game_mp_x86 + 0x000361c0, hkGScr_LoadGameTypeScript);

			Scr_GetFunctionHook.Inject(uo_game_mp_x86 + 0x0003d230, hkScr_GetFunction);
			Scr_GetMethodHook.Inject(uo_game_mp_x86 + 0x0003d330, hkScr_GetMethod);

			ConsoleCommandHook.Inject(uo_game_mp_x86 + 0x0004f0d0, hkConsoleCommand);
			ClientCommandHook.Inject(uo_game_mp_x86 + 0x00024f70, hkClientCommand);

			// Gsc Callbacks
			G_InitGameHook.Inject(uo_game_mp_x86 + 0x0002bfc0, hkG_InitGame);

			FireWeaponAntilagHook.Inject(uo_game_mp_x86 + 0x00056850, hkFireWeaponAntilag);
			Weapon_MeleeHook.Inject(uo_game_mp_x86 + 0x000556a0, hkWeapon_Melee);
			G_SayHook.Inject(uo_game_mp_x86 + 0x00022b50, hkG_Say);
			ClientInactivityTimerHook.Inject(uo_game_mp_x86 + 0x0001aa30, hkClientInactivityTimer);

			Cmd_VoteHook.Inject(uo_game_mp_x86 + 0x00024150, hkCmd_Vote);
			Cmd_CallVoteHook.Inject(uo_game_mp_x86 + 0x00023310, hkCmd_CallVote);
		
			G_BounceMissileHook.Inject(uo_game_mp_x86 + 0x0002ff40, hkG_BounceMissile);
			G_ExplodeMissileHook.Inject(uo_game_mp_x86 + 0x00030420, hkG_ExplodeMissile);
			G_ExplodeSmokeHook.Inject(uo_game_mp_x86 + 0x00030aa0, hkG_ExplodeSmoke);

			// Fixes
			VehicleCrashFixHook.Inject(uo_game_mp_x86 + 0x0001b482, VehicleCrashFix_n);
			VEH_UnlinkPlayerFixHook.Inject(uo_game_mp_x86 + 0x0004804a, VEH_UnlinkPlayerFix_n);

			// Utility
			// G_RunFrameHook.Inject(uo_game_mp_x86 + 0x0002cf40, hkG_RunFrame);
		}

		FlushInstructionCache(GetCurrentProcess(), NULL, NULL);

		std::println("[CoDPlusPlus] - Server Initialized");
	}

	void OnServerShutdown()
	{
		ServerCleanup();

		CodeCallback = {};
		svs = nullptr;
		level = nullptr;
		g_entities = nullptr;
		gameCvarTable = nullptr;

		// Dll is unloaded anyways - no need to detach hooks, only dispose them
		using namespace Hook::Detour;
		{
			GScr_LoadGameTypeScriptHook.Dispose();
			Scr_GetFunctionHook.Dispose();
			Scr_GetMethodHook.Dispose();
			ConsoleCommandHook.Dispose();
			ClientCommandHook.Dispose();
			G_InitGameHook.Dispose();
			FireWeaponAntilagHook.Dispose();
			Weapon_MeleeHook.Dispose();
			G_SayHook.Dispose();
			ClientInactivityTimerHook.Dispose();
			Cmd_VoteHook.Dispose();
			Cmd_CallVoteHook.Dispose();
			G_BounceMissileHook.Dispose();
			G_ExplodeMissileHook.Dispose();
			G_ExplodeSmokeHook.Dispose();
			VehicleCrashFixHook.Dispose();
			VEH_UnlinkPlayerFixHook.Dispose();
			// G_RunFrameHook.Dispose();
		}

		std::println("[CoDPlusPlus] - Server Disposed");
	}

	void ServerCleanup()
	{
		ThreadPool.Clear();
		{
			std::unique_lock<std::mutex> lock(TaskListMutex);
			AllocatedTasks.clear();
		}

		gsc_commands.clear();
		gsc_clientcommands.clear();
	}

	void AddConsoleCommands()
	{
		Cmd_AddCommand("reload_map_bindings", MapBindings::ReloadMapBindings);

		#ifdef CLIENT
			Cmd_AddCommand("devgui", ImGuiManager::Toggle);
		#endif
	}
}