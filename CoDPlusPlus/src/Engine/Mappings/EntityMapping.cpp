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

	void G_SetOrigin(gentity_t* ent, vec3_t origin)
	{
		VectorCopy(origin, ent->pos.trBase);

		ent->pos.trType = TR_STATIONARY;
		ent->pos.trTime = 0;
		ent->pos.trDuration = 0;
		VectorClear(ent->pos.trDelta);

		VectorCopy(origin, ent->currentOrigin);

		if (ent->client)
		{
			VectorCopy(origin, ent->client->origin);
		}
		else
		{
			SV_LinkEntity(ent);
		}
	}

	void SV_LinkEntity(gentity_t* ent)
	{
		_asm
		{
			mov edi, ent
			mov eax, 0x00467050
			call eax
		}
	}
}