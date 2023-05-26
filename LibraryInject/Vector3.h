#pragma once
struct Vector3 {
	float x, y, z;

	operator float* ()
	{
		return &x;
	}

	Vector3 operator-(const Vector3& rhs) const
	{
		return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
	}
	Vector3 operator+(const Vector3& rhs) const
	{
		return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

    Vector3 operator*(const float& rhs) const
	{
        return Vector3(x * rhs, y * rhs, z * rhs);
    }
    Vector3 operator/(const float& rhs) const
	{
        return Vector3(x / rhs, y / rhs, z / rhs);
    }
};