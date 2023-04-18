#include "ImGuiManager.h"
#include "WinApiHelper.h"

#pragma comment(lib, "opengl32.lib")
//#include <gl/GL.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

HWND ImGuiManager::hWnd = nullptr;
HGLRC ImGuiManager::wglContext = nullptr;

bool ImGuiManager::IsInitialized = false;
bool ImGuiManager::ShouldShow = false;

void ImGuiManager::Initialize(HDC hDc)
{
	if (WindowFromDC(hDc) == hWnd) return; // Nothing changed, everything initialized

	if (WindowFromDC(hDc) != hWnd && ImGuiManager::IsInitialized) // Window handle changed, need to re-init
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
	ImGuiManager::wglContext = wglCreateContext(hDc);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplOpenGL3_Init("#version 460");

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGuiManager::IsInitialized = true;
	std::cout << "[ImGui] - Initialized" << std::endl;
}

HGLRC ImGuiManager::BeginFrame(HDC hDc)
{
	HGLRC o_WglContext = wglGetCurrentContext();
	wglMakeCurrent(hDc, ImGuiManager::wglContext);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return o_WglContext;
}

void ImGuiManager::EndFrame(HDC hDc, HGLRC o_WglContext)
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	wglMakeCurrent(hDc, o_WglContext);
}

void ImGuiManager::Dispose()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool ImGuiManager::Toggle() { return ImGuiManager::ShouldShow = !ImGuiManager::ShouldShow; }
bool ImGuiManager::Show()	{ return ImGuiManager::ShouldShow = true; }
bool ImGuiManager::Hide()	{ return ImGuiManager::ShouldShow = false; }

void ImGuiManager::Tick()
{
	ImGui::ShowDemoWindow();
}