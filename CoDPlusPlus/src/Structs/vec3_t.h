#pragma once
#include <glm/vec3.hpp>

struct vec3_t
{
	float x, y, z;

	operator float* ()
	{
		return &x;
	}

	operator glm::vec3()
	{
		return glm::vec3(x, y, z);
	}

	vec3_t operator-(const vec3_t& rhs) const
	{
		return vec3_t(x - rhs.x, y - rhs.y, z - rhs.z);
	}
	vec3_t operator+(const vec3_t& rhs) const
	{
		return vec3_t(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	vec3_t operator*(const float& rhs) const
	{
		return vec3_t(x * rhs, y * rhs, z * rhs);
	}
	vec3_t operator/(const float& rhs) const
	{
		return vec3_t(x / rhs, y / rhs, z / rhs);
	}
};