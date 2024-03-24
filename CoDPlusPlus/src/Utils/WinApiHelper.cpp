#include <Utils/WinApiHelper.h>
#include <Utils/ImGuiManager.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>

#include <Hook/Hook.h>
#include <Hook/Detours.h>

#include <psapi.h>
#include <filesystem>
#include <fstream>
#include <format>
#include <iomanip>
#include <ImageHlp.h>

#pragma comment(lib, "dbghelp.lib")

namespace WinApiHelper
{
	IMAGEHLP_MODULE GetModuleInfo(PVOID address)
	{
		HANDLE proc = GetCurrentProcess();

		IMAGEHLP_MODULE moduleInfo;
		moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
		SymGetModuleInfo(proc, (DWORD)address, &moduleInfo);

		return moduleInfo;
	}

	LONG WINAPI CrashLogger(struct _EXCEPTION_POINTERS* pExceptionInfo)
	{
		HANDLE process = GetCurrentProcess();
		HANDLE thread = GetCurrentThread();

		SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);
		SymInitialize(process, nullptr, TRUE);

		std::filesystem::create_directory("codplusplus");
		std::ofstream outfile("codplusplus/crash.log", std::ios::app);

		time_t t = time(0);
		std::tm tm = *std::localtime(&t);
		std::string exceptionAtModule = GetModuleInfo(pExceptionInfo->ExceptionRecord->ExceptionAddress).ModuleName;

		outfile << "------------- Crash Log [" << std::put_time(&tm, "%d.%m.%Y %H:%M:%S") << "] -------------\n";
		outfile << std::format("Exception code 0x{:08x} at memory address {} in module {}\n", pExceptionInfo->ExceptionRecord->ExceptionCode, pExceptionInfo->ExceptionRecord->ExceptionAddress, exceptionAtModule);
		
		outfile << std::format("\nEsp 0x{:08x}", pExceptionInfo->ContextRecord->Esp);
		outfile << std::format("\nEbp 0x{:08x}", pExceptionInfo->ContextRecord->Ebp);

		outfile << std::format("\nEdi 0x{:08x}", pExceptionInfo->ContextRecord->Edi);
		outfile << std::format("\nEsi 0x{:08x}", pExceptionInfo->ContextRecord->Esi);
		outfile << std::format("\nEbx 0x{:08x}", pExceptionInfo->ContextRecord->Ebx);
		outfile << std::format("\nEdx 0x{:08x}", pExceptionInfo->ContextRecord->Edx);
		outfile << std::format("\nEcx 0x{:08x}", pExceptionInfo->ContextRecord->Ecx);
		outfile << std::format("\nEax 0x{:08x}", pExceptionInfo->ContextRecord->Eax);

		outfile.flush();
		outfile << "\n\n";

		// Callstack
		outfile << "Callstack:\n";

		CONTEXT* context = pExceptionInfo->ContextRecord;
		STACKFRAME64 stackFrame;
		memset(&stackFrame, 0, sizeof(STACKFRAME));

		stackFrame.AddrPC.Offset = context->Eip;
		stackFrame.AddrPC.Mode = AddrModeFlat;
		stackFrame.AddrFrame.Offset = context->Ebp;
		stackFrame.AddrFrame.Mode = AddrModeFlat;
		stackFrame.AddrStack.Offset = context->Esp;
		stackFrame.AddrStack.Mode = AddrModeFlat;

		int frameCount = 0;
		while (StackWalk64(IMAGE_FILE_MACHINE_I386, process, thread, &stackFrame, context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
		{
			DWORD64 address = stackFrame.AddrPC.Offset;
			auto moduleInfo = GetModuleInfo((PVOID)address);

			outfile << std::format("Frame {}: 0x{:08x} in {}.{:08x}\n", frameCount++, address, moduleInfo.ModuleName, address - moduleInfo.BaseOfImage);

			if (address == 0) {
				break;
			}
		}

		outfile.flush();
		outfile << "\n";

		// Modules
		outfile << "Modules:\n";

		HMODULE moduleHandles[1024];
		DWORD bytesNeeded;
		if (EnumProcessModules(process, moduleHandles, sizeof(moduleHandles), &bytesNeeded))
		{
			int moduleCount = bytesNeeded / sizeof(HMODULE);

			for (int i = 0; i < moduleCount; ++i)
			{
				char moduleName[MAX_PATH];

				if (K32GetModuleBaseNameA(process, moduleHandles[i], moduleName, sizeof(moduleName) / sizeof(char)))
				{
					outfile << moduleName << " - " << "0x" << moduleHandles[i] << "\n";
				}
			}
		}

		outfile << "-----------------------------------------------------------\n";

		outfile.flush();

		if (outfile.is_open())
			outfile.close();

		SymCleanup(process);

		return EXCEPTION_EXECUTE_HANDLER;
	}

	void SetExceptionFilters()
	{
		SetUnhandledExceptionFilter(CrashLogger);
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
		io.MouseDrawCursor = ImGuiManager::InteractiveMode;

		if (ImGuiManager::InteractiveMode)
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