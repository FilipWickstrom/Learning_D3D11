#include <chrono>			//Time
#include <sstream>			//Writing in setwindowtext
#include <fcntl.h>			//Console
#include <io.h>				//Console
#include "WindowHelper.h"	//Includes windows, iostream
#include "Renderer.h"		//Includes windows, d3d11

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
#ifdef _DEBUG
	RedirectIOToConsole();
#endif // _DEBUG

	const UINT WIDTH = 1920;
	const UINT HEIGHT = 1080;
	HWND window;

	//Setup the window where everything can be drawn
	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShow, window))
	{
		std::cerr << "Failed to setup window..." << std::endl;
		return -1;
	}

	//Holds some functions for the rendering
	Renderer renderer;

	//Setup d3d11
	if (!renderer.InitD3D11(WIDTH, HEIGHT, window))
	{
		std::cerr << "Failed to setup d3d11..." << std::endl;
		return -1;
	}
	
	//Setup pipeline
	if (!renderer.InitPipeline())
	{
		std::cerr << "Failed to setup pipeline..." << std::endl;
		return -1;
	}

	//Setup the constant buffer for wvp
	if (!renderer.LoadWVP(WIDTH, HEIGHT))
	{
		std::cerr << "Failed to setup constant buffer for wvp..." << std::endl;
		return -1;
	}

	//Setup the constant buffer for the light
	if (!renderer.LoadLight())
	{
		std::cerr << "Failed to setup constant buffer for the light..." << std::endl;
		return -1;
	}

	MSG msg = {};
	float rotation = 0.0f;
	float speed = 0.5f;	//Basespeed of the rotation

	//Gameloop
	while (msg.message != WM_QUIT)
	{
		auto start = std::chrono::steady_clock::now();

		//Looks at the message and removes it from message queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		renderer.Draw(rotation * speed, WIDTH, HEIGHT);

		//Calculates deltatime
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<float>deltatime = end - start;
		
		rotation += deltatime.count();

		//When the rotation*speed reaches 2PI (360 degrees), we reset rotation to 0.0f
		if ( (rotation * speed) >= DirectX::XM_2PI)
		{
			rotation = 0.0f;
		}

		//Update the window text
		std::wstringstream windowtext;
		windowtext << L"Hello Triangle | FPS: " << (int)(1.0f / deltatime.count()) << " | TimePerFrame : " << deltatime.count() << "s";
		SetWindowText(window, windowtext.str().c_str());
	}

	return 0;
}