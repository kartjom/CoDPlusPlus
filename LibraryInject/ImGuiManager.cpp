#include "ImGuiManager.h"
#include "WinApiHelper.h"

#pragma comment(lib, "opengl32.lib")
//#include <gl/GL.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <format>
#include "Vector3.h"
#include "OpenGLHelper.h"
#include "CoDUO.h"

using namespace OpenGLHelper;
using namespace CoDUO;

namespace ImGuiManager
{
	void Initialize(HDC hDc)
	{
		if (WindowFromDC(hDc) == hWnd) return; // Nothing changed, everything initialized

		if (WindowFromDC(hDc) != hWnd && IsInitialized) // Window handle changed, need to re-init
		{
			std::cout << "[ImGui] - Window handle changed, destroying context" << std::endl;

			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}

		hWnd = WindowFromDC(hDc);
		LONG wLPTR = SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WinApiHelper::h_WndProc);

		if (!wLPTR) return;

		WinApiHelper::o_WndProc = (WNDPROC)wLPTR;
		wglContext = wglCreateContext(hDc);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplOpenGL3_Init("#version 460");

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		IsInitialized = true;
		std::cout << "[ImGui] - Initialized" << std::endl;
	}

	HGLRC BeginFrame(HDC hDc)
	{
		HGLRC o_WglContext = wglGetCurrentContext();
		wglMakeCurrent(hDc, wglContext);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		return o_WglContext;
	}

	void EndFrame(HDC hDc, HGLRC o_WglContext)
	{
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		wglMakeCurrent(hDc, o_WglContext);
	}

	void Dispose()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	bool Toggle() { return ShouldShow = !ShouldShow; }
	bool Show() { return ShouldShow = true; }
	bool Hide() { return ShouldShow = false; }

	void InteractiveTick()
	{
		//ImGui::ShowDemoWindow();
	}

	void Tick()
	{
		static int shouldShow = 0;
		if (GetAsyncKeyState(VK_DELETE) & 1)
		{
			shouldShow = (++shouldShow) % 3;
			std::cout << shouldShow << std::endl;
		}

		if (!uo_game_mp_x86 || !shouldShow) return;

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(refdef->width, refdef->height));
		ImGui::Begin("Canvas", 0, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);

		int entsOnList = 0;
		for (int i = 0; i <= GENTITY_COUNT; i++)
		{
			gentity_t* ent = &g_entities[i];
			if (!ent->classname) continue;

			const char* classname = SL_ConvertToString(ent->classname);
			if (!classname) continue;
			
			const char* targetname = nullptr;
			if (ent->targetname) targetname = SL_ConvertToString(ent->targetname);

			Vector3 screen;
			if (WorldToScreen(ent->origin, screen, refdef) && IsOnScreen(screen, refdef->width, refdef->height))
			{
				std::string ct_formatted = std::format("[{}] {} {}", i, classname, targetname ? targetname : "");
				std::string origin_formatted = std::format("{:.2f} {:.2f} {:.2f}", ent->origin.x, ent->origin.y, ent->origin.z);

				ImGui::GetWindowDrawList()->AddText(ImVec2(screen.x, screen.y), ImColor(1.0f, 1.0f, 1.0f, 1.0f), ct_formatted.c_str());
				ImGui::GetWindowDrawList()->AddText(ImVec2(screen.x, screen.y + 16), ImColor(0.75f, 0.75f, 0.75f, 1.0f), origin_formatted.c_str());
			
				if (shouldShow > 1 && entsOnList < 20)
				{
					ImGui::SetNextWindowPos(ImVec2(10, 20));
					ImGui::SetNextWindowSize(ImVec2(600, 350));
					ImGui::Begin("List", 0, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
						std::string text = std::format("{} |  {}", ct_formatted, origin_formatted);
						ImGui::Text(text.c_str());
						entsOnList++;
					ImGui::End();
				}
			}
		}

		ImGui::End();
	}
}