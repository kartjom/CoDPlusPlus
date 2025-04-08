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
	char pad_01F4[36]; //0x01F4
	int32_t numConnectedClients; //0x0218
	int32_t sortedClients[64]; //0x021C
	char voteString[1024]; //0x031C
	char voteDisplayString[1024]; //0x071C
	int32_t voteTime; //0x0B1C
	char pad_0B20[4]; //0x0B20
	int32_t voteYes; //0x0B24
	int32_t voteNo; //0x0B28
	int32_t numVotingClients; //0x0B2C
};