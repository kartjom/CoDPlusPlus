#include "CoDUO_Callbacks.h"
#include "CoDUO.h"

#include <format>

using namespace CoDUO;
namespace CoDUO::Gsc
{
	void Scr_Sandbox()
	{
		Scr_MakeArray();

		Scr_AddInt(69);
		Scr_AddArrayStringIndexed(G_NewString("health"));

		Scr_AddString("kartjom");
		Scr_AddArrayStringIndexed(G_NewString("name"));
	}

	void Scr_StringToCmd()
	{
		const char* str = Scr_GetString(0);

		if (str)
		{
			std::string cmd = std::format("{}\n", str);
			trap_SendConsoleCommand(EXEC_APPEND, cmd.c_str());
		}
	}

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
			Vector3 vieworigin;
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
}