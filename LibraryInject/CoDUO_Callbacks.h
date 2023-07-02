#pragma once
#include <string>
#include <unordered_map>
#include "coduo_structs.h"

namespace CoDUO::Gsc
{
	void Scr_StringToCmd();

	void Scr_GetWeaponInfo();
	void Scr_GetWeaponIndexByName();

	void Scr_String_ToUpper();
	void Scr_String_ToLower();
	void Scr_String_StartsWith();
	void Scr_String_EndsWith();
	void Scr_String_Contains();
	void Scr_String_Replace();
	void Scr_String_Split();

	void Scr_LuaDoFile();
	void Scr_LuaDoString();

	inline std::unordered_map<std::string, gsc_function_t> gsc_functions
	{
		gsc_function("console", Scr_StringToCmd),

		gsc_function("getweapondef", Scr_GetWeaponInfo),
		gsc_function("getweaponindexbyname", Scr_GetWeaponIndexByName),

		gsc_function("string_toupper", Scr_String_ToUpper),
		gsc_function("string_tolower", Scr_String_ToLower),
		gsc_function("string_startswith", Scr_String_StartsWith),
		gsc_function("string_endswith", Scr_String_EndsWith),
		gsc_function("string_contains", Scr_String_Contains),
		gsc_function("string_replace", Scr_String_Replace),
		gsc_function("string_split", Scr_String_Split),

		gsc_function("lua_dofile", Scr_LuaDoFile),
		gsc_function("lua_dostring", Scr_LuaDoString),
	};
}

namespace CoDUO::Gsc
{
	void Scr_ForceRename(int param);
	void Scr_GetViewOrigin(int param);
	void Scr_GetViewAngles(int param);
	void Scr_GetParent(int param);
	void Scr_GetOwner(int param);
	void Scr_GetWeaponDefIndex(int param);
	void Scr_GetWeaponSlotInfo(int param);
	void Scr_SetWeaponAmmo(int param);

	inline std::unordered_map<std::string, gsc_function_t> gsc_methods
	{
		gsc_function("rename", Scr_ForceRename),

		gsc_function("getvieworigin", Scr_GetViewOrigin),
		gsc_function("getviewangles", Scr_GetViewAngles),

		gsc_function("getparent", Scr_GetParent),
		gsc_function("getowner", Scr_GetOwner),

		gsc_function("getweaponindex", Scr_GetWeaponDefIndex),
		gsc_function("getweaponslotinfo", Scr_GetWeaponSlotInfo),
		gsc_function("setweaponammo", Scr_SetWeaponAmmo),
	};
}