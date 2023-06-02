#pragma once
struct vec3_t {
	float x, y, z;

	operator float* ()
	{
		return &x;
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