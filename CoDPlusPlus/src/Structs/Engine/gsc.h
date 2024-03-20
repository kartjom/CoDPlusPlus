#pragma once
#include <cstdint>
#include <string>
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
	union {
		int Integer;
		float Float;
		struct vec3_t* Vector;
		uint32_t StringIndex;
	};

	uint32_t type;
};

enum class VarType
{
	Undefined = 0,
	String = 1,
	Vector = 3,
	Float = 4,
	Integer = 5,
	Entity = 7,
	Function = 9,
};

struct ScrVar
{
	VarType Type;

	std::string String;
	union
	{
		void* Address;
		int Integer;
		float Float;
		vec3_t Vector;
	};
};