#pragma once
#include "PassInitFunc.h"

const int NROFBUFFERS = 2;


class FirstPass
{
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11SamplerState* m_anisoSampler;
	ID3D11InputLayout* m_inputLayout;
	//constant buffer for transformatrix

	//G-Buffers
	ID3D11Texture2D* m_renderTargetTextures[NROFBUFFERS];
	ID3D11RenderTargetView* m_renderTargetViews[NROFBUFFERS];
	ID3D11ShaderResourceView* m_shaderResourceViews[NROFBUFFERS];			//where the pixelshader returns its data

	ID3D11Texture2D* m_depthTexture;		//placera i renderer senare
	ID3D11DepthStencilView* m_depthView;	//placera i render istället
	D3D11_VIEWPORT m_viewport;

	//IN MESH SET TEXTURE 2D TO WHICH IT WANTS

	float m_clearColour[4] = { 0.0f, 1.0f, 0.0f, 0.0f };

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

	void Bind(ID3D11DeviceContext* deviceContext);		//change to bool later???

	ID3D11ShaderResourceView* GetShaderResourceView(int index);


	//Change to this for the devicecontext so that it know to use all of this objects and instructions
	//Bind()	so that is works with the current setup.
	//		set outmerger to this render targets
	//		clear all the rendertargets.
	//		bind the right input layout
	//		bind everything else that is useful
	//			texture from objects?
	//			sampler
	//			constantbuffer which holds transformation

};