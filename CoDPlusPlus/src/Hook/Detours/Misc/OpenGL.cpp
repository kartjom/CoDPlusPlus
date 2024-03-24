#ifdef CLIENT

#include <iostream>
#include <Hook/Detours.h>
#include <Utils/ImGuiManager.h>

namespace Detours
{
	ImplementOverride(BOOL, __stdcall, wglSwapBuffers)(HDC hDc)
	{
		try
		{
			ImGuiManager::Initialize(hDc);

			HGLRC o_WglContext = ImGuiManager::BeginFrame(hDc);
			ImGuiManager::Tick();
			if (ImGuiManager::InteractiveMode)
			{
				ImGuiManager::InteractiveTick();
			}
			ImGuiManager::EndFrame(hDc, o_WglContext);
		}
		catch (...)
		{
			std::cout << "[wglSwapBuffers] - Exception" << std::endl;
		}

		return Original(wglSwapBuffers)(hDc);
	}

	ImplementDetour(wglSwapBuffers)
	{
		_asm
		{
			call dword ptr[Wrapper(wglSwapBuffers)]
		}

		JumpBack(wglSwapBuffers)
	}
}

#endif