#pragma once
#define GENTITY_SIZE 0x34C // 844
#define GENTITY_MAX 1021
#define WORLDSPAWN 1022

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
	int32_t number; //0x0000
	char pad_0004[12]; //0x0004
	int32_t time; //0x0010
	char pad_0014[4]; //0x0014
	Vector3 origin; //0x0018
	Vector3 velocity; //0x0024
	char pad_0030[12]; //0x0030
	Vector3 viewangles; //0x003C
	char pad_0048[280]; //0x0048
	class gclient_t* client; //0x0160
	char pad_0164[32]; //0x0164
	int16_t classname; //0x0184
	char pad_0186[30]; //0x0186
	uint32_t frames; //0x01A4
	char pad_01A8[62]; //0x01A8
	int16_t targetname; //0x01E6
	char pad_01E8[88]; //0x01E8
	int32_t health; //0x0240
	char pad_0244[264]; //0x0244
}; //Size: 0x034C

class gclient_t
{
public:
	int32_t commandTime; //0x0000
	int32_t pm_type; //0x0004
	char pad_0008[12]; //0x0008
	Vector3 origin; //0x0014
	Vector3 velocity; //0x0020
	char pad_002C[24]; //0x002C
	float leanf; //0x0044
	char pad_0048[144]; //0x0048
	int32_t weapon; //0x00D8
	char pad_00DC[12]; //0x00DC
	Vector3 viewangles; //0x00E8
	char pad_00F4[48]; //0x00F4
	int32_t maxhealth; //0x0124
	char pad_0128[17612]; //0x0128
	char name[32]; //0x45F4
	char pad_4614[8]; //0x4614
	int32_t noclip; //0x461C
};