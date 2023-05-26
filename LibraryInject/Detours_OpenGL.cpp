#include "Detours.h"
#include "ImGuiManager.h"

namespace Detours
{
	ImplementOverride(BOOL, __stdcall, wglSwapBuffers)(HDC hDc)
	{
		try
		{
			ImGuiManager::Initialize(hDc);

			HGLRC o_WglContext = ImGuiManager::BeginFrame(hDc);
			ImGuiManager::Tick();
			if (ImGuiManager::ShouldShow)
			{
				ImGuiManager::InteractiveTick();
			}
			ImGuiManager::EndFrame(hDc, o_WglContext);
		}
		catch (...)
		{

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