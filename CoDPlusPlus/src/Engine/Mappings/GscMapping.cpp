#include <Engine/CoDUO.h>

namespace CoDUO
{
	int32_t __cdecl Scr_LoadScript(const char* file)
	{
		auto Scr_LoadScript_f = (decltype(Scr_LoadScript)*)(0x00480150);
		return Scr_LoadScript_f(file);
	}

	int32_t __cdecl Scr_GetFunctionHandle(const char* file, const char* method)
	{
		auto Scr_GetFunctionHandle_f = (decltype(Scr_GetFunctionHandle)*)(0x0047fe50);
		return Scr_GetFunctionHandle_f(file, method);
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
	int32_t __cdecl Scr_GetNumParam()
	{
		auto Scr_GetNumParam_f = (decltype(Scr_GetNumParam)*)(0x00490640);
		return Scr_GetNumParam_f();
	}

	VarType __cdecl Scr_GetType(int param)
	{
		auto Scr_GetType_f = (decltype(Scr_GetType)*)(0x00490530);
		return Scr_GetType_f(param);
	}

	VarType __cdecl Scr_GetPointerType(int index)
	{
		auto Scr_GetPointerType_f = (decltype(Scr_GetPointerType)*)(0x004905a0);
		return Scr_GetPointerType_f(index);
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

	int32_t __cdecl Scr_GetInt(int param)
	{
		auto Scr_GetInt_f = (decltype(Scr_GetInt)*)(0x0048fc00);
		return Scr_GetInt_f(param);
	}

	float __cdecl Scr_GetFloat(int param)
	{
		auto Scr_GetFloat_f = (decltype(Scr_GetFloat)*)(0x0048ff00);
		return Scr_GetFloat_f(param);
	}

	vec3_t __cdecl Scr_GetVector(int param)
	{
		typedef void(__cdecl* Scr_GetVector_t)(int param, float* destination);
		Scr_GetVector_t Scr_GetVector_f = (Scr_GetVector_t)(0x004902a0);

		vec3_t vec;
		Scr_GetVector_f(param, vec);

		return vec;
	}

	int32_t __cdecl Scr_GetConstString(int param)
	{
		auto Scr_GetConstString_f = (decltype(Scr_GetConstString)*)(0x0048ffa0);
		return Scr_GetConstString_f(param);
	}

	const char* __cdecl Scr_GetString(int param)
	{
		int str_id = Scr_GetConstString(param);
		return SL_ConvertToString(str_id);
	}

	gentity_t* Scr_GetEntity(int param)
	{
		_asm
		{
			mov ecx, param

			mov eax, uo_game_mp_x86
			add eax, 0x0004e1e0
			call eax
		}
	}

	gentity_t* Scr_GetVarEntity(VariableValue* var)
	{
		if (var && Scr_GetVarType(var) == VarType::Entity)
		{
			uint16_t index = (uint16_t)var->pointerValue;
			uint32_t offset = index * 0xC;
			uint8_t* tableBase = (uint8_t*)(0x00aa6b6a);
			uint16_t entityNum = *(uint16_t*)(tableBase + offset);

			if (entityNum >= 0 && entityNum <= WORLDSPAWN)
			{
				return g_entities + entityNum;
			}
		}

		return nullptr;
	}

	int32_t __cdecl Scr_GetFunction(int param)
	{
		auto Scr_GetFunction_f = (decltype(Scr_GetFunction)*)(0x00490370);
		return Scr_GetFunction_f(param);
	}
}

namespace CoDUO
{
	void __cdecl Scr_AddUndefined()
	{
		auto Scr_AddUndefined_f = (decltype(Scr_AddUndefined)*)(0x004906d0);
		Scr_AddUndefined_f();
	}

	void __cdecl Scr_AddBool(qboolean value)
	{
		auto Scr_AddBool_f = (decltype(Scr_AddBool)*)(0x00490650);
		Scr_AddBool_f(value);
	}

	void __cdecl Scr_AddInt(int value)
	{
		auto Scr_AddInt_f = (decltype(Scr_AddInt)*)(0x00490670);
		Scr_AddInt_f(value);
	}

	void __cdecl Scr_AddFloat(float value)
	{
		auto Scr_AddFloat_f = (decltype(Scr_AddFloat)*)(0x00490690);
		Scr_AddFloat_f(value);
	}

	void __cdecl Scr_AddVector(float* value)
	{
		auto Scr_AddVector_f = (decltype(Scr_AddVector)*)(0x004908c0);
		Scr_AddVector_f(value);
	}

	void __cdecl Scr_AddString(const char* string)
	{
		auto Scr_AddString_f = (decltype(Scr_AddString)*)(0x004907f0);
		Scr_AddString_f(string);
	}

	void __cdecl Scr_AddConstString(int32_t index)
	{
		auto Scr_AddConstString_f = (decltype(Scr_AddConstString)*)(0x00490890);
		Scr_AddConstString_f(index);
	}

	void __cdecl Scr_AddEntityNum(int32_t index, class_num_t classNum)
	{
		auto Scr_AddEntityNum_f = (decltype(Scr_AddEntityNum)*)(0x00490720);
		Scr_AddEntityNum_f(index, classNum);
	}

	void __cdecl Scr_AddEntity(gentity_t* entity)
	{
		Scr_AddEntityNum(entity->number, CLASS_NUM_ENTITY);
	}

	void __cdecl Scr_AddClient(gclient_t* client)
	{
		Scr_AddEntityNum(client->clientNum, CLASS_NUM_ENTITY);
	}

	void __cdecl Scr_MakeArray()
	{
		auto Scr_MakeArray_f = (decltype(Scr_MakeArray)*)(0x00490910);
		Scr_MakeArray_f();
	}

	void __cdecl Scr_AddArray()
	{
		auto Scr_AddArray_f = (decltype(Scr_AddArray)*)(0x00490960);
		Scr_AddArray_f();
	}

	void __cdecl Scr_AddArrayStringIndexed(int32_t str_index)
	{
		auto Scr_AddArrayStringIndexed_f = (decltype(Scr_AddArrayStringIndexed)*)(0x004909e0);
		Scr_AddArrayStringIndexed_f(str_index);
	}
}