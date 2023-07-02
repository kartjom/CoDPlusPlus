#include "CoDUO.h"
#include "Hook.h"
#include "Detours.h"

#include <iostream>

using namespace CoDUO::Gsc;
namespace CoDUO
{
	void G_GetPlayerViewOrigin(gentity_t* ent, float* destination)
	{
		_asm
		{
			mov esi, destination
			mov edi, ent
			mov eax, uo_game_mp_x86
			add eax, 0x000563b0
			call eax
		}
	}

	void trap_GetUserinfo(int num, char* buffer, int bufferSize)
	{
		_asm
		{
			push bufferSize
			push buffer
			push num
			push 0x21
			mov eax, syscall
			call eax

			add esp, 0x10
		}
	}

	void trap_SetUserinfo(int num, const char* buffer)
	{
		_asm
		{
			push buffer
			push num
			push 0x22
			mov eax, syscall
			call eax

			add esp, 0xC
		}
	}

	void BaseAttach()
	{
		BYTE payloadBuffer[] = { 0xEB }; // Console cvars
		Hook::Patch(0x0043DD86, payloadBuffer, 1); // Read Only
		Hook::Patch(0x0043DDA3, payloadBuffer, 1); // Write Protected
		Hook::Patch(0x0043DDC1, payloadBuffer, 1); // Cheat Protected

		refdef = (refdef_t*)(0x0489A100);
		cvar_indexes = (cvar_t*)(0x009987A0);
		cvar_indexes = cvar_indexes->next; // first one is junk, remove if something's broken

		std::cout << "[CoDUOMP] - BaseAttach" << std::endl;
	}

	void uo_game_mp_x86_OnAttach()
	{
		g_entities = (gentity_t*)(uo_game_mp_x86 + 0x00118d40);
		gameCvarTable = (cvarTable_t*)(uo_game_mp_x86 + 0x00086A58);
		bg_iNumWeapons = (int32_t*)(uo_game_mp_x86 + 0x0010ED3C);

		CodeCallback = {};

		DetourRet(uo_game_mp_x86 + 0x000361c0, Detours::GScr_LoadGameTypeScript, 8);
		DetourRet(uo_game_mp_x86 + 0x0005689d, Detours::ShootCallback, 6);
		DetourRet(uo_game_mp_x86 + 0x00055727, Detours::MeleeCallback, 5);
		DetourRet(uo_game_mp_x86 + 0x00022B5D, Detours::PlayerSayCallback, 7);
		DetourRet(uo_game_mp_x86 + 0x0002ff58, Detours::ProjectileBounceCallback, 5);
		DetourRet(uo_game_mp_x86 + 0x00030420, Detours::ProjectileExplodeCallback, 5);

		DetourRet(uo_game_mp_x86 + 0x0003D230, Detours::LoadFunctionMP, 6);
		DetourRet(uo_game_mp_x86 + 0x0003D330, Detours::LoadMethodMP, 8);

		std::cout << "[uo_game_mp_x86] - OnAttach" << std::endl;
	}

	void uo_game_mp_x86_OnDetach()
	{
		g_entities = nullptr;
		gameCvarTable = nullptr;
		bg_iNumWeapons = nullptr;

		CodeCallback = {}; // Clear all callbacks

		std::cout << "[uo_game_mp_x86] - OnDetach" << std::endl;
	}
}