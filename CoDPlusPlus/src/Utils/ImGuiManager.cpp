#ifdef CLIENT

#include "ImGuiManager.h"
#include "WinApiHelper.h"
#include <print>

#pragma comment(lib, "opengl32.lib")
//#include <gl/GL.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

#include <Engine/CoDUO.h>
#include <Utils/DevGui/SharedState.h>
#include <Utils/DevGui/Client.h>
#include <Utils/DevGui/Server.h>
#include <Utils/DevGui/Debug.h>

using namespace CoDUO;

namespace ImGuiManager
{
	void Initialize(HDC hDc)
	{
		if (WindowFromDC(hDc) == hWnd) return; // Nothing changed, everything initialized

		if (WindowFromDC(hDc) != hWnd && IsInitialized) // Window handle changed, need to re-init
		{
			std::println("[ImGui] - Window handle changed, destroying context");

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
		io.IniFilename = nullptr;

		IsInitialized = true;
		std::println("[ImGui] - Initialized");
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

	void CursorToCenter()
	{
		int width, height;
		RECT window_rect;

		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);

		GetWindowRect(hWnd, &window_rect);
		if (window_rect.left < 0) {
			window_rect.left = 0;
		}
		if (window_rect.top < 0) {
			window_rect.top = 0;
		}
		if (window_rect.right >= width) {
			window_rect.right = width - 1;
		}
		if (window_rect.bottom >= height - 1) {
			window_rect.bottom = height - 1;
		}
		int window_center_x = (window_rect.right + window_rect.left) / 2;
		int window_center_y = (window_rect.top + window_rect.bottom) / 2;

		SetCursorPos(window_center_x, window_center_y);
	}

	bool Toggle()
	{
		return InteractiveMode ? Hide() : Show();
	}

	bool Show()
	{
		CursorToCenter();
		s_wmv.mouseInitialized = false;

		return InteractiveMode = true;
	}

	bool Hide()
	{
		CursorToCenter();
		s_wmv.mouseInitialized = true;

		return InteractiveMode = false;
	}

	void InteractiveTick()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_Once);

		if (!ImGui::Begin("CoDPlusPlus DevGui", &InteractiveMode))
		{
			ImGui::End();
			return;
		}

		if (ImGui::BeginTabBar("##TabBar"))
		{
			DevGui::Client::RenderTab();
			DevGui::Server::RenderTab();
			DevGui::Debug::RenderTab();

			ImGui::EndTabBar();
		}

		ImGui::End();

		if (!InteractiveMode)
		{
			Hide();
		}
	}

	void Tick()
	{
		DevGui::Debug::ForceFoV();
		DevGui::Debug::RenderActions();
	}
}

#endif