#include "CoDUO.h"
#include <Hook/Hook.h>
#include <Hook/Detours.h>

#include <iostream>

using namespace CoDUO::Gsc;
namespace CoDUO
{
	void BaseAttach()
	{
		BYTE payloadBuffer[] = { 0xEB }; // Console cvars
		Hook::Patch(0x0043DD86, payloadBuffer, 1); // Read Only
		Hook::Patch(0x0043DDA3, payloadBuffer, 1); // Write Protected
		Hook::Patch(0x0043DDC1, payloadBuffer, 1); // Cheat Protected

		refdef = (refdef_t*)(0x0489A100);
		cvar_indexes = (cvar_t*)(0x009987A0);
		cvar_indexes = cvar_indexes->next; // first one is junk, remove if something's broken
		Cmd_Argv = (char**)(0x00964DC0);
		Cmd_Argc = (int*)(0x009677C0);

		DetourRet(0x00457702, Detours::SV_Map_LoadConfig, 8);

		std::cout << "[CoDUOMP] - BaseAttach" << std::endl;
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
		g_entities = (gentity_t*)(uo_game_mp_x86 + 0x00118d40);
		gameCvarTable = (cvarTable_t*)(uo_game_mp_x86 + 0x00086A58);
		bg_iNumWeapons = (int32_t*)(uo_game_mp_x86 + 0x0010ED3C);

		std::unique_lock<std::mutex> lock(HttpMutex);
		BackgroundHttpResults = {};
		CodeCallback = {};

		DetourRet(uo_game_mp_x86 + 0x000361c0, Detours::GScr_LoadGameTypeScript, 8);
		DetourRet(uo_game_mp_x86 + 0x0001b1e6, Detours::Tick, 6);

		DetourRet(uo_game_mp_x86 + 0x0005689d, Detours::ShootCallback, 6);
		DetourRet(uo_game_mp_x86 + 0x00055727, Detours::MeleeCallback, 5);
		DetourRet(uo_game_mp_x86 + 0x00022B5D, Detours::PlayerSayCallback, 7);
		DetourRet(uo_game_mp_x86 + 0x0002ff58, Detours::ProjectileBounceCallback, 5);
		DetourRet(uo_game_mp_x86 + 0x00030420, Detours::ProjectileExplodeCallback, 5);
		DetourRet(uo_game_mp_x86 + 0x00023310, Detours::VoteCallCallback, 6);
		DetourRet(uo_game_mp_x86 + 0x0002415f, Detours::PlayerVoteCallback, 6);

		DetourRet(uo_game_mp_x86 + 0x0003D230, Detours::LoadFunctionMP, 6);
		DetourRet(uo_game_mp_x86 + 0x0003D330, Detours::LoadMethodMP, 8);
		
		FlushInstructionCache(GetCurrentProcess(), NULL, NULL);

		std::cout << "[uo_game_mp_x86] - OnAttach" << std::endl;
	}

	void uo_game_mp_x86_OnDetach()
	{
		svs = nullptr;
		g_entities = nullptr;
		gameCvarTable = nullptr;
		bg_iNumWeapons = nullptr;

		std::unique_lock<std::mutex> lock(HttpMutex);
		BackgroundHttpResults = {};
		CodeCallback = {};

		std::cout << "[uo_game_mp_x86] - OnDetach" << std::endl;
	}

	void Tick()
	{
		std::unique_lock<std::mutex> lock(HttpMutex);
		if (CodeCallback.OnHttpResponse)
		{
			if (!BackgroundHttpResults.empty())
			{
				HttpResult result = BackgroundHttpResults.front();
				BackgroundHttpResults.pop();

				Scr_AddString(result.Body.c_str());
				Scr_AddInt(result.StatusCode);
				Scr_AddString(result.Identifier.c_str());
				Scr_RunScript(CodeCallback.OnHttpResponse, 3);
			}
		}
	}
}