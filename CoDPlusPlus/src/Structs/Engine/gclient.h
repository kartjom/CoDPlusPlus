#pragma once
#include "stdint.h"
#include <Structs/vec3_t.h>

enum team_t
{
	TEAM_NONE = 0,
	TEAM_AXIS = 1,
	TEAM_ALLIES = 2,
	TEAM_SPECTATOR = 3,
	TEAM_NUM_TEAMS = 4,
};

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
	char pad_0048[16]; //0x0048
	int32_t groundEntityNum; //0x0058
	char pad_005C[36]; //0x005C
	int32_t movementDir; //0x0080
	int32_t eFlags; //0x0084
	char pad_0088[76]; //0x0088
	int32_t clientNum; //0x00D4
	int32_t weapon; //0x00D8
	char pad_00DC[4]; //0x00DC
	float aimProgress; //0x00E0
	char pad_00E4[4]; //0x00E4
	vec3_t viewangles; //0x00E8
	int32_t viewheight_i; //0x00F4
	float viewheight_f; //0x00F8
	char pad_00FC[4]; //0x00FC
	int32_t viewheight_i_2; //0x0100
	int32_t stance_change_dir; //0x0104
	char pad_0108[28]; //0x0108
	int32_t maxHealth; //0x0124
	char pad_0128[1076]; //0x0128
	vec3_t mins; //0x055C
	vec3_t maxs; //0x0568
	int32_t viewheight_prone; //0x0574
	int32_t viewheight_crouched; //0x0578
	int32_t viewheight_standing; //0x057C
	char pad_0580[48]; //0x0580
	float stamina; //0x05B0
	char pad_05B4[16352]; //0x05B4
	int32_t voteCount; //0x4594
	int32_t teamVoteCount; //0x4598
	int32_t complaints; //0x459C
	int32_t complaintClient; //0x45A0
	int32_t complaintEndTime; //0x45A4
	char pad_45A8[20]; //0x45A8
	team_t sessionTeam; //0x45BC
	char pad_45C0[52]; //0x45C0
	char name[32]; //0x45F4
	char pad_4614[8]; //0x4614
	int32_t noclip; //0x461C
	char pad_4620[8]; //0x4620
	int32_t lastCmdTime; //0x4628
	int32_t buttons; //0x462C
	int32_t oldbuttons; //0x4630
	char pad_4634[28]; //0x4634
	vec3_t viewangles2; //0x4650
	char pad_465C[28]; //0x465C
	int32_t inactivityTime; //0x4678
	char pad_467C[4]; //0x467C
	int32_t inactivityWarning; //0x4680
	char pad_4684[176]; //0x4684
}; //Size: 0x4734