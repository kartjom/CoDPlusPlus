#pragma once
#include <Windows.h>

class WinApiHelper
{
public:
	static WNDPROC o_WndProc;

	static LRESULT CALLBACK h_WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static void CreateConsole();
	static void CreateDetours();
};