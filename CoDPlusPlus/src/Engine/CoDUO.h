#pragma once
#include <Structs/Engine/level_locals.h>
#include <Structs/Engine/server.h>
#include <Structs/Engine/gentity.h>
#include <Structs/Engine/gclient.h>
#include <Structs/Engine/weapondef.h>
#include <Structs/Engine/cvar.h>
#include <Structs/Engine/cmd.h>
#include <Structs/Engine/trace.h>
#include <Structs/Engine/refdef.h>
#include <Structs/Engine/input.h>
#include <Engine/ScriptLayer/Gsc/GscExtensions.h>
#include <wtypes.h>

typedef char* (__cdecl* syscall_t)(int32_t, ...);
typedef char* (__cdecl* va_t)(const char*, ...);

namespace CoDUO
{
	inline syscall_t syscall = (syscall_t)(0x004685A0);
	inline va_t va = (va_t)(0x0044fab0);
	inline DWORD uo_game_mp_x86 = 0;

	inline serverStatic_t* svs = nullptr;
	inline level_locals_t* level = nullptr;
	inline refdef_t* refdef = nullptr;
	inline WinMouseVars_t* s_wmv = nullptr;
	inline gentity_t* g_entities = nullptr;
	inline cvarTable_t* gameCvarTable = nullptr;
	inline cvar_t* cvar_indexes = nullptr;
	inline int32_t* bg_iNumWeapons = nullptr;

	inline cmd_function_t** cmd_functions;
	inline char** Cmd_Argv = nullptr;
	inline int* Cmd_Argc = nullptr;
};

namespace CoDUO /* GscMapping.cpp */
{
	int32_t Scr_LoadScript(const char* file);
	int32_t Scr_GetFunctionHandle(const char* file, const char* method);
	int32_t Scr_RunScript(int32_t scriptHandle, uint32_t argc);

	int32_t Scr_GetNumParam();
	int32_t Scr_GetType(int param);
	int32_t Scr_GetInt(int param);
	float Scr_GetFloat(int param);
	void Scr_GetVector(int param, void* destination);
	vec3_t Scr_GetVector(int param);
	const char* Scr_GetString(int param);
	int32_t Scr_GetConstString(int param);
	gentity_t* Scr_GetEntity(int param);
	int32_t Scr_GetFunction(int param);

	void Scr_AddUndefined();
	void Scr_AddBool(int value);
	void Scr_AddInt(int value);
	void Scr_AddFloat(float value);
	void Scr_AddVector(float* value);
	void Scr_AddString(const char* string);
	void Scr_AddConstString(int32_t index);
	void Scr_AddEntityNum(int index);
	void Scr_MakeArray();
	void Scr_AddArray();
	void Scr_AddArrayStringIndexed(int32_t str_index);
}

namespace CoDUO /* CvarMapping.cpp */
{
	void Cbuf_AddText(const char* text);
	void trap_SendConsoleCommand(int exec_when, const char* text);
	void Cmd_AddCommand(const char* cmd_name, void* function);
	void Cmd_RemoveCommand(const char* cmd_name);
	bool Cmd_HasCommand(const char* cmd_name);
	cvar_t* Cvar_FindVar(const char* var_name);
	cvar_t* Cvar_Get(const char* var_name, const char* var_value, int flags);
	cvar_t* Cvar_Set(const char* var_name, const char* value, uint32_t force);
}

namespace CoDUO /* WeaponMapping.cpp */
{
	int32_t BG_GetNumWeapons();

	bool BG_IsWeaponIndexValid(int32_t index);
	int32_t BG_GetWeaponIndexForName(const char* name);
	weapondef_t* BG_GetWeaponDef(int32_t index);
}

namespace CoDUO /* StringUtilsMapping.cpp */
{
	char* CopyString(const char* in);
	int32_t G_NewString(const char* string);
	const char* SL_ConvertToString(uint32_t index);
	void SL_RemoveRefToString(uint16_t index);

	const char* Info_ValueForKey(const char* buffer, const char* key);
	void Info_SetValueForKey(char* buffer, const char* key, const char* value);
}

namespace CoDUO /* PlayerMapping.cpp */
{
	void G_GetPlayerViewOrigin(gentity_t* ent, float* destination);

	weaponslot_t BG_GetWeaponSlotInfo(gentity_t* player, int32_t weaponIndex);
	void BG_SetPlayerSlotAmmo(gentity_t* player, int32_t weaponIndex, int32_t clip, int32_t reserve);

	void trap_GetUserinfo(int num, char* buffer, int bufferSize);
	void trap_SetUserinfo(int num, const char* buffer);

	std::string NET_AdrToString(netadr_t adr);
}

namespace CoDUO /* UtilsMapping.cpp */
{
	void trap_Trace(trace_t* trace, vec3_t* start, vec3_t* end, int passEntityNum, int contentMask);

	void AngleVectors(float* angles, float* forward, float* right, float* up);
	void Bullet_Endpos(float spread, float* forward, float* out_vec);

	float BG_GetAimSpread(gentity_t* ent);
	void SetRayLength(vec3_t& start, vec3_t& end, float newLength);

	trace_t G_GetEyeTrace(gentity_t* player, float range, float spread);
}

namespace CoDUO
{
	void BaseAttach();

	void uo_game_mp_x86_OnAttach();
	void uo_game_mp_x86_OnDetach();
	void uo_game_mp_x86_Cleanup();

	void ServerTick();
}