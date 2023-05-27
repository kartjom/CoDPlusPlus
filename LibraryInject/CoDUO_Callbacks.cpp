#include "CoDUO_Callbacks.h"
#include "CoDUO.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace CoDUO;
namespace CoDUO::Gsc
{
	void Scr_GetCurrentDate()
	{
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		std::ostringstream oss;
		oss << std::put_time(&tm, "%d.%m.%Y %H:%M:%S");
		auto str = oss.str();

		Scr_AddString(str.c_str());
	}

	void Scr_GetViewAngles(int entId)
	{
		gentity_t* entity = &g_entities[entId];

		if (entity->client)
		{
			Scr_AddVector(&entity->client->viewangles);
			return;
		}

		Scr_AddUndefined();
	}
}