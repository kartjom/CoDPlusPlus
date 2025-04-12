#include <Engine/CoDUO.h>
#include <Hook/Hook.h>
#include <Hook/Detours.h>
#include <print>

using namespace CoDUO;
using namespace CoDUO::Gsc;

namespace Hook::Detour
{
	bool OnVoteCalled(gentity_t* player);
}

namespace Hook::Detour
{
	/* int levelTime - ECX */
	void __cdecl hkG_InitGame(int randomSeed, int restart, int savePersist)
	{
		int levelTime = (int)CapturedContext.ecx;

		_asm mov ecx, CapturedContext.ecx // int levelTime
		G_InitGameHook.OriginalFn(randomSeed, restart, savePersist);

		ServerCleanup();

		if (CodeCallback.OnInitialize)
		{
			Scr_AddBool(restart);
			Scr_AddInt(levelTime);
			Scr_RunScript(CodeCallback.OnInitialize, 2);
		}
	}

	void __cdecl hkFireWeaponAntilag(gentity_t* player)
	{
		FireWeaponAntilagHook.OriginalFn(player);

		if (CodeCallback.OnPlayerShoot && player && player->client)
		{
			Scr_AddEntity(player);
			Scr_RunScript(CodeCallback.OnPlayerShoot, 1);
		}
	}

	/* weaponParams_t* wp - EBX */
	void __cdecl hkWeapon_Melee(gentity_t* player)
	{
		weaponParams_t* wp = (weaponParams_t*)CapturedContext.ebx;

		_asm push ebx
		_asm mov ebx, CapturedContext.ebx // weaponParams_t* wp
		Weapon_MeleeHook.OriginalFn(player);
		_asm pop ebx

		if (CodeCallback.OnPlayerMelee && player && player->client)
		{
			trace_t trace;
			vec3_t end = wp->muzzleTrace + (wp->forward * 64.f);

			trap_Trace(&trace, &wp->muzzleTrace, &end, player->number, 0x2802031);

			if (trace.entityNum >= 0 && trace.entityNum <= WORLDSPAWN)
			{
				Scr_AddEntityNum(trace.entityNum);
			}
			else
			{
				Scr_AddUndefined();
			}

			Scr_AddEntity(player);
			Scr_RunScript(CodeCallback.OnPlayerMelee, 2);
		}
	}

