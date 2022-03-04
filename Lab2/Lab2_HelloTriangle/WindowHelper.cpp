#include "WindowHelper.h"

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

bool SetupWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow, HWND& window)
{
    const wchar_t CLASS_NAME[] = L"Hello triangle Window Class";
    WNDCLASS wc = {};   //Resets everything to 0/nullptr or something like that

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    //Creates the real window with all the values
    window = CreateWindowEx(0, CLASS_NAME, L"Hello Triangle Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height, nullptr, nullptr, instance, nullptr);

    if (window == nullptr)
    {
        std::cerr << "HWND was nullptr, last error: " << GetLastError() << std::endl;
        return false;
    }

    ShowWindow(window, nCmdShow);
    return true;
}