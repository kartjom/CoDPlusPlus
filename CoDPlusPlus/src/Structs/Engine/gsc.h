#pragma once
#include <Engine/ScriptLayer/Gsc/ScrVar/VarType.h>
#include <Structs/vec3_t.h>

#define gsc_register(name, callback) { name, { name, callback, 0 } }

struct gsc_function_t
{
	const char* name;
	void* callback;
	int developer;
};

struct VariableValue
{
	union
	{
		int intValue;
		float floatValue;
		uint32_t stringValue;
		vec3_t* vectorValue;
		uint32_t pointerValue;
	};
	VarType type;
};