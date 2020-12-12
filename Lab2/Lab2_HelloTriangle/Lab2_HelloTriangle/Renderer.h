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

	//Texture
	ID3D11Texture2D* texture2d;
	ID3D11ShaderResourceView* textureShaderResourceView;
	ID3D11SamplerState* samplerState;	//Settings for sampling

	//Constant buffers and structs for them
	ID3D11Buffer* constBufPerFrameWVP;
	ID3D11Buffer* constBufPerFrameLight;

	struct constBufWVP
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};
	constBufWVP cbWVP;

	struct constBufLight
	{
		DirectX::XMFLOAT3 lightpos;
		float padding0;
		DirectX::XMFLOAT3 lightColour;
		float lightRange;
		DirectX::XMFLOAT3 camPos;
		float padding1;
	};
	constBufLight cbLight;

public:
	Renderer() = default;
	~Renderer();
	
	bool InitD3D11(UINT width, UINT height, HWND window);
	bool InitPipeline();
	void Draw(float rotation, UINT width, UINT height);

	bool LoadWVP(UINT width, UINT height);
	bool LoadLight();

	void UpdateWVP(float rotation, UINT width, UINT height);
	void UpdateLight();
};
