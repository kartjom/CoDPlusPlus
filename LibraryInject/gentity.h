#pragma once
#include <wtypes.h>
typedef enum {
	TR_STATIONARY,
	TR_INTERPOLATE,             // non-parametric, but interpolate between snapshots
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_LINEAR_STOP_BACK,        //----(SA)	added.  so reverse movement can be different than forward
	TR_SINE,                    // value = base + sin( time / duration ) * delta
	TR_GRAVITY,
	// Ridah
	TR_GRAVITY_LOW,
	TR_GRAVITY_FLOAT,           // super low grav with no gravity acceleration (floating feathers/fabric/leaves/...)
	TR_GRAVITY_PAUSED,          //----(SA)	has stopped, but will still do a short trace to see if it should be switched back to TR_GRAVITY
	TR_ACCELERATE,
	TR_DECCELERATE,
	// Gordon
	TR_SPLINE,
	TR_LINEAR_PATH
} trType_t;

typedef enum {
	ET_GENERAL, //0
	ET_PLAYER, //1
	ET_CORPSE, //2
	ET_ITEM, //3
	ET_MISSILE, //4
	ET_MOVER, //5
	ET_PORTAL, //6
	ET_INVISIBLE, //7
	ET_SCRIPTMOVER, //8
	ET_UNKNOWN, //9
	ET_FX, //10
	ET_TURRET, //11
	ET_EVENTS //12
} entityType_t;

typedef struct {
	char data[64]; //holds some values like origin of player before on turret, is being used flag, degrees of freedom (arcs)
} turret_entity_info; //size 0x40

typedef struct {
	float x, y, z;
} vec3_t;

typedef struct {
	trType_t trType;
	int trTime;
	int trDuration;             // if non 0, trTime + trDuration = stop time
	//----(SA)	removed
	vec3_t trBase;
	vec3_t trDelta;             // velocity, etc
	//----(SA)	removed
} trajectory_t;

typedef struct gentity_s gentity_t;
typedef struct gclient_s gclient_t;

typedef struct entityState_s {
	int number;
	entityType_t eType; //4
	int eFlags; //8
	trajectory_t pos; //12
	trajectory_t apos; //48
	int unk; //84 //time??
	int unk2; //88 //time2??
	vec3_t origin2; //92
	vec3_t angles2; //104 (guessed name)
	int otherEntityNum; //116
	int otherEntityNum2; //120
	int groundEntityNum; //124
	int constantLight; //128
	int loopSound; //132
	int surfaceFlags; //136
	int modelindex; //140
	int clientNum; //144
	char ___cba[0x34];
	/*
	gentity_t *teammaster; //152
	int eventParm; //160
	int eventSequence; //164
	int events[4]; //168
	int eventParms[4]; //184
	*/

	int weapon; //200
	int legsAnim; //204
	int torsoAnim; //208
	float leanf; //212
	int loopfxid; //216
	int hintstring; //220
	int animMovetype; //224
} entityState_t;

#pragma pack(push, 1)
typedef struct playerState_s
{
	int commandTime;
	int pm_type;
	int bobCycle;
	int pm_flags;
	int pm_time;
	vec3_t origin;
	vec3_t velocity;
	char gap_2C[20];
	float leanf;
	int speed;
	char gap_48[12];
	int groundEntityNum;
	char gap_58[12];
	int jumpTime;
	int field_68;
	int legsTime;
	int legsAnim;
	int torsoTime;
	int torsoAnim;
	int movementDir;
	int eFlags;
	char gap_84[24];
	int field_9C;
	int field_A0;
	int field_A4;
	int field_A8;
	int clientNum;
	int weapon;
	int field_B4;
	char gap_B8;
	char gap_B9;
	char gap_BA[2];
	int field_BC;
	vec3_t viewangles;
	char gap_CC[40];
	int health;
	char gap_F8[556];
	vec3_t mins;
	vec3_t maxs;
	float viewheight_prone;
	int viewheight_crouched;
	float viewheight_standing;
	int field_348;
	float runSpeedScale;
	float sprintSpeedScale;
	char gap_354[40];
	float friction;
	char gap_380[68];
	float fTorsoHeight;
	float fTorsoPitch;
	float fWaistPitch;
	char rest[7416];
	int end;
} playerState_t;
#pragma pack(pop)

