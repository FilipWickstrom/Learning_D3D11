#pragma once
#include "WindowHandler.h"
#include "FirstPass.h"
#include "SecondPass.h"

#include "MeshObject.h"
#include "Camera.h"

#include "ScreenQuad.h"

//SOME SETTINGS?
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
	ScreenQuad m_screenQuad;

	std::vector<MeshObject*> m_objects;

	constantBufferWVP m_cbWVP;	//change name??
	ID3D11Buffer* m_WVPBuffer;	//change name??
	
	
	//Camera* m_camera;
	float m_rotationtest;
	//Deltatime
private:
	//keyboardmovement();
	bool CreateDeviceAndSwapChain(HWND window);
	bool SetupWVP_CB();
	void Render();

public:
	Renderer(UINT winWidth, UINT winHeight);
	~Renderer();

	//Setting up window handeler, directx handler and etc
	bool Setup(HINSTANCE hInstance, int nCmdShow, HWND& window);

	//Game loop
	void StartGameLoop();

};
