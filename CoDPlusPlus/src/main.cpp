#include <Windows.h>
#include <iostream>

#include <Utils/WinApiHelper.h>
#include <Utils/OpenGLHelper.h>
#include <Utils/ImGuiManager.h>
#include <Engine/ScriptLayer/Lua/LuaState.h>
#include <Engine/CoDUO.h>

using namespace CoDUO;

DWORD WINAPI MainThread(LPVOID param)
{
	WinApiHelper::CreateConsole("Console");

	LuaState::Init();

	WinApiHelper::InjectDetours();
	OpenGLHelper::InjectDetours();

	CoDUO::BaseAttach();

	while (true)
	{
		Sleep(1);
	}

	ImGuiManager::Dispose();
	LuaState::Dispose();

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