#pragma once

#define MAX_CVAR_VALUE_STRING 256
#define GAME_CVAR_TABLE_SIZE 132
#define MAX_CVARS 2048
#define MAX_STRING_CHARS 1024

#define CVAR_ARCHIVE        1		// set to cause it to be saved to vars.rc, used for system variables, not for player specific configurations
#define CVAR_USERINFO       2		// sent to server on connect or change
#define CVAR_SERVERINFO     4		// sent in response to front end requests
#define CVAR_SYSTEMINFO     8		// these cvars will be duplicated on all clients
#define CVAR_INIT           16		// don't allow change from console at all, but can be set from the command line
#define CVAR_LATCH          32		// will only change when C code next does a Cvar_Get(), so it can't be changed without proper initialization. Modified will be set, even though the value hasn't changed yet
#define CVAR_ROM            64		// display only, cannot be set by user at all
#define CVAR_USER_CREATED   128		// created by a set command
#define CVAR_TEMP           256		// can be set even when cheats are disabled, but is not archived
#define CVAR_CHEAT          512		// can not be changed if cheats are disabled
#define CVAR_NORESTART      1024    // do not clear when a cvar_restart is issued
#define CVAR_WOLFINFO       2048    // DHM - NERVE :: Like userinfo, but for wolf multiplayer info

enum cbufExec_t
{
	EXEC_NOW,           // don't return until completed, a VM should NEVER use this because some commands might cause the VM to be unloaded...
	EXEC_INSERT,        // insert at current position, but don't run yet
	EXEC_APPEND         // add to end of the command buffer (normal case)
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
	int modificationCount;      // for tracking changes
	int trackChange;			// track this variable, and announce if changed, boolean
	//int teamShader;			// track and if changed, update shader state, boolean
};

struct cvar_t
{
	char* name;
	char* string;
	char* resetString;			// cvar_restart will reset to this value
	char* latchedString;		// for CVAR_LATCH vars
	int flags;
	int modified;				// set each time the cvar is changed
	int modificationCount;		// incremented each time the cvar is changed
	float value;                // atof( string )
	int integer;                // atoi( string )
	cvar_t* next;
	cvar_t* hashNext;
};