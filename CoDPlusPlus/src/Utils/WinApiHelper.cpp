#include <Utils/WinApiHelper.h>
#include <Utils/ImGuiManager.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>

#include <Hook/Hook.h>
#include <Hook/Detours.h>

#include <psapi.h>
#include <fstream>
#include <format>
#include <iomanip>

namespace WinApiHelper
{
	LONG WINAPI CustomUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* pExceptionInfo)
	{
		std::ofstream outfile;
		outfile.open("codplusplus_crash.log", std::ios::app);

		time_t t = time(0);
		std::tm tm = *std::localtime(&t);

		outfile << "------------- Crash Log [" << std::put_time(&tm, "%d.%m.%Y %H:%M:%S") << "] -------------\n";
		outfile << std::format("At memory address {} code 0x{:x}\n", pExceptionInfo->ExceptionRecord->ExceptionAddress, pExceptionInfo->ExceptionRecord->ExceptionCode);
		
		outfile << std::format("\nESP 0x{:x}", pExceptionInfo->ContextRecord->Esp);
		outfile << std::format("\nEBP 0x{:x}", pExceptionInfo->ContextRecord->Ebp);

		outfile << std::format("\nEDI 0x{:x}", pExceptionInfo->ContextRecord->Edi);
		outfile << std::format("\nESI 0x{:x}", pExceptionInfo->ContextRecord->Esi);
		outfile << std::format("\nEBX 0x{:x}", pExceptionInfo->ContextRecord->Ebx);
		outfile << std::format("\nEDX 0x{:x}", pExceptionInfo->ContextRecord->Edx);
		outfile << std::format("\nECX 0x{:x}", pExceptionInfo->ContextRecord->Ecx);
		outfile << std::format("\nEAX 0x{:x}", pExceptionInfo->ContextRecord->Eax);

		outfile << "\n\n";

		HMODULE moduleHandles[1024];
		DWORD bytesNeeded;
		HANDLE processHandle = GetCurrentProcess();
		if (EnumProcessModules(processHandle, moduleHandles, sizeof(moduleHandles), &bytesNeeded))
		{
			int moduleCount = bytesNeeded / sizeof(HMODULE);

			for (int i = 0; i < moduleCount; ++i)
			{
				char moduleName[MAX_PATH];

				if (K32GetModuleBaseNameA(processHandle, moduleHandles[i], moduleName, sizeof(moduleName) / sizeof(char)))
				{
					outfile << moduleName << " - " << "0x" << moduleHandles[i] << "\n";
				}
			}
		}

		outfile << "-----------------------------------------------------------\n";

		return EXCEPTION_EXECUTE_HANDLER;
	}

	void SetExceptionFilters()
	{
		SetUnhandledExceptionFilter(CustomUnhandledExceptionFilter);
	}

	bool CheckGame()
	{
		char baseName[32];
		if (GetModuleBaseNameA(GetCurrentProcess(), nullptr, baseName, 32))
		{
			if (_stricmp(baseName, "CoDUOMP.exe") == 0)
			{
				return true;
			}
		}

		return false;
	}

	void CreateConsole(const char* title)
	{
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

		SetConsoleTitleA(title);
	}

	void InjectDetours()
	{
		Detours::LoadLibraryA_o = Hook::LoadFromDLL<LoadLibraryA_t>("kernel32.dll", "LoadLibraryA");
		DetourRet(Hook::BaseAddress + 0x6B8FB, Detours::LoadLibraryA, 6);

		Detours::FreeLibrary_kernelbase = Hook::LoadFromDLL<DWORD>("kernelbase.dll", "FreeLibrary");
		DetourRet(Hook::LoadFromDLL<DWORD>("kernel32.dll", "FreeLibrary") + 6, Detours::FreeLibrary, 6);

	#ifdef CLIENT
		Detours::SetPhysicalCursorPos_o = Hook::LoadFromDLL<SetPhysicalCursorPos_t>("user32.dll", "SetPhysicalCursorPos");
		DetourRet(Hook::BaseAddress + 0x69C3B, Detours::SetPhysicalCursorPos, 6);
	#endif
	}
}

#ifdef CLIENT

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace WinApiHelper
{
	LRESULT CALLBACK h_WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
}

#endif