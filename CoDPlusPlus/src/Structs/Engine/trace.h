#pragma once
#include <Structs/vec3_t.h>

#define MASK_SHOT 0x2802031

#define trace_GetSurfaceType(type) syscall(0x44, type >> 0x14 & 0x1f)
struct trace_t
{
	float fraction;
	vec3_t endpos;
	vec3_t normal;
	int type;
	int contents;
	char* textureName;
	short entityNum;
	int surfaceFlags;
};