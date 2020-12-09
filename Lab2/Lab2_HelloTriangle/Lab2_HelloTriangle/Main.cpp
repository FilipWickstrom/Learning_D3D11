#include <Windows.h>
#include <iostream>
#include <d3d11.h>
#include <chrono>
#include <string>
#include <sstream> //writing in setwindowtext

//For console
#include <fcntl.h>
#include <io.h>

#include "WindowHelper.h"
#include "Renderer.h"

//Make it possible to use the console window
void RedirectIOToConsole()
{
	AllocConsole();
	HANDLE stdHandle;
	int hConsole;
	FILE* fp;
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsole = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
	fp = _fdopen(hConsole, "w");
	freopen_s(&fp, "CONOUT$", "w", stdout);
	std::cout << "Console is active" << std::endl;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	RedirectIOToConsole();
	const UINT WIDTH = 1280;
	const UINT HEIGHT = 720;
	HWND window;

	//Setup the window where everything can be drawn
	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShow, window))
	{
		std::cerr << "Failed to setup window..." << std::endl;
		return -1;
	}

	Renderer renderer;

	//Setup d3d11
	if (!renderer.InitD3D11(WIDTH, HEIGHT, window))
		return -1;
	
	//Setup pipeline
	if (!renderer.InitPipeline())
		return -1;

	//Setup the constant buffer
	if (!renderer.loadConstBuffer(WIDTH, HEIGHT))
		return -1;

	MSG msg = {};
	float dt = 0.0f;
	float speed = 0.5f;	//Base speed of the rotation

	//Gameloop
	while (msg.message != WM_QUIT)
	{
		auto start = std::chrono::steady_clock::now();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		renderer.Draw(dt*speed, WIDTH, HEIGHT);

		//Calculates deltatime
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double>deltatime = end - start;
		dt += (float)deltatime.count();
		
		//Update the window text
		std::wstringstream windowtext;
		windowtext << L"Hello Triangle Window | FPS: " << (int)(1.0f / deltatime.count()) << " | TimePerFrame : " << deltatime.count() << "s";
		SetWindowText(window, windowtext.str().c_str());
	}

	return 0;
}