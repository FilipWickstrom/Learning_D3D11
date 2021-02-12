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
	UINT m_winWidth;
	UINT m_winHeight;
	
	//Hold the most useful stuff
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	IDXGISwapChain* m_swapChain;

	FirstPass m_firstPass;
	SecondPass m_secondPass;

	std::vector<MeshObject*> m_objects;

	ConstantBuffers m_constBuffers;

	
	Camera m_camera;
	Movement m_movement;

	float m_rotationtest;	//****remove


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
