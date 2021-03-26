#include "Rasterizer.h"

Rasterizer::Rasterizer()
{
    m_defaultRS = nullptr;
    m_wireframeRS = nullptr;
    m_wireFrameON = false;
}

Rasterizer::~Rasterizer()
{
    if (m_defaultRS)
        m_defaultRS->Release();
    if (m_wireframeRS)
        m_wireframeRS->Release();
}

bool Rasterizer::CreateRasterizerStates(ID3D11Device* device)
{
    D3D11_RASTERIZER_DESC desc;
    desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;       //Do not use any kind of culling
    desc.FrontCounterClockwise = false;						//Going to be clockwise order
    desc.DepthBias = 0;
    desc.DepthBiasClamp = 0.0f;
    desc.SlopeScaledDepthBias = 0.0f;
    desc.DepthClipEnable = true;
    desc.ScissorEnable = false;
    desc.MultisampleEnable = false;
    desc.AntialiasedLineEnable = false;

    //Default mode
    desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    if (FAILED(device->CreateRasterizerState(&desc, &m_defaultRS)))
    {
        std::cerr << "Failed to create default rasterizer state..." << std::endl;
        return false;
    }

    //Wireframed version
    desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    if (FAILED(device->CreateRasterizerState(&desc, &m_wireframeRS)))
    {
        std::cerr << "Failed to create wireframe rasterizer state..." << std::endl;
        return false;
    }

    return true;
}

bool Rasterizer::Initialize(ID3D11Device* device)
{
    if (!CreateRasterizerStates(device))
    {
        std::cerr << "CreateRasterizerStates() failed..." << std::endl;
        return false;
    }

    return true;
}

void Rasterizer::Bind(ID3D11DeviceContext* deviceContext)
{
    if (m_wireFrameON)
        deviceContext->RSSetState(m_wireframeRS);
    else
        deviceContext->RSSetState(m_defaultRS);
}

void Rasterizer::UnBind(ID3D11DeviceContext* deviceContext)
{
    deviceContext->RSSetState(nullptr);
}

void Rasterizer::TurnOnWireframe(bool onOrOff)
{
    m_wireFrameON = onOrOff;
}
