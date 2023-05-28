#include "CoDUO_Callbacks.h"
#include "CoDUO.h"

#include <format>

using namespace CoDUO;
namespace CoDUO::Gsc
{
	void Scr_StringToCmd()
	{
		const char* str = SL_ConvertToString( Scr_GetConstString(0) );		
		std::string cmd = std::format("{}\n", str);

		trap_SendConsoleCommand(EXEC_APPEND, cmd.c_str());
	}

	void Scr_GetViewAngles(int entId)
	{
		gentity_t* ent = &g_entities[entId];
		if (ent->client)
		{
			Scr_AddVector(&ent->client->viewangles);
			return;
		}

		Scr_AddUndefined();
	}
}