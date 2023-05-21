#pragma once
#include <cstdint>
#include <wtypes.h>

#include "coduo_structs.h"

namespace CoDUO
{
	inline DWORD uo_game_mp_x86 = 0;
	inline uint32_t CodeCallback_PlayerShoot = 0;

	inline gentity_t* g_entities = nullptr;
	inline cvarTable_t* gameCvarTable = nullptr;

	uint32_t Scr_LoadScript(const char* file);
	uint32_t Scr_GetFunctionHandle(const char* file, const char* method);
	uint32_t Scr_RunScript(uint32_t scriptHandle, uint32_t argc);

	void Scr_AddUndefined();
	void Scr_AddBool(bool value);
	void Scr_AddInt(int value);
	void Scr_AddFloat(float value);
	void Scr_AddVector(void* value);
	void Scr_AddString(const char* string);
	void Scr_AddEntity(int index);

	const char* SL_ConvertToString(int32_t index);

	void RuntimePatch();
	void uo_game_mp_x86_OnAttach();
	void uo_game_mp_x86_OnDetach();
};