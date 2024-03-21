#include <Engine/CoDUO.h>

namespace CoDUO
{
	void Cbuf_AddText(const char* text)
	{
		_asm
		{
			mov eax, text
			mov esi, 0x0042bc10
			call esi
		}
	}

	void trap_SendConsoleCommand(int exec_when, const char* text)
	{
		_asm
		{
			push text
			push exec_when
			push 0x17
			call syscall

			add esp, 0xC
		}
	}

	void Cmd_AddCommand(const char* cmd_name, void* function)
	{
		_asm
		{
			push function
			push cmd_name
			mov eax, 0x0042C3B0
			call eax

			add esp, 0x8
		}
	}

	void Cmd_RemoveCommand(const char* cmd_name)
	{
		_asm
		{
			push cmd_name
			mov eax, 0x0042c4a0
			call eax

			add esp, 0x4
		}
	}

	bool Cmd_HasCommand(const char* cmd_name)
	{
		cmd_function_t* cmd;

		for (cmd = *cmd_functions; cmd; cmd = cmd->next)
		{
			if (!strcmp(cmd_name, cmd->name))
			{
				return true;
			}
		}

		return false;
	}

	cvar_t* Cvar_FindVar(const char* var_name)
	{
		_asm
		{
			push var_name
			mov eax, 0x0043D8F0
			call eax

			add esp, 0x4
		}
	}

	cvar_t* Cvar_Get(const char* var_name, const char* var_value, int flags)
	{
		_asm
		{
			push flags
			push var_value
			push var_name
			mov eax, 0x0043D9E0
			call eax

			add esp, 0xC
		}
	}

	cvar_t* Cvar_Set(const char* var_name, const char* value, uint32_t force)
	{
		_asm
		{
			push force
			push value
			push var_name
			mov eax, 0x0043DC50
			call eax

			add esp, 0xC
		}
	}
}