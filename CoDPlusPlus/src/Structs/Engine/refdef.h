#pragma once
#include <Structs/vec3_t.h>

struct refdef_t
{
	int x, y, width, height;
	float fov_x, fov_y;
	vec3_t vieworg;
	vec3_t viewaxis[3];        // transformation matrix
};