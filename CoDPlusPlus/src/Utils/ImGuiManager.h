#ifdef CLIENT
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace ImGuiManager
{
    inline HWND hWnd = nullptr;
    inline HGLRC wglContext = nullptr;

    inline bool IsInitialized = false;
    inline bool InteractiveMode = false;

    void Initialize(HDC hDc);

    HGLRC BeginFrame(HDC hDc);
    void EndFrame(HDC hDc, HGLRC o_WglContext);

    void InteractiveTick();
    void Tick();

    void Dispose();

    bool Toggle();
    bool Show();
    bool Hide();
};

#endif