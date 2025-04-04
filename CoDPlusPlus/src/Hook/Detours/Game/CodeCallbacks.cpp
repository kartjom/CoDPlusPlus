﻿#include <Engine/CoDUO.h>
#include <Hook/Hook.h>
#include <Hook/Detours.h>
#include <print>

using namespace CoDUO;
using namespace CoDUO::Gsc;

namespace Hook::Detour
{
	void OnPlayerMelee(gentity_t* player, int16_t target_num);
	bool OnPlayerInactivity(gclient_t* player);
	void OnPlayerVote(gclient_t* player);
	bool OnVoteCalled(gentity_t* player);
}

namespace Hook::Detour
{
	/* int levelTime - ECX */
	void __cdecl hkG_InitGame(int randomSeed, int restart, int savePersist)
	{
		_asm mov ecx, CapturedContext.ecx // int levelTime
		G_InitGameHook.OriginalFn(randomSeed, restart, savePersist);

		ServerCleanup();

		if (CodeCallback.OnInitialize)
		{
			Scr_RunScript(CodeCallback.OnInitialize, 0);
		}
	}

	void __cdecl hkFireWeaponAntilag(gentity_t* player)
	{
		FireWeaponAntilagHook.OriginalFn(player);

		if (CodeCallback.OnPlayerShoot && player && player->client)
		{
			Scr_AddEntityNum(player->number);
			Scr_RunScript(CodeCallback.OnPlayerShoot, 1);
		}
	}

	_declspec(naked) void PlayerMelee_n() noexcept
	{
		_asm pushad
		if (CodeCallback.OnPlayerMelee)
		{
			_asm
			{
				mov eax, [esp + 0x8C]
				push ax // target_num

				push ebp // player

				call OnPlayerMelee

				add esp, 0x6 // 2 args, 6 bytes
			}
		}
		_asm popad

		_asm // restore
		{
			add esp, 0x2C
			test al, 0x10
		}

		_asm jmp[PlayerMeleeHook.Return] // jump back
	}

	/* gentity_t* ent - ECX */
	void __cdecl hkG_Say(gentity_t* target, int mode, char* chatText)
	{
		gentity_t* ent = (gentity_t*)CapturedContext.ecx;
		if (CodeCallback.OnPlayerSay && ent && ent->client && chatText && *chatText)
		{
			std::string gsc_text = chatText;
			int gsc_mode = mode;
			bool gsc_console;

			int length = gsc_text.length();
			if (gsc_text[0] == 0x14 && gsc_text[length - 1] == 0x15)
			{
				gsc_text[length - 1] = 0; // remove end character

				gsc_mode = 2; // vchat
				gsc_text.erase(0, 1); // skip first character
				gsc_console = false;
			}
			else if (gsc_text[0] == 0x15)
			{
				gsc_text.erase(0, 1);
				gsc_console = false;
			}
			else
			{
				gsc_console = true;
			}

			Scr_AddBool(gsc_console);
			Scr_AddInt(gsc_mode);
			Scr_AddString(gsc_text.c_str());
			Scr_AddEntityNum(ent->number);
			Scr_RunScript(CodeCallback.OnPlayerSay, 4);

			if (Scr_ReturnValue.Type == VarType::String)
			{
				if (Scr_ReturnValue.String.empty())
					return; // don't display message

				char* replaced = (char*)Scr_ReturnValue.String.c_str();

				_asm mov ecx, CapturedContext.ecx // gentity_t* ent
				G_SayHook.OriginalFn(target, mode, replaced); // replace message

				return;
			}
		}

		_asm mov ecx, CapturedContext.ecx // gentity_t* ent
		G_SayHook.OriginalFn(target, mode, chatText);
	}

	_declspec(naked) void PlayerInactivity_n() noexcept
	{
		_asm pushad
		if (CodeCallback.OnPlayerInactivity)
		{
			_asm
			{
				push eax // player
				call OnPlayerInactivity

				add esp, 0x4 // 1 arg, 4 bytes

				cmp al, 1 // Skip inactivity kick
				jne inactivity_continue

				popad
				mov eax, 1 // return value
				ret
			}
		}
	inactivity_continue: // Kick the player
		_asm popad

		_asm // restore
		{
			mov ecx, uo_game_mp_x86
			add ecx, 0x0030fac0
			sub eax, [ecx]
		}

		_asm jmp[PlayerInactivityHook.Return] // jump back
	}

