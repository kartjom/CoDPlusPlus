#pragma once
struct Vector3 {
	float x, y, z;

	operator float* ()
	{
		return &x;
	}
};

class gentity_t
{
public:
	char pad_0000[16]; //0x0000
	unsigned int time; //0x0010
	char pad_0014[4]; //0x0014
	Vector3 origin; //0x0018
	char pad_0024[28]; //0x0024
	Vector3 viewangles; //0x0040
	char pad_004C[768]; //0x004C
};