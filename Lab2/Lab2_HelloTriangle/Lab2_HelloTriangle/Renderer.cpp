#include "Renderer.h"

Renderer::~Renderer()
{
	this->device->Release();
	this->deviceContext->Release();
	this->swapChain->Release();
	this->renderTargetView->Release();
	this->depthStencilTexture->Release();
	this->depthStencilView->Release();
	this->vertexShader->Release();
	this->pixelShader->Release();
	this->inputLayout->Release();
	this->vertexbuffer->Release();

	this->constBufferPerFrame->Release();

	this->texture2d->Release();
	this->textureShaderResourceView->Release();
	this->samplerState->Release();
}

bool Renderer::InitD3D11(UINT width, UINT height, HWND window)
{
	bool returnvalue = true;
	if (!SetupD3D11(width, height, window, device, deviceContext, swapChain, renderTargetView, depthStencilTexture, depthStencilView, viewport))
	{
		std::cerr << "Failed to setup d3d11..." << std::endl;
		returnvalue = false;
	}
	return returnvalue;
}

bool Renderer::InitPipeline()
{
	bool returnvalue = true;
	if (!SetupPipeline(device, vertexbuffer, vertexShader, pixelShader, inputLayout, texture2d, textureShaderResourceView, samplerState))
	{
		std::cerr << "Failed to setup pipeline..." << std::endl;
		returnvalue = false;
	}
	return returnvalue;
}

void Renderer::Draw(float angle, UINT width, UINT height)
{
	//Overriting screen with this colour before new frame
	deviceContext->ClearRenderTargetView(renderTargetView, clearColour);	//Clearing backbuffer
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->IASetInputLayout(inputLayout);
	
	deviceContext->VSSetShader(vertexShader, nullptr, 0);
	deviceContext->PSSetShader(pixelShader, nullptr, 0);

	UINT stride = sizeof(SimpleVertex);		//Distance between vertices in bytes
	UINT offset = 0;	//Where to start in the array
	deviceContext->IASetVertexBuffers(0, 1, &vertexbuffer, &stride, &offset);

	updateConstBuffer(angle, width, height);
	//light
	//deviceContext->PSSetConstantBuffers(0, 1, &constlightbuffer);

	//For texture
	deviceContext->PSSetShaderResources(0, 1, &textureShaderResourceView);
	deviceContext->PSSetSamplers(0, 1, &samplerState);

	//Where to draw and present the vertices
	deviceContext->RSSetViewports(1, &viewport);
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView); //OM = outputmerge
	deviceContext->Draw(4, 0);	//Drawing the 4 vertices
	swapChain->Present(1, 0);	//Syncing with Vsync
}

bool Renderer::loadConstBuffer()
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(ConstantBuffer);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initdata;
	initdata.pSysMem = &constBuf;
	initdata.SysMemPitch = 0;
	initdata.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &initdata, &constBufferPerFrame);

	return !FAILED(hr);
}

void Renderer::updateConstBuffer(float rotation, UINT width, UINT height)
{	
	//First is the worlds scale, second is for the rotation and third is for ...???...*** 
	constBuf.world = DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f) * DirectX::XMMatrixRotationY(rotation) * DirectX::XMMatrixTranslation(0, 0, 1));

	DirectX::XMVECTOR eyePos = { 0.0f, 0.0f, 0.0f };
	DirectX::XMVECTOR focus = { 0.0f, 0.0f, 1.0f };
	DirectX::XMVECTOR up = { 0.0f, 1.0f, 0.0f };

	//Where the camera is looking
	constBuf.view = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(eyePos, focus, up));
	constBuf.projection = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI * 0.45f, float(width)/height, 0.1f, 15.0f));

	deviceContext->UpdateSubresource(constBufferPerFrame, 0, nullptr, &constBuf, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &constBufferPerFrame);
}
