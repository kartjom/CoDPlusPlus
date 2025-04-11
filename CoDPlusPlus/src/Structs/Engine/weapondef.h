#pragma once
#include "stdint.h"

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

struct weaponParams_t
{
	vec3_t forward;
	vec3_t right;
	vec3_t up;
	vec3_t muzzleTrace;
	vec3_t gunForward;
	weapondef_t* weaponDef;
};

struct weaponslot_t
{
	weapondef_t* weapondef;
	int32_t clip;
	int32_t reserve;
};