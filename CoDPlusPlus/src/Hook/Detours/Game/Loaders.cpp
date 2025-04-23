#include <Engine/CoDUO.h>
#include <Hook/Detours.h>
#include <stdio.h>
#include <fstream>
#include <filesystem>
#include <Utils/String/String.h>
#include <Utils/WinApiHelper.h>
#include <Utils/OpenGLHelper.h>

using namespace CoDUO;
using namespace CoDUO::Gsc;

namespace Hook::Detour
{
	bool RunScriptConsoleCommand(const char* name);
	bool RunScriptClientCommand(gentity_t* player, const char* name);
}

namespace Hook::Detour
{
	void __cdecl hkGScr_LoadGameTypeScript()
	{
		GScr_LoadGameTypeScriptHook.Invoke();

		constexpr const char* _codplusplus = "maps/mp/gametypes/_codplusplus";
		if (Scr_LoadScript(_codplusplus))
		{
			CodeCallback.OnInitialize = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnInitialize");
			CodeCallback.OnPlayerShoot = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnPlayerShoot");
			CodeCallback.OnPlayerMelee = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnPlayerMelee");
			CodeCallback.OnPlayerSay = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnPlayerSay");
			CodeCallback.OnPlayerVote = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnPlayerVote");
			CodeCallback.OnPlayerInactivity = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnPlayerInactivity");
			CodeCallback.OnPlayerVoteCalled = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnPlayerVoteCalled");
			CodeCallback.OnProjectileBounce = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnProjectileBounce");
			CodeCallback.OnProjectileExplode = Scr_GetFunctionHandle(_codplusplus, "CodeCallback_OnProjectileExplode");
		}
	}

	void* __cdecl hkScr_GetFunction(const char** pName, int* pType)
	{
		if (auto it = gsc_functions.find(*pName); it != gsc_functions.end())
		{
			gsc_function_t& scr_fn = it->second;

			*pName = scr_fn.name;
			*pType = scr_fn.developer;
			return scr_fn.callback;
		}

		return Scr_GetFunctionHook.Invoke(pName, pType);
	}

	void* __cdecl hkScr_GetMethod(const char** pName, int* pType)
	{
		if (auto it = gsc_methods.find(*pName); it != gsc_methods.end())
		{
			gsc_function_t& scr_fn = it->second;

			*pName = scr_fn.name;
			*pType = scr_fn.developer;
			return scr_fn.callback;
		}

		return Scr_GetMethodHook.Invoke(pName, pType);
	}

	qboolean __cdecl hkConsoleCommand()
	{
		// We first lookup our cmd - this allows overriding vanilla behavior
		const char* cmd = Cmd_Argv(0);
		qboolean cmdFound = RunScriptConsoleCommand(cmd);

		if (cmdFound == 0)
		{
			cmdFound = ConsoleCommandHook.Invoke();
		}

		return cmdFound;
	}

	/* int clientNum - ECX */
	void __cdecl hkClientCommand()
	{
		// We first lookup our cmd - this allows overriding vanilla behavior
		int clientNum = ClientCommandHook.CapturedContext.ecx;
		gentity_t* player = g_entities + clientNum;

		if (!player->client)
			return;

		const char* cmd = Cmd_Argv(0);
		int cmdFound = RunScriptClientCommand(player, cmd);

		if (cmdFound == 0)
		{
			ClientCommandHook.SetECX(clientNum);
			ClientCommandHook.Invoke();
		}
	}
}

namespace Hook::Detour
{
	bool __cdecl RunScriptConsoleCommand(const char* cmd_name)
	{
		std::string name_lowercase = String::ToLower(cmd_name);

		if (auto it = gsc_commands.find(name_lowercase); it != gsc_commands.end())
		{
			if (int32_t handle = it->second)
			{
				Scr_MakeArray();
				for (int i = 1; i < Cmd_Argc(); i++)
				{
					Scr_AddString(Cmd_Argv(i));
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
		std::string name_lowercase = String::ToLower(cmd_name);

		if (auto it = gsc_clientcommands.find(name_lowercase); player && player->client && it != gsc_clientcommands.end())
		{
			if (int32_t handle = it->second)
			{
				Scr_MakeArray();
				for (int i = 1; i < Cmd_Argc(); i++)
				{
					Scr_AddString(Cmd_Argv(i));
					Scr_AddArray();
				}

				Scr_AddEntity(player);
				Scr_RunScript(handle, 2);

				return true;
			}
		}

		return false;
	}
}