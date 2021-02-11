#include "ConstantBuffers.h"

ConstantBuffers::ConstantBuffers()
{
	m_WVPMatrix = {};
	m_WVPBuffer = nullptr;
}

ConstantBuffers::~ConstantBuffers()
{
	if (m_WVPBuffer)
		m_WVPBuffer->Release();
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

