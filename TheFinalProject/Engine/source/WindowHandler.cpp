#include "PCH.h"
#include "WindowHandler.h"

//Help-function for setting up the window
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //Checks messages. Can check more if needed
    switch (message)
    {
    //Is window destroyed?
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    
    case WM_ACTIVATEAPP:
        DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
        DirectX::Mouse::ProcessMessage(message, wParam, lParam);
        break;

    //Mouse handling
    case WM_INPUT:
    case WM_MOUSEMOVE:
    case WM_MOUSEHOVER:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        DirectX::Mouse::ProcessMessage(message, wParam, lParam);
        break;

    //Keyboardhandling
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
        break;

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
    window = CreateWindowEx(0, CLASS_NAME, L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, winWidth, winHeight, nullptr, nullptr, instance, nullptr);

    if (window == nullptr)
    {
        std::cerr << "HWND was nullptr, last error: " << GetLastError() << std::endl;
        return false;
    }

    ShowWindow(window, nCmdShow);
    return true;
}