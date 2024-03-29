#pragma once
#include "stdint.h"
#include <Structs/vec3_t.h>

#define GENTITY_SIZE 0x34C // 844
#define GENTITY_COUNT 1022
#define WORLDSPAWN 1022
#define NO_GENTITY 1023

typedef enum {
	ET_GENERAL = 0,
	ET_PLAYER = 1,
	ET_PLAYER_CORPSE = 2,
	ET_ITEM = 3,
	ET_MISSLE = 4,
	ET_MOVER = 5,
	ET_PORTAL = 6,
	ET_INVISIBLE = 7,
	ET_SCRIPTMOVER = 8,
	ET_SOUND_BLEND = 9,
	ET_LOOP_FX = 10,
	ET_TURRET = 11,
	ET_VEHICLE = 12,
	ET_VEHICLE_COLLMAP = 14,
	ET_EVENTS = 16,
} entityType_t;

struct gentity_t
{
	int32_t number; //0x0000
	int32_t eType; //0x0004
	char pad_0008[4]; //0x0008
	int32_t trType; //0x000C
	int32_t trTime; //0x0010
	int32_t trDuration; //0x0014
	vec3_t trBase; //0x0018
	vec3_t trDelta; //0x0024
	char pad_0030[12]; //0x0030
	vec3_t viewangles; //0x003C
	char pad_0048[132]; //0x0048
	int32_t weapon; //0x00CC
	char pad_00D0[40]; //0x00D0
	int32_t svFlags; //0x00F8
	char pad_00FC[64]; //0x00FC
	vec3_t currentOrigin; //0x013C
	char pad_0148[12]; //0x0148
	int32_t ownerNum; //0x0154
	int32_t r_eventTime; //0x0158
	char pad_015C[4]; //0x015C
	struct gclient_t* client; //0x0160
	char pad_0164[8]; //0x0164
	int32_t inuse; //0x016C
	char pad_0170[20]; //0x0170
	int32_t classname; //0x0184
	int32_t spawnflags; //0x0188
	int32_t flags; //0x018C
	int32_t eventTime; //0x0190
	int32_t freeAfterEvent; //0x0194
	char pad_0198[8]; //0x0198
	int32_t clipmask; //0x01A0
	uint32_t frames; //0x01A4
	struct gentity_t* parent; //0x01A8
	char pad_01AC[58]; //0x01AC
	int16_t targetname; //0x01E6
	char pad_01E8[36]; //0x01E8
	int32_t nextthink; //0x020C
	void (*think)(gentity_t* self);
	void (*reached)(gentity_t* self);
	void (*blocked)(gentity_t* self, gentity_t* other);
	void (*touch)(gentity_t* self, gentity_t* other, struct trace_t* trace);
	void (*use)(gentity_t* self, gentity_t* other, gentity_t* activator);
	void (*pain)(gentity_t* self, gentity_t* attacker, int damage, vec3_t point);
	void (*die)(gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int damage, int mod);
	char pad_022C[20]; //0x022C
	int32_t health; //0x0240
	char pad_0244[4]; //0x0244
	int32_t damage; //0x0248
	int32_t explosionInnerDamage; //0x024C
	int32_t explosionOuterDamage; //0x0250
	int32_t explosionRadius; //0x0254
	int32_t methodOfDeath; //0x0258
	int32_t splashMethodOfDeath; //0x025C
	char pad_0260[236]; //0x0260
}; //Size: 0x034C