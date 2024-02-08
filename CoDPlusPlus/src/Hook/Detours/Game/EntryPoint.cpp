#include <Engine/CoDUO.h>
#include <Hook/Detours.h>
#include <Utils/WinApiHelper.h>
#include <Utils/OpenGLHelper.h>
#include <Utils/Logger/FileLogger.h>

using namespace Utils;
using namespace CoDUO;

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
			WinApiHelper::CreateConsole("Console");
			FileLogger::BeginLog("codplusplus_app.log");

			WinApiHelper::InjectDetours();

			#ifdef CLIENT
				OpenGLHelper::InjectDetours();
			#endif

			CoDUO::BaseAttach();
			FlushInstructionCache(GetCurrentProcess(), NULL, NULL);

			IsLibraryInitialized = true;
		}
	}
}