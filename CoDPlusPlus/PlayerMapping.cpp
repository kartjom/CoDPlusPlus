#include "CoDUO.h"

namespace CoDUO
{
	void G_GetPlayerViewOrigin(gentity_t* ent, float* destination)
	{
		_asm
		{
			mov esi, destination
			mov edi, ent
			mov eax, uo_game_mp_x86
			add eax, 0x000563b0
			call eax
		}
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

	void trap_GetUserinfo(int num, char* buffer, int bufferSize)
	{
		_asm
		{
			push bufferSize
			push buffer
			push num
			push 0x21
			call syscall

			add esp, 0x10
		}
	}

	void trap_SetUserinfo(int num, const char* buffer)
	{
		_asm
		{
			push buffer
			push num
			push 0x22
			call syscall

			add esp, 0xC
		}
	}
}