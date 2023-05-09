#pragma once
#include <Windows.h>

namespace WinApiHelper
{
	inline WNDPROC o_WndProc = nullptr;

	LRESULT CALLBACK h_WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void CreateConsole();
	void InjectDetours();
};