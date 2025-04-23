#pragma once
#include <Engine/ScriptLayer/Gsc/ScrVar/ScrVar.h>
#include <unordered_map>

namespace CoDUO::Gsc
{
	inline struct Callbacks {
		uint32_t OnInitialize;			//	 bRestart

		uint32_t OnPlayerShoot;			//	 ePlayer
		uint32_t OnPlayerMelee;			//	 ePlayer, eTarget (optional)
		uint32_t OnPlayerSay;			//	 ePlayer, sText, iMode, bConsole
		uint32_t OnPlayerVote;			//	 ePlayer, bVote
		uint32_t OnPlayerInactivity;	//	 ePlayer
		uint32_t OnPlayerVoteCall;		//	 ePlayer, sVoteType, aVoteInfo

		uint32_t OnProjectileBounce;	//	 eProjectile
		uint32_t OnProjectileExplode;	//	 eProjectile
	} CodeCallback = {};

	inline ScrVar Scr_InternalReturnValue = {};
}

namespace CoDUO::Gsc
{
	void Scr_StringToCmd();
	void Scr_AddCommand();
	void Scr_AddClientCommand();

	void Scr_GetSystemTime();

	void Scr_GetWeaponInfo();
	void Scr_GetWeaponIndexByName();

	void Scr_Format();
	void Scr_String_ToUpper();
	void Scr_String_ToLower();
	void Scr_String_StartsWith();
	void Scr_String_EndsWith();
	void Scr_String_Contains();
	void Scr_String_Replace();
	void Scr_String_Split();

	void Scr_Await();
	void Scr_HttpGet();

	inline std::unordered_map<std::string, int32_t> gsc_commands;
	inline std::unordered_map<std::string, int32_t> gsc_clientcommands;

	inline std::unordered_map<std::string, gsc_function_t> gsc_functions
	{
		gsc_register("console", Scr_StringToCmd), /* string */
		gsc_register("addcommand", Scr_AddCommand), /* string, function */
		gsc_register("addclientcommand", Scr_AddClientCommand), /* string, function */
		gsc_register("getsystemtime", Scr_GetSystemTime), /* format? */

		gsc_register("getweapondef", Scr_GetWeaponInfo), /* weaponIndex */
		gsc_register("getweaponindexbyname", Scr_GetWeaponIndexByName), /* weaponName */

		gsc_register("format", Scr_Format), /* format, ...args */
		gsc_register("string_toupper", Scr_String_ToUpper), /* string */
		gsc_register("string_tolower", Scr_String_ToLower), /* string */
		gsc_register("string_startswith", Scr_String_StartsWith), /* string, substr */
		gsc_register("string_endswith", Scr_String_EndsWith), /* string, substr */
		gsc_register("string_contains", Scr_String_Contains), /* string, substr */
		gsc_register("string_replace", Scr_String_Replace), /* string, from, to */
		gsc_register("string_split", Scr_String_Split), /* string, delimiter */

		gsc_register("await", Scr_Await), /* handle */
		gsc_register("http_get", Scr_HttpGet), /* host, endpoint */
	};
}

namespace CoDUO::Gsc
{
	void Scr_ForceRename(int);
	void Scr_GetIP(int);

	void Scr_GetViewOrigin(int);
	void Scr_GetViewAngles(int);
	void Scr_GetViewDirection(int);
	void Scr_GetEyeTrace(int);

	void Scr_GetParent(int);
	void Scr_GetOwner(int);

	void Scr_GetWeaponDefIndex(int);
	void Scr_GetWeaponSlotInfo(int);
	void Scr_SetWeaponAmmo(int);

	inline std::unordered_map<std::string, gsc_function_t> gsc_methods
	{
		gsc_register("rename", Scr_ForceRename), /* string */
		gsc_register("getip", Scr_GetIP),

		gsc_register("getvieworigin", Scr_GetViewOrigin),
		gsc_register("getviewangles", Scr_GetViewAngles),
		gsc_register("getviewdirection", Scr_GetViewDirection),
		gsc_register("geteyetrace", Scr_GetEyeTrace), /* range, spread */

		gsc_register("getparent", Scr_GetParent),
		gsc_register("getowner", Scr_GetOwner),

		gsc_register("getweaponindex", Scr_GetWeaponDefIndex),
		gsc_register("getweaponslotinfo", Scr_GetWeaponSlotInfo), /* weaponIndex */
		gsc_register("setweaponammo", Scr_SetWeaponAmmo), /* weaponIndex, clip, reserve */
	};
}