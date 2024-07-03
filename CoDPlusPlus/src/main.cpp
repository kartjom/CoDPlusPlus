#include <Windows.h>
#include <Engine/CoDUO.h>
#include <Engine/ScriptLayer/Gsc/Async/Task/Task.h>
#include <Utils/ThreadPool/ThreadPool.h>
#include <Utils/WinApiHelper.h>
#include <Utils/OpenGLHelper.h>
#include <Hook/Detours.h>
#include <Hook/Hook.h>

using namespace Utils;
using namespace CoDUO;
using namespace CoDUO::Gsc::Async;

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

		TaskManager::InitializeGarbageCollector();
		ThreadPool.Initialize();

		CoDUO::BaseAttach();

		FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
		DisableThreadLibraryCalls(hModule);
	}

	return TRUE;
}