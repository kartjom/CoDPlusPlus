#include <Engine/CoDUO.h>
#include <Hook/Detours.h>
#include <stdio.h>
#include <fstream>
#include <filesystem>

#define MAP_BINDINGS "map_bindings.txt"

using namespace CoDUO;
using namespace CoDUO::Gsc;

void* GetFunctionCallback(const char* value)
{
	if (gsc_functions.find(value) != gsc_functions.end())
	{
		return gsc_functions[value].callback;
	}

	return 0;
}

void* GetMethodCallback(const char* value)
{
	if (gsc_methods.find(value) != gsc_methods.end())
	{
		return gsc_methods[value].callback;
	}

	return 0;
}

void LoadMapBinding()
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

namespace Detours
{
	ImplementDetour(GScr_LoadGameTypeScript)
	{
		_asm pushad

		if (Scr_LoadScript("maps/mp/gametypes/_callbacksetup"))
		{
			CodeCallback.OnPlayerShoot = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnPlayerShoot");
			CodeCallback.OnPlayerMelee = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnPlayerMelee");
			CodeCallback.OnPlayerSay = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnPlayerSay");
			CodeCallback.OnProjectileBounce = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnProjectileBounce");
			CodeCallback.OnProjectileExplode = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnProjectileExplode");
			CodeCallback.OnHttpResponse = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnHttpResponse");
		}

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

	ImplementDetour(Tick)
	{
		_asm pushad

		Tick();

		_asm popad

		_restore
		{
			sub esp, 0x254
		}

		JumpBack(Tick)
	}

	ImplementDetour(ShootCallback)
	{
		_asm pushad

		if (CodeCallback.OnPlayerShoot)
		{
			gentity_t* player = nullptr;
			_asm
			{
				mov player, edi
			}

			if (player && player->client)
			{
				Scr_AddEntityNum(player->number);
				Scr_RunScript(CodeCallback.OnPlayerShoot, 1);
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
			gentity_t* player = nullptr;
			int16_t target_num = -1;

			_asm
			{
				mov player, ebp

				mov eax, [esp + 0x8C]
				mov target_num, ax
			}

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

		_asm popad

		_restore
		{
			add esp, 0x2C
			test al, 0x10
		}

		JumpBack(MeleeCallback)
	}

	ImplementDetour(ProjectileBounceCallback)
	{
		_asm pushad

		if (CodeCallback.OnProjectileBounce)
		{
			gentity_t* projectile = nullptr;

			_asm
			{
				mov projectile, ebp
			}

			if (projectile)
			{
				Scr_AddEntityNum(projectile->number);
				Scr_RunScript(CodeCallback.OnProjectileBounce, 1);
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
			gentity_t* projectile = nullptr;

			_asm
			{
				mov projectile, ebp
			}

			if (projectile)
			{
				Scr_AddEntityNum(projectile->number);
				Scr_RunScript(CodeCallback.OnProjectileExplode, 1);
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

			mov [esp + 0x20], eax
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
			mov eax, dword ptr ss:[esp + 0x4]
			mov edx, dword ptr ds:[eax]
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

			mov [esp + 0x20], eax
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
			mov eax, dword ptr ss:[esp + 0x8]
			mov edx, dword ptr ss:[esp + 0x4]
		}

		JumpBack(LoadMethodMP)

		_asm
		{
			method_found:
			ret
		}
	}

	ImplementDetour(PlayerSayCallback)
	{
		_asm pushad

		if (CodeCallback.OnPlayerSay)
		{
			_asm
			{
				mov	ebp, esp
			}

			static int clientNum = -1;
			static int mode = -1;
			static char* text = nullptr;

			_asm
			{
				mov eax, esp

				add eax, 0x208
				mov edi, [eax]
				mov clientNum, edi

				add eax, 0x4
				mov edi, [eax]
				mov mode, edi

				add eax, 0x4
				mov edi, [eax]
				add edi, 0x1
				mov text, edi
			}

			if (text && clientNum >= 0 && clientNum < 128)
			{
				if (text[strlen(text) - 1] == 21) text[strlen(text) - 1] = '\0';

				Scr_AddInt(mode);
				Scr_AddString(text);
				Scr_AddEntityNum(clientNum);
				Scr_RunScript(CodeCallback.OnPlayerSay, 3);
			}
		}

		_asm popad

		_restore
		{
			mov dword ptr ss:[esp + 0x1E0], eax
		}

		JumpBack(PlayerSayCallback)
	}

	ImplementDetour(SV_Map_LoadConfig)
	{
		_asm pushad

		LoadMapBinding();

		_asm popad

		_restore
		{
			mov esi, dword ptr ds:[0x9677C0]
			test esi, esi
		}

		JumpBack(SV_Map_LoadConfig)
	}
}