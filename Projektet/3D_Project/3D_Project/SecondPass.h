#pragma once
#include "PassInitFunc.h"

class SecondPass
{
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11SamplerState* m_anisoSampler;
	ID3D11InputLayout* m_inputLayout;
	//constant buffer for transformatrix

	ID3D11RenderTargetView* m_backbuffer;

	//screen quad

	ID3D11Texture2D* m_depthTexture;		//räcker med en...
	ID3D11DepthStencilView* m_depthView;	//räcker med en... placera i renderer istället
	D3D11_VIEWPORT m_viewport;

	//IN MESH SET TEXTURE 2D TO WHICH IT WANTS

	float m_clearColour[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

private:
	bool InitInputLayout(ID3D11Device* device, std::string vsByteCode);

	//Init some kind of resource view and stuff?
	bool InitRenderTargetView(ID3D11Device* device, IDXGISwapChain* swapChain);

public:
	SecondPass();
	~SecondPass();

	//Full initialization of everything needed for the first pass to work
	bool Initialize(ID3D11Device* device, UINT winWidth, UINT winHeight, IDXGISwapChain* swapChain);

	void Bind(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* srv1, ID3D11ShaderResourceView* srv2);

};