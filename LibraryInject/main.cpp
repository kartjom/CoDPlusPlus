#include <Windows.h>
#include <iostream>

#include "WinApiHelper.h"
#include "ImGuiManager.h"
#include "Hook.h"
#include "Detours.h"
#include "CoDUO.h"

/// LoadLibrary ///////////////////////////////////////////////////////
typedef HMODULE(__stdcall* LoadLibrary_t)(LPCSTR lpLibFileName);
LoadLibrary_t LoadLibrary_o;

void uo_game_mp_x86_OnAttach();
HMODULE __stdcall LoadLibrary_w(LPCSTR lpLibFileName)
{
	std::string dllName(lpLibFileName);

	HMODULE handle = LoadLibrary_o(lpLibFileName);

	if (dllName.find("uo_game_mp_x86.dll") != std::string::npos)
	{
		CoDUO::uo_game_mp_x86 = (DWORD)handle;
		uo_game_mp_x86_OnAttach();
	}

	return handle;
}

DWORD LoadLibraryA_Ret;
_declspec(naked) void LoadLibraryA_h()
{
	_asm
	{
		mov edi, dword ptr[LoadLibrary_w]
		jmp[LoadLibraryA_Ret]
	}
}
///////////////////////////////////////////////////////////////////////

/// LoadLibrary ///////////////////////////////////////////////////////
typedef BOOL(__stdcall* FreeLibrary_t)(HMODULE hLibModule);
FreeLibrary_t FreeLibrary_o;

void uo_game_mp_x86_OnDetach();
BOOL __stdcall FreeLibrary_w(HMODULE hLibModule)
{
	char buff[MAX_PATH];
	GetModuleFileNameA(hLibModule, buff, MAX_PATH);

	std::string dllName(buff);
	std::cout << dllName << std::endl;

	if (dllName.find("uo_game_mp_x86.dll") != std::string::npos)
	{
		uo_game_mp_x86_OnDetach();
		CoDUO::uo_game_mp_x86 = 0;
	}

	return FreeLibrary_o(hLibModule);
}

DWORD FreeLibrary_Ret;
_declspec(naked) void FreeLibrary_h()
{
	_asm
	{
		call dword ptr[FreeLibrary_w]
		jmp [FreeLibrary_Ret]
	}
}
///////////////////////////////////////////////////////////////////////

/// wglSwapBuffers ////////////////////////////////////////////////////
typedef BOOL(__stdcall* wglSwapBuffers_t)(HDC hDc);
wglSwapBuffers_t wglSwapBuffers_o;

BOOL __stdcall wglSwapBuffers_w(HDC hDc)
{
	ImGuiManager::Initialize(hDc);

	if (ImGuiManager::ShouldShow)
	{
		HGLRC o_WglContext = ImGuiManager::BeginFrame(hDc);
		ImGuiManager::Tick();
		ImGuiManager::EndFrame(hDc, o_WglContext);
	}

	return wglSwapBuffers_o(hDc);
}

DWORD wglSwapBuffers_Ret;
_declspec(naked) void wglSwapBuffers_h()
{
	_asm
	{
		call dword ptr[wglSwapBuffers_w]
		jmp [wglSwapBuffers_Ret]
	}
}
///////////////////////////////////////////////////////////////////////

/// SetPhysicalCursorPos //////////////////////////////////////////////
typedef BOOL(__stdcall* SetPhysicalCursorPos_t)(int x, int y);
SetPhysicalCursorPos_t SetPhysicalCursorPos_o;

BOOL __stdcall SetPhysicalCursorPos_w(int x, int y)
{
	if (ImGuiManager::ShouldShow)
	{
		return false;
	}

	return SetPhysicalCursorPos_o(x, y);
}

DWORD SetPhysicalCursorPos_Ret;
_declspec(naked) void SetPhysicalCursorPos_h()
{
	_asm
	{
		call dword ptr[SetPhysicalCursorPos_w]
		jmp [SetPhysicalCursorPos_Ret]
	}
}
///////////////////////////////////////////////////////////////////////

void uo_game_mp_x86_OnAttach()
{
	CoDUO::g_entities = (gentity_t*)(CoDUO::uo_game_mp_x86 + 0x00118d40);

	DetourRet(CoDUO::uo_game_mp_x86 + 0x000361c0, Detours::GScr_LoadGameTypeScript, 8);
	DetourRet(CoDUO::uo_game_mp_x86 + 0x122A6, Detours::ShootCallback, 9);
}

void uo_game_mp_x86_OnDetach()
{
	CoDUO::g_entities = nullptr;
}

DWORD WINAPI MainThread(LPVOID param)
{
	WinApiHelper::CreateConsole();

	LoadLibrary_o = Hook::LoadFromDLL<LoadLibrary_t>("kernel32.dll", "LoadLibraryA");
	Hook::DetourA(Hook::BaseAddress + 0x6B8FB, LoadLibraryA_h, 6, &LoadLibraryA_Ret);
	
	//FreeLibrary_o = Hook::LoadFromDLL<FreeLibrary_t>("kernel32.dll", "FreeLibrary");
	//Hook::Detour(Hook::BaseAddress + 0x1c0c, FreeLibrary_h, 6, &FreeLibrary_Ret);
	
	wglSwapBuffers_o = Hook::LoadFromDLL<wglSwapBuffers_t>("opengl32.dll", "wglSwapBuffers");
	Hook::DetourA(Hook::BaseAddress + 0xF6723, wglSwapBuffers_h, 6, &wglSwapBuffers_Ret);
	
	SetPhysicalCursorPos_o = Hook::LoadFromDLL<SetPhysicalCursorPos_t>("user32.dll", "SetPhysicalCursorPos");
	Hook::DetourA(Hook::BaseAddress + 0x69C3B, SetPhysicalCursorPos_h, 6, &SetPhysicalCursorPos_Ret);

	while (true)
	{
		Sleep(1);
	}

	ImGuiManager::Dispose();

	//FreeLibraryAndExitThread((HMODULE)param, 0);

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		CreateThread(0, 0, MainThread, hModule, 0, 0);
		DisableThreadLibraryCalls(hModule);
	}

	return TRUE;
}