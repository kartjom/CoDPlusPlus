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
		gsc_register("console", Scr_StringToCmd),

		gsc_register("getweapondef", Scr_GetWeaponInfo),
		gsc_register("getweaponindexbyname", Scr_GetWeaponIndexByName),

		gsc_register("string_toupper", Scr_String_ToUpper),
		gsc_register("string_tolower", Scr_String_ToLower),
		gsc_register("string_startswith", Scr_String_StartsWith),
		gsc_register("string_endswith", Scr_String_EndsWith),
		gsc_register("string_contains", Scr_String_Contains),
		gsc_register("string_replace", Scr_String_Replace),
		gsc_register("string_split", Scr_String_Split),

		gsc_register("lua_dofile", Scr_LuaDoFile),
		gsc_register("lua_dostring", Scr_LuaDoString),
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
		gsc_register("rename", Scr_ForceRename),

		gsc_register("getvieworigin", Scr_GetViewOrigin),
		gsc_register("getviewangles", Scr_GetViewAngles),

		gsc_register("getparent", Scr_GetParent),
		gsc_register("getowner", Scr_GetOwner),

		gsc_register("getweaponindex", Scr_GetWeaponDefIndex),
		gsc_register("getweaponslotinfo", Scr_GetWeaponSlotInfo),
		gsc_register("setweaponammo", Scr_SetWeaponAmmo),
	};
}