#pragma once
#include <Windows.h>

class ImGuiManager
{
public:
    static HWND hWnd;
    static HGLRC wglContext;

    static bool IsInitialized;
    static bool ShouldShow;

    static void Initialize(HDC hDc);

    static HGLRC BeginFrame(HDC hDc);
    static void Tick();
    static void EndFrame(HDC hDc, HGLRC o_WglContext);

    static void Dispose();

    static bool Toggle();
    static bool Show();
    static bool Hide();
};