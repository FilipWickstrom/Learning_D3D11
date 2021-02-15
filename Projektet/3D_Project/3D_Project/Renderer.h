#pragma once
#include "WindowHandler.h"
#include "FirstPass.h"
#include "SecondPass.h"
#include "ConstantBuffers.h"
#include "MeshObject.h"
#include "Camera.h"
#include "FPSCounter.h"

#include "Movement.h"

//Settings that can be changed. Put in separate file and read from later?
//Vsync on/off
//SSAO on/off later?

class Renderer
{
private:
	//Window settings
	UINT m_winWidth;
	UINT m_winHeight;
	
	//Points to all the buffers
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	IDXGISwapChain* m_swapChain;

	//Deferred rendering
	FirstPass m_firstPass;
	SecondPass m_secondPass;

	//Hold all objects in the map
	std::vector<MeshObject*> m_objects;

	//For the camera
	ConstantBuffers m_constBuffers;
	Camera m_camera;
	Movement m_movement;


	FPSCounter m_fpscounter;
	float m_deltatime;

private:
	//keyboardmovement();
	void Render();

public:
	Renderer(UINT winWidth, UINT winHeight);
	~Renderer();

	//Setting up window handeler, directx handler and etc
	bool Setup(HINSTANCE hInstance, int nCmdShow, HWND& window);

	//Game loop
	void StartGameLoop(HWND& window);

};
