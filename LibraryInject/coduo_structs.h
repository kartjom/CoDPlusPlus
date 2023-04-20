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
	int32_t number;
	char pad_0000[12]; //0x0000
	int32_t time; //0x0010
	char pad_0014[4]; //0x0014
	Vector3 origin; //0x0018
	Vector3 velocity; //0x0024
	char pad_0030[12]; //0x0030
	Vector3 viewangles; //0x003C
	char pad_0048[813]; //0x0048
}; //Size: 0x0375