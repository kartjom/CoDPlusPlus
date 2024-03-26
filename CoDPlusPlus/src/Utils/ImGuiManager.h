#ifdef CLIENT
#pragma once

#include <Windows.h>

namespace ImGuiManager
{
    inline HWND hWnd = nullptr;
    inline HGLRC wglContext = nullptr;

    inline bool IsInitialized = false;
    inline bool InteractiveMode = false;

    inline struct {
        float fov = 80.f;
        bool force_fov = false;

        bool show_console;

        bool draw_gentity = false;
        bool draw_gentity_window = false;
        int gentity_window_max = 32;
    } DevGuiState = {};

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