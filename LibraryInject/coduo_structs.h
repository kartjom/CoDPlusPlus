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
	int32_t time; //0x0010
	char pad_0014[4]; //0x0014
	Vector3 origin; //0x0018
	char pad_0024[24]; //0x0024
	Vector3 viewangles; //0x003C
	char pad_0048[797]; //0x0048
};