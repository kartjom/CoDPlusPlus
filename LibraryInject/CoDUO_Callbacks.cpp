#include "CoDUO_Callbacks.h"
#include "CoDUO.h"
#include "LuaState.h"

#include <format>

using namespace CoDUO;
namespace CoDUO::Gsc
{
	/* Functions */

	void Scr_StringToCmd()
	{
		const char* str = Scr_GetString(0);

		if (str)
		{
			size_t strSize = strlen(str) + 1;

			char* cmd = (char*)alloca(strSize + 1);
			memcpy(cmd, str, strSize);
			cmd[strSize] = '\n';

			trap_SendConsoleCommand(EXEC_APPEND, cmd);
		}
	}

	void Scr_GetWeaponInfo()
	{
		int index = Scr_GetInt(0);
		weapondef_t* weapon = G_GetWeaponDef(index);

		if (weapon && weapon->name)
		{
			Scr_MakeArray();

			Scr_AddInt(weapon->number);
			Scr_AddArrayStringIndexed(G_NewString("number"));		

			Scr_AddString(weapon->name);
			Scr_AddArrayStringIndexed(G_NewString("name"));

			Scr_AddInt(weapon->clipSize);
			Scr_AddArrayStringIndexed(G_NewString("clipSize"));
			
			Scr_AddInt(weapon->maxAmmo);
			Scr_AddArrayStringIndexed(G_NewString("maxAmmo"));
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_LuaDoFile()
	{
		const char* fileName = Scr_GetString(0);

		if (fileName)
		{
			luaL_dofile(LuaState::Lua(), fileName);
		}
	}

	void Scr_LuaDoString()
	{
		const char* str = Scr_GetString(0);

		if (str)
		{
			luaL_dostring(LuaState::Lua(), str);
		}
	}

	/* Methods */

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
			weaponslot_t slot = G_GetWeaponSlotInfo(ent, index);

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
}