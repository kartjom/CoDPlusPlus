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

	ScrVar Scr_RunScript(int32_t scriptHandle, uint32_t argc)
	{
		typedef uint16_t(__cdecl* Scr_ExecThread_t)(int32_t, uint32_t);
		typedef void(__cdecl* Scr_FreeThread_t)(int16_t);

		Scr_ExecThread_t Scr_ExecThread = (Scr_ExecThread_t)(0x0048f3e0);
		Scr_FreeThread_t Scr_FreeThread = (Scr_FreeThread_t)(0x0048f640);

		uint16_t handle = Scr_ExecThread(scriptHandle, argc);
		ScrVar return_value = CoDUO::Gsc::Scr_ReturnValue;
		Scr_FreeThread(handle);

		return return_value;
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

	int32_t Scr_GetPointerType(int index)
	{
		_asm
		{
			push index
			mov eax, 0x004905a0
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

	int32_t Scr_GetFunction(int param)
	{
		_asm
		{
			push param
			mov eax, 0x00490370
			call eax

			add esp, 0x4
		}
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

	void Scr_AddBool(qboolean value)
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

	void Scr_AddEntity(gentity_t* entity)
	{
		Scr_AddEntityNum(entity->number);
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