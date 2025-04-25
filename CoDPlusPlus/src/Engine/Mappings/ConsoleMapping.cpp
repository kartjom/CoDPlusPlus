#include <Engine/CoDUO.h>

namespace CoDUO
{
	int Cmd_Argc()
	{
		int& cmd_argc = *(int*)(0x009677c0);
		return cmd_argc;
	}

	const char* Cmd_Argv(int arg)
	{
		const char** cmd_argv = (const char**)(0x00964dc0);
		if (arg >= Cmd_Argc())
		{
			return "";
		}

		return cmd_argv[arg];
	}

	void Cbuf_AddText(const char* text)
	{
		_asm
		{
			mov eax, text

			mov esi, 0x0042bc10
			call esi
		}
	}

	void __cdecl Cmd_AddCommand(const char* cmd_name, void* function)
	{
		auto Cmd_AddCommand_f = (decltype(Cmd_AddCommand)*)(0x0042c3b0);
		Cmd_AddCommand_f(cmd_name, function);
	}

	void __cdecl Cmd_RemoveCommand(const char* cmd_name)
	{
		auto Cmd_RemoveCommand_f = (decltype(Cmd_RemoveCommand)*)(0x0042c4a0);
		Cmd_RemoveCommand_f(cmd_name);
	}

	bool Cmd_HasCommand(const char* cmd_name)
	{
		cmd_function_t* cmd;

		for (cmd = cmd_functions; cmd; cmd = cmd->next)
		{
			if (!strcmp(cmd_name, cmd->name))
			{
				return true;
			}
		}

		return false;
	}

	cvar_t* __cdecl Cvar_FindVar(const char* var_name)
	{
		auto Cvar_FindVar_f = (decltype(Cvar_FindVar)*)(0x0043d8f0);
		return Cvar_FindVar_f(var_name);
	}

	cvar_t* __cdecl Cvar_Get(const char* var_name, const char* var_value, int flags)
	{
		auto Cvar_Get_f = (decltype(Cvar_Get)*)(0x0043d9e0);
		return Cvar_Get_f(var_name, var_value, flags);
	}

	cvar_t* __cdecl Cvar_Set(const char* var_name, const char* value, uint32_t force)
	{
		auto Cvar_Set_f = (decltype(Cvar_Set)*)(0x0043dc50);
		return Cvar_Set_f(var_name, value, force);
	}
}