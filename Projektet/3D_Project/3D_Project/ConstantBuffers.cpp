#include "ConstantBuffers.h"

ConstantBuffers::ConstantBuffers()
{
	m_WVPMatrix = {};
	m_WVPBuffer = nullptr;
	m_lights = {};
	m_lightsBuffer = nullptr;
}

ConstantBuffers::~ConstantBuffers()
{
	if (m_WVPBuffer)
		m_WVPBuffer->Release();
	if (m_lightsBuffer)
		m_lightsBuffer->Release();
}

bool ConstantBuffers::CreateWVPBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(WVPMatrix);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &m_WVPMatrix;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &data, &m_WVPBuffer);
	return !FAILED(hr);
}

void ConstantBuffers::UpdateWVP(ID3D11DeviceContext* deviceContext)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext->Map(m_WVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &m_WVPMatrix, sizeof(WVPMatrix));
	deviceContext->Unmap(m_WVPBuffer, 0);
}

bool ConstantBuffers::InitializeWVP(ID3D11Device* device, XMFLOAT4X4 view, XMFLOAT4X4 proj)
{
	if (!CreateWVPBuffer(device))
	{
		std::cerr << "Failed to create WVP buffer..." << std::endl;
		return false;
	}
	//World set to just an identity matrix from start
	XMStoreFloat4x4(&m_WVPMatrix.world, XMMatrixIdentity());
	m_WVPMatrix.view = view;
	m_WVPMatrix.projection = proj;

	return true;
}

void ConstantBuffers::SetWVPToVS(ID3D11DeviceContext* deviceContext)
{
	deviceContext->VSSetConstantBuffers(0, 1, &m_WVPBuffer);
}

void ConstantBuffers::UpdateWorld(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 world)
{
	m_WVPMatrix.world = world;
	UpdateWVP(deviceContext);
}

void ConstantBuffers::UpdateView(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 view)
{
	m_WVPMatrix.view = view;
	UpdateWVP(deviceContext);
}

void ConstantBuffers::UpdateProjection(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 proj)
{
	m_WVPMatrix.projection = proj;
	UpdateWVP(deviceContext);
}


//Lights
bool ConstantBuffers::CreateLightsBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(LightStruct);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &m_lights;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &data, &m_lightsBuffer);
	return !FAILED(hr);
}

bool ConstantBuffers::InitializeLights(ID3D11Device* device)
{
	if (!CreateLightsBuffer(device))
	{
		std::cerr << "Failed to create lights buffer..." << std::endl;
		return false;
	}

	//******TEMP********
	m_lights.lightPos = { 0.0f,2.0f,0.0f };
	m_lights.lightColour = { 1.0f,1.0f,1.0f,1.0f };
	m_lights.camPos = { 0.0f, 0.0f, 0.0f };
	m_lights.lightRange = 10.0f;

	return true;
}

void ConstantBuffers::SetLightToPS(ID3D11DeviceContext* deviceContext)
{
	deviceContext->PSSetConstantBuffers(0, 1, &m_lightsBuffer);
}

void ConstantBuffers::UpdateLights(ID3D11DeviceContext* deviceContext, Camera& camera)
{
	//TEMP
	m_lights.camPos = camera.GetPosition();
	m_lights.lightPos = camera.GetPosition();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext->Map(m_lightsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &m_lights, sizeof(LightStruct));
	deviceContext->Unmap(m_lightsBuffer, 0);
}

