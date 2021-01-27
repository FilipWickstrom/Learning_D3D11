#include "PipelineHandler.h"

//Reading an image file
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//Reads the cso files and saves them to vertex shader at the device
bool LoadShaders(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, std::string& vShaderByteCode)
{
    std::string shaderData;
    std::ifstream reader;

    //Vertexshader
    reader.open("Shaders/VertexShader.cso", std::ios::binary | std::ios::ate);  //ate: Begins at the end
    if (!reader.is_open())
    {
        std::cerr << "Could not open vertex shader..." << std::endl;
        return false;
    }

    shaderData.reserve((unsigned int)reader.tellg());  //How many bytes to reserve in string. Reads from the pointer where it is
    reader.seekg(0, std::ios::beg);
    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());  //Copy data from start until string is filled

    if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader)))
    {
        std::cerr << "Failed to create vertex shader..." << std::endl;
        return false;
    }

    vShaderByteCode = shaderData;
    shaderData.clear();
    reader.close();

    //Pixelshader
    reader.open("Shaders/PixelShader.cso", std::ios::binary | std::ios::ate);
    if (!reader.is_open())
    {
        std::cerr << "Could not open pixel shader..." << std::endl;
        return false;
    }

    shaderData.reserve((unsigned int)reader.tellg());
    reader.seekg(0, std::ios::beg);
    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader)))
    {
        std::cerr << "Failed to create pixel shader..." << std::endl;
        return false;
    }

    return true;
}

bool CreateInputLayout(ID3D11Device* device, ID3D11InputLayout*& inputLayout, const std::string& vShaderByteCode)
{
    //Input the structure for how vertexes should be
    D3D11_INPUT_ELEMENT_DESC inputDesc[3] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOUR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HRESULT hr = device->CreateInputLayout(inputDesc, 3, vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);
    return !FAILED(hr);
}

bool CreateTexture(ID3D11Device* device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV)
{
    int textureWidth, textureHeight, channels;
    //Unsigned char because 1 byte (8 bits) which is good for format later on
    unsigned char* image = stbi_load("../Textures/TechFlip.png", &textureWidth, &textureHeight, &channels, STBI_rgb_alpha);

    //Description
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = textureWidth;
    desc.Height = textureHeight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;   //UNORM: 0 - 1
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_IMMUTABLE;     //Only reads from GPU
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;    //Bind texture to shader
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &image[0];   //Pointer to data
    data.SysMemPitch = textureWidth * channels;     //Distance from one line to another
    data.SysMemSlicePitch = 0;

    if (FAILED(device->CreateTexture2D(&desc, &data, &texture)))
    {
        std::cerr << "Failed to create texture..." << std::endl;
        stbi_image_free(image);
        return false;
    }

    //Freeing memory
    stbi_image_free(image);

    HRESULT hr = device->CreateShaderResourceView(texture, nullptr, &textureSRV);
    return !FAILED(hr);
}

bool CreateSamplerState(ID3D11Device* device, ID3D11SamplerState*& sampler)
{
    D3D11_SAMPLER_DESC desc;
    desc.Filter = D3D11_FILTER_ANISOTROPIC;     //Anisotropic interpolation for sampling. Best quality of samling
    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //Can repeat the texture if needed. 
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; //Have to go from 0 to > 1 to repeat texture
    desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.MipLODBias = 0;
    desc.MaxAnisotropy = 16;    //Highest quality
    desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0; //Not relevant. When outside the texture fill outside with this colour
    desc.MinLOD = 0;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = device->CreateSamplerState(&desc, &sampler);
    return !FAILED(hr);
}

bool SetupPipeline(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader,
                   ID3D11InputLayout*& inputLayout, ID3D11Texture2D*& texture,
                   ID3D11ShaderResourceView*& textureSRV, ID3D11SamplerState*& sampler)
{
    std::string vShaderByteCode;
    if (!LoadShaders(device, vShader, pShader, vShaderByteCode))
    {
        std::cerr << "Failed to load shaders..." << std::endl;
        return false;
    }

    if (!CreateInputLayout(device, inputLayout, vShaderByteCode))
    {
        std::cerr << "Failed to create input layout..." << std::endl;
        return false;
    }

    /*if (!CreateTexture(device, texture, textureSRV))
    {
        std::cerr << "Failed to create texture or srv..." << std::endl;
        return false;
    }*/

    if (!CreateSamplerState(device, sampler))
    {
        std::cerr << "Failed to create sampler state..." << std::endl;
        return false;
    }

    return true;
}
