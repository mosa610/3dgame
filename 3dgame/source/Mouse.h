#pragma once
#include <Windows.h>
#include <DirectXMath.h>

class Mouse
{
private:
    Mouse() {}
    ~Mouse() {}
public:
    static Mouse& GetInstance()
    {
        static Mouse instance;
        return instance;
    }

    void Update(HWND hWnd)
    {
        POINT cursor;
        RECT rc;
        ::GetCursorPos(&cursor);
        ::ScreenToClient(hWnd, &cursor);
        GetClientRect(hWnd, &rc);
        UINT screenW = rc.right - rc.left;
        UINT screenH = rc.bottom - rc.top;

        cursor_position.x = static_cast<float>(cursor.x) / screenW;
        cursor_position.y = static_cast<float>(cursor.y) / screenH;
    }
    DirectX::XMFLOAT2 GetCursorPosition() { return cursor_position; }
private:
    DirectX::XMFLOAT2 cursor_position;
};