	_declspec(naked) void PlayerVote_n() noexcept
	{
		_asm // restore
		{
			call syscall
			add esp, 0x14
		}

		_asm pushad
		if (CodeCallback.OnPlayerVote)
		{
			_asm
			{
				push esi // player as gclient
				call OnPlayerVote

				add esp, 0x4 // 1 arg, 4 bytes
			}
		}
		_asm popad

		_asm jmp[PlayerVoteHook.Return] // jump back
	}

	_declspec(naked) void VoteCalled_n() noexcept
	{
		_asm pushad
		if (CodeCallback.OnVoteCalled)
		{
			_asm
			{
				push edi // player
				call OnVoteCalled

				add esp, 0x4 // 1 arg, 4 bytes

				cmp al, 1 // Skip vote
				jne vote_continue

				add esp, 0x55C
				popad
				ret
			}
		}
	vote_continue: // Vote goes on
		_asm popad

		_asm // restore
		{
			mov eax, uo_game_mp_x86
			add eax, 0x003105e0
			mov eax,[eax]
		}

		_asm jmp[VoteCalledHook.Return] // jump back
	}

	void __cdecl hkG_BounceMissile(gentity_t* ent, trace_t* trace)
	{
		G_BounceMissileHook.OriginalFn(ent, trace);

		if (CodeCallback.OnProjectileBounce && ent)
		{
			Scr_AddEntityNum(ent->number);
			Scr_RunScript(CodeCallback.OnProjectileBounce, 1);
		}
	}

	void __cdecl hkG_ExplodeMissile(gentity_t* ent)
	{
		G_ExplodeMissileHook.OriginalFn(ent);

		if (CodeCallback.OnProjectileExplode && ent)
		{
			Scr_AddEntityNum(ent->number);
			Scr_RunScript(CodeCallback.OnProjectileExplode, 1);
		}
	}

	void __cdecl hkG_ExplodeSmoke(gentity_t* ent)
	{
		G_ExplodeSmokeHook.OriginalFn(ent);

		if (CodeCallback.OnProjectileExplode && ent)
		{
			Scr_AddEntityNum(ent->number);
			Scr_RunScript(CodeCallback.OnProjectileExplode, 1);
		}
	}
}

namespace Hook::Detour
{
	void __cdecl OnPlayerMelee(gentity_t* player, int16_t target_num)
	{
		if (player)
		{
			if (target_num >= 0 && target_num < WORLDSPAWN)
			{
				Scr_AddEntityNum(g_entities[target_num].number);
			}
			else
			{
				Scr_AddUndefined();
			}

			Scr_AddEntityNum(player->number);
			Scr_RunScript(CodeCallback.OnPlayerMelee, 2);
		}
	}

	bool __cdecl OnPlayerInactivity(gclient_t* player)
	{
		if (player)
		{
			Scr_AddEntityNum(player->clientNum);
			Scr_RunScript(CodeCallback.OnPlayerInactivity, 1);

			if (Scr_ReturnValue.Type == VarType::Integer && Scr_ReturnValue.Integer == 0)
			{
				cvar_t* g_inactivity = Cvar_FindVar("g_inactivity");

				player->inactivityTime = (g_inactivity != nullptr ? g_inactivity->integer : 180) * 1000 + level->time;
				player->inactivityWarning = 0;

				return true; // Skip inactivity penalty if script returned false
			}
		}

		return false;
	}

	void __cdecl OnPlayerVote(gclient_t* player)
	{
		if (*Cmd_Argc >= 2 && player)
		{
			Scr_AddString(Cmd_Argv[1]);
			Scr_AddEntityNum(player->clientNum);
			Scr_RunScript(CodeCallback.OnPlayerVote, 2);
		}
	}

	bool __cdecl OnVoteCalled(gentity_t* player)
	{
		if (*Cmd_Argc >= 2 && player && player->client)
		{
			Scr_MakeArray();
			for (int i = 2; i < *Cmd_Argc; i++)
			{
				Scr_AddString(Cmd_Argv[i]);
				Scr_AddArray();
			}

			Scr_AddString(Cmd_Argv[1]); // Vote Type
			Scr_AddEntityNum(player->number);
			Scr_RunScript(CodeCallback.OnVoteCalled, 3);

			if (Scr_ReturnValue.Type == VarType::Integer && Scr_ReturnValue.Integer == 0)
			{
				return true; // Skip the vote if script returned false
			}
		}

		return false;
	}
}