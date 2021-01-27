#pragma once
#include "WindowHandler.h"
#include "D3D11Handler.h"
#include "PipelineHandler.h"

#include "MeshObject.h"

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

	//Shaders
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_inputLayout;

	//Textures
	ID3D11Texture2D* m_2dtexture;
	ID3D11ShaderResourceView* m_textureShaderResourceView;
	ID3D11SamplerState* m_samplerState;

	MeshObject m_mesh1;

	//Constant buffers
	//Deltatime
	//Nr of vertices in scene? How many to draw

public:
	Renderer(UINT winWidth, UINT winHeight);
	~Renderer();

	//Setting up window handeler, directx handler and etc
	bool Setup(HINSTANCE hInstance, int nCmdShow, HWND& window);

	//Game loop
	void Update();
	void Draw();	//Make private?

};
