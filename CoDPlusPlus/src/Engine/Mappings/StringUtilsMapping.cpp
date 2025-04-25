#include <Engine/CoDUO.h>

namespace CoDUO
{
	char* CopyString(const char* input)
	{
		_asm
		{
			mov ebx, input

			mov eax, 0x00435560
			call eax
		}
	}

	int32_t G_NewString(const char* string)
	{
		_asm
		{
			mov edi, string

			mov eax, uo_game_mp_x86
			add eax, 0x0004dbc0
			call eax
		}
	}

	const char* G_EntityTypeString(entityType_t eType)
	{
		switch (eType)
		{
			case ET_GENERAL: return "ET_GENERAL";
			case ET_PLAYER: return "ET_PLAYER";
			case ET_PLAYER_CORPSE: return "ET_PLAYER_CORPSE";
			case ET_ITEM: return "ET_ITEM";
			case ET_MISSILE: return "ET_MISSILE";
			case ET_MOVER: return "ET_MOVER";
			case ET_PORTAL: return "ET_PORTAL";
			case ET_INVISIBLE: return "ET_INVISIBLE";
			case ET_SCRIPTMOVER: return "ET_SCRIPTMOVER";
			case ET_SOUND_BLEND: return "ET_SOUND_BLEND";
			case ET_LOOP_FX: return "ET_LOOP_FX";
			case ET_TURRET: return "ET_TURRET";
			case ET_VEHICLE: return "ET_VEHICLE";
			case ET_VEHICLE_COLLMAP: return "ET_VEHICLE_COLLMAP";
		}

		if (eType >= ET_EVENTS) return "ET_EVENTS";

		return "ET_UNKNOWN";
	}

	uint32_t __cdecl SL_GetString(const char* value, int32_t user)
	{
		auto SL_GetString_f = (decltype(SL_GetString)*)(0x004828e0);
		return SL_GetString_f(value, user);
	}

	const char* __cdecl SL_ConvertToString(uint32_t index)
	{
		auto SL_ConvertToString_f = (decltype(SL_ConvertToString)*)(0x00482290);
		return SL_ConvertToString_f(index);
	}

	void SL_RemoveRefToString(uint16_t index)
	{
		_asm
		{
			xor eax, eax
			mov ax, index

			mov ecx, 0x00482b00
			call ecx
		}
	}

	void __cdecl Z_Free(void* mem)
	{
		auto Z_Free_f = (decltype(Z_Free)*)(0x0056d9cf);
		Z_Free_f(mem);
	}

	const char* Info_ValueForKey(const char* buffer, const char* key)
	{
		_asm
		{
			mov ebx, key
			mov ecx, buffer

			mov eax, uo_game_mp_x86
			add eax, 0x00058180
			call eax
		}
	}

	void Info_SetValueForKey(char* buffer, const char* key, const char* value)
	{
		_asm
		{
			push value
			mov ecx, key
			push buffer

			mov eax, uo_game_mp_x86
			add eax, 0x00058560
			call eax

			add esp, 0x8
		}
	}
}