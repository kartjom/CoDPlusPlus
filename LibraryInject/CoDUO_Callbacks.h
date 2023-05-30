#pragma once
#include <string>
#include <unordered_map>
#include "coduo_structs.h"

namespace CoDUO::Gsc
{
	void Scr_StringToCmd();

	void Scr_ForceRename(int param);
	void Scr_GetViewAngles(int param);
	void Scr_GetViewOrigin(int param);

	inline std::unordered_map<std::string, gsc_function_t> gsc_functions
	{
		gsc_function("console", Scr_StringToCmd)
	};

	inline std::unordered_map<std::string, gsc_function_t> gsc_methods
	{
		gsc_function("rename", Scr_ForceRename),
		gsc_function("getviewangles", Scr_GetViewAngles),
		gsc_function("getvieworigin", Scr_GetViewOrigin),
	};
}