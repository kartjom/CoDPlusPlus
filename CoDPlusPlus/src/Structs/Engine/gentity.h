#pragma once
#include "stdint.h"
#include <Structs/q_shared.h>
#include <Structs/vec3_t.h>

#define GENTITY_SIZE 0x34C // 844
#define GENTITY_COUNT 1022
#define WORLDSPAWN 1022
#define NO_GENTITY 1023

enum class_num_t
{
	CLASS_NUM_ENTITY,
	CLASS_NUM_HUDELEM,
	CLASS_NUM_VEHICLENODE,
};

enum entityType_t
{
	ET_GENERAL = 0,
	ET_PLAYER = 1,
	ET_PLAYER_CORPSE = 2,
	ET_ITEM = 3,
	ET_MISSILE = 4,
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
};

enum trType_t
{
	TR_STATIONARY,
	TR_INTERPOLATE,             // non-parametric, but interpolate between snapshots
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_LINEAR_STOP_BACK,        //----(SA)	added.  so reverse movement can be different than forward
	TR_SINE,                    // value = base + sin( time / duration ) * delta
	TR_GRAVITY,
	TR_GRAVITY_LOW,
	TR_GRAVITY_FLOAT,           // super low grav with no gravity acceleration (floating feathers/fabric/leaves/...)
	TR_GRAVITY_PAUSED,          //----(SA)	has stopped, but will still do a short trace to see if it should be switched back to TR_GRAVITY
	TR_ACCELERATE,
	TR_DECCELERATE
};

struct trajectory_t
{
	trType_t trType;
	int trTime;
	int trDuration;             // if non 0, trTime + trDuration = stop time
	vec3_t trBase;
	vec3_t trDelta;             // velocity, etc
};

struct gentity_t
{
	int32_t number; //0x0000
	entityType_t eType; //0x0004
	int32_t eFlags; //0x0008
	trajectory_t pos; //0x000C
	trajectory_t apos; //0x0030
	int32_t time; //0x0054
	int32_t time2; //0x0058
	vec3_t origin2; //0x005C
	vec3_t angles2; //0x0068
	int32_t otherEntityNum; //0x0074
	int32_t attackerEntityNum; //0x0078
	int32_t groundEntityNum; //0x007C
	char pad_0080[36]; //0x0080
	int32_t eventParm; //0x00A4
	char pad_00A8[36]; //0x00A8
	int32_t weapon; //0x00CC
	char pad_00D0[40]; //0x00D0
	int32_t svFlags; //0x00F8
	char pad_00FC[12]; //0x00FC
	vec3_t mins; //0x0108
	vec3_t maxs; //0x0114
	int32_t contents; //0x0120
	vec3_t absmin; //0x0124
	vec3_t absmax; //0x0130
	vec3_t currentOrigin; //0x013C
	vec3_t currentAngles; //0x0148
	int32_t ownerNum; //0x0154
	int32_t r_eventTime; //0x0158
	char pad_015C[4]; //0x015C
	struct gclient_t* client; //0x0160
	char pad_0164[8]; //0x0164
	int32_t inuse; //0x016C
	char pad_0170[12]; //0x0170
	char pad_017C; //0x017C
	byte takedamage; //0x017D
	byte active; //0x017E
	char pad_017F[5]; //0x017F
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
	void (*think)(gentity_t* self); //0x0210
	void (*reached)(gentity_t* self); //0x0214
	void (*blocked)(gentity_t* self, gentity_t* other); //0x0218
	void (*touch)(gentity_t* self, gentity_t* other, struct trace_t* trace); //0x021C
	void (*use)(gentity_t* self, gentity_t* other, gentity_t* activator); //0x0220
	void (*pain)(gentity_t* self, gentity_t* attacker, int damage, vec3_t point); //0x0224
	void (*die)(gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int damage, int mod); //0x0228
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