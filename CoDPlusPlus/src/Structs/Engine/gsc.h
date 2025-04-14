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


// 0x005ca6f8 - all var type strings
enum class VarType
{
	Undefined			= 0,
	String				= 1,
	LocalizedString		= 2,
	Vector				= 3,
	Float				= 4,
	Integer				= 5,
	Codepos				= 6,
	Object				= 7,
	KeyValue			= 8,
	Function			= 9,
	Stack				= 10,
	Animation			= 11,
	Thread				= 12,
	Entity				= 13,
	Struct				= 14,
	Array				= 15,
	DeadThread			= 16,
	DeadEntity			= 17,
	DeadObject			= 18,
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