#include "GscExtensions.h"
#include <Engine/CoDUO.h>

using namespace CoDUO;
namespace CoDUO::Gsc
{
	void Scr_ForceRename(int param)
	{
		const char* str = Scr_GetString(0);

		gentity_t* ent = &g_entities[param];
		if (str && ent && ent->client)
		{
			char buf[MAX_STRING_CHARS];
			trap_GetUserinfo(param, buf, sizeof(buf));

			Info_SetValueForKey(buf, "name", str);
			trap_SetUserinfo(param, buf);

			memcpy(ent->client->name, str, 32);
			ent->client->name[31] = '\0';
		}
	}

	void Scr_GetViewOrigin(int param)
	{
		gentity_t* ent = &g_entities[param];
		if (ent && ent->client)
		{
			vec3_t vieworigin;
			G_GetPlayerViewOrigin(ent, vieworigin);

			Scr_AddVector(vieworigin);
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_GetViewAngles(int param)
	{
		gentity_t* ent = &g_entities[param];
		if (ent && ent->client)
		{
			Scr_AddVector(ent->client->viewangles);
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_GetViewDirection(int param)
	{
		gentity_t* ent = &g_entities[param];
		if (ent && ent->client)
		{
			vec3_t dir = AnglesToDirection(ent->client->viewangles);
			Scr_AddVector(dir);
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_GetEyeTrace(int param)
	{
		gentity_t* ent = &g_entities[param];
		if (ent && ent->client)
		{
			float range = Scr_GetFloat(0);
			float spread = Scr_GetFloat(1);

			trace_t tr = G_GetEyeTrace(ent, range, spread);

			Scr_MakeArray();

			Scr_AddFloat(tr.fraction);
			Scr_AddArrayStringIndexed(G_NewString("fraction"));

			Scr_AddVector(tr.endpos);
			Scr_AddArrayStringIndexed(G_NewString("position"));

			if (tr.entityNum == 1022 || tr.entityNum == 1023)
			{
				Scr_AddUndefined();
			}
			else
			{
				Scr_AddEntityNum(tr.entityNum);
			}
			Scr_AddArrayStringIndexed(G_NewString("entity"));

			if (tr.fraction < 1.0f)
			{
				Scr_AddVector(tr.normal);
				Scr_AddArrayStringIndexed(G_NewString("normal"));

				char* surfaceType = trace_GetSurfaceType(tr.type);
				Scr_AddString(surfaceType);
				Scr_AddArrayStringIndexed(G_NewString("surfacetype"));
			}
			else
			{
				vec3_t normal{};
				Scr_AddVector(normal);
				Scr_AddArrayStringIndexed(G_NewString("normal"));

				Scr_AddString("default");
				Scr_AddArrayStringIndexed(G_NewString("surfacetype"));
			}
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_GetParent(int param)
	{
		gentity_t* ent = &g_entities[param];
		if (ent && ent->parent)
		{
			Scr_AddEntityNum(ent->parent->number);
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_GetOwner(int param)
	{
		gentity_t* ent = &g_entities[param];
		if (ent && ent->ownerNum < 1023)
		{
			gentity_t* owner = &g_entities[ent->ownerNum];
			Scr_AddEntityNum(owner->number);
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_GetWeaponDefIndex(int param)
	{
		gentity_t* ent = &g_entities[param];
		if (ent && ent->client)
		{
			Scr_AddInt(ent->client->weapon);
		}
		else
		{
			Scr_AddInt(ent->weapon);
		}
	}

	void Scr_GetWeaponSlotInfo(int param)
	{
		gentity_t* ent = &g_entities[param];
		int index = Scr_GetInt(0);

		if (ent && ent->client)
		{
			weaponslot_t slot = BG_GetWeaponSlotInfo(ent, index);

			if (slot.weapondef)
			{
				Scr_MakeArray();

				Scr_AddInt(slot.weapondef->number);
				Scr_AddArrayStringIndexed(G_NewString("number"));

				Scr_AddString(slot.weapondef->name);
				Scr_AddArrayStringIndexed(G_NewString("name"));

				Scr_AddInt(slot.clip);
				Scr_AddArrayStringIndexed(G_NewString("clip"));

				Scr_AddInt(slot.reserve);
				Scr_AddArrayStringIndexed(G_NewString("reserve"));
			}
			else
			{
				Scr_AddUndefined();
			}
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_SetWeaponAmmo(int param)
	{
		gentity_t* ent = &g_entities[param];

		int weaponIndex = Scr_GetInt(0);
		int clip = Scr_GetInt(1);
		int reserve = Scr_GetInt(2);

		if (ent && ent->client)
		{
			BG_SetPlayerSlotAmmo(ent, weaponIndex, clip, reserve);
		}
	}
}