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
CONST BOOL FULLSCREEN{ FALSE };
CONST LPCWSTR APPLICATION_NAME{ L"3DGAME" };



class Framework
{
public:
    Framework(HWND hwnd);
    ~Framework();
private:
    void Update(float elapsedTime);
    void Render(float elapsedTime);

    void calculate_frame_stats();
public:
    int Run();
    LRESULT CALLBACK Handle_message(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    const HWND          hwnd;
    high_resolution_timer tictoc;
    Graphics            graphics;
};