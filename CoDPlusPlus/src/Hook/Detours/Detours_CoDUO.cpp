#include <Engine/CoDUO.h>
#include <Hook/Detours.h>
#include <stdio.h>
#include <fstream>
#include <filesystem>

#define MAP_BINDINGS "map_bindings.txt"

using namespace CoDUO;
using namespace CoDUO::Gsc;

namespace Detours
{
	void* GetFunctionCallback(const char* value);
	void* GetMethodCallback(const char* value);
	void Scr_ExecThread_GscReturnValue();
	void LoadMapBinding();

	void LoadGameTypeScript();
	void OnPlayerShoot(gentity_t* player);
	void OnPlayerMelee(gentity_t* player, int16_t target_num);
	bool OnPlayerSay(gentity_t* player, char** text_ptr, int mode);
	void OnPlayerVote(gclient_t* player);
	bool OnVoteCalled(gentity_t* player);

	void OnProjectileBounce(gentity_t* projectile);
	void OnProjectileExplode(gentity_t* projectile);
}

namespace Detours
{
	ImplementDetour(LoadFunctionMP)
	{
		_asm sub esp, 0x4
		_asm pushad

		_asm
		{
			mov eax, [esp + 0x28]
			mov eax, [eax]
			push eax
			call GetFunctionCallback
			add esp, 0x4

			mov[esp + 0x20], eax
		}

		_asm popad

		_asm
		{
			pop eax
			cmp eax, 0
			jne function_found
		}

		_restore
		{
			mov eax, dword ptr ss : [esp + 0x4]
			mov edx, dword ptr ds : [eax]
		}

		JumpBack(LoadFunctionMP)

		_asm
		{
			function_found:
			ret
		}
	}

	ImplementDetour(LoadMethodMP)
	{
		_asm sub esp, 0x4
		_asm pushad

		_asm
		{
			mov eax, [esp + 0x28]
			mov eax, [eax]
			push eax
			call GetMethodCallback
			add esp, 0x4

			mov[esp + 0x20], eax
		}

		_asm popad

		_asm
		{
			pop eax
			cmp eax, 0
			jne method_found
		}

		_restore
		{
			mov eax, dword ptr ss : [esp + 0x8]
			mov edx, dword ptr ss : [esp + 0x4]
		}

		JumpBack(LoadMethodMP)

		_asm
		{
			method_found:
			ret
		}
	}

	ImplementDetour(Scr_ExecThread_GscReturnValue)
	{
		_restore
		{
			mov eax, 0x0048f0b0
			call eax
		}

		_asm pushad

		Scr_ExecThread_GscReturnValue();

		_asm popad

		JumpBack(Scr_ExecThread_GscReturnValue)
	}

	ImplementDetour(SV_Map_LoadConfig)
	{
		_asm pushad

		LoadMapBinding();

		_asm popad

		_restore
		{
			mov esi, dword ptr ds : [0x9677C0]
			test esi, esi
		}

		JumpBack(SV_Map_LoadConfig)
	}

	ImplementDetour(GScr_LoadGameTypeScript)
	{
		_asm pushad

		LoadGameTypeScript();

		_asm popad

		_restore
		{
			sub esp, 0x44

			mov eax, uo_game_mp_x86
			add eax, 0x00082650

			mov eax,[eax]
		}

		JumpBack(GScr_LoadGameTypeScript)
	}

	ImplementDetour(ShootCallback)
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

		_restore
		{
			mov eax, dword ptr ss : [ebp + 0x160]
		}

