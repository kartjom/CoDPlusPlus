#include <Windows.h>
#include <iostream>

#include "WinApiManager.h"
#include "ImGuiManager.h"
#include "Hook.h"

DWORD uo_game_mp_x86 = 0;
uint32_t CodeCallback_Custom = 0;

////////////////////////////////
struct Vector3 {
	float x, y, z;
};

class gentity_t
{
public:
	char pad_0000[16]; //0x0000
	int32_t time; //0x0010
	char pad_0014[4]; //0x0014
	Vector3 origin; //0x0018
	char pad_0024[28]; //0x0024
	Vector3 viewangles; //0x0040
	char pad_004C[768]; //0x004C
};

gentity_t* g_entities;
////////////////////////////////

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
		uo_game_mp_x86 = (DWORD)handle;
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
		uo_game_mp_x86 = 0;
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

/// SetPhysicalCursorPos //////////////////////////////////////////////
DWORD GScr_LoadGameTypeScript_Ret;

uint32_t Scr_LoadScript(const char* file)
{
	_asm
	{
		push [file]
		mov eax, [file]
		mov edi, 0x00480150
		call edi
		add esp, 0x4
	}
}

uint32_t Scr_GetFunctionHandle(const char* file, const char* method)
{
	_asm
	{
		push [method]
		push [file]
		mov eax, 0x0047FE50
		call eax
		add esp, 0x8
	}
}

_declspec(naked) void GScr_LoadGameTypeScript_h()
{
	_asm pushad

	if (Scr_LoadScript("maps/mp/gametypes/_callbacksetup"))
	{
		CodeCallback_Custom = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_Custom");
	}

	_asm popad

	_asm
	{
		sub esp,0x44

		mov eax, uo_game_mp_x86
		add eax, 0x00082650

		mov eax, [eax]
		jmp [GScr_LoadGameTypeScript_Ret]
	}
}
///////////////////////////////////////////////////////////////////////

/// Shoot Callback ///////////////////////////////////////////////////
uint32_t Scr_RunScript(uint32_t scriptHandle, uint32_t argc)
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

DWORD ShootCallback_Ret;
_declspec(naked) void ShootCallback_h()
{
	_asm pushad

	if (CodeCallback_Custom != 0)
	{
		Scr_RunScript(CodeCallback_Custom, 0);
	}

	_asm popad

	_asm
	{
		lea ecx, dword ptr ds : [ecx + eax * 0x4 + 0x334]
		sub dword ptr ds : [ecx], esi

		jmp [ShootCallback_Ret]
	}
}
///////////////////////////////////////////////////////////////////////

void uo_game_mp_x86_OnAttach()
{
	g_entities = (gentity_t*)(uo_game_mp_x86 + 0x00118d40);

	Hook::Detour(uo_game_mp_x86 + 0x000361c0, GScr_LoadGameTypeScript_h, 8, &GScr_LoadGameTypeScript_Ret);
	Hook::Detour(uo_game_mp_x86 + 0x122A6, ShootCallback_h, 9, &ShootCallback_Ret);
}

void uo_game_mp_x86_OnDetach()
{
	g_entities = nullptr;
}

DWORD WINAPI MainThread(LPVOID param)
{
	WinApiManager::CreateConsole();

	LoadLibrary_o = Hook::LoadFromDLL<LoadLibrary_t>("kernel32.dll", "LoadLibraryA");
	Hook::Detour(Hook::BaseAddress + 0x6B8FB, LoadLibraryA_h, 6, &LoadLibraryA_Ret);
	
	FreeLibrary_o = Hook::LoadFromDLL<FreeLibrary_t>("kernel32.dll", "FreeLibrary");
	//Hook::Detour(Hook::BaseAddress + 0x1c0c, FreeLibrary_h, 6, &FreeLibrary_Ret);
	
	wglSwapBuffers_o = Hook::LoadFromDLL<wglSwapBuffers_t>("opengl32.dll", "wglSwapBuffers");
	Hook::Detour(Hook::BaseAddress + 0xF6723, wglSwapBuffers_h, 6, &wglSwapBuffers_Ret);
	
	SetPhysicalCursorPos_o = Hook::LoadFromDLL<SetPhysicalCursorPos_t>("user32.dll", "SetPhysicalCursorPos");
	Hook::Detour(Hook::BaseAddress + 0x69C3B, SetPhysicalCursorPos_h, 6, &SetPhysicalCursorPos_Ret);

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