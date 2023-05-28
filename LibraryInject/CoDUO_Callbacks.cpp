#include "CoDUO_Callbacks.h"
#include "CoDUO.h"

using namespace CoDUO;
namespace CoDUO::Gsc
{
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