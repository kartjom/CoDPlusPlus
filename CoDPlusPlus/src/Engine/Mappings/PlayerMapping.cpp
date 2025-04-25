#include <Engine/CoDUO.h>
#include <format>

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

	void SV_GetUserinfo(int index, char* buffer, int bufferSize)
	{
		_asm
		{
			mov eax, index
			mov ebx, buffer
			mov edi, bufferSize

			mov esi, 0x0045f0d0
			call esi
		}
	}

	void SV_SetUserinfo(int index, const char* val)
	{
		_asm
		{
			mov eax, index
			push val

			mov esi, 0x0045f040
			call esi

			add esp, 0x4
		}
	}

	std::string NET_AdrToString(netadr_t adr)
	{
		switch (adr.type)
		{
		case NA_LOOPBACK:
			return "loopback";
		case NA_BOT:
			return "bot";
		case NA_IP:
			return std::format("{}.{}.{}.{}", adr.ip[0], adr.ip[1], adr.ip[2], adr.ip[3]);
		default:
			return std::format("{:02x}{:02x}{:02x}{:02x}.{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
				adr.ipx[0], adr.ipx[1], adr.ipx[2], adr.ipx[3], adr.ipx[4], adr.ipx[5], adr.ipx[6], adr.ipx[7], adr.ipx[8], adr.ipx[9]);
		}
	}

	void SV_GameSendServerCommand(int clientNum, int cmd_type, const char* text)
	{
		_asm
		{
			mov eax, text
			mov edx, cmd_type
			mov ecx, clientNum

			mov ebx, 0x0045c7e0
			call ebx
		}
	}

	void SV_GameDropClient(int clientNum, const char* reason)
	{
		_asm
		{
			mov eax, clientNum
			push reason

			mov esi, 0x0045c830
			call esi

			add esp, 0x4
		}
	}
}