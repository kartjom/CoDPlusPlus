#include <Engine/CoDUO.h>

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
}