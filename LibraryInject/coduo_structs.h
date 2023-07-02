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
	int32_t viewheight_i; //0x00F4
	float viewheight_f; //0x00F8
	char pad_00FC[4]; //0x00FC
	int32_t viewheight_i_2; //0x0100
	int32_t stance_change_dir; //0x0104
	char pad_0108[28]; //0x0108
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

struct weapondef_t
{
	int32_t number; //0x0000
	char* name; //0x0004
	char* displayName; //0x0008
	char* AIOverlayDescription; //0x000C
	char* gunModel; //0x0010
	char* handModel; //0x0014
	char pad_0018[4]; //0x0018
	char* idleAnim; //0x001C
	char* emptyIdleAnim; //0x0020
	char* fireAnim; //0x0024
	char* holdFireAnim; //0x0028
	char* lastShotAnim; //0x002C
	char pad_0030[4]; //0x0030
	char* meleeAnim; //0x0034
	char* reloadAnim; //0x0038
	char* reloadEmptyAnim; //0x003C
	char pad_0040[8]; //0x0040
	char* raiseAnim; //0x0048
	char* dropAnim; //0x004C
	char* altRaiseAnim; //0x0050
	char* altDropAnim; //0x0054
	char* adsFireAnim; //0x0058
	char* adsLastShotAnim; //0x005C
	char pad_0060[16]; //0x0060
	char* adsUpAnim; //0x0070
	char* adsDownAnim; //0x0074
	char* modeName; //0x0078
	char pad_007C[24]; //0x007C
	char* viewFlashEffect; //0x0094
	char* worldFlashEffect; //0x0098
	char* pickupSound; //0x009C
	char pad_00A0[8]; //0x00A0
	char* pullbackSound; //0x00A8
	char* fireSound; //0x00AC
	char pad_00B0[20]; //0x00B0
	char* reloadSound; //0x00C4
	char* reloadEmptySound; //0x00C8
	char pad_00CC[12]; //0x00CC
	char* altSwitchSound; //0x00D8
	char pad_00DC[36]; //0x00DC
	char* shellEjectEffect; //0x0100
	char pad_0104[4]; //0x0104
	char* reticle; //0x0108
	char* reticleSide; //0x010C
	char pad_0110[180]; //0x0110
	char* radiantName; //0x01C4
	char* worldModel; //0x01C8
	char pad_01CC[8]; //0x01CC
	char* hudIcon; //0x01D4
	char* modeIcon; //0x01D8
	char* ammoIcon; //0x01DC
	int32_t startAmmo; //0x01E0
	char pad_01E4[12]; //0x01E4
	int32_t clientIndex; //0x01F0
	int32_t maxAmmo; //0x01F4
	int32_t clipSize; //0x01F8
	char* sharedAmmoCapName; //0x01FC
	char pad_0200[8]; //0x0200
	int32_t damage; //0x0208
	char pad_020C[8]; //0x020C
	int32_t damageInnerRadius; //0x0214
	int32_t damageOuterRadius; //0x0218
	char pad_021C[76]; //0x021C
	int32_t fuseTime; //0x0268
	char pad_026C[228]; //0x026C
	char* killIcon; //0x0350
	char pad_0354[20]; //0x0354
	char* altWeapon; //0x0368
	char pad_036C[12]; //0x036C
	int32_t explosionRadius; //0x0378
	int32_t explosionInnerDamage; //0x037C
	int32_t explosionOuterDamage; //0x0380
	char pad_0384[8]; //0x0384
	char* projectileModel; //0x038C
};

struct weaponslot_t
{
	weapondef_t* weapondef;
	int32_t clip;
	int32_t reserve;
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

struct cplane_t {
	vec3_t normal;
	float dist;
	unsigned char type;              // for fast side tests: 0,1,2 = axial, 3 = nonaxial
	unsigned char signbits;          // signx + (signy<<1) + (signz<<2), used as lookup during collision
	char pad[2];
};

struct trace_t {
	qboolean allsolid;      // if true, plane is not valid
	qboolean startsolid;    // if true, the initial point was in a solid area
	float fraction;         // time completed, 1.0 = didn't hit anything
	vec3_t endpos;          // final position
	cplane_t plane;         // surface normal at impact, transformed to world space
	int surfaceFlags;       // surface hit
	int contents;           // contents on other side of surface hit
	int entityNum;          // entity the contacted sirface is a part of
};

struct refdef_t
{
	int x, y, width, height;
	float fov_x, fov_y;
	vec3_t vieworg;
	vec3_t viewaxis[3];        // transformation matrix
};

// GSC
#define gsc_register(name, callback) { name, { name, callback, 0 } }
struct gsc_function_t
{
	const char* name;
	void* callback;
	int developer;
};