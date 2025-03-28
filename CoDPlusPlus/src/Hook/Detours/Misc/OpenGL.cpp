#ifdef CLIENT
#include <Hook/Detours.h>
#include <Utils/ImGuiManager.h>
#include <print>

namespace Hook::Detour
{
	void __stdcall hkGLimp_EndFrame()
	{
		try
		{
			HDC hDc = *(HDC*)(0x39826E4); // glwstate_t.hDC
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
			std::println("[wglSwapBuffers] - Exception");
		}

		GLimp_EndFrameHook.OriginalFn();
	}
}
#endif