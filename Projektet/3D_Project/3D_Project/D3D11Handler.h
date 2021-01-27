#pragma once
#include <d3d11.h>
#include <iostream>

//Settings for swapchain and creates device and swapchain
bool CreateInterfaces(UINT width, UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& deviceContext, IDXGISwapChain*& swapChain);

//Creates a render-target view that points to the back buffer
bool CreateRenderTargetView(ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11RenderTargetView*& rtv);

//Creates the description of the depth and stencil (not needed but have to be in the same)
bool CreateDepthStencil(ID3D11Device* device, UINT width, UINT height, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView);

//Sets the viewport to the size of the window
void SetViewport(D3D11_VIEWPORT& viewport, UINT width, UINT height);

//Final function - Tests and setups everything from top
bool SetupD3D11(UINT width,                             //Window size
                UINT height,
                HWND window,                            //The window to display on
                ID3D11Device*& device,                  //Virtual adapter, create resources
                ID3D11DeviceContext*& deviceContext,    //For binding resources
                IDXGISwapChain*& swapChain,             //Storing rendered data before presenting
                ID3D11RenderTargetView*& rtv,           //Identifies the render-target subresources that can be accessed during rendering
                ID3D11Texture2D*& dsTexture,            //Texture for the depth
                ID3D11DepthStencilView*& dsView,        //Accesses a texture resource during depth-stencil testing
                D3D11_VIEWPORT& viewport                //Which part to see of the screen
);

