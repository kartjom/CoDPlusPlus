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
	void* GetFunctionCallback(const char* value);
	void* GetMethodCallback(const char* value);

	bool RunScriptConsoleCommand(const char* name);
	bool RunScriptClientCommand(gentity_t* player, const char* name);
}

namespace Hook::Detour
{
	void __cdecl hkGScr_LoadGameTypeScript()
	{
		GScr_LoadGameTypeScriptHook.OriginalFn();

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

	int __cdecl hkConsoleCommand()
	{
		// We first lookup our cmd - this allows overriding vanilla behavior
		char* cmd = Cmd_Argv[0];
		int cmdFound = RunScriptConsoleCommand(cmd);

		if (cmdFound == 0)
		{
			cmdFound = ConsoleCommandHook.OriginalFn();
		}

		return cmdFound;
	}

	/* int clientNum - ECX */
	void __cdecl hkClientCommand()
	{
		// We first lookup our cmd - this allows overriding vanilla behavior
		int clientNum = CapturedContext.ecx;

		gentity_t* player = g_entities + clientNum;
		char* cmd = Cmd_Argv[0];
		int cmdFound = RunScriptClientCommand(player, cmd);

		if (cmdFound == 0)
		{
			_asm mov ecx, CapturedContext.ecx // int clientNum
			ClientCommandHook.OriginalFn();
		}
	}
}

namespace Hook::Detour
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

	bool __cdecl RunScriptConsoleCommand(const char* cmd_name)
	{
		std::string name_lowercase(cmd_name);
		for (int i = 0; name_lowercase[i]; i++)
		{
			name_lowercase[i] = tolower(name_lowercase[i]);
		}

		if (auto it = gsc_commands.find(name_lowercase); it != gsc_commands.end())
		{
			if (int32_t handle = it->second)
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

	bool __cdecl RunScriptClientCommand(gentity_t* player, const char* cmd_name)
	{
		std::string name_lowercase(cmd_name);
		for (int i = 0; name_lowercase[i]; i++)
		{
			name_lowercase[i] = tolower(name_lowercase[i]);
		}

		if (auto it = gsc_clientcommands.find(name_lowercase); player && player->client && it != gsc_clientcommands.end())
		{
			if (int32_t handle = it->second)
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