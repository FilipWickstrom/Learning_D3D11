#pragma once
#include "InitFunctions.h"
#include "ScreenQuad.h"
#include <vector>

class SecondPass
{
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11SamplerState* m_anisoSampler;
	ID3D11InputLayout* m_inputLayout;

	ID3D11RenderTargetView* m_backbuffer;
	ScreenQuad m_screenQuad;
	D3D11_VIEWPORT m_viewport;

	float m_clearColour[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

private:
	bool InitInputLayout(ID3D11Device* device, std::string vsByteCode);

	//Init some kind of resource view and stuff?
	bool InitRenderTargetView(ID3D11Device* device, IDXGISwapChain* swapChain);

public:
	SecondPass();
	~SecondPass();

	//Full initialization of everything needed for the first pass to work
	bool Initialize(ID3D11Device* device, UINT winWidth, UINT winHeight, IDXGISwapChain* swapChain);

	//Binding everything needed including all the gbuffers
	void Bind(ID3D11DeviceContext* deviceContext, std::vector<ID3D11ShaderResourceView*>gbuffers);

};