	/* gentity_t* ent - ECX */
	void __cdecl hkG_Say(gentity_t* target, int mode, char* chatText)
	{
		gentity_t* ent = (gentity_t*)CapturedContext.ecx;
		if (CodeCallback.OnPlayerSay && ent && ent->client && chatText && *chatText)
		{
			std::string gsc_text = chatText;
			int gsc_mode = mode;
			qboolean gsc_console;

			int length = gsc_text.length();
			if (gsc_text[0] == 0x14 && gsc_text[length - 1] == 0x15)
			{
				gsc_text[length - 1] = 0; // remove end character

				gsc_mode = 2; // vchat
				gsc_text.erase(0, 1); // skip first character
				gsc_console = qfalse;
			}
			else if (gsc_text[0] == 0x15)
			{
				gsc_text.erase(0, 1);
				gsc_console = qfalse;
			}
			else
			{
				gsc_console = qtrue;
			}

			Scr_AddBool(gsc_console);
			Scr_AddInt(gsc_mode);
			Scr_AddString(gsc_text.c_str());
			Scr_AddEntity(ent);
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

	/* gclient_t* client - EAX */
	qboolean __cdecl hkClientInactivityTimer()
	{
		gclient_t* client = (gclient_t*)CapturedContext.eax;
		cvar_t* g_inactivity = Cvar_FindVar("g_inactivity");

		if (!g_inactivity->integer)
		{
			// give everyone some time, so if the operator sets g_inactivity during
			// gameplay, everyone isn't kicked
			client->inactivityTime = level->time + 60 * 1000;
			client->inactivityWarning = qfalse;
		}
		else if ( client->cmd_forwardmove || client->cmd_rightmove || client->cmd_upmove || (client->cmd_buttons & 1) )
		{
			client->inactivityTime = level->time + g_inactivity->integer * 1000;
			client->inactivityWarning = qfalse;
		}
		else if (!client->localClient)
		{
			if (level->time > client->inactivityTime)
			{
				// Allow script to override inactivity penalty
				if (CodeCallback.OnPlayerInactivity && client)
				{
					Scr_AddEntityNum(client->clientNum);
					Scr_RunScript(CodeCallback.OnPlayerInactivity, 1);

					// We check if script returned 'true'
					if (Scr_ReturnValue.Type == VarType::Integer && Scr_ReturnValue.Integer == 1)
					{
						// Reset inactivity time
						client->inactivityTime = client->inactivityTime = level->time + g_inactivity->integer * 1000;
						client->inactivityWarning = qfalse;

						return qtrue; // Don't drop the client
					}
				}

				SV_GameDropClient(client - level->clients, "GAME_DROPPEDFORINACTIVITY");
				return qfalse;
			}

			if (level->time > client->inactivityTime - 10000 && !client->inactivityWarning)
			{
				client->inactivityWarning = qtrue;
				SV_GameSendServerCommand(client - level->clients, 0, "c \"GAME_INACTIVEDROPWARNING\"");
			}
		}

		return qtrue;
	}

	/* gentity_t* ent - ECX */
	void __cdecl hkCmd_Vote()
	{
		gentity_t* ent = (gentity_t*)CapturedContext.ecx;
		bool isValid = ent && ent->client;

		// Original Cmd_VoteHook_f can overwrite some stuff
		int clientNum = isValid ? ent->client->clientNum : 0;
		bool canVote = isValid && level->voteTime && !(ent->client->eFlags & EF_VOTED) && ent->client->sessionTeam != TEAM_SPECTATOR;

		// Call original now - script can have side effects (client kick)
		_asm mov ecx, CapturedContext.ecx // gentity_t* ent
		Cmd_VoteHook.OriginalFn();

		client_t* cl = svs->clients + clientNum;
		if (CodeCallback.OnPlayerVote && canVote && cl && cl->state != CS_ZOMBIE)
		{
			std::string msg = Cmd_Argv(1);
			if (msg.length() < 2) msg.resize(2);

			qboolean vote = msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1';

			Scr_AddBool(vote);
			Scr_AddEntity(ent);
			Scr_RunScript(CodeCallback.OnPlayerVote, 2);
		}
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
			mov eax, [eax]
		}

		_asm jmp[VoteCalledHook.Return] // jump back
	}

	void __cdecl hkG_BounceMissile(gentity_t* ent, trace_t* trace)
	{
		G_BounceMissileHook.OriginalFn(ent, trace);

		if (CodeCallback.OnProjectileBounce && ent)
		{
			Scr_AddEntity(ent);
			Scr_RunScript(CodeCallback.OnProjectileBounce, 1);
		}
	}

	void __cdecl hkG_ExplodeMissile(gentity_t* ent)
	{
		G_ExplodeMissileHook.OriginalFn(ent);

		if (CodeCallback.OnProjectileExplode && ent)
		{
			Scr_AddEntity(ent);
			Scr_RunScript(CodeCallback.OnProjectileExplode, 1);
		}
	}

	void __cdecl hkG_ExplodeSmoke(gentity_t* ent)
	{
		G_ExplodeSmokeHook.OriginalFn(ent);

		if (CodeCallback.OnProjectileExplode && ent)
		{
			Scr_AddEntity(ent);
			Scr_RunScript(CodeCallback.OnProjectileExplode, 1);
		}
	}
}

namespace Hook::Detour
{
	bool __cdecl OnVoteCalled(gentity_t* player)
	{
		if (Cmd_Argc() >= 2 && player && player->client)
		{
			Scr_MakeArray();
			for (int i = 2; i < Cmd_Argc(); i++)
			{
				Scr_AddString( Cmd_Argv(i) );
				Scr_AddArray();
			}

			Scr_AddString( Cmd_Argv(1) ); // Vote Type
			Scr_AddEntity(player);
			Scr_RunScript(CodeCallback.OnVoteCalled, 3);

			if (Scr_ReturnValue.Type == VarType::Integer && Scr_ReturnValue.Integer == 0)
			{
				return true; // Skip the vote if script returned false
			}
		}

		return false;
	}
}