typedef struct usercmd_s {
	int serverTime;
	BYTE buttons; //console,chat talking, aim down the sight, attackbutton, usebutton
	BYTE wbuttons; //lean right,left,reload
	BYTE weapon;
	BYTE flags;
	int angles[3];

	signed char forwardmove, rightmove, upmove;
	BYTE unknown; //could be doubleTap or client
} usercmd_t;

#pragma pack(push, 1)
struct gclient_s
{
	playerState_t ps;
	int ab;
	int cb;
	int spectatorClient;
	int idklell;
	int archivetime;
	char gap_20E0[16];
	usercmd_t cmd;
	char gap_2109[204];
	int forceSpectatorClient;
	char gap_21D8[4];
	int noclip;
	char bs;
	char gap_21E1[43];
	int field_220C;
	int field_2210;
	char gap_2214[8140];
	int end;
};
#pragma pack(pop)


typedef enum {
	PERK_QUICK_RELOAD
} perk_names;

#define MAX_PERKS 16

typedef struct gitem_s {
	char* classname;
	//rest idc
} gitem_t;

struct gentity_s {
	entityState_t s;
	float PM_GetViewHeightLerpTime_unknown;
	float some_view_angle_when_prone;
	float some_view_angle_when_prone2;
	int linked;
	int svFlags;
	int singleClient;
	int unk2;
	float mins[3];
	float maxs[3];
	int contents;
	float absmin[3];
	float absmax[3];
	float currentOrigin[3];
	float currentAngles[3];
	int ownerNum;
	int r_eventTime;
	int unk3;
	gclient_t* client;
	int* turret_info;
	char inuse;
	char physicsObject;
	char sound1;
	char sound2;
	char sound2to1;
	char sound4;
	char soundLoop;
	char sound6;
	char sound3to2;
	char sound8;
	char sound9;
	char sound10;
	char soundSoftclose;
	char sound12;
	char noise_index;
	char idk;
	char idk2;
	char takedamage;
	char active;
	char idk3;
	char moverState;
	unsigned char modelindex2;
	unsigned short classname;
	int spawnflags;
	int flags;
	int eventTime;
	int freeAfterEvent;
	int unlinkAfterEvent;
	float physicsBounce;
	int clipmask;
	int framenum;
	gentity_t* parent;
	gentity_t* nextTrain;
	char rest_fill_for_ida;
	char gap_1A1[3];
	vec3_t pos1;
	vec3_t pos2;
	char gap_1BC[28];
	int team;
	char gap_1DC[4];
	float speed;
	float closespeed;
	vec3_t movedir;
	int gDuration;
	int gDurationBack;
	int nextthink;
	int think;
	int reached;
	int blocked;
	int touch;
	int use;
	int pain;
	int die;
	int field_21C;
	int controller;
	char gap_224;
	char gap_225[11];
	int health;
	int maxhealth;
	int damage;
	char gap_23C;
	char gap_23D[19];
	int accumulate;
	char gap_254[12];
	gentity_t* teamchain;
	gentity_t* teammaster;
	float wait;
	float random;
	char gap_270[4];
	float delay;
	char gap_278[32];
	gitem_t* item;
	int gap_29C;
	char gap_2A0[8];
	int threshold;
	int harc;
	char gap_2B0[4];
	int missionlevel;
	char gap_2B8[16];
	int spawnitem;
	char gap_2CC[24];
	int generlink_field_idk;
	char rest[44];
};
#pragma pack(push, 1)