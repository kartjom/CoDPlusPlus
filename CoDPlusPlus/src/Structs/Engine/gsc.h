#pragma once
#include <cstdint>

#define gsc_register(name, callback) { name, { name, callback, 0 } }

struct gsc_function_t
{
	const char* name;
	void* callback;
	int developer;
};

struct ScrVar {
	union {
		int Integer;
		float Float;
		struct vec3_t* Vector;
		uint32_t StringIndex;
	};

	const char* String();
};