#pragma once
#include "WindowHandler.h"
#include "FirstPass.h"
#include "SecondPass.h"
#include "ConstantBuffers.h"
#include "Scene.h"
#include "Camera.h"
#include "Movement.h"
#include "FPSCounter.h"
#include "Tessellation.h"

class Renderer
{
private:
	//Window settings
	UINT m_winWidth;
	UINT m_winHeight;

	//Settings			//Should later be changeble with ImGui
	bool m_VSync;
	
	//Points to all the buffers
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	IDXGISwapChain* m_swapChain;

	//Deferred rendering
	FirstPass m_firstPass;
	SecondPass m_secondPass;

	//Hold all the objects
	Scene m_scene;

	//For the camera
	ConstantBuffers m_constBuffers;
	Camera m_camera;
	Movement m_movement;

	Tessellation m_tessellation;

	//Framerate and frametime
	FPSCounter m_fpscounter;
	float m_deltatime;

private:
	void Render();
	void Present();

public:
	Renderer(UINT winWidth, UINT winHeight);
	~Renderer();

	//Setting up window handeler, directx handler and etc
	bool Setup(HINSTANCE hInstance, int nCmdShow, HWND& window);

	//Game loop
	void StartGameLoop(HWND& window);

};
