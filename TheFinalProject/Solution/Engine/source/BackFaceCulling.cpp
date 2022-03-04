#include "PCH.h"
#include "BackFaceCulling.h"

BackFaceCulling::BackFaceCulling()
{
	m_geometryShader = nullptr;
	m_cullingOn = true;
}

BackFaceCulling::~BackFaceCulling()
{
	if (m_geometryShader)
		m_geometryShader->Release();
}

bool BackFaceCulling::LoadGeometryShader(ID3D11Device* device)
{
	std::string shaderData;
	std::ifstream reader;

	reader.open(SHADERPATH + "BacksideCullingGS.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Failed to open BacksideCullingGS.cso..." << std::endl;
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

bool BackFaceCulling::Initialize(ID3D11Device* device)
{
	if (!LoadGeometryShader(device))
	{
		std::cerr << "LoadGeometryShader() failed..." << std::endl;
		return false;
	}
	return true;
}

void BackFaceCulling::Bind(ID3D11DeviceContext* deviceContext)
{
	if (m_cullingOn)
		deviceContext->GSSetShader(m_geometryShader, nullptr, 0);
	else
		deviceContext->GSSetShader(nullptr, nullptr, 0);
}

void BackFaceCulling::UnBind(ID3D11DeviceContext* deviceContext)
{
	deviceContext->GSSetShader(nullptr, nullptr, 0);
}

void BackFaceCulling::SetCullingOn(bool onOrOff)
{
	m_cullingOn = onOrOff;
}