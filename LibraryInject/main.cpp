#include <Windows.h>
#include <iostream>

#pragma comment(lib, "opengl32.lib")
#include <gl/GL.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

HANDLE process;
DWORD baseAddress;
DWORD uo_game_mp_x86;

typedef BOOL(__stdcall* wglSwapBuffers_t)(HDC hDc);
wglSwapBuffers_t wglSwapBuffers_o;

WNDPROC o_WndProc;
HGLRC g_WglContext;
bool initImGui = false;
bool showImGui = false;
HWND hWnd = nullptr;

void Detour(DWORD hookAddress, void* jumpTo, int len, DWORD* ret)
{
	*ret = hookAddress + len;

	DWORD protection;
	VirtualProtect((void*)hookAddress, len, PAGE_EXECUTE_READWRITE, &protection);

	DWORD relativeAddress = ((DWORD)jumpTo - hookAddress) - 5;

	*(BYTE*)hookAddress = 0xE9; // JMP
	*(DWORD*)(hookAddress + 1) = relativeAddress;

	VirtualProtect((void*)hookAddress, len, protection, &protection);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK h_WndProc(const HWND hWnd, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDrawCursor = showImGui;
	io.WantSetMousePos = showImGui;

	if (showImGui)
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

		// Alt
		if (wParam == SC_KEYMENU && (lParam >> 16) <= 0) return 0;

		// If we want to disable movement with menu open
		//return 0;
	}

	return CallWindowProc(o_WndProc, hWnd, uMsg, wParam, lParam);
}

void ExitStatus(bool* status, bool value)
{
	if (status) *status = value;
}

void InitOpenGL3(HDC hDc)
{
	if (WindowFromDC(hDc) == hWnd && initImGui) return;
	bool tStatus = true;

	hWnd = WindowFromDC(hDc);
	LONG wLPTR = SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)h_WndProc);

	if (initImGui)
	{
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplOpenGL3_Init("#version 460");
		return;
	}

	if (!wLPTR) return ExitStatus(&initImGui, false);

	o_WndProc = (WNDPROC)wLPTR;
	g_WglContext = wglCreateContext(hDc);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	tStatus &= ImGui_ImplWin32_Init(hWnd);
	tStatus &= ImGui_ImplOpenGL3_Init("#version 460");

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	initImGui = true;
	return ExitStatus(&initImGui, tStatus);
}

void RenderOpenGL3(HDC hDc, HGLRC WglContext)
{
	bool tStatus = true;

	HGLRC o_WglContext = wglGetCurrentContext();
	tStatus &= wglMakeCurrent(hDc, WglContext);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (showImGui)
	{
		ImGui::ShowDemoWindow();
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	tStatus &= wglMakeCurrent(hDc, o_WglContext);
}

/// LoadLibrary ///////////////////////////////////////////////////////
typedef HMODULE(__stdcall* LoadLibrary_t)(LPCSTR lpLibFileName);
LoadLibrary_t LoadLibrary_o;

void OnAttach();
HMODULE __stdcall LoadLibrary_w(LPCSTR lpLibFileName)
{
	std::string dllName(lpLibFileName);

	HMODULE handle = LoadLibrary_o(lpLibFileName);

	if (dllName.find("uo_game_mp_x86.dll") != std::string::npos)
	{
		uo_game_mp_x86 = (DWORD)handle;
		OnAttach();

		std::cout << "Code attached\n";
	}

	return handle;
}

DWORD LoadLibraryA_Ret;
_declspec(naked) void LoadLibraryA_h()
{
	_asm
	{
		mov edi, dword ptr[LoadLibrary_w]
		jmp[LoadLibraryA_Ret]
	}
}
///////////////////////////////////////////////////////////////////////

/// wglSwapBuffers ////////////////////////////////////////////////////
BOOL __stdcall wglSwapBuffers_w(HDC hDc)
{
	InitOpenGL3(hDc);
	RenderOpenGL3(hDc, g_WglContext);

	return wglSwapBuffers_o(hDc);
}

DWORD wglSwapBuffers_Ret;
_declspec(naked) void wglSwapBuffers_h()
{
	_asm
	{
		call dword ptr[wglSwapBuffers_w]
		jmp [wglSwapBuffers_Ret]
	}
}
///////////////////////////////////////////////////////////////////////

/// SetPhysicalCursorPos //////////////////////////////////////////////
typedef BOOL(__stdcall* SetPhysicalCursorPos_t)(int x, int y);
SetPhysicalCursorPos_t SetPhysicalCursorPos_o;

BOOL __stdcall SetPhysicalCursorPos_w(int x, int y)
{
	if (showImGui)
	{
		return false;
	}

	return SetPhysicalCursorPos_o(x, y);
}

DWORD SetPhysicalCursorPos_Ret;
_declspec(naked) void SetPhysicalCursorPos_h()
{
	_asm
	{
		call dword ptr[SetPhysicalCursorPos_w]
		jmp [SetPhysicalCursorPos_Ret]
	}
}
///////////////////////////////////////////////////////////////////////

struct gentity
{
	BYTE unknown1[388];
	const char* classname;
};

typedef gentity* (__cdecl* G_Spawn_t)();
G_Spawn_t G_Spawn;

void OnAttach()
{
	G_Spawn = (G_Spawn_t)(uo_game_mp_x86 + 0x52e90);
}

DWORD WINAPI MainThread(LPVOID param)
{
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

	baseAddress = (DWORD)GetModuleHandleA(NULL);
	process = GetCurrentProcess();

	LoadLibrary_o = (LoadLibrary_t)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	Detour(baseAddress + 0x6B8FB, LoadLibraryA_h, 6, &LoadLibraryA_Ret);
	
	wglSwapBuffers_o = (wglSwapBuffers_t)GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");
	Detour(baseAddress + 0xF6723, wglSwapBuffers_h, 6, &wglSwapBuffers_Ret);
	
	SetPhysicalCursorPos_o = (SetPhysicalCursorPos_t)GetProcAddress(GetModuleHandleA("user32.dll"), "SetPhysicalCursorPos");
	Detour(baseAddress + 0x69C3B, SetPhysicalCursorPos_h, 6, &SetPhysicalCursorPos_Ret);

	while (true)
	{
		if (GetAsyncKeyState(VK_END) & 1)
		{
			showImGui = !showImGui;
			//ReadProcessMemory();
		}

		Sleep(1);
	}

	//FreeLibraryAndExitThread((HMODULE)param, 0);

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		CreateThread(0, 0, MainThread, hModule, 0, 0);
		DisableThreadLibraryCalls(hModule);
	}

	return TRUE;
}