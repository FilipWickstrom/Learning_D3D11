#pragma once

bool SetupWindow(HINSTANCE instance,	//OS uses this to identify the exe
				 UINT winWidth,			//Size
				 UINT winHeight,		//Size
				 int nCmdShow,			//If window is minimized, maximized, normal etc...
				 HWND& window);			//The window to draw on
