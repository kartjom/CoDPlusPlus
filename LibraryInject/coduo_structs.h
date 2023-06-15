#pragma once
#include "stdint.h"
#include "vec3_t.h"

typedef uint32_t qboolean;

#define GENTITY_SIZE 0x34C // 844
#define GENTITY_COUNT 1022
#define WORLDSPAWN 1022
#define NO_GENTITY 1023

#define MAX_CVAR_VALUE_STRING 256
#define GAME_CVAR_TABLE_SIZE 132
#define MAX_CVARS 2048
#define MAX_STRING_CHARS 1024

#define CVAR_ARCHIVE        1   // set to cause it to be saved to vars.rc
// used for system variables, not for player
// specific configurations
#define CVAR_USERINFO       2   // sent to server on connect or change
#define CVAR_SERVERINFO     4   // sent in response to front end requests
#define CVAR_SYSTEMINFO     8   // these cvars will be duplicated on all clients
#define CVAR_INIT           16  // don't allow change from console at all,
								// but can be set from the command line
#define CVAR_LATCH          32  // will only change when C code next does
								// a Cvar_Get(), so it can't be changed
								// without proper initialization.  modified
								// will be set, even though the value hasn't
								// changed yet
#define CVAR_ROM            64  // display only, cannot be set by user at all
#define CVAR_USER_CREATED   128 // created by a set command
#define CVAR_TEMP           256 // can be set even when cheats are disabled, but is not archived
#define CVAR_CHEAT          512 // can not be changed if cheats are disabled
#define CVAR_NORESTART      1024    // do not clear when a cvar_restart is issued
#define CVAR_WOLFINFO       2048    // DHM - NERVE :: Like userinfo, but for wolf multiplayer info

enum cbufExec_t
{
	EXEC_NOW,           // don't return until completed, a VM should NEVER use this because some commands might cause the VM to be unloaded...
	EXEC_INSERT,        // insert at current position, but don't run yet
	EXEC_APPEND         // add to end of the command buffer (normal case)
};

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
	char pad_0158[8]; //0x0158
	class gclient_t* client; //0x0160
	char pad_0164[32]; //0x0164
	int32_t classname; //0x0184
	int32_t spawnflags; //0x0188
	int32_t flags; //0x018C
	char pad_0190[16]; //0x0190
	int32_t clipmask; //0x01A0
	uint32_t frames; //0x01A4
	class gentity_t* parent; //0x01A8
	char pad_01AC[58]; //0x01AC
	int16_t targetname; //0x01E6
	char pad_01E8[36]; //0x01E8
	int32_t nextthink; //0x020C
	void* think; //0x0210
	char pad_0214[44]; //0x0214
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

struct gclient_t
{
	int32_t commandTime; //0x0000
	int32_t pm_type; //0x0004
	char pad_0008[12]; //0x0008
	vec3_t origin; //0x0014
	vec3_t velocity; //0x0020
	char pad_002C[8]; //0x002C
	int32_t grenadeTimeLeft; //0x0034
	char pad_0038[12]; //0x0038
	float leanf; //0x0044
	char pad_0048[144]; //0x0048
	int32_t weapon; //0x00D8
	char pad_00DC[12]; //0x00DC
	vec3_t viewangles; //0x00E8
	char pad_00F4[48]; //0x00F4
	int32_t maxhealth; //0x0124
	char pad_0128[1076]; //0x0128
	vec3_t mins; //0x055C
	vec3_t maxs; //0x0568
	int32_t viewheight_prone; //0x0574
	int32_t viewheight_crouched; //0x0578
	int32_t viewheight_standing; //0x057C
	char pad_0580[48]; //0x0580
	float stamina; //0x05B0
	char pad_05B4[16448]; //0x05B4
	char name[32]; //0x45F4
	char pad_4614[8]; //0x4614
	int32_t noclip; //0x461C
};

struct vmCvar_t
{
	int handle;
	int modificationCount;
	float value;
	int integer;
	char string[MAX_CVAR_VALUE_STRING];
};

struct cvarTable_t
{
	vmCvar_t* vmCvar;
	char* cvarName;
	char* defaultString;
	int cvarFlags;
	int modificationCount;          // for tracking changes
	int trackChange;           // track this variable, and announce if changed, boolean
	//int teamShader;      // track and if changed, update shader state, boolean
};

struct cvar_t
{
	char* name;
	char* string;
	char* resetString;       // cvar_restart will reset to this value
	char* latchedString;     // for CVAR_LATCH vars
	int flags;
	int modified;              // set each time the cvar is changed
	int modificationCount;          // incremented each time the cvar is changed
	float value;                    // atof( string )
	int integer;                    // atoi( string )
	cvar_t* next;
	cvar_t* hashNext;
};

struct refdef_t
{
	int x, y, width, height;
	float fov_x, fov_y;
	vec3_t vieworg;
	vec3_t viewaxis[3];        // transformation matrix
};

// GSC
#define gsc_function(name, callback) { name, { name, callback, 0 } }
struct gsc_function_t
{
	const char* name;
	void* callback;
	int developer;
};