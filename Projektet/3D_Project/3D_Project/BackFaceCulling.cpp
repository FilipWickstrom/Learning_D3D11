#include "BackFaceCulling.h"

BackFaceCulling::BackFaceCulling()
{
	m_geometryShader = nullptr;
	m_rasterizerState = nullptr;

}

BackFaceCulling::~BackFaceCulling()
{
	if (m_geometryShader)
		m_geometryShader->Release();
	if (m_rasterizerState)
		m_rasterizerState->Release();
}

bool BackFaceCulling::LoadGeometryShader(ID3D11Device* device)
{
	std::string shaderData;
	std::ifstream reader;

	reader.open("Shaders/GeometryShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Failed to open GeometryShader.cso..." << std::endl;
		return false;
	}
	shaderData.reserve((unsigned int)reader.tellg());
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateGeometryShader(shaderData.c_str(), shaderData.length(), nullptr, &m_geometryShader)))
	{
		std::cerr << "Failed to create geometryshader..." << std::endl;
		return false;
	}

	return true;
}

bool BackFaceCulling::CreateRasterizerState(ID3D11Device* device)
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;	//Automatic culling is off
	desc.FrontCounterClockwise = false;
	desc.AntialiasedLineEnable = false;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = true;			//true or false in implementation? what is okay?
	desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	desc.MultisampleEnable = false;
	desc.ScissorEnable = false;
	desc.SlopeScaledDepthBias = 0.0f;

	return !FAILED(device->CreateRasterizerState(&desc, &m_rasterizerState));
}

bool BackFaceCulling::Initialize(ID3D11Device* device)
{
	if (!LoadGeometryShader(device))
	{
		std::cerr << "LoadGeometryShader() failed..." << std::endl;
		return false;
	}

	if (!CreateRasterizerState(device))
	{
		std::cerr << "CreateRasterizerState() failed..." << std::endl;
		return false;
	}

	return true;
}

void BackFaceCulling::Bind(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//NEEDED?

	//Use this mode: if (true), else RSSetState(nullptr);
	deviceContext->RSSetState(m_rasterizerState);
	deviceContext->GSSetShader(m_geometryShader, nullptr, 0);
}

void BackFaceCulling::UnBind(ID3D11DeviceContext* deviceContext)
{
	deviceContext->RSSetState(nullptr);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
}
