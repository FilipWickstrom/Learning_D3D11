#include "PCH.h"
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
	//Can use the console in debugging mode
	#ifdef _DEBUG
		RedirectIOToConsole();
	#endif

	HWND window = {};

	//Set renderwindow to the size of the monitor
	Renderer renderer(GetSystemMetrics(SM_CXSCREEN),
					  GetSystemMetrics(SM_CYSCREEN));

	//Setting up windowhandler, directx handler, pipeline, etc...
	if (renderer.Setup(hInstance, nCmdShow, window))
		renderer.StartGameLoop(window);
	else
		std::cerr << "Renderer.Setup() failed..." << std::endl;

	return 0;
}