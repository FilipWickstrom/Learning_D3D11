#pragma once
#include <d3d11.h>
#include <string>
#include <iostream>
#include <fstream>

//SOME MORE COMMENTS***
bool InitializeShaders(ID3D11Device* device,
					  ID3D11VertexShader*& vertexShader,
					  std::string vsPath,
					  ID3D11PixelShader*& pixelShader,
					  std::string psPath, 
					  std::string& vsByteCode);

bool InitializeAnisoSampler(ID3D11Device* device,
							ID3D11SamplerState*& sampler);

void InitializeViewport(D3D11_VIEWPORT& viewport,
						UINT winWidth, 
						UINT winHeight);

bool InitializeDepthBuffer(ID3D11Device* device,
						   ID3D11Texture2D*& depthTexture,
						   ID3D11DepthStencilView*& depthView, 
						   UINT winWidth,
						   UINT winHeight);