#include <Windows.h>
#include <iostream>

#include "WinApiHelper.h"
#include "ImGuiManager.h"
#include "Hook.h"
#include "Detours.h"

DWORD WINAPI MainThread(LPVOID param)
{
	WinApiHelper::CreateConsole();

	Detours::LoadLibraryA_o = Hook::LoadFromDLL<LoadLibraryA_t>("kernel32.dll", "LoadLibraryA");
	DetourRet(Hook::BaseAddress + 0x6B8FB, Detours::LoadLibraryA, 6);

	Detours::SetPhysicalCursorPos_o = Hook::LoadFromDLL<SetPhysicalCursorPos_t>("user32.dll", "SetPhysicalCursorPos");
	DetourRet(Hook::BaseAddress + 0x69C3B, Detours::SetPhysicalCursorPos, 6);
	
	Detours::wglSwapBuffers_o = Hook::LoadFromDLL<wglSwapBuffers_t>("opengl32.dll", "wglSwapBuffers");
	DetourRet(Hook::BaseAddress + 0xF6723, Detours::wglSwapBuffers, 6);

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