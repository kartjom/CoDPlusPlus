#include <Engine/CoDUO.h>

namespace CoDUO
{
	void G_FreeEntity(gentity_t* ent)
	{
		_asm
		{
			push ent
			mov eax, uo_game_mp_x86
			add eax, 0x00053130
			call eax

			add esp, 0x4
		}
	}

	void G_DeleteEntity(gentity_t* ent)
	{
		if (!ent || ent->client || !ent->inuse) return;

		ent->think = G_FreeEntity;
		ent->nextthink = level->time + 100;
	}
}