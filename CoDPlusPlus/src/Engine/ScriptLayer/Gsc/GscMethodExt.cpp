#include "GscExtensions.h"
#include <Engine/CoDUO.h>
#include <limits>

using namespace CoDUO;
namespace CoDUO::Gsc
{
	void Scr_ForceRename(int entNum)
	{
		const char* str = Scr_GetString(0);

		gentity_t* ent = g_entities + entNum;
		if (str && ent && ent->client)
		{
			char buf[MAX_STRING_CHARS];
			trap_GetUserinfo(entNum, buf, sizeof(buf));

			Info_SetValueForKey(buf, "name", str);
			trap_SetUserinfo(entNum, buf);

			memcpy(ent->client->name, str, 32);
			ent->client->name[31] = '\0';
		}
	}

	void Scr_GetIP(int entNum)
	{
		gentity_t* ent = g_entities + entNum;
		client_t* client = svs->clients + entNum;
		if (ent && ent->client && client)
		{
			std::string ip = NET_AdrToString(client->netchan.remoteAddress);
			Scr_AddString(ip.c_str());
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_GetViewOrigin(int entNum)
	{
		gentity_t* ent = g_entities + entNum;
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

	void Scr_GetViewAngles(int entNum)
	{
		gentity_t* ent = g_entities + entNum;
		if (ent && ent->client)
		{
			Scr_AddVector(ent->client->viewangles);
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_GetViewDirection(int entNum)
	{
		gentity_t* ent = g_entities + entNum;
		if (ent && ent->client)
		{
			vec3_t viewangles(ent->client->viewangles2.x , ent->client->viewangles2.y , ent->client->viewangles.z);
			vec3_t viewdir;
			AngleVectors(viewangles, viewdir, NULL, NULL);

			Scr_AddVector(viewdir);
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_GetEyeTrace(int entNum)
	{
		gentity_t* ent = g_entities + entNum;
		if (ent && ent->client)
		{
			int numParams = Scr_GetNumParam();

			float range = numParams > 0 ? Scr_GetFloat(0) : std::numeric_limits<float>::lowest();
			float spread = numParams > 1 ? Scr_GetFloat(1) : std::numeric_limits<float>::lowest();

			trace_t tr = G_GetEyeTrace(ent, range, spread);

			Scr_MakeArray();

			Scr_AddFloat(tr.fraction);
			Scr_AddArrayStringIndexed(SL_GetString("fraction", 1));

			Scr_AddVector(tr.endpos);
			Scr_AddArrayStringIndexed(SL_GetString("position", 1));

			if (tr.entityNum == 1022 || tr.entityNum == 1023)
			{
				Scr_AddUndefined();
			}
			else
			{
				Scr_AddEntityNum(tr.entityNum);
			}
			Scr_AddArrayStringIndexed(SL_GetString("entity", 1));

			if (tr.fraction < 1.0f)
			{
				Scr_AddVector(tr.normal);
				Scr_AddArrayStringIndexed(SL_GetString("normal", 1));

				char* surfaceType = trace_GetSurfaceType(tr.type);
				Scr_AddString(surfaceType);
				Scr_AddArrayStringIndexed(SL_GetString("surfacetype", 1));
			}
			else
			{
				vec3_t normal;
				Scr_AddVector(normal);
				Scr_AddArrayStringIndexed(SL_GetString("normal", 1));

				Scr_AddString("default");
				Scr_AddArrayStringIndexed(SL_GetString("surfacetype", 1));
			}
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_GetParent(int entNum)
	{
		gentity_t* ent = g_entities + entNum;
		if (ent && ent->parent)
		{
			Scr_AddEntityNum(ent->parent->number);
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_GetOwner(int entNum)
	{
		gentity_t* ent = g_entities + entNum;
		if (ent && ent->ownerNum < 1023)
		{
			gentity_t* owner = g_entities + ent->ownerNum;
			Scr_AddEntityNum(owner->number);
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_GetWeaponDefIndex(int entNum)
	{
		gentity_t* ent = g_entities + entNum;
		if (ent && ent->client)
		{
			Scr_AddInt(ent->client->weapon);
		}
		else
		{
			Scr_AddInt(ent->weapon);
		}
	}

	void Scr_GetWeaponSlotInfo(int entNum)
	{
		gentity_t* ent = g_entities + entNum;
		int index = Scr_GetInt(0);

		if (ent && ent->client)
		{
			weaponslot_t slot = BG_GetWeaponSlotInfo(ent, index);

			if (slot.weapondef)
			{
				Scr_MakeArray();

				Scr_AddInt(slot.weapondef->number);
				Scr_AddArrayStringIndexed(SL_GetString("number", 1));

				Scr_AddString(slot.weapondef->name);
				Scr_AddArrayStringIndexed(SL_GetString("name", 1));

				Scr_AddInt(slot.clip);
				Scr_AddArrayStringIndexed(SL_GetString("clip", 1));

				Scr_AddInt(slot.reserve);
				Scr_AddArrayStringIndexed(SL_GetString("reserve", 1));
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

	void Scr_SetWeaponAmmo(int entNum)
	{
		gentity_t* ent = g_entities + entNum;

		int weaponIndex = Scr_GetInt(0);
		int clip = Scr_GetInt(1);
		int reserve = Scr_GetInt(2);

		if (ent && ent->client)
		{
			BG_SetPlayerSlotAmmo(ent, weaponIndex, clip, reserve);
		}
	}
}