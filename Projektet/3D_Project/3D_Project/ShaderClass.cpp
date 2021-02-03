#include "ShaderClass.h"

ShaderClass::ShaderClass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_anisoSampler = nullptr;
    m_vsByteCode = "";
}

ShaderClass::~ShaderClass()
{
	if (m_vertexShader != nullptr)
		m_vertexShader->Release();
	if (m_pixelShader != nullptr)
		m_pixelShader->Release();
	if (m_layout != nullptr)
		m_layout->Release();
	if (m_anisoSampler != nullptr)
		m_anisoSampler->Release();
}

bool ShaderClass::InitializeShader(ID3D11Device* device, std::string vertexShaderPath, std::string pixelShaderPath)
{
    std::string shaderData;
    std::ifstream reader;

    //Vertexshader
    reader.open("Shaders/"+vertexShaderPath, std::ios::binary | std::ios::ate);  //ate: Begins at the end
    if (!reader.is_open())
    {
        std::cerr << "Could not open vertex shader..." << std::endl;
        return false;
    }

    shaderData.reserve((unsigned int)reader.tellg());  //How many bytes to reserve in string. Reads from the pointer where it is
    reader.seekg(0, std::ios::beg);
    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());  //Copy data from start until string is filled

    if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &m_vertexShader)))
    {
        std::cerr << "Failed to create vertex shader..." << std::endl;
        return false;
    }

    m_vsByteCode = shaderData;
    shaderData.clear();
    reader.close();

    //Pixelshader
    reader.open("Shaders/"+pixelShaderPath, std::ios::binary | std::ios::ate);
    if (!reader.is_open())
    {
        std::cerr << "Could not open pixel shader..." << std::endl;
        return false;
    }

    shaderData.reserve((unsigned int)reader.tellg());
    reader.seekg(0, std::ios::beg);
    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &m_pixelShader)))
    {
        std::cerr << "Failed to create pixel shader..." << std::endl;
        return false;
    }

    return true;
}

bool ShaderClass::InitializeSampler(ID3D11Device* device)
{
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;     //Anisotropic interpolation for sampling. Best quality of samling
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 16;    //Highest quality
    samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = device->CreateSamplerState(&samplerDesc, &m_anisoSampler);
    return !FAILED(hr);
}

void ShaderClass::RenderShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->IASetInputLayout(m_layout);
    deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
    deviceContext->PSSetSamplers(0, 1, &m_anisoSampler);
}