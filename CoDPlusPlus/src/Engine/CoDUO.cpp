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
		BYTE payloadBuffer[] = { 0xEB }; // Console cvars
		Hook::Patch(0x0043DD86, payloadBuffer, 1); // Read Only
		Hook::Patch(0x0043DDA3, payloadBuffer, 1); // Write Protected
		Hook::Patch(0x0043DDC1, payloadBuffer, 1); // Cheat Protected

		refdef = (refdef_t*)(0x0489A100);
		s_wmv = (WinMouseVars_t*)(0x009CDBBC);
		cvar_indexes = (cvar_t*)(0x009987A0);
		cvar_indexes = cvar_indexes->next; // first one is junk, remove if something's broken
		cmd_functions = (cmd_function_t**)(0x00964db8);
		Cmd_Argv = (char**)(0x00964DC0);
		Cmd_Argc = (int*)(0x009677C0);

		DetourRet(0x00457702, Detours::SV_Map_LoadConfig, 8);
		DetourRet(0x0048f40e, Detours::Scr_ExecThread_GscReturnValue, 5);

		#ifdef CLIENT
			Cmd_AddCommand("devgui", ImGuiManager::Toggle);
		#endif

		std::println("[CoDUOMP] - BaseAttach");
	}

	void uo_game_mp_x86_OnAttach()
	{
		if (uo_game_mp_x86 == 0)
		{
			MessageBox(NULL,
				(LPCWSTR)L"uo_game_mp_x86.dll not found. Make sure the file is in the mod directory.",
				(LPCWSTR)L"Fatal error",
				MB_ICONERROR | MB_OK | MB_DEFBUTTON2);

			exit(-69);
		}

		svs = (serverStatic_t*)(0x4907BC0);
		level = (level_locals_t*)(uo_game_mp_x86 + 0x0030fac0);
		g_entities = (gentity_t*)(uo_game_mp_x86 + 0x00118d40);
		gameCvarTable = (cvarTable_t*)(uo_game_mp_x86 + 0x00086A58);
		bg_iNumWeapons = (int32_t*)(uo_game_mp_x86 + 0x0010ED3C);

		CodeCallback = {};

		uo_game_mp_x86_Cleanup();

		DetourRet(uo_game_mp_x86 + 0x0003D230, Detours::LoadFunctionMP, 6);
		DetourRet(uo_game_mp_x86 + 0x0003D330, Detours::LoadMethodMP, 8);
		DetourRet(uo_game_mp_x86 + 0x0004f1d2, Detours::ConsoleCommand, 5);
		DetourRet(uo_game_mp_x86 + 0x0002556a, Detours::ClientCommand, 5);

		DetourRet(uo_game_mp_x86 + 0x000361c0, Detours::GScr_LoadGameTypeScript, 8);

		DetourRet(uo_game_mp_x86 + 0x0002c46f, Detours::PostInitGame, 5);
		DetourRet(uo_game_mp_x86 + 0x0005689d, Detours::ShootCallback, 6);
		DetourRet(uo_game_mp_x86 + 0x00055727, Detours::MeleeCallback, 5);
		DetourRet(uo_game_mp_x86 + 0x00022B5D, Detours::PlayerSayCallback, 7);
		DetourRet(uo_game_mp_x86 + 0x0002444c, Detours::PlayerVoteCallback, 9);
		DetourRet(uo_game_mp_x86 + 0x00023698, Detours::VoteCallCallback, 5);
		DetourRet(uo_game_mp_x86 + 0x0001aaa0, Detours::PlayerInactivity, 6);
		DetourRet(uo_game_mp_x86 + 0x0002ff58, Detours::ProjectileBounceCallback, 5);
		DetourRet(uo_game_mp_x86 + 0x00030448, Detours::ProjectileExplodeCallback, 6);
		DetourRet(uo_game_mp_x86 + 0x00030ac5, Detours::SmokeExplodeCallback, 5);

		DetourRet(uo_game_mp_x86 + 0x0001b482, Detours::VehicleCrashFix, 6);
		DetourRet(uo_game_mp_x86 + 0x0004804a, Detours::VEH_UnlinkPlayerFix, 5);

		// Not used but available
		//DetourRet(uo_game_mp_x86 + 0x0001b1e6, Detours::ServerTick, 6);
		
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

	void ServerTick()
	{
	}
}