#include "PipelineHelper.h"
#include <string>
#include <fstream>
#include <iostream>

//Reading a image file
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool LoadShaders(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, std::string& vShaderByteCode)
{
    std::string shaderData;
    std::ifstream reader;
    
    //Vertexshader
    reader.open("../Debug/VertexShader.cso", std::ios::binary | std::ios::ate); //Fix link...***
    if (!reader.is_open())
    {
        std::cerr << "Could not open vertex shader..." << std::endl;
        return false;
    }

    reader.seekg(0, std::ios::end);
    shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
    reader.seekg(0, std::ios::beg);
    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader)))
    {
        std::cerr << "Failed to create vertex shader..." << std::endl;
        return false;
    }

    vShaderByteCode = shaderData;
    shaderData.clear();
    reader.close();
    
    //Pixelshader
    reader.open("../Debug/PixelShader.cso", std::ios::binary | std::ios::ate);  //Fix link...***
    if (!reader.is_open())
    {
        std::cerr << "Could not open pixel shader..." << std::endl;
        return false;
    }

    reader.seekg(0, std::ios::end);
    shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
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
    D3D11_INPUT_ELEMENT_DESC inputDesc[2] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
        //{"COLOUR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HRESULT hr = device->CreateInputLayout(inputDesc, 2, vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

    return !FAILED(hr);
}

bool CreateWVP(ID3D11Device* device, ID3D11DeviceContext*& immediateContext)
{
    //Projection matrix info
    float angle = DirectX::XM_PI * 0.45f; //81 degrees
    float aspectRatio = 16.0f / 9.0f;
    float farClipDistance = 15.0f;
    float nearClipDistance = 0.1f;
    ConstantBuffer cbCamera;

    //World matrix
    //DirectX::XMStoreFloat4x4(&cbCamera.World, DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f) * DirectX::XMMatrixRotationY(angle) * DirectX::XMMatrixTranslation(0, 0, 1));
    //DirectX::XMStoreFloat4x4(&cbCamera.World, DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(9.0f/16.0f, 1.0f, 1.0f) * DirectX::XMMatrixRotationZ(0.0f) * DirectX::XMMatrixTranslation(9.0f/16.0f, 1.0f, 1.0f)));

    //View matrix
    /*DirectX::XMVECTOR eyepos = { 0.0f, 0.0f, 0.0f };
    DirectX::XMVECTOR focus = { 0.5f, 0.0f, 1.0f };
    DirectX::XMVECTOR updir = { 0.0f, 1.0f, 0.0f };
    DirectX::XMStoreFloat4x4(&cbCamera.View, DirectX::XMMatrixLookAtLH(eyepos, focus, updir));*/

    //Projection matrix
    //DirectX::XMStoreFloat4x4(&cbCamera.Projection, DirectX::XMMatrixPerspectiveFovLH(angle, aspectRatio, nearClipDistance, farClipDistance));

    ID3D11Buffer* pConstBuf = nullptr;
    D3D11_BUFFER_DESC constBufDesc;
    constBufDesc.ByteWidth = sizeof(cbCamera);
    constBufDesc.Usage = D3D11_USAGE_DYNAMIC;
    constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constBufDesc.MiscFlags = 0;
    constBufDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &cbCamera;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateBuffer(&constBufDesc, &data, &pConstBuf);

    if (FAILED(hr))
    {
        pConstBuf->Release();
        return false;
    }   
    else
    {
        //Sets the buffer
        immediateContext->VSSetConstantBuffers(0, 1, &pConstBuf);
        return true;
    }
    //pConstBuf->Release();
}


bool CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer*& vertexBuffer)
{
    //Hardcoded quad where order is important               //Should later have: position,UV,normal***
    SimpleVertex quad[4] =
    {
        { {-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f} },    //Down left
        { {-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f} },    //Up left
        { { 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f} },    //Down right
        { { 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f} }     //Up right
    };

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = sizeof(quad);
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = quad;
    data.SysMemPitch = 0;       //For 2D textures
    data.SysMemSlicePitch = 0;  //For 3D textures

    HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
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
    desc.Usage = D3D11_USAGE_IMMUTABLE;     //Cant be changed
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &image[0];
    data.SysMemPitch = textureWidth * channels;
    data.SysMemSlicePitch = 0;

    if (FAILED(device->CreateTexture2D(&desc, &data, &texture)))
    {
        std::cerr << "Failed to create texture..." << std::endl;
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
    desc.Filter = D3D11_FILTER_ANISOTROPIC;     //Filter type
    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //Can repeat the texture if needed
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.MipLODBias = 0;
    desc.MaxAnisotropy = 16;    //Highest quality
    desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0; //Not relevant. When outside the texture fill outside with this colour
    desc.MinLOD = 0;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = device->CreateSamplerState(&desc, &sampler);
    return !FAILED(hr);
}


bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, 
                   ID3D11DeviceContext*& immediateContext, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV, ID3D11SamplerState*& sampler)
{
    std::string vShaderByteCode;
    if (!LoadShaders(device, vShader, pShader, vShaderByteCode))
    {
        std::cerr << "Error loading shaders..." << std::endl;
        return false;
    }
   
    if (!CreateInputLayout(device, inputLayout, vShaderByteCode))
    {
        std::cerr << "Error creating input layout..." << std::endl;
        return false;
    }

    /*if (!CreateWVP(device, immediateContext))
    {
        std::cerr << "Error creating world view projection buffer..." << std::endl;
        return false;
    }*/

    if (!CreateVertexBuffer(device, vertexBuffer))
    {
        std::cerr << "Error creating vertex buffer..." << std::endl;
        return false;
    }

    if (!CreateTexture(device, texture, textureSRV))
    {
        std::cerr << "Error creating texture or srv..." << std::endl;
        return false;
    }

    if (!CreateSamplerState(device, sampler))
    {
        std::cerr << "Error creating sampler state..." << std::endl;
        return false;
    }

    return true;
}
