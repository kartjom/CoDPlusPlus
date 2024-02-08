#include <Engine/CoDUO.h>
#include <Hook/Detours.h>
#include <stdio.h>
#include <fstream>
#include <filesystem>
#include <Utils/WinApiHelper.h>
#include <Utils/OpenGLHelper.h>
#include <Utils/Logger/FileLogger.h>

#define MAP_BINDINGS "map_bindings.txt"

using namespace CoDUO;
using namespace CoDUO::Gsc;

namespace Detours
{
	void* GetFunctionCallback(const char* value);
	void* GetMethodCallback(const char* value);
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

	void __cdecl LoadMapBindings()
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
}