#include "CoDUO.h"

namespace CoDUO
{
	int32_t BG_GetNumWeapons()
	{
		return bg_iNumWeapons != nullptr ? *bg_iNumWeapons : 0;
	}

	bool BG_IsWeaponIndexValid(int32_t index)
	{
		return uo_game_mp_x86 > 0 && index > 0 && index <= BG_GetNumWeapons();
	}

	int32_t BG_GetWeaponIndexForName(const char* name)
	{
		_asm
		{
			mov esi, name
			mov eax, uo_game_mp_x86
			add eax, 0x00011010
			call eax
		}
	}

	weapondef_t* BG_GetWeaponDef(int32_t index)
	{
		weapondef_t* weaponinfo = nullptr;

		if (BG_IsWeaponIndexValid(index))
		{
			uintptr_t ptr = *(uintptr_t*)(uo_game_mp_x86 + 0x0010ed40);
			weaponinfo = *(weapondef_t**)(ptr + index * 4);
		}

		return weaponinfo;
	}

	weaponslot_t BG_GetWeaponSlotInfo(gentity_t* player, int32_t weaponIndex)
	{
		weaponslot_t slot = {};

		if (BG_IsWeaponIndexValid(weaponIndex) && player->client != nullptr)
		{
			weapondef_t* def = BG_GetWeaponDef(weaponIndex);
			if (def->clientIndex > 0)
			{
				int clipAddr = (DWORD)(player->client) + (def->clientIndex * 4) + 0x334;
				int reserveAddr = (DWORD)(player->client) + (def->clientIndex * 4) + 0x134;

				slot.weapondef = def;
				slot.clip = *(DWORD*)clipAddr;
				slot.reserve = *(DWORD*)reserveAddr;
			}
		}

		return slot;
	}

	void BG_SetPlayerSlotAmmo(gentity_t* player, int32_t weaponIndex, int32_t clip, int32_t reserve)
	{
		if (clip < 0 && reserve < 0) return;

		if (BG_IsWeaponIndexValid(weaponIndex) && player->client != nullptr)
		{
			weapondef_t* def = BG_GetWeaponDef(weaponIndex);
			if (def->clientIndex > 0)
			{
				if (clip >= 0)
				{
					int clipAddr = (DWORD)(player->client) + (def->clientIndex * 4) + 0x334;
					*(DWORD*)clipAddr = clip;
				}

				if (reserve >= 0)
				{
					int reserveAddr = (DWORD)(player->client) + (def->clientIndex * 4) + 0x134;
					*(DWORD*)reserveAddr = reserve;
				}
			}
		}
	}
}