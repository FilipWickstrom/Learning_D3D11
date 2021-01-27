#include "WindowHandler.h"

//Help-function for setting up the window
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //Checks messages. Can check more if needed
    switch (message)
    {
    case WM_DESTROY:    //Check if window is destroyed
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }
    //On default, takes care of the message
    return DefWindowProc(hWnd, message, wParam, lParam);
}

bool SetupWindow(HINSTANCE instance, UINT winWidth, UINT winHeight, int nCmdShow, HWND& window)
{
    const wchar_t CLASS_NAME[] = L"Window Class"; 
    WNDCLASS wc = {};   //Resets the values

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    //Creates the window. Overlapped, pop-up or child window with an extended style 
    window = CreateWindowEx(0, CLASS_NAME, L"3D project", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, winWidth, winHeight, nullptr, nullptr, instance, nullptr);

    if (window == nullptr)
    {
        std::cerr << "HWND was nullptr, last error: " << GetLastError() << std::endl;
        return false;
    }

    ShowWindow(window, nCmdShow);
    return true;
}