		JumpBack(ShootCallback)
	}

	ImplementDetour(MeleeCallback)
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

		_restore
		{
			add esp, 0x2C
			test al, 0x10
		}

		JumpBack(MeleeCallback)
	}

	ImplementDetour(PlayerSayCallback)
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

		_restore
		{
			mov dword ptr ss : [esp + 0x1E0] , eax
		}

		JumpBack(PlayerSayCallback)
	}

	ImplementDetour(PlayerVoteCallback)
	{
		_restore
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

		JumpBack(PlayerVoteCallback)
	}

	ImplementDetour(VoteCallCallback)
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

		_restore
		{
			mov eax, uo_game_mp_x86
			add eax, 0x003105e0
			mov eax, [eax]
		}

		JumpBack(VoteCallCallback)
	}

	ImplementDetour(ProjectileBounceCallback)
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

		_restore
		{
			mov eax, dword ptr ss:[esp + 0x44]
			push edi
		}

		JumpBack(ProjectileBounceCallback)
	}

	ImplementDetour(ProjectileExplodeCallback)
	{
		_asm pushad

		if (CodeCallback.OnProjectileExplode)
		{
			_asm
			{
				push ebp // projectile
				call OnProjectileExplode

				add esp, 0x4 // 1 arg, 4 bytes
			}
		}

		_asm popad

		_restore
		{
			sub esp, 0x54
			push ebx
			push ebp
		}

		JumpBack(ProjectileExplodeCallback);
	}

	ImplementDetour(Tick)
	{
		_asm pushad

		ServerTick();

		_asm popad

		_restore
		{
			sub esp, 0x254
		}

		JumpBack(Tick)
	}
}

namespace Detours
{
	void* __cdecl GetFunctionCallback(const char* value)
	{
		if (gsc_functions.find(value) != gsc_functions.end())
		{
			return gsc_functions[value].callback;
		}

		return 0;
	}

	void* __cdecl GetMethodCallback(const char* value)
	{
		if (gsc_methods.find(value) != gsc_methods.end())
		{
			return gsc_methods[value].callback;
		}

		return 0;
	}

	void __cdecl Scr_ExecThread_GscReturnValue()
	{
		try
		{
			VariableValue* var = Scr_GetValue(-1); // get top of the stack

			Scr_ReturnValue = {};
			Scr_ReturnValue.Type = (VarType)var->type;
			switch (Scr_ReturnValue.Type)
			{
			case VarType::Undefined:
				Scr_ReturnValue.Integer = 0;
				break;
			case VarType::String:
			{
				const char* str = SL_ConvertToString(var->StringIndex);
				Scr_ReturnValue.String = (str ? str : "<null>");
				break;
			}
			case VarType::Vector:
				Scr_ReturnValue.Vector = *var->Vector;
				break;
			case VarType::Float:
				Scr_ReturnValue.Float = var->Float;
				break;
			case VarType::Integer:
				Scr_ReturnValue.Integer = var->Integer;
				break;
			case VarType::Entity:
				Scr_ReturnValue.Integer = var->Integer;
				break;
			default:
				Scr_ReturnValue.Integer = var->Integer;
			}

		}
		catch (std::exception& e)
		{
			printf("Scr_ExecThread_GscReturnValue: %s\n", e.what());
		}
	}

	void __cdecl LoadMapBinding()
	{
		char* mapname = Cmd_Argv[1];
		if (mapname == nullptr || *mapname == '\0' || !std::filesystem::exists(MAP_BINDINGS)) return;

		std::ifstream file(MAP_BINDINGS, std::ifstream::in);
		if (!file.is_open()) return;

		std::string _default;

		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string key, value;
			if (std::getline(iss, key, '=') && std::getline(iss, value))
			{
				if (_default.empty() && _stricmp(key.c_str(), "default") == 0)
				{
					_default = value;
					continue;
				}

				if (_stricmp(key.c_str(), mapname) == 0)
				{
					Cvar_Set("fs_game", value.c_str(), 1);

					file.close();
					return;
				}
			}
		}
		file.close();

		if (!_default.empty())
		{
			Cvar_Set("fs_game", _default.c_str(), 1);
		}
	}

	void __cdecl LoadGameTypeScript()
	{
		if (Scr_LoadScript("maps/mp/gametypes/_callbacksetup"))
		{
			CodeCallback.OnInitialize = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnInitialize");
			CodeCallback.OnPlayerShoot = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnPlayerShoot");
			CodeCallback.OnPlayerMelee = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnPlayerMelee");
			CodeCallback.OnPlayerSay = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnPlayerSay");
			CodeCallback.OnPlayerVote = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnPlayerVote");
			CodeCallback.OnVoteCalled = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnVoteCalled");
			CodeCallback.OnProjectileBounce = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnProjectileBounce");
			CodeCallback.OnProjectileExplode = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnProjectileExplode");
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