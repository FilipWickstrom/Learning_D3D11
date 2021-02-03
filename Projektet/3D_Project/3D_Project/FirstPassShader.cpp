#include "FirstPassShader.h"

FirstPassShader::FirstPassShader()
	:ShaderClass()
{
}

FirstPassShader::~FirstPassShader()
{
}

bool FirstPassShader::InitializeInputLayout(ID3D11Device* device)
{
	//Input the structure for how vertexes should be
	D3D11_INPUT_ELEMENT_DESC inputDesc[3] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, 3, m_vsByteCode.c_str(), m_vsByteCode.length(), &m_layout);
	return !FAILED(hr);
}

bool FirstPassShader::Initialize(ID3D11Device* device, std::string vsPath, std::string psPath)
{
	if (!InitializeShader(device, vsPath, psPath))
	{
		std::cerr << "Failed to initialize shaders..." << std::endl;
		return false;
	}

	if (!InitializeInputLayout(device))
	{
		std::cerr << "Failed to initialize input layout..." << std::endl;
		return false;
	}

	if (!InitializeSampler(device))
	{
		std::cerr << "Failed to initialize sampler..." << std::endl;
		return false;
	}

	return true;
}

void FirstPassShader::Render(ID3D11DeviceContext* deviceContext)
{
	RenderShader(deviceContext);
}
