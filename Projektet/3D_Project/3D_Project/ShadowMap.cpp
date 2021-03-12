#include "ShadowMap.h"

ShadowMap::ShadowMap()
{
	m_width = 1024;
	m_height = 1024;
	
	//View matrix
	m_pos =		{ 5.0f, 2.0f, 0.0f };	//Right side of spawn 2 up
	m_focus =	{-5.0f, 0.0f, 0.0f };	//Left side of spawn
	m_up =		{ 0.0f, 1.0f, 0.0f };
	XMVECTOR pos = XMLoadFloat3(&m_pos);
	XMVECTOR focus = XMLoadFloat3(&m_focus);
	XMVECTOR up = XMLoadFloat3(&m_up);
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(pos, focus, up));

	//Projection matrix
	m_nearPlane = 0.1f;
	m_farPlane = 100.0f;
	m_fov = XM_PI * 0.5f;	//90 degrees
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveFovLH(m_fov, (float)(m_width/m_height), m_nearPlane, m_farPlane));

	//Setting up the spotlight
	m_spotLight.position = m_pos;
	m_spotLight.direction = XMFLOAT3(m_focus.x - m_pos.x, m_focus.y - m_pos.y, m_focus.z - m_pos.z); //From pos to focuspoint
	m_spotLight.range = 20.0f;
	m_spotLight.colour = { 1.0f, 1.0f, 1.0f };
	m_spotLight.exponent = 4.0f;
	m_SpotLightBuffer = nullptr;

	//Depth buffer
	m_depthView = nullptr;
	m_depthSRV = nullptr;
	m_viewport = {};
	m_shadowShader = nullptr;

	//WVP
	m_shadowWVP.view = m_viewMatrix;
	m_shadowWVP.projection = m_projectionMatrix;
	XMStoreFloat4x4(&m_shadowWVP.world, XMMatrixIdentity()); //Will be changed for every gameobject
	m_shadowWVPBuffer = nullptr;
}

ShadowMap::~ShadowMap()
{
	if (m_SpotLightBuffer)
		m_SpotLightBuffer->Release();
	if (m_depthView)
		m_depthView->Release();
	if (m_depthSRV)
		m_depthSRV->Release();
	if (m_shadowShader)
		m_shadowShader->Release();
	if (m_shadowWVPBuffer)
		m_shadowWVPBuffer->Release();

}

bool ShadowMap::CreateSpotLightBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(Spotlight);
	desc.Usage = D3D11_USAGE_DYNAMIC;	//Only need to read on GPU no write
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &m_spotLight;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &data, &m_SpotLightBuffer);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create spotlight buffer..." << std::endl;
		return false;
	}
	else
	{
		return true;
	}
}

bool ShadowMap::CreateDepthBuffer(ID3D11Device* device)
{
	/*---- Prepare the texture ----*/
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;						//No CPU access
	textureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;	//24 bits for depth, 8 for stencil (not needed though)	//USE DXGI_FORMAT_R32_TYPELESS instead?
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 1;                         //Multisampled texture
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;                  //Only one sample per pixel
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;           //Read and write access to GPU

	//Depth map
	ID3D11Texture2D* depthMap = nullptr;

	HRESULT hr;
	hr = device->CreateTexture2D(&textureDesc, nullptr, &depthMap);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create depth stencil texture..." << std::endl;
		if (depthMap)
			depthMap->Release();
		return false;
	}

	/*----- Prepare the depth view ------*/
	D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
	viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;        //24 bits of depth and 8 for stencil. Values from 0.0f - 1.0f
	viewDesc.Texture2D.MipSlice = 0;                        //Index of mipmap to use
	viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; //How the resource can be accessed
	viewDesc.Flags = 0;

	hr = device->CreateDepthStencilView(depthMap, &viewDesc, &m_depthView);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create depth stencil view..." << std::endl;
		if (depthMap)
			depthMap->Release();
		return false;
	}
	
	/*----- Prepare the shader resource -----*/
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	hr = device->CreateShaderResourceView(depthMap, &srvDesc, &m_depthSRV);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create shader resource view for the depth..." << std::endl;
		if (depthMap)
			depthMap->Release();
		return false;
	}
	
	/*----- Prepare the viewport ------*/
	m_viewport.Height = (float)m_height;
	m_viewport.Width = (float)m_width;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	if (depthMap)
		depthMap->Release();

	return true;
}

