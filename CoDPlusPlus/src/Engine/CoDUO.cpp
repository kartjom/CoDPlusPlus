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
			ScrThread_ReturnValueHook.Inject(0x0048f40e, ScrThread_ReturnValue_n, 5);
			SV_MapHook.Inject(0x00457650, hkSV_Map, 6);
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
		bg_iNumWeapons = (int32_t*)(uo_game_mp_x86 + 0x0010ed3c);

		using namespace Hook::Detour;
		{
			// Loaders
			GScr_LoadGameTypeScriptHook.Inject(uo_game_mp_x86 + 0x000361c0, hkGScr_LoadGameTypeScript, 8);

			Scr_GetFunctionHook.Inject(uo_game_mp_x86 + 0x0003d230, hkScr_GetFunction, 6);
			Scr_GetMethodHook.Inject(uo_game_mp_x86 + 0x0003d330, hkScr_GetMethod, 8);

			ConsoleCommandHook.Inject(uo_game_mp_x86 + 0x0004f0d0, hkConsoleCommand, 6);
			ClientCommandHook.Inject(uo_game_mp_x86 + 0x00024f70, hkClientCommand, 6);

			// Gsc Callbacks
			G_InitGameHook.Inject(uo_game_mp_x86 + 0x0002bfc0, hkG_InitGame, 6);

			FireWeaponAntilagHook.Inject(uo_game_mp_x86 + 0x00056850, hkFireWeaponAntilag, 8);
			Weapon_MeleeHook.Inject(uo_game_mp_x86 + 0x000556a0, hkWeapon_Melee, 6);
			G_SayHook.Inject(uo_game_mp_x86 + 0x00022b50, hkG_Say, 6);
			PlayerInactivityHook.Inject(uo_game_mp_x86 + 0x0001aaa0, PlayerInactivity_n, 6);

			Cmd_VoteHook.Inject(uo_game_mp_x86 + 0x00024150, hkCmd_Vote, 8);
			VoteCalledHook.Inject(uo_game_mp_x86 + 0x00023698, VoteCalled_n, 5);
		
			G_BounceMissileHook.Inject(uo_game_mp_x86 + 0x0002ff40, hkG_BounceMissile, 9);
			G_ExplodeMissileHook.Inject(uo_game_mp_x86 + 0x00030420, hkG_ExplodeMissile, 5);
			G_ExplodeSmokeHook.Inject(uo_game_mp_x86 + 0x00030aa0, hkG_ExplodeSmoke, 8);

			// Fixes
			VehicleCrashFixHook.Inject(uo_game_mp_x86 + 0x0001b482, VehicleCrashFix_n, 6);
			VEH_UnlinkPlayerFixHook.Inject(uo_game_mp_x86 + 0x0004804a, VEH_UnlinkPlayerFix_n, 5);

			// Utility
			// G_RunFrameHook.Inject(uo_game_mp_x86 + 0x0002cf40, hkG_RunFrame, 6);
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
		bg_iNumWeapons = nullptr;

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
			PlayerInactivityHook.Dispose();
			Cmd_VoteHook.Dispose();
			VoteCalledHook.Dispose();
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