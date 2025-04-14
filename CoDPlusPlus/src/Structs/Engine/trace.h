#pragma once
#include <Structs/vec3_t.h>

#define MASK_SHOT 0x2802031

struct trace_t
{
	float fraction;
	vec3_t endpos;
	vec3_t normal;
	int surfaceFlags;
	int contents;
	char* textureName;
	unsigned short entityNum;
	unsigned short partName;
	unsigned short partGroup;
	unsigned char allSolid; // not sure about it
	unsigned char startSolid;
};