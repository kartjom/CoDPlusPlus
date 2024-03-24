#include <Engine/CoDUO.h>
#include <Hook/Detours.h>
#include <stdio.h>
#include <fstream>
#include <filesystem>
#include <Utils/WinApiHelper.h>
#include <Utils/OpenGLHelper.h>

using namespace CoDUO;
using namespace CoDUO::Gsc;

namespace Detours
{
	void* GetFunctionCallback(const char* value);
	void* GetMethodCallback(const char* value);

	bool RunCustomConsoleCommand(const char* name);
	bool RunCustomClientCommand(gentity_t* player, const char* name);

	void LoadMapBindings();
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

	ImplementDetour(SV_Map_LoadConfig)
	{
		_asm pushad

		LoadMapBindings();

		_asm popad

		_restore
		{
			mov esi, dword ptr ds : [0x9677C0]
			test esi, esi
		}

		JumpBack(SV_Map_LoadConfig)
	}

	ImplementDetour(ServerTick)
	{
		_asm pushad

		CoDUO::ServerTick();

		_asm popad

		_restore
		{
			sub esp, 0x254
		}

		JumpBack(ServerTick)
	}

	ImplementDetour(ConsoleCommand)
	{
		_asm pushad

		_asm
		{
			lea eax, [esp + 0x20]
			push eax // cmd_name

			call RunCustomConsoleCommand

			add esp, 0x4

			cmp al, 1
			je cmd_found
		}

		_asm popad

		_restore
		{
			mov eax, uo_game_mp_x86
			add eax, 0x001ee4ac
			mov eax, [eax]
		}

		JumpBack(ConsoleCommand)

	
		_asm
		{
			cmd_found:
			popad
			add esp, 0x404
			mov eax, 0x1
			ret
		}
	}

	ImplementDetour(ClientCommand)
	{
		_asm pushad

		_asm
		{
			lea eax, [esp + 0x28]
			push eax // cmd_name
			push edi // player

			call RunCustomClientCommand

			add esp, 0x8

			cmp al, 1
			je cmd_found
		}

		_asm popad

		_restore
		{
			mov eax, 0x1869f
		}

		JumpBack(ClientCommand)

		_asm
		{
			cmd_found:
			popad
			add esp, 0x40c
			mov eax, 0x1
			ret
		}
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

	bool __cdecl RunCustomConsoleCommand(const char* cmd_name)
	{
		std::string name_lowercase(cmd_name);
		for (int i = 0; name_lowercase[i]; i++)
		{
			name_lowercase[i] = tolower(name_lowercase[i]);
		}

		if (gsc_commands.find(name_lowercase) != gsc_commands.end())
		{
			int32_t handle = gsc_commands[name_lowercase];

			if (handle)
			{
				int argc = *Cmd_Argc;

				Scr_MakeArray();
				for (int i = 1; i < argc; i++)
				{
					Scr_AddString(Cmd_Argv[i]);
					Scr_AddArray();
				}

				Scr_RunScript(handle, 1);

				return true;
			}
		}

		return false;
	}

	bool __cdecl RunCustomClientCommand(gentity_t* player, const char* cmd_name)
	{
		std::string name_lowercase(cmd_name);
		for (int i = 0; name_lowercase[i]; i++)
		{
			name_lowercase[i] = tolower(name_lowercase[i]);
		}

		if (player && player->client && gsc_clientcommands.find(name_lowercase) != gsc_clientcommands.end())
		{
			int32_t handle = gsc_clientcommands[name_lowercase];

			if (handle)
			{
				int argc = *Cmd_Argc;

				Scr_MakeArray();
				for (int i = 1; i < argc; i++)
				{
					Scr_AddString(Cmd_Argv[i]);
					Scr_AddArray();
				}

				Scr_AddEntityNum(player->number);
				Scr_RunScript(handle, 2);

				return true;
			}
		}

		return false;
	}

	void __cdecl LoadMapBindings()
	{
		constexpr const char* map_bindings = "codplusplus/map_bindings.txt";

		char* mapname = Cmd_Argv[1];
		if (mapname == nullptr || *mapname == '\0' || !std::filesystem::exists(map_bindings)) return;

		std::ifstream file(map_bindings, std::ifstream::in);
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
}