#include "CoDUO.h"

namespace CoDUO
{
	int32_t G_NewString(const char* string)
	{
		_asm
		{
			mov edi, string
			mov eax, uo_game_mp_x86
			add eax, 0x0004dbc0
			call eax
		}
	}

	const char* SL_ConvertToString(int32_t index)
	{
		_asm
		{
			push index
			mov eax, 0x00482290
			call eax

			add esp, 0x4
		}
	}

	const char* Info_ValueForKey(const char* buffer, const char* key)
	{
		_asm
		{
			mov ebx, key
			mov ecx, buffer

			mov eax, uo_game_mp_x86
			add eax, 0x00058180
			call eax
		}
	}

	void Info_SetValueForKey(char* buffer, const char* key, const char* value)
	{
		_asm
		{
			mov edx, buffer
			push value
			push buffer
			mov ecx, key

			mov eax, uo_game_mp_x86
			add eax, 0x00058560
			call eax

			add esp, 0x8
		}
	}
}