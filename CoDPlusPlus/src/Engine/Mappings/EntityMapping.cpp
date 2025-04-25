#include <Engine/CoDUO.h>

namespace CoDUO
{
	void __cdecl G_FreeEntity(gentity_t* ent)
	{
		auto G_FreeEntity_f = (decltype(G_FreeEntity)*)(uo_game_mp_x86 + 0x00053130);
		G_FreeEntity_f(ent);
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