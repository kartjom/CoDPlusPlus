﻿#include <Windows.h>
#include <Engine/CoDUO.h>
#include <Engine/Async/Task/Task.h>
#include <Engine/MapBindings/MapBindings.h>
#include <Utils/ThreadPool/ThreadPool.h>
#include <Utils/WinApiHelper.h>
#include <Utils/OpenGLHelper.h>
#include <Hook/Detours.h>
#include <Hook/Hook.h>

using namespace CoDUO;

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (!WinApiHelper::CheckGame())
		{
			return FALSE;
		}

		WinApiHelper::SetExceptionFilters();
		WinApiHelper::CreateConsole("CoDPlusPlus");
		WinApiHelper::InjectDetours();

		#ifdef CLIENT
			OpenGLHelper::InjectDetours();
		#endif

		Threading::Async::TaskManager::InitializeGarbageCollector();
		Threading::ThreadPool.Initialize();

		MapBindings::ReloadMapBindings();
		CoDUO::BaseInitialize();

		FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
		DisableThreadLibraryCalls(hModule);
	}

	return TRUE;
}