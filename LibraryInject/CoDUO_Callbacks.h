#pragma once
#include <string>
#include <unordered_map>
#include "coduo_structs.h"

namespace CoDUO::Gsc
{
	void Scr_GetCurrentDate();
	void Scr_GetViewAngles(int);

	inline std::unordered_map<std::string, gsc_function_t> gsc_functions
	{
		gsc_function("getdate", Scr_GetCurrentDate)
	};

	inline std::unordered_map<std::string, gsc_function_t> gsc_methods
	{
		gsc_function("getviewangles", Scr_GetViewAngles)
	};
}