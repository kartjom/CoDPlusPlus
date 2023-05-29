#pragma once
#include <cstdint>
#include <wtypes.h>
#include "CoDUO_Callbacks.h"

namespace CoDUO
{
	inline DWORD uo_game_mp_x86 = 0;
	inline uint32_t CodeCallback_PlayerShoot = 0;

	inline refdef_t* refdef = nullptr;
	inline gentity_t* g_entities = nullptr;
	inline cvarTable_t* gameCvarTable = nullptr;
	inline cvar_t* cvar_indexes = nullptr;

	int32_t Scr_LoadScript(const char* file);
	int32_t Scr_GetFunctionHandle(const char* file, const char* method);
	int32_t Scr_RunScript(int32_t scriptHandle, uint32_t argc);

	int32_t Scr_GetNumParam();
	int32_t Scr_GetType(int param);
	int32_t Scr_GetInt(int param);
	float Scr_GetFloat(int param);
	void Scr_GetVector(int param, void* destination);
	Vector3 Scr_GetVector(int param);
	const char* Scr_GetString(int param);
	int32_t Scr_GetConstString(int param);
	gentity_t* Scr_GetEntity(int param);

	void Scr_AddUndefined();
	void Scr_AddBool(bool value);
	void Scr_AddInt(int value);
	void Scr_AddFloat(float value);
	void Scr_AddVector(void* value);
	void Scr_AddString(const char* string);
	void Scr_AddEntity(int index);

	const char* SL_ConvertToString(int32_t index);

	void trap_SendConsoleCommand(int exec_when, const char* text);
	void Cmd_AddCommand(const char* cmd_name, void* function);
	cvar_t* Cvar_FindVar(const char* var_name);
	cvar_t* Cvar_Get(const char* var_name, const char* var_value, int flags);
	cvar_t* Cvar_Set(const char* var_name, const char* value, qboolean force);

	void BaseAttach();
	void uo_game_mp_x86_OnAttach();
	void uo_game_mp_x86_OnDetach();
};