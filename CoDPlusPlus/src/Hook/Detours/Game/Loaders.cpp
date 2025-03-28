#include <Engine/CoDUO.h>
#include <Hook/Detours.h>
#include <stdio.h>
#include <fstream>
#include <filesystem>
#include <Utils/WinApiHelper.h>
#include <Utils/OpenGLHelper.h>

using namespace CoDUO;
using namespace CoDUO::Gsc;

namespace Hook::Detour
{
	void LoadGameTypeScript();

	void* GetFunctionCallback(const char* value);
	void* GetMethodCallback(const char* value);

	bool RunCustomConsoleCommand(const char* name);
	bool RunCustomClientCommand(gentity_t* player, const char* name);
}

namespace Hook::Detour
{
	_declspec(naked) void LoadGameTypeScript_n() noexcept
	{
		_asm pushad
		LoadGameTypeScript();
		_asm popad

		_asm // restore
		{
			sub esp, 0x44

			mov eax, uo_game_mp_x86
			add eax, 0x00082650

			mov eax, [eax]
		}

		_asm jmp[LoadGameTypeScriptHook.Return] // jump back
	}

	_declspec(naked) void LookupFunction_n() noexcept
	{
		_asm sub esp, 0x4

		_asm pushad
		_asm
		{
			mov eax, [esp + 0x28]
			mov eax, [eax]
			push eax // value
			call GetFunctionCallback
			add esp, 0x4 // 1 arg, 4 bytes

			mov[esp + 0x20], eax
		}
		_asm popad

		_asm
		{
			pop eax
			cmp eax, 0
			jne function_found
		}

		_asm // restore
		{
			mov eax, dword ptr ss : [esp + 0x4]
			mov edx, dword ptr ds : [eax]
		}

		_asm jmp[LookupFunctionHook.Return] // jump back

		_asm
		{
			function_found:
			ret
		}
	}

	_declspec(naked) void LookupMethod_n() noexcept
	{
		_asm sub esp, 0x4

		_asm pushad
		_asm
		{
			mov eax, [esp + 0x28]
			mov eax, [eax]
			push eax // value
			call GetMethodCallback
			add esp, 0x4 // 1 arg, 4 bytes

			mov[esp + 0x20], eax
		}
		_asm popad

		_asm
		{
			pop eax
			cmp eax, 0
			jne method_found
		}

		_asm // restore
		{
			mov eax, dword ptr ss : [esp + 0x8]
			mov edx, dword ptr ss : [esp + 0x4]
		}

		_asm jmp[LookupMethodHook.Return] // jump back

		_asm
		{
			method_found:
			ret
		}
	}

	_declspec(naked) void LookupCommand_n() noexcept
	{
		_asm pushad
		_asm
		{
			lea eax, [esp + 0x20]
			push eax // cmd_name

			call RunCustomConsoleCommand

			add esp, 0x4 // 1 arg, 4 bytes

			cmp al, 1
			je cmd_found
		}
		_asm popad

		_asm // restore
		{
			mov eax, uo_game_mp_x86
			add eax, 0x001ee4ac
			mov eax, [eax]
		}

		_asm jmp[LookupCommandHook.Return] // jump back
	
		_asm
		{
			cmd_found:
			popad
			add esp, 0x404
			mov eax, 0x1
			ret
		}
	}

	_declspec(naked) void LookupClientCommand_n() noexcept
	{
		_asm pushad
		_asm
		{
			lea eax, [esp + 0x28]
			push eax // cmd_name
			push edi // player

			call RunCustomClientCommand

			add esp, 0x8 // 2 args, 8 bytes

			cmp al, 1
			je cmd_found
		}
		_asm popad

		_asm // restore
		{
			mov eax, 0x1869f
		}

		_asm jmp[LookupClientCommandHook.Return] // jump back

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

namespace Hook::Detour
{
	void __cdecl LoadGameTypeScript()
	{
		constexpr const char* _codplusplus = "maps/mp/gametypes/_codplusplus";
		if (Scr_LoadScript(_codplusplus))
		{
			CodeCallback.OnInitialize = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnInitialize");
			CodeCallback.OnPlayerShoot = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnPlayerShoot");
			CodeCallback.OnPlayerMelee = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnPlayerMelee");
			CodeCallback.OnPlayerSay = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnPlayerSay");
			CodeCallback.OnPlayerVote = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnPlayerVote");
			CodeCallback.OnPlayerInactivity = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnPlayerInactivity");
			CodeCallback.OnVoteCalled = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnVoteCalled");
			CodeCallback.OnProjectileBounce = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnProjectileBounce");
			CodeCallback.OnProjectileExplode = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnProjectileExplode");
		}
	}

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
}