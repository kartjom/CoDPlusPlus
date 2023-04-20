#include "CoDUO.h"
#include "Hook.h"
#include "Detours.h"

DWORD CoDUO::uo_game_mp_x86 = 0;
uint32_t CoDUO::CodeCallback_Custom = 0;

gentity_t* CoDUO::g_entities = nullptr;

uint32_t CoDUO::Scr_LoadScript(const char* file)
{
	_asm
	{
		push[file]
		mov eax, [file]
		mov edi, 0x00480150
		call edi
		add esp, 0x4
	}
}

uint32_t CoDUO::Scr_GetFunctionHandle(const char* file, const char* method)
{
	_asm
	{
		push[method]
		push[file]
		mov eax, 0x0047FE50
		call eax
		add esp, 0x8
	}
}

uint32_t CoDUO::Scr_RunScript(uint32_t scriptHandle, uint32_t argc)
{
	_asm
	{
		push argc
		push scriptHandle
		mov eax, 0x0048f3e0
		call eax // Scr_ExecThread

		push eax
		mov eax, 0x0048f640
		call eax // Scr_FreeThread

		add esp, 0xC
	}
}

void CoDUO::uo_game_mp_x86_OnAttach()
{
	CoDUO::g_entities = (gentity_t*)(CoDUO::uo_game_mp_x86 + 0x00118d40);

	DetourRet(CoDUO::uo_game_mp_x86 + 0x000361c0, Detours::GScr_LoadGameTypeScript, 8);
	DetourRet(CoDUO::uo_game_mp_x86 + 0x122A6, Detours::ShootCallback, 9);
}

void CoDUO::uo_game_mp_x86_OnDetach()
{
	CoDUO::g_entities = nullptr;
}