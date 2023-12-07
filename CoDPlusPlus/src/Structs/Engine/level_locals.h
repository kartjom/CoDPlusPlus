#pragma once
#include "stdint.h"

struct level_locals_t
{
	struct gclient_t* clients; //0x0000
	struct gentity_t* gentities; //0x0004
	int32_t gentitySize; //0x0008
	int32_t num_entities; //0x000C
	char pad_0010[468]; //0x0010
	int32_t maxclients; //0x01E4
	int32_t framenum; //0x01E8
	int32_t time; //0x01EC
	int32_t previousTime; //0x01F0
};