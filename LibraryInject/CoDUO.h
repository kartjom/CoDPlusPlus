#pragma once
#include <cstdint>
#include <wtypes.h>

#include "coduo_structs.h"

class CoDUO
{
public:
	static DWORD uo_game_mp_x86;
	static uint32_t CodeCallback_Custom;

	static gentity_t* g_entities;

	static uint32_t Scr_LoadScript(const char* file);
	static uint32_t Scr_GetFunctionHandle(const char* file, const char* method);
	static uint32_t Scr_RunScript(uint32_t scriptHandle, uint32_t argc);

	static void uo_game_mp_x86_OnAttach();
	static void uo_game_mp_x86_OnDetach();
};