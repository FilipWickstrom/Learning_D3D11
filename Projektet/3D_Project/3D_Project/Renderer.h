#pragma once
#include "WindowHandler.h"
#include "D3D11Handler.h"
#include "FirstPassShader.h"

#include "MeshObject.h"
#include "Camera.h"

class Renderer
{
private:
	UINT m_winWidth;
	UINT m_winHeight;
	
	//Directx
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	IDXGISwapChain* m_swapChain;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilTexture;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;
	
	FirstPassShader m_firstpass;
	//SecondPassShader m_secondpass

	MeshObject m_mesh0;
	MeshObject m_mesh1;

	constantBufferWVP m_cbWVP;	//change name??
	ID3D11Buffer* m_WVPBuffer;	//change name??
	
	
	//Camera* m_camera;
	//std::vector <MeshObject> m_meshes;
	float m_rotationtest;
	//Deltatime
private:
	//keyboardmovement();
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
