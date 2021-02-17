#pragma once
#include "InitFunctions.h"

const int NROFBUFFERS = 3;


class FirstPass
{
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11SamplerState* m_anisoSampler;
	ID3D11InputLayout* m_inputLayout;
	//constant buffer for transformatrix

	//G-Buffers
	ID3D11Texture2D* m_renderTargetTextures[NROFBUFFERS] = { nullptr };
	ID3D11RenderTargetView* m_renderTargetViews[NROFBUFFERS] = { nullptr };
	ID3D11ShaderResourceView* m_shaderResourceViews[NROFBUFFERS] = { nullptr }; //where the pixelshader returns its data
	ID3D11ShaderResourceView* m_nullptrs[NROFBUFFERS] = { nullptr };

	ID3D11Texture2D* m_depthTexture;
	ID3D11DepthStencilView* m_depthView;
	D3D11_VIEWPORT m_viewport;

	float m_clearColour[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

private:
	bool InitInputLayout(ID3D11Device* device, std::string vsByteCode);
	
	//Initialize rendertarget textures - textures that the shader can write to
	bool InitRTtextures(ID3D11Device* device, UINT winWidth, UINT winHeight);
	
	//Initialize rendertarget views - for accessing the textures
	bool InitRTviews(ID3D11Device* device);

	//Initialize rendertarget shaders - for accessing the textures through the shaders
	bool InitRTshaders(ID3D11Device* device);

public:
	FirstPass();
	~FirstPass();

	//Full initialization of everything needed for the first pass to work
	bool Initialize(ID3D11Device* device, UINT winWidth, UINT winHeight);

	//Bind the most relavant to render to the first pass
	void Bind(ID3D11DeviceContext* deviceContext);

	//Getting the gbuffers shader resource view with the save data. Needed in the second pass
	ID3D11ShaderResourceView* GetShaderResourceView(int index);

};