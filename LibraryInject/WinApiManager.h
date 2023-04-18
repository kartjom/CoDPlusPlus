#pragma once
#include <Windows.h>

class WinApiManager
{
public:
	static WNDPROC o_WndProc;

	static LRESULT CALLBACK h_WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

