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

namespace Detours
{
	void CoDPlusPlus_Initialize();
}

namespace Detours
{
	ImplementDetour(InjectEntryPoint)
	{
		_asm pushad

		CoDPlusPlus_Initialize();

		_asm popad

		_restore
		{
			mov ecx, 0x00469e50
			call ecx
		}

		JumpBack(InjectEntryPoint)
	}
}

namespace Detours
{
	void __cdecl CoDPlusPlus_Initialize()
	{
		static bool IsLibraryInitialized = false;

		if (!IsLibraryInitialized)
		{
			WinApiHelper::SetExceptionFilters();
			WinApiHelper::CreateConsole("CoDPlusPlus");
			WinApiHelper::InjectDetours();

			#ifdef CLIENT
				OpenGLHelper::InjectDetours();
			#endif

			TaskManager::InitializeGarbageCollector();
			ThreadPool.Initialize(64);

			CoDUO::BaseAttach();

			// Unhook from the game loop
			Hook::Patch(Detours::InjectEntryPoint_addr, Detours::InjectEntryPoint_bytes, 5);
			FlushInstructionCache(GetCurrentProcess(), NULL, NULL);

			IsLibraryInitialized = true;
		}
	}
}