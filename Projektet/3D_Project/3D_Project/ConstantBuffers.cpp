#include "ConstantBuffers.h"

ConstantBuffers::ConstantBuffers()
{
	m_WVPMatrix = {};
	m_WVPBuffer = nullptr;
	m_lights = {};
	m_lightsBuffer = nullptr;
	m_camStruct = {};
	m_camBuffer = nullptr;
	m_material = {};
	m_materialBuffer = nullptr;

	m_followCamera = true;
	m_renderMode = 1;
	m_usingSecondCam = false;
}

ConstantBuffers::~ConstantBuffers()
{
	if (m_WVPBuffer)
		m_WVPBuffer->Release();
	if (m_lightsBuffer)
		m_lightsBuffer->Release();
	if (m_camBuffer)
		m_camBuffer->Release();
	if (m_materialBuffer)
		m_materialBuffer->Release();
}

bool ConstantBuffers::CreateBuffers(ID3D11Device* device)
{
	//WVP
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
	if (FAILED(hr))
	{
		std::cerr << "Failed to create WVP buffer..." << std::endl;
		return false;
	}

	//Lights
	desc.ByteWidth = sizeof(LightStruct);
	data.pSysMem = &m_lights;
	hr = device->CreateBuffer(&desc, &data, &m_lightsBuffer);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create lights buffer..." << std::endl;
		return false;
	}

	//Camera
	desc.ByteWidth = sizeof(CamStruct);
	data.pSysMem = &m_camStruct;
	hr = device->CreateBuffer(&desc, &data, &m_camBuffer);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create cam buffer..." << std::endl;
		return false;
	}

	//Material
	desc.ByteWidth = sizeof(Material);
	data.pSysMem = &m_material;
	hr = device->CreateBuffer(&desc, &data, &m_materialBuffer);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create material buffer..." << std::endl;
		return false;
	}

	return true;
}

bool ConstantBuffers::Initialize(ID3D11Device* device, const Camera& camera)
{
	if (!CreateBuffers(device))
	{
		std::cerr << "Failed to create buffers..." << std::endl;
		return false;
	}

	//World set to just an identity matrix from start
	XMStoreFloat4x4(&m_WVPMatrix.world, XMMatrixIdentity());
	m_WVPMatrix.view = camera.GetViewMatrix();
	m_WVPMatrix.projection = camera.GetProjMatrix();

	//Camera light
	m_lights.pointlights[0].position = { 0.0f,1.0f,0.0f, 1.0f };
	m_lights.pointlights[0].colour = { 0.8f, 0.8f, 0.8f,1.0f };
	m_lights.pointlights[0].range = 7.0f;

	//Front light - yellow
	m_lights.pointlights[1].position = { 6.0f, 3.0f, 8.0f, 1.0f };
	m_lights.pointlights[1].colour = { 0.6f, 0.6f, 0.2f, 1.0f };
	m_lights.pointlights[1].range = 10.0f;

	//Back light - blue
	m_lights.pointlights[2].position = { 7.0f, 3.0f, -8.0f, 1.0f };
	m_lights.pointlights[2].colour = { 0.2f, 0.2f, 0.8f, 1.0f };
	m_lights.pointlights[2].range = 10.0f;

	//Cam
	m_camStruct.camPos = camera.GetPosition();
	m_camStruct.renderMode = 1;

	return true;
}

void ConstantBuffers::UpdateWVP(ID3D11DeviceContext* deviceContext)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext->Map(m_WVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &m_WVPMatrix, sizeof(WVPMatrix));
	deviceContext->Unmap(m_WVPBuffer, 0);
}

void ConstantBuffers::SetWVP(ID3D11DeviceContext* deviceContext)
{
	deviceContext->VSSetConstantBuffers(0, 1, &m_WVPBuffer);
	deviceContext->DSSetConstantBuffers(0, 1, &m_WVPBuffer);
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

void ConstantBuffers::SetLightsToPS(ID3D11DeviceContext* deviceContext)
{
	deviceContext->PSSetConstantBuffers(0, 1, &m_lightsBuffer);
}

void ConstantBuffers::UpdateLights(ID3D11DeviceContext* deviceContext, Camera& camera)
{
	if (m_followCamera)
	{
		//Pointlight on the camera
		m_lights.pointlights[0].position = XMFLOAT4(camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z, 0.0f);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		deviceContext->Map(m_lightsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &m_lights, sizeof(LightStruct));
		deviceContext->Unmap(m_lightsBuffer, 0);
	}
}

void ConstantBuffers::SetCamToPS(ID3D11DeviceContext* deviceContext)
{
	deviceContext->PSSetConstantBuffers(1, 1, &m_camBuffer);
}

void ConstantBuffers::UpdateCam(ID3D11DeviceContext* deviceContext)
{
	//Update to the current location of the camera
	m_camStruct.camPos = m_currentCamPos;
	m_camStruct.renderMode = m_renderMode;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext->Map(m_camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &m_camStruct, sizeof(CamStruct));
	deviceContext->Unmap(m_camBuffer, 0);
}

void ConstantBuffers::SetCamToGS(ID3D11DeviceContext* deviceContext)
{
	deviceContext->GSSetConstantBuffers(0, 1, &m_camBuffer);
}

void ConstantBuffers::SetCamPos(XMFLOAT3 position)
{
	m_currentCamPos = position;
}

const bool ConstantBuffers::IsUsingSecondCam() const
{
	return m_usingSecondCam;
}

void ConstantBuffers::SetSecondCamera(bool onOrOff)
{
	m_usingSecondCam = onOrOff;
}

void ConstantBuffers::UpdateMaterial(ID3D11DeviceContext* deviceContext, XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular)
{
	m_material.ambient = ambient;
	m_material.diffuse = diffuse;
	m_material.specular = specular;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext->Map(m_materialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &m_material, sizeof(Material));
	deviceContext->Unmap(m_materialBuffer, 0);
}

void ConstantBuffers::SetMaterialPS(ID3D11DeviceContext* deviceContext)
{
	deviceContext->PSSetConstantBuffers(0, 1, &m_materialBuffer);
}

void ConstantBuffers::SetFollowCamera(bool trueOrFalse)
{
	m_followCamera = trueOrFalse;
}

void ConstantBuffers::SetRenderMode(UINT mode)
{
	m_renderMode = mode;
}

