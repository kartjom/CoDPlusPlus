#include "CoDUO.h"
#include "Hook.h"
#include "Detours.h"

DWORD CoDUO::uo_game_mp_x86 = 0;
uint32_t CoDUO::CodeCallback_PlayerShoot = 0;

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

void CoDUO::Scr_AddInt(int value)
{
	_asm
	{
		push value
		mov eax, 0x00490670
		call eax

		add esp, 0x4
	}
}

void CoDUO::Scr_AddVector(void* value)
{
	_asm
	{
		push value
		mov eax, 0x004908C0
		call eax

		add esp, 0x4
	}
}

void CoDUO::Scr_AddString(const char* string)
{
	_asm
	{
		push [string]
		mov eax, 0x004907F0
		call eax

		add esp, 0x4
	}
}

void CoDUO::Scr_AddEntity(int index)
{
	_asm
	{
		push 0x0
		push index
		mov eax, 0x00490720
		call eax

		add esp, 0x8
	}
}

const char* CoDUO::SL_ConvertToString(int32_t index)
{
	_asm
	{
		push index
		mov eax, 0x00482290
		call eax

		add esp, 0x4
	}
}

void CoDUO::uo_game_mp_x86_OnAttach()
{
	CoDUO::g_entities = (gentity_t*)(CoDUO::uo_game_mp_x86 + 0x00118d40);

	DetourRet(CoDUO::uo_game_mp_x86 + 0x000361c0, Detours::GScr_LoadGameTypeScript, 8);
	DetourRet(CoDUO::uo_game_mp_x86 + 0x0005689d, Detours::ShootCallback, 6);
}

void CoDUO::uo_game_mp_x86_OnDetach()
{
	CoDUO::g_entities = nullptr;
}