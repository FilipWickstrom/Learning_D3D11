#pragma once
#include <d3d11.h>
#include <string>
#include <iostream>		//Error handling
#include <fstream>		//Open files

/*--------------------------For renderer-----------------------------*/

//Setups the swapchain that handles how it should be rendered
bool CreateDeviceAndSwapChain(HWND window,
							  UINT winWidth,
							  UINT winHeight, 
							  IDXGISwapChain*& swapChain, 
							  ID3D11Device*& device,
							  ID3D11DeviceContext*& deviceContext);

/*-----------------First and second pass uses this-------------------*/

//Reading shader files and creates vertex- and pixelshader
bool InitializeShaders(ID3D11Device* device,
					   ID3D11VertexShader*& vertexShader,
					   std::string vsPath,
					   ID3D11PixelShader*& pixelShader,
					   std::string psPath, 
					   std::string& vsByteCode);	//Return value for later use

//Initializing the sampler that uses anisotropic filtering
bool InitializeAnisoSampler(ID3D11Device* device,
							ID3D11SamplerState*& sampler);

//Sets the viewport to the size of the window
void InitializeViewport(D3D11_VIEWPORT& viewport,
						UINT winWidth, 
						UINT winHeight);

//Initializing the depth buffer 
bool InitializeDepthBuffer(ID3D11Device* device,
						   ID3D11Texture2D*& depthTexture,
						   ID3D11DepthStencilView*& depthView, 
						   UINT winWidth,
						   UINT winHeight);