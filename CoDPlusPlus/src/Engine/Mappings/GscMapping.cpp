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
		Scr_FreeThread(handle);

		return CoDUO::Gsc::Scr_InternalReturnValue;
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

	VarType Scr_GetType(int param)
	{
		_asm
		{
			push param
			mov eax, 0x00490530
			call eax

			add esp, 0x4
		}
	}

	VarType Scr_GetPointerType(int index)
	{
		_asm
		{
			push index
			mov eax, 0x004905a0
			call eax

			add esp, 0x4
		}
	}

	VarType Scr_GetVarType(VariableValue* var)
	{
		if (!var)
			return VarType::Undefined;

		if (var->type == VarType::Object)
		{
			constexpr int VAR_MASK = 0x1F;

			uint8_t* base = (uint8_t*)0x00aa6b6c;
			uint16_t pointerValue = (uint16_t)var->pointerValue;

			uint32_t rawValue = *(uint32_t*)(base + pointerValue * 0xC);
			uint32_t objectType = rawValue & VAR_MASK;

			return (VarType)objectType;
		}
		
		return var->type;
	}

	const char* Scr_GetTypeName(VarType type)
	{
		const char** var_typename = (const char**)(0x005ca6f8);

		if (type >= VarType::Undefined && type < VarType::DeadObject)
		{
			return var_typename[(int)type];
		}

		return "";
	}

	VariableValue* Scr_GetValue(int32_t param)
	{
		VariableValue*& scrVmPub_top = *(VariableValue**)(0x00b6ac90);
		return &scrVmPub_top[-param];
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
			add eax, 0x0004E1E0
			call eax

			add esp, 0x4
		}
	}

	gentity_t* Scr_GetVarEntity(VariableValue* var)
	{
		if (var && Scr_GetVarType(var) == VarType::Entity)
		{
			uint16_t index = (uint16_t)var->pointerValue;
			uint32_t offset = index * 0xC;
			uint8_t* tableBase = (uint8_t*)(0x00AA6B6A);
			uint16_t entityNum = *(uint16_t*)(tableBase + offset);

			if (entityNum >= 0 && entityNum <= WORLDSPAWN)
			{
				return g_entities + entityNum;
			}
		}

		return nullptr;
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