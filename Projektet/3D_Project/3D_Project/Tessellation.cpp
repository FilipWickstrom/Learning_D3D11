#include "Tessellation.h"

Tessellation::Tessellation()
{
	m_hullShader = nullptr;
	m_domainShader = nullptr;
	m_rasterizerState = nullptr;

	m_wireframeOn = false;	//make changeable with imgui later***
}

Tessellation::~Tessellation()
{
	if (m_hullShader)
		m_hullShader->Release();
	if (m_domainShader)
		m_domainShader->Release();
	if (m_rasterizerState)
		m_rasterizerState->Release();
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

