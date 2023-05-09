#include "Detours.h"
#include "ImGuiManager.h"

namespace Detours
{
	ImplementOverride(BOOL, __stdcall, wglSwapBuffers)(HDC hDc)
	{
		ImGuiManager::Initialize(hDc);

		if (ImGuiManager::ShouldShow)
		{
			HGLRC o_WglContext = ImGuiManager::BeginFrame(hDc);
			ImGuiManager::Tick();
			ImGuiManager::EndFrame(hDc, o_WglContext);
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