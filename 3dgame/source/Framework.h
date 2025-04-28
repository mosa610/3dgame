#pragma once
#pragma once

#include <Windows.h>
#include <sstream>
#include "high_resolution_timer.h"
#include "Graphics/Graphics.h"
#include "Graphics/ImguiRenderer.h"


#define USE_IMGUI 1
CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
CONST BOOL FULLSCREEN{ TRUE };
CONST LPCWSTR APPLICATION_NAME{ L"3DGAME" };



class Framework
{
public:
    Framework(HWND hwnd, BOOL fullscreen = false);
    ~Framework();
private:
    void Update(float elapsedTime);
    void Render(float elapsedTime);

    void calculate_frame_stats();

    void stylize_window(BOOL fullscreen);

    void on_size_changed(UINT64 width, UINT height);

    void create_direct2d_objects();

public:
    int Run();
    LRESULT CALLBACK Handle_message(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    size_t video_memory_usage()
    {
        DXGI_QUERY_VIDEO_MEMORY_INFO video_memory_info;
        adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &video_memory_info);
        return video_memory_info.CurrentUsage / 1024 / 1024;
    }

private:
    const HWND          hwnd;
    high_resolution_timer tictoc;
    Graphics            graphics;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2d1_device_context;
    Microsoft::WRL::ComPtr<IDWriteTextFormat> dwrite_text_formats[8];
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> d2d_solid_color_brushes[8];
    Microsoft::WRL::ComPtr<IDXGIAdapter3> adapter;

    BOOL fullscreen_mode{ FALSE };
    BOOL vsync{ TRUE };
    BOOL tearing_supported{ FALSE };

    RECT windowed_rect;
    DWORD windowed_style;
    SIZE framebuffer_dimensions;

    float elapsed_time = 0.0f;
};