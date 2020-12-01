#include <Windows.h>
#include <iostream>
#include <d3d11.h>
#include <chrono> //time
#include <string>
#include <sstream> //writing in setwindowtext

//Help functions
#include "WindowHelper.h"
#include "D3D11Helper.h"
#include "PipelineHelper.h"

void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport,
			ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout, ID3D11Buffer* vertexBuffer,
			ID3D11ShaderResourceView* textureSRV, ID3D11SamplerState* sampler)
{
	float clearColour[4] = { 0, 0, 0, 0 };	 //Overriting screen with this colour
	//Cleaning before new frame
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	//update rotation

	UINT stride = sizeof(SimpleVertex);		//Distance between vertices in bytes
	UINT offset = 0;	//Where to start in the array
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetShader(pShader, nullptr, 0);

	//For texture
	immediateContext->PSSetShaderResources(0, 1, &textureSRV);
	immediateContext->PSSetSamplers(0, 1, &sampler);

	immediateContext->OMSetRenderTargets(1, &rtv, dsView);
	immediateContext->Draw(4, 0);	//4 vertices is needed for a quad
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	const UINT WIDTH = 1024;
	const UINT HEIGHT = 576;
	HWND window;

	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShow, window))
	{
		std::cerr << "Failed to setup window..." << std::endl;
		return -1;
	}

	//I = Interface
	ID3D11Device* device;
	ID3D11DeviceContext* immediateContext;
	IDXGISwapChain* swapChain;		//Handles backbuffer
	ID3D11RenderTargetView* rtv;	//Render target view
	ID3D11Texture2D* dsTexture;		//Depth stencil texture
	ID3D11DepthStencilView* dsView;
	D3D11_VIEWPORT viewport;

	//For the triangle
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vertexbuffer;

	//WVP
	//ID3D11Buffer* constbuffer;

	//Texture
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* textureSRV;
	ID3D11SamplerState* sampler;	//Settings for sampling

	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, swapChain, rtv, dsTexture, dsView, viewport))
	{
		std::cerr << "Failed to setup d3d11..." << std::endl;
		return -1;
	}

	if (!SetupPipeline(device, vertexbuffer, vShader, pShader, inputLayout, immediateContext, texture, textureSRV, sampler))
	{
		std::cerr << "Failed to setup pipeline..." << std::endl;
		return -1;
	}

	MSG msg = {};
	double dt = 0;
	//Update loop. Stops when closing the window
	while (msg.message != WM_QUIT)
	{
		//Start timer
		auto start = std::chrono::steady_clock::now();

		//Check if msg exists
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//Calculates the light (later)*** Should be calculated in pixelshader...
		Render(immediateContext, rtv, dsView, viewport, vShader, pShader, inputLayout, vertexbuffer, textureSRV, sampler);
		swapChain->Present(0, 0);	//Actually draws the window


		//Calculates the difference in time and prints to windowtext (COULD MOVE TO SEPARATE FILE...******)
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double>deltatime = end - start;
		std::wstringstream windowtext;
		windowtext << L"Hello Triangle Window | FPS: "  << (int)(1.0f/ deltatime.count()) << " | TimePerFrame : " << deltatime.count() << "ms";
		SetWindowText(window, windowtext.str().c_str());

		dt = deltatime.count();	//use this for rotation later.
	}

	OutputDebugString(L"Testing debug: Closing down the window...\n");	//****Just for testing debug messages

	//Releasing all pointers to avoid errors
	sampler->Release();
	textureSRV->Release();
	texture->Release();
	vertexbuffer->Release();
	inputLayout->Release();
	pShader->Release();
	vShader->Release();
	dsView->Release();
	dsTexture->Release();
	rtv->Release();
	swapChain->Release();
	immediateContext->Release();
	device->Release();

	return 0;
}
