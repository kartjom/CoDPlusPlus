#pragma once
#include <string>
#include <unordered_map>
#include "coduo_structs.h"

namespace CoDUO::Gsc
{
	inline struct Callbacks {
		uint32_t OnPlayerShoot = 0; /* ePlayer */
		uint32_t OnPlayerMelee = 0; /* ePlayer, eTarget (optional) */
		uint32_t OnPlayerSay = 0; /* ePlayer, sText, iMode */
		uint32_t OnProjectileBounce = 0; /* eProjectile */
		uint32_t OnProjectileExplode = 0; /* eProjectile */
	} CodeCallback;
}

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
		gsc_register("console", Scr_StringToCmd), /* string */

		gsc_register("getweapondef", Scr_GetWeaponInfo), /* weaponIndex */
		gsc_register("getweaponindexbyname", Scr_GetWeaponIndexByName), /* weaponName */

		gsc_register("string_toupper", Scr_String_ToUpper), /* string */
		gsc_register("string_tolower", Scr_String_ToLower), /* string */
		gsc_register("string_startswith", Scr_String_StartsWith), /* string, substr */
		gsc_register("string_endswith", Scr_String_EndsWith), /* string, substr */
		gsc_register("string_contains", Scr_String_Contains), /* string, substr */
		gsc_register("string_replace", Scr_String_Replace), /* string, from, to */
		gsc_register("string_split", Scr_String_Split), /* string, delimiter */

		gsc_register("lua_dofile", Scr_LuaDoFile), /* fileName */
		gsc_register("lua_dostring", Scr_LuaDoString), /* string */
	};
}

namespace CoDUO::Gsc
{
	void Scr_ForceRename(int param);

	void Scr_GetViewOrigin(int param);
	void Scr_GetViewAngles(int param);
	void Scr_GetViewDirection(int param);
	void Scr_GetEyeTrace(int param);

	void Scr_GetParent(int param);
	void Scr_GetOwner(int param);

	void Scr_GetWeaponDefIndex(int param);
	void Scr_GetWeaponSlotInfo(int param);
	void Scr_SetWeaponAmmo(int param);

	inline std::unordered_map<std::string, gsc_function_t> gsc_methods
	{
		gsc_register("rename", Scr_ForceRename), /* string */

		gsc_register("getvieworigin", Scr_GetViewOrigin),
		gsc_register("getviewangles", Scr_GetViewAngles),
		gsc_register("getviewdirection", Scr_GetViewDirection),
		gsc_register("geteyetrace", Scr_GetEyeTrace),

		gsc_register("getparent", Scr_GetParent),
		gsc_register("getowner", Scr_GetOwner),

		gsc_register("getweaponindex", Scr_GetWeaponDefIndex),
		gsc_register("getweaponslotinfo", Scr_GetWeaponSlotInfo), /* weaponIndex */
		gsc_register("setweaponammo", Scr_SetWeaponAmmo), /* weaponIndex, clip, reserve */
	};
}