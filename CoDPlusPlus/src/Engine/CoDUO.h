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
typedef void (__cdecl* Com_Printf_t)(const char*, ...);

namespace CoDUO /* CoDUOMP */
{
	inline syscall_t syscall = (syscall_t)(0x004685A0);
	inline Com_Printf_t Com_Printf = (Com_Printf_t)(0x00439cc0);
	inline va_t va = (va_t)(0x0044fab0);

	inline refdef_t& refdef = *(refdef_t*)(0x0489a100);
	inline WinMouseVars_t& s_wmv = *(WinMouseVars_t*)(0x009cdbbc);
	inline cmd_function_t*& cmd_functions = *(cmd_function_t**)(0x00964db8);
	inline cvar_t* cvar_indexes = (cvar_t*)(0x009827a0); // cvar_t cvar_indexes[MAX_CVARS];
	inline int& cvar_numIndexes = *(int*)(0x04927ea0);
};

namespace CoDUO /* uo_game_mp_x86 */
{
	inline DWORD uo_game_mp_x86 = 0;

	inline serverStatic_t* svs = nullptr;
	inline level_locals_t* level = nullptr;
	inline gentity_t* g_entities = nullptr;
	inline cvarTable_t* gameCvarTable = nullptr;
}

namespace CoDUO /* GscMapping.cpp */
{
	int32_t Scr_LoadScript(const char* file);
	int32_t Scr_GetFunctionHandle(const char* file, const char* method);
	ScrVar Scr_RunScript(int32_t scriptHandle, uint32_t argc);

	int32_t Scr_GetNumParam();
	int32_t Scr_GetType(int param);
	int32_t Scr_GetPointerType(int index);
	VarType Scr_GetVarType(VariableValue* var);
	const char* Scr_GetTypeName(VarType type);

	int32_t Scr_GetInt(int param);
	float Scr_GetFloat(int param);
	void Scr_GetVector(int param, void* destination);
	vec3_t Scr_GetVector(int param);
	const char* Scr_GetString(int param);
	int32_t Scr_GetConstString(int param);
	gentity_t* Scr_GetEntity(int param);
	int32_t Scr_GetFunction(int param);

	void Scr_AddUndefined();
	void Scr_AddBool(qboolean value);
	void Scr_AddInt(int value);
	void Scr_AddFloat(float value);
	void Scr_AddVector(float* value);
	void Scr_AddString(const char* string);
	void Scr_AddConstString(int32_t index);
	void Scr_AddEntityNum(int index);
	void Scr_AddEntity(gentity_t* entity);
	void Scr_MakeArray();
	void Scr_AddArray();
	void Scr_AddArrayStringIndexed(int32_t str_index);
}

namespace CoDUO /* EntityMapping.cpp */
{
	void G_FreeEntity(gentity_t* ent);
	void G_DeleteEntity(gentity_t* ent);

	void G_SetOrigin(gentity_t* ent, vec3_t origin);
	void SV_LinkEntity(gentity_t* ent);
}

namespace CoDUO /* CvarMapping.cpp */
{
	int Cmd_Argc();
	const char* Cmd_Argv(int arg);

	void Cbuf_AddText(const char* text);
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
	char* CopyString(const char* input);
	int32_t G_NewString(const char* string);
	const char* G_EntityTypeString(int32_t eType);
	unsigned int SL_GetString(const char* value, int user);
	const char* SL_ConvertToString(uint32_t index);
	void SL_RemoveRefToString(uint16_t index);

	void Z_Free(void* mem);

	const char* Info_ValueForKey(const char* buffer, const char* key);
	void Info_SetValueForKey(char* buffer, const char* key, const char* value);
}

namespace CoDUO /* MemoryMapping.cpp */
{
	void RemoveRefToVector(vec3_t* vectorValue);
	void RemoveRefToObject(uint32_t id);
}

namespace CoDUO /* PlayerMapping.cpp */
{
	void G_GetPlayerViewOrigin(gentity_t* ent, float* destination);

	weaponslot_t BG_GetWeaponSlotInfo(gentity_t* player, int32_t weaponIndex);
	void BG_SetPlayerSlotAmmo(gentity_t* player, int32_t weaponIndex, int32_t clip, int32_t reserve);

	void trap_GetUserinfo(int num, char* buffer, int bufferSize);
	void trap_SetUserinfo(int num, const char* buffer);

	std::string NET_AdrToString(netadr_t adr);

	void SV_GameSendServerCommand(int clientNum, int cmd_type, const char* text);
	void SV_GameDropClient(int clientNum, const char* reason);
}

namespace CoDUO /* UtilsMapping.cpp */
{
	void trap_Trace(trace_t* trace, vec3_t* start, vec3_t* end, int passEntityNum, int contentMask);

	void AngleVectors(float* angles, float* forward, float* right, float* up);
	void Bullet_Endpos(float spread, float* forward, float* out_vec);

	float BG_GetAimSpread(gentity_t* ent);
	void SetRayLength(vec3_t& start, vec3_t& end, float newLength);

	trace_t G_GetEyeTrace(gentity_t* player, float range, float spread);

	const char* trace_GetSurfaceType(int32_t surfaceFlags);
	const char* trace_GetHitPartName(uint16_t partName);
	const char* trace_GetHitLocationString(uint16_t partGroup);
}

namespace CoDUO /* FileSystemMapping.cpp */
{
	int FS_ReadFile(const char* qpath, void** buffer);
}

namespace CoDUO
{
	void BaseInitialize();
	void AddConsoleCommands();

	void OnServerInitialize();
	void OnServerShutdown();
	void ServerCleanup();
}