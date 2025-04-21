#pragma once
#include <stdint.h>

// 0x005ca6f8 - all var type names
enum class VarType : uint32_t
{
	Undefined = 0,
	String = 1,
	LocalizedString = 2,
	Vector = 3,
	Float = 4,
	Integer = 5,
	Codepos = 6,
	Object = 7,
	KeyValue = 8,
	Function = 9,
	Stack = 10,
	Animation = 11,
	Thread = 12,
	Entity = 13,
	Struct = 14,
	Array = 15,
	DeadThread = 16,
	DeadEntity = 17,
	DeadObject = 18,
};