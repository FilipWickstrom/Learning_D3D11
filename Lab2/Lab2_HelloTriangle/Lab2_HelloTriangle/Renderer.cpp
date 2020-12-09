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
	this->texture2d->Release();
	this->textureShaderResourceView->Release();
	this->samplerState->Release();

	this->constBufPerFrameWVP->Release();
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

	deviceContext->VSSetConstantBuffers(0, 1, &constBufPerFrameWVP);
	updateConstBuffer(angle, width, height);
	//light
	//deviceContext->PSSetConstantBuffers(0, 1, &constlightbuffer);

	//For texture
	deviceContext->PSSetShaderResources(0, 1, &textureShaderResourceView);
	deviceContext->PSSetSamplers(0, 1, &samplerState);

	//Where to draw and present the vertices
	deviceContext->RSSetViewports(1, &viewport);
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView); //OM = outputmerge
	deviceContext->Draw(8, 0);	//Drawing all the vertices
	swapChain->Present(1, 0);	//Syncing with Vsync
}

bool Renderer::loadConstBuffer(UINT width, UINT height)
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(constBufWVP);
	desc.Usage = D3D11_USAGE_DYNAMIC;				//Needed as the constant buffer is going to be updated
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//Also needed for constant buffer
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	//Value for where the camera is looking
	DirectX::XMVECTOR eyePos = { 0.0f, 0.0f, 0.0f };
	DirectX::XMVECTOR focus = { 0.0f, 0.0f, 1.0f };
	DirectX::XMVECTOR up = { 0.0f, 1.0f, 0.0f };
	DirectX::XMStoreFloat4x4(&cbWVP.view, DirectX::XMMatrixLookAtLH(eyePos, focus, up));

	//Perspective and how long the camera can see
	DirectX::XMStoreFloat4x4(&cbWVP.projection, DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI * 0.45f, float(width) / height, 0.1f, 5.0f));
		
	//First param is the scale of the world, second is for the rotation and third is for how far away you are from the object
	DirectX::XMStoreFloat4x4(&cbWVP.world, DirectX::XMMatrixScaling(0.8f, 0.8f, 0.8f) * DirectX::XMMatrixRotationY(0) * DirectX::XMMatrixTranslation(0, 0, 1));

	D3D11_SUBRESOURCE_DATA initdata;
	initdata.pSysMem = &cbWVP;
	initdata.SysMemPitch = 0;
	initdata.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &initdata, &constBufPerFrameWVP);
	return !FAILED(hr);
}

void Renderer::updateConstBuffer(float rotation, UINT width, UINT height)
{	
	//Updates the world matrix as it is going to rotate. View- and projection-matrix still the same in this laboration

	//First param is the scale of the world, second is for the rotation and third is for how far away you are from the object
	DirectX::XMStoreFloat4x4(&cbWVP.world, DirectX::XMMatrixScaling(0.8f, 0.8f, 0.8f) * DirectX::XMMatrixRotationY(rotation) * DirectX::XMMatrixTranslation(0, 0, 1));

	//Mapping the constantbuffers information to GPU memory
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext->Map(constBufPerFrameWVP, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &cbWVP, sizeof(constBufWVP));
	deviceContext->Unmap(constBufPerFrameWVP, 0);
	//Map uses less CPU time then updatesubresource
}
