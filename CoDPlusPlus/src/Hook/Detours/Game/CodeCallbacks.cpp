#include <Engine/CoDUO.h>
#include <Hook/Detours.h>
#include <print>

using namespace CoDUO;
using namespace CoDUO::Gsc;

namespace Hook::Detour
{
	void OnInitialize();
	void OnPlayerShoot(gentity_t* player);
	void OnPlayerMelee(gentity_t* player, int16_t target_num);
	bool OnPlayerSay(gentity_t* player, char** text_ptr, int mode);
	bool OnPlayerInactivity(gclient_t* player);
	void OnPlayerVote(gclient_t* player);
	bool OnVoteCalled(gentity_t* player);
	void OnProjectileBounce(gentity_t* projectile);
	void OnProjectileExplode(gentity_t* projectile);
}

namespace Hook::Detour
{
	_declspec(naked) void Initialize_n() noexcept
	{
		_asm // restore
		{
			add esp, 0xc
			pop esi
			pop ebp
		}

		_asm pushad
		OnInitialize();
		_asm popad

		_asm jmp[InitializeHook.Return] // jump back
	}

	_declspec(naked) void PlayerShoot_n() noexcept
	{
		_asm pushad
		if (CodeCallback.OnPlayerShoot)
		{
			_asm
			{
				push edi // player
				call OnPlayerShoot

				add esp, 0x4 // 1 arg, 4 bytes
			}
		}
		_asm popad

		_asm // restore
		{
			mov eax, dword ptr ss : [ebp + 0x160]
		}

		_asm jmp[PlayerShootHook.Return] // jump back
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

	_declspec(naked) void PlayerSay_n() noexcept
	{
		_asm pushad
		if (CodeCallback.OnPlayerSay)
		{
			_asm
			{
				mov eax, esp

				add eax, 0x20C
				mov edi, [eax]
				push edi // mode

				add eax, 0x4
				//mov edi, [eax]
				//push edi // text
				push eax // ptr to text ptr

				push ecx // player

				call OnPlayerSay

				add esp, 0xC // 3 args, 12 bytes

				cmp al, 1 // Skip G_Say
				jne say_continue

				popad
				pop ebp
				pop ebx
				add esp, 0x1DC
				ret
			}
		}
	say_continue:
		_asm popad

		_asm // restore
		{
			mov dword ptr ss : [esp + 0x1E0], eax
		}

		_asm jmp[PlayerSayHook.Return] // jump back
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

	_declspec(naked) void ProjectileBounce_n() noexcept
	{
		_asm pushad
		if (CodeCallback.OnProjectileBounce)
		{
			_asm
			{
				push ebp // projectile
				call OnProjectileBounce

				add esp, 0x4 // 1 arg, 4 bytes
			}
		}
		_asm popad

		_asm // restore
		{
			mov eax, dword ptr ss : [esp + 0x44]
			push edi
		}

		_asm jmp[ProjectileBounceHook.Return] // jump back
	}

	_declspec(naked) void ProjectileExplode_n() noexcept
	{
		_asm pushad
		if (CodeCallback.OnProjectileExplode)
		{
			_asm
			{
				push edi // projectile
				call OnProjectileExplode

				add esp, 0x4 // 1 arg, 4 bytes
			}
		}
		_asm popad

		_asm // restore
		{
			mov eax, dword ptr [edi + 0x1a8]
		}

		_asm jmp[ProjectileExplodeHook.Return]; // jump back
	}

	_declspec(naked) void SmokeExplode_n() noexcept
	{
		_asm pushad
		if (CodeCallback.OnProjectileExplode)
		{
			_asm
			{
				push edi // projectile
				call OnProjectileExplode

				add esp, 0x4 // 1 arg, 4 bytes
			}
		}
		_asm popad

		_asm // restore
		{
			push ebx
			push esi
			lea ebx, [edi + 0xc]
		}

		_asm jmp[SmokeExplodeHook.Return]; // jump back
	}
}

namespace Hook::Detour
{
	void __cdecl OnInitialize()
	{
		uo_game_mp_x86_Cleanup();

		if (CodeCallback.OnInitialize)
		{
			Scr_RunScript(CodeCallback.OnInitialize, 0);
		}
	}

	void __cdecl OnPlayerShoot(gentity_t* player)
	{
		if (player && player->client)
		{
			Scr_AddEntityNum(player->number);
			Scr_RunScript(CodeCallback.OnPlayerShoot, 1);
		}
	}

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

	bool __cdecl OnPlayerSay(gentity_t* player, char** text_ptr, int mode)
	{
		static char StringBuffer[256];

		if (text_ptr && *text_ptr && player && player->client)
		{
			strcpy(StringBuffer, *text_ptr); // string under the text_ptr
			char* text = StringBuffer;

			if (text[0] == 0x14 && text[strlen(text) - 1] == 0x15)
			{
				text[strlen(text) - 1] = '\0'; // vchat end character, broken in gsc

				mode = 2; // set mode to vchat
				text += 1; // skip first character
				Scr_AddBool(0);
			}
			else if (text[0] == 0x15)
			{
				text += 1; // skip first character
				Scr_AddBool(0);
			}
			else
			{
				Scr_AddBool(1);
			}

			Scr_AddInt(mode);
			Scr_AddString(text);
			Scr_AddEntityNum(player->number);
			Scr_RunScript(CodeCallback.OnPlayerSay, 4);

			if (Scr_ReturnValue.Type == VarType::String)
			{
				if (Scr_ReturnValue.String.empty())
					return true; // message won't be displayed if callback returns empty string

				strcpy(StringBuffer, Scr_ReturnValue.String.c_str());
				*text_ptr = StringBuffer;
			}
		}

		return false;
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

	void __cdecl OnProjectileBounce(gentity_t* projectile)
	{
		if (projectile)
		{
			Scr_AddEntityNum(projectile->number);
			Scr_RunScript(CodeCallback.OnProjectileBounce, 1);
		}
	}

	void __cdecl OnProjectileExplode(gentity_t* projectile)
	{
		if (projectile)
		{
			Scr_AddEntityNum(projectile->number);
			Scr_RunScript(CodeCallback.OnProjectileExplode, 1);
		}
	}
}