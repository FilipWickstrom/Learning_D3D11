#include "Tessellation.h"

Tessellation::Tessellation()
{
	m_hullShader = nullptr;
	m_domainShader = nullptr;
	m_rasterizerState = nullptr;
	m_wireframeOn = false;
	m_tessellationBuffer = nullptr;
	m_tessellSettings.level = 16.0f;
	m_tessellSettings.depth = 0.4f;
}

Tessellation::~Tessellation()
{
	if (m_hullShader)
		m_hullShader->Release();
	if (m_domainShader)
		m_domainShader->Release();
	if (m_rasterizerState)
		m_rasterizerState->Release();
	if (m_tessellationBuffer)
		m_tessellationBuffer->Release();
}

bool Tessellation::LoadShaders(ID3D11Device* device)
{
	std::string shaderData;
	std::ifstream reader;

	//	HullShader
	reader.open("Shaders/HullShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Failed to open hullshader.cso..." << std::endl;
		return false;
	}
	shaderData.reserve((unsigned int)reader.tellg());
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateHullShader(shaderData.c_str(), shaderData.length(), nullptr, &m_hullShader)))
	{
		std::cerr << "Failed to create hullshader..." << std::endl;
		return false;
	}
	//Cleanup before next shader
	reader.close();
	shaderData.clear();

	//	DomainShader
	reader.open("Shaders/DomainShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Failed to open domainshader.cso..." << std::endl;
		return false;
	}
	shaderData.reserve((unsigned int)reader.tellg());
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateDomainShader(shaderData.c_str(), shaderData.length(), nullptr, &m_domainShader)))
	{
		std::cerr << "Failed to create domain shader..." << std::endl;
		return false;
	}
	reader.close();

	return true;
}

bool Tessellation::CreateRasterizerState(ID3D11Device* device)
{
	D3D11_RASTERIZER_DESC desc;
	desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	desc.FrontCounterClockwise = false;						//Going to be clockwise order
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = true;
	desc.ScissorEnable = false;
	desc.MultisampleEnable = false;
	desc.AntialiasedLineEnable = false;

	HRESULT hr = device->CreateRasterizerState(&desc, &m_rasterizerState);
	return !FAILED(hr);
}

bool Tessellation::CreateTessellSettings(ID3D11Device* device)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(tessellSettings);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &m_tessellSettings;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &data, &m_tessellationBuffer);
	return !FAILED(hr);
}

bool Tessellation::Initialize(ID3D11Device* device)
{
	//Load shaders
	if (!LoadShaders(device))
	{
		std::cerr << "Loadshaders() failed..." << std::endl;
		return false;
	}

	//Create the rasterizer
	if (!CreateRasterizerState(device))
	{
		std::cerr << "CreateRasterizerState() failed..." << std::endl;
		return false;
	}

	//Create the constant buffer
	if (!CreateTessellSettings(device))
	{
		std::cerr << "Failed to create tessellation constant buffer..." << std::endl;
		return false;
	}

	return true;
}

void Tessellation::SetShaders(ID3D11DeviceContext* deviceContext, bool useTessellation, ID3D11ShaderResourceView* displaceMapSRV)
{
	//Setting up tessellation if needed for object
	if (useTessellation)
	{
		deviceContext->HSSetShader(m_hullShader, nullptr, 0);
		deviceContext->DSSetShader(m_domainShader, nullptr, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		deviceContext->DSSetShaderResources(0, 1, &displaceMapSRV);
		deviceContext->HSSetConstantBuffers(0, 1, &m_tessellationBuffer);
		deviceContext->DSSetConstantBuffers(1, 1, &m_tessellationBuffer);
	}
	else
	{
		deviceContext->HSSetShader(nullptr, nullptr, 0);
		deviceContext->DSSetShader(nullptr, nullptr, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	//Wireframe shows if on
	if (m_wireframeOn)
		deviceContext->RSSetState(m_rasterizerState);
	else
		deviceContext->RSSetState(nullptr);
}

void Tessellation::SetWireframe(bool trueorfalse)
{
	m_wireframeOn = trueorfalse;
}

void Tessellation::TurnOff(ID3D11DeviceContext* deviceContext)
{
	deviceContext->RSSetState(nullptr);
	deviceContext->HSSetShader(nullptr, nullptr, 0);
	deviceContext->DSSetShader(nullptr, nullptr, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Tessellation::UpdateLOD(ID3D11DeviceContext* deviceContext, float level)
{
	//Set the new level
	m_tessellSettings.level = level;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext->Map(m_tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &m_tessellSettings, sizeof(tessellSettings));
	deviceContext->Unmap(m_tessellationBuffer, 0);
}

void Tessellation::UpdateDepth(ID3D11DeviceContext* deviceContext, float depth)
{
	m_tessellSettings.depth = depth;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext->Map(m_tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &m_tessellSettings, sizeof(tessellSettings));
	deviceContext->Unmap(m_tessellationBuffer, 0);
}

