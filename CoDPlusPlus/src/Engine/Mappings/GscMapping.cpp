#include <Engine/CoDUO.h>

namespace CoDUO
{
	int32_t Scr_LoadScript(const char* file)
	{
		_asm
		{
			push[file]
			mov eax, [file]
			mov edi, 0x00480150
			call edi
			add esp, 0x4
		}
	}

	int32_t Scr_GetFunctionHandle(const char* file, const char* method)
	{
		_asm
		{
			push[method]
			push[file]
			mov eax, 0x0047FE50
			call eax
			add esp, 0x8
		}
	}

	int32_t Scr_RunScript(int32_t scriptHandle, uint32_t argc)
	{
		_asm
		{
			push argc
			push scriptHandle
			mov eax, 0x0048f3e0
			call eax // Scr_ExecThread

			push eax
			mov eax, 0x0048f640
			call eax // Scr_FreeThread

			add esp, 0xC
		}
	}
}

namespace CoDUO
{
	int32_t Scr_GetNumParam()
	{
		_asm
		{
			mov eax, 0x00490640
			call eax
		}
	}

	int32_t Scr_GetType(int param)
	{
		_asm
		{
			push param
			mov eax, 0x00490530
			call eax

			add esp, 0x4
		}
	}

	int32_t Scr_GetInt(int param)
	{
		_asm
		{
			push param
			mov eax, 0x0048FC00
			call eax

			add esp, 0x4
		}
	}

	float Scr_GetFloat(int param)
	{
		_asm
		{
			push param
			mov eax, 0x0048FF00
			call eax

			add esp, 0x4
		}
	}

	void Scr_GetVector(int param, void* destination)
	{
		_asm
		{
			push destination
			push param
			mov eax, 0x004902A0
			call eax

			add esp, 0x8
		}
	}

	vec3_t Scr_GetVector(int param)
	{
		vec3_t vec;
		Scr_GetVector(param, &vec);

		return vec;
	}

	int32_t Scr_GetConstString(int param)
	{
		_asm
		{
			push param
			mov eax, 0x0048FFA0
			call eax

			add esp, 0x4
		}
	}

	const char* Scr_GetString(int param)
	{
		int str_id = Scr_GetConstString(param);
		return SL_ConvertToString(str_id);
	}

	gentity_t* Scr_GetEntity(int param)
	{
		_asm
		{
			push param
			mov esi, param
			mov eax, uo_game_mp_x86
			add eax, 0x4E1E0
			call eax

			add esp, 0x4
		}
	}

	VariableValue* Scr_GetValue(uint32_t index)
	{
		VariableValue* ptr = (VariableValue*)(*(uintptr_t*)0x00b6ac90 + (index + 1) * 8);
		return ptr;
	}
}

namespace CoDUO
{
	void Scr_AddUndefined()
	{
		_asm
		{
			mov eax, 0x004906d0
			call eax
		}
	}

	void Scr_AddBool(int value)
	{
		_asm
		{
			push value
			mov eax, 0x00490650
			call eax

			add esp, 0x4
		}
	}

	void Scr_AddInt(int value)
	{
		_asm
		{
			push value
			mov eax, 0x00490670
			call eax

			add esp, 0x4
		}
	}

	void Scr_AddFloat(float value)
	{
		_asm
		{
			push value
			mov eax, 0x00490690
			call eax

			add esp, 0x4
		}
	}

	void Scr_AddVector(float* value)
	{
		_asm
		{
			push value
			mov eax, 0x004908C0
			call eax

			add esp, 0x4
		}
	}

	void Scr_AddString(const char* string)
	{
		_asm
		{
			push[string]
			mov eax, 0x004907F0
			call eax

			add esp, 0x4
		}
	}

	void Scr_AddConstString(int32_t index)
	{
		_asm
		{
			push index
			mov eax, 0x00490890
			call eax

			add esp, 0x4
		}
	}

	void Scr_AddEntityNum(int index)
	{
		_asm
		{
			push 0x0
			push index
			mov eax, 0x00490720
			call eax

			add esp, 0x8
		}
	}

	void Scr_MakeArray()
	{
		_asm
		{
			mov eax, 0x00490910
			call eax
		}
	}

	void Scr_AddArray()
	{
		_asm
		{
			mov eax, 0x00490960
			call eax
		}
	}

	void Scr_AddArrayStringIndexed(int32_t str_index)
	{
		_asm
		{
			push str_index
			mov eax, 0x004909E0
			call eax

			add esp, 0x4
		}
	}
}