bool ShadowMap::LoadVertexShader(ID3D11Device* device)
{
	std::string shaderData;
	std::ifstream reader;
	
	reader.open("Shaders/ShadowVS.cso", std::ios::binary | std::ios::ate);  //ate: Begins at the end
	if (!reader.is_open())
	{
		std::cerr << "Could not open vertex shader for shadows..." << std::endl;
		return false;
	}

	shaderData.reserve((unsigned int)reader.tellg());  //How many bytes to reserve in string. Reads from the pointer where it is
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());  //Copy data from start until string is filled

	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &m_shadowShader)))
	{
		std::cerr << "Failed to create vertex shader for shadows..." << std::endl;
		return false;
	}
	reader.close();

	return true;
}

bool ShadowMap::CreateShadowWVPBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(ShadowWVP);
	desc.Usage = D3D11_USAGE_DYNAMIC;	//Only need to read on GPU no write
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &m_shadowWVP;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &data, &m_shadowWVPBuffer);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create shadow wvp buffer..." << std::endl;
		return false;
	}
	else
	{
		return true;
	}
}

bool ShadowMap::Initialize(ID3D11Device* device)
{
	if (!CreateSpotLightBuffer(device))
	{
		std::cerr << "CreateSpotLightBuffer() failed..." << std::endl;
		return false;
	}

	if (!CreateDepthBuffer(device))
	{
		std::cerr << "CreateDepthBuffer() failed..." << std::endl;
		return false;
	}

	if (!LoadVertexShader(device))
	{
		std::cerr << "LoadVertexShader() failed..." << std::endl;
		return false;
	}

	if (!CreateShadowWVPBuffer(device))
	{
		std::cerr << "CreateShadowWVPBuffer() failed..." << std::endl;
		return false;
	}

	return true;
}

void ShadowMap::BindShadowVS(ID3D11DeviceContext* deviceContext)
{
	//Going to use this viewport with 512x512 res
	deviceContext->RSSetViewports(1, &m_viewport);

	//Using this vertexshader
	deviceContext->VSSetShader(m_shadowShader, nullptr, 0);
	
	//Not going to use the pixelshader
	deviceContext->PSSetShader(nullptr, nullptr, 0);

	//Only going to output to the depthbuffer
	
	ID3D11RenderTargetView* nullRTV = nullptr;
	deviceContext->OMSetRenderTargets(1, &nullRTV, m_depthView);
	//deviceContext->OMSetDepthStencilState()

	//Clearing the previous view
	deviceContext->ClearDepthStencilView(m_depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//Set WVP constant buffer
	deviceContext->VSSetConstantBuffers(0, 1, &m_shadowWVPBuffer);
}

void ShadowMap::UpdateShadowWVP(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 newWorld)
{
	m_shadowWVP.world = newWorld;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext->Map(m_shadowWVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &m_shadowWVP, sizeof(ShadowWVP));
	deviceContext->Unmap(m_shadowWVPBuffer, 0);
}

void ShadowMap::BindToPS(ID3D11DeviceContext* deviceContext)
{
	//Set the constant buffer to pixelshader. Used in the second pass
	deviceContext->PSSetConstantBuffers(2, 1, &m_SpotLightBuffer);

	//Using this as depth as a texture
	deviceContext->PSSetShaderResources(6, 1, &m_depthSRV);

	//Set the constant buffer
	deviceContext->PSSetConstantBuffers(3, 1, &m_shadowWVPBuffer);
}

void ShadowMap::DisableSRV(ID3D11DeviceContext* deviceContext)
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	deviceContext->PSSetShaderResources(6, 1, &nullSRV);
}
