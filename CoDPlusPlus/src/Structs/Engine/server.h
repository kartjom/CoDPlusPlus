#pragma once

enum netadrtype_t {
	NA_BOT,
	NA_BAD,                 // an address lookup failed
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
	NA_IPX,
	NA_BROADCAST_IPX
};

enum netsrc_t {
	NS_CLIENT,
	NS_SERVER
};

enum clientState_t {
	CS_FREE,        // can be reused for a new connection
	CS_ZOMBIE,      // client has been disconnected, but don't reuse connection for a couple seconds
	CS_CONNECTED,   // has been assigned to a client_t, but no gamestate yet
	CS_PRIMED,      // gamestate has been sent, but client hasn't sent a usercmd
	CS_ACTIVE       // client is fully in game
};

struct netadr_t {
	netadrtype_t type;

	unsigned char ip[4];
	unsigned char ipx[10];

	unsigned short port;
};

struct netchan_t
{
	netsrc_t sock; //0x0000
	int dropped; //0x0004
	netadr_t remoteAddress; //0x0008
	int qport; //0x001C
	int incomingSequence; //0x0020
	int outgoingSequence; //0x0024
	int fragmentSequence; //0x0028
	int fragmentLength; //0x002C
	char fragmentBuffer[32768]; //0x0030
	int unsentFragments; //0x8030
	int unsentLength; //0x8034
	char unsentBuffer[32768]; //0x8038
}; //Size: 0x10038

struct client_t
{
	clientState_t state; //0x0000
	char pad_0004[8]; //0x0004
	char userinfo[1024]; //0x000C
	char pad_040C[67124]; //0x040C
	void* gentity; //0x10A40
	char name[32]; //0x10A44
	char downloadName[64]; //0x10A64
	int download; //0x10AA4
	int downloadSize; //0x10AA8
	int downloadCount; //0x10AAC
	char N00032B1C[566672]; //0x10AB0
	netchan_t netchan; //0x9B040
	char pad_AB078[60]; //0xAB078
};

struct serverStatic_t {
	int initialized;                   // sv_init has completed
	int time;                          // will be strictly increasing across level changes
	int time2;
	int snapFlagServerBit;             // ^= SNAPFLAG_SERVERCOUNT every SV_SpawnServer()
	client_t* clients;                 // [sv_maxclients->integer];
};