#include "WinApiHelper.h"
#include "ImGuiManager.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

WNDPROC WinApiHelper::o_WndProc = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinApiHelper::h_WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_END) & 1) ImGuiManager::Toggle();

	ImGuiIO& io = ImGui::GetIO();
	io.MouseDrawCursor = ImGuiManager::ShouldShow;

	if (ImGuiManager::ShouldShow)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		{
			return true;
		}

		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
			io.MouseDown[0] = !io.MouseDown[0];
			return 0;
		case WM_RBUTTONDOWN:
			io.MouseDown[1] = !io.MouseDown[1];
			return 0;
		case WM_MBUTTONDOWN:
			io.MouseDown[2] = !io.MouseDown[2];
			return 0;
		case WM_MOUSEWHEEL:
			return 0;
		case WM_MOUSEMOVE:
			io.MousePos.x = (signed short)(lParam);
			io.MousePos.y = (signed short)(lParam >> 16);
			return 0;
		}
	}

	// Alt Context Menu
	if (wParam == SC_KEYMENU && (lParam >> 16) <= 0) return 0;

	return CallWindowProc(o_WndProc, hWnd, uMsg, wParam, lParam);
}

void WinApiHelper::CreateConsole()
{
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
}
