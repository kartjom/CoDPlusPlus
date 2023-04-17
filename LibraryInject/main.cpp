#include <Windows.h>
#include <iostream>

#pragma comment(lib, "opengl32.lib")
#include <gl/GL.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

//#include "gentity.h"

#define baseAddress 0x400000
#define LoadFromDLL(dll, method) GetProcAddress(GetModuleHandleA(dll), method)
#define SAFE_EXEC(code) \
_asm {pushad} \
code \
_asm {popad}

HANDLE process;
DWORD uo_game_mp_x86;

typedef BOOL(__stdcall* wglSwapBuffers_t)(HDC hDc);
wglSwapBuffers_t wglSwapBuffers_o;

WNDPROC o_WndProc;
HGLRC g_WglContext;
bool initImGui = false;
bool showImGui = false;
HWND hWnd = nullptr;

typedef uint32_t(__cdecl* Scr_LoadScript_t)(const char* file);
Scr_LoadScript_t Scr_LoadScript = (Scr_LoadScript_t)(0x00480150);

typedef uint32_t(__cdecl* Scr_GetFunctionHandle_t)(const char* file, const char* function);
Scr_GetFunctionHandle_t Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)(0x0047FE50);

typedef uint32_t(__cdecl* Scr_ExecThread_t)(uint32_t scriptHandle, uint32_t argc);
Scr_ExecThread_t Scr_ExecThread = (Scr_ExecThread_t)(0x0048f3e0);

typedef uint32_t(__cdecl* Scr_FreeThread_t)(uint32_t threadHandle);
Scr_FreeThread_t Scr_FreeThread = (Scr_FreeThread_t)(0x0048f640);

uint32_t CodeCallback_Custom = 0;

////////////////////////////////
struct Vector3 {
	float x, y, z;
};

class gentity_t
{
public:
	char pad_0000[16]; //0x0000
	int32_t time; //0x0010
	char pad_0014[4]; //0x0014
	Vector3 origin; //0x0018
	char pad_0024[28]; //0x0024
	Vector3 viewangles; //0x0040
	char pad_004C[768]; //0x004C
};

gentity_t* g_entities;
/// ////////////////////////////

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
	if (GetAsyncKeyState(VK_END) & 1) showImGui = !showImGui;

	ImGuiIO& io = ImGui::GetIO();
	io.MouseDrawCursor = showImGui;

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
	}

	// Alt Context Menu
	if (wParam == SC_KEYMENU && (lParam >> 16) <= 0) return 0;

	return CallWindowProc(o_WndProc, hWnd, uMsg, wParam, lParam);
}

void InitOpenGL3(HDC hDc)
{
	if (WindowFromDC(hDc) == hWnd) return; // Nothing changed, everything initialized

	if (WindowFromDC(hDc) != hWnd && initImGui) // Window handle changed, need to re-init
	{
		std::cout << "[ImGui] - Window handle changed, destroying context" << std::endl;

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	hWnd = WindowFromDC(hDc);
	LONG wLPTR = SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)h_WndProc);

	if (!wLPTR) return;

	o_WndProc = (WNDPROC)wLPTR;
	g_WglContext = wglCreateContext(hDc);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplOpenGL3_Init("#version 460");

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	initImGui = true;
	std::cout << "[ImGui] - Initialized" << std::endl;
}

void RenderOpenGL3(HDC hDc, HGLRC WglContext)
{
	HGLRC o_WglContext = wglGetCurrentContext();
	wglMakeCurrent(hDc, WglContext);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (showImGui && g_entities)
	{
		//ImGui::ShowDemoWindow();
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Example: Property editor"))
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
		{
			for (int obj_i = 0; obj_i <= 1023; obj_i++)
			{
				gentity_t ent = g_entities[obj_i];

				ImGui::PushID(obj_i);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				bool node_open = ImGui::TreeNode("Object", "%s_%u", "gentity_", obj_i);
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("Time %d (%f, %f, %f)", ent.time, ent.origin.x, ent.origin.y, ent.origin.z);

				ImGui::PopID();
			}
			ImGui::EndTable();
		}
		ImGui::PopStyleVar();
		ImGui::End();
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	wglMakeCurrent(hDc, o_WglContext);
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

/// SetPhysicalCursorPos //////////////////////////////////////////////
DWORD GScr_LoadGameTypeScript_Ret;
const char* s_callbacksetup = "maps/mp/gametypes/_callbacksetup";
const char* s_PlayerConnect = "CodeCallback_Custom";
_declspec(naked) void GScr_LoadGameTypeScript_h()
{
	_asm pushad

	// Scr_LoadScript
	_asm
	{
		push [s_callbacksetup]
		mov eax, [s_callbacksetup]
		mov edi, 0x00480150
		call edi
		add esp, 0x4
	}

	// Scr_GetFunctionHandle
	_asm
	{
		push [s_PlayerConnect]
		push [s_callbacksetup]
		mov eax, 0x0047FE50
		call eax
		mov CodeCallback_Custom, eax
		add esp, 0x8
	}

	_asm popad
	_asm
	{
		sub esp,0x44

		mov eax, uo_game_mp_x86
		add eax, 0x00082650

		mov eax, [eax]
		jmp [GScr_LoadGameTypeScript_Ret]
	}
}
///////////////////////////////////////////////////////////////////////

/// Shoot Callback ///////////////////////////////////////////////////
DWORD ShootCallback_Ret;
_declspec(naked) void ShootCallback_h()
{
	_asm pushad

	if (CodeCallback_Custom != 0)
	{
		_asm
		{
			push 0
			push CodeCallback_Custom
			mov eax, 0x0048f3e0
			call eax // Scr_ExecThread

			push eax
			mov eax, 0x0048f640
			call eax // Scr_FreeThread

			add esp, 0xC
		}

	}

	_asm popad

	_asm
	{
		lea ecx, dword ptr ds : [ecx + eax * 0x4 + 0x334]
		sub dword ptr ds : [ecx], esi

		jmp [ShootCallback_Ret]
	}
}
///////////////////////////////////////////////////////////////////////

void OnAttach()
{
	g_entities = (gentity_t*)(uo_game_mp_x86 + 0x00118d40);

	Detour(uo_game_mp_x86 + 0x000361c0, GScr_LoadGameTypeScript_h, 8, &GScr_LoadGameTypeScript_Ret);
	Detour(uo_game_mp_x86 + 0x122A6, ShootCallback_h, 9, &ShootCallback_Ret);
}

DWORD WINAPI MainThread(LPVOID param)
{
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

	//baseAddress = (DWORD)GetModuleHandleA(NULL);
	process = GetCurrentProcess();

	LoadLibrary_o = (LoadLibrary_t)LoadFromDLL("kernel32.dll", "LoadLibraryA");
	Detour(baseAddress + 0x6B8FB, LoadLibraryA_h, 6, &LoadLibraryA_Ret);
	
	wglSwapBuffers_o = (wglSwapBuffers_t)LoadFromDLL("opengl32.dll", "wglSwapBuffers");
	Detour(baseAddress + 0xF6723, wglSwapBuffers_h, 6, &wglSwapBuffers_Ret);
	
	SetPhysicalCursorPos_o = (SetPhysicalCursorPos_t)LoadFromDLL("user32.dll", "SetPhysicalCursorPos");
	Detour(baseAddress + 0x69C3B, SetPhysicalCursorPos_h, 6, &SetPhysicalCursorPos_Ret);

	while (true)
	{
		Sleep(1);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

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