#pragma once
#include <d3d11.h>
#include "D3D11Helper.h"
#include "PipelineHelper.h"

class Renderer
{
private:
	float clearColour[4] = { 0, 0, 0, 0 };

	//I indicates interface
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	IDXGISwapChain* swapChain;		//Handles backbuffer
	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* depthStencilTexture;
	ID3D11DepthStencilView* depthStencilView;
	D3D11_VIEWPORT viewport;

	//For the triangle
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vertexbuffer;

	//WVP
	ID3D11Buffer* constBufferPerFrame;

	//Texture
	ID3D11Texture2D* texture2d;
	ID3D11ShaderResourceView* textureShaderResourceView;
	ID3D11SamplerState* samplerState;	//Settings for sampling

	//Some kind of light buffer

	struct ConstantBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};
	ConstantBuffer constBuf;

public:
	Renderer() = default;
	~Renderer();
	
	bool InitD3D11(UINT width, UINT height, HWND window);
	bool InitPipeline();
	void Draw(float rotation, UINT width, UINT height);

	bool loadConstBuffer();
	void updateConstBuffer(float rotation, UINT width, UINT height);

};
