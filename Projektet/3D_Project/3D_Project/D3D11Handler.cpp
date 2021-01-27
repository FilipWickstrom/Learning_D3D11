#include "D3D11Handler.h"


bool CreateInterfaces(UINT width, UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& deviceContext, IDXGISwapChain*& swapChain)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;     //Refreshrate limiter. 0 = no limit
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  //8 bits for every R, G, B, A - with values from 0.0 - 1.0
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SampleDesc.Count = 1;     //Nr of multisamples per pixel. 1 = does not use it
    swapChainDesc.SampleDesc.Quality = 0;   //Quality of anti-alising
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;  //Need to have to buffers to right to for ...flip_discard
    swapChainDesc.OutputWindow = window;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;   //Better performance and lower power usage (https://devblogs.microsoft.com/directx/dxgi-flip-model/)
    swapChainDesc.Flags = 0;

    //In debugging mode, show messages
    UINT flags = 0;
    if (_DEBUG)
        flags = D3D11_CREATE_DEVICE_DEBUG;
    
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 }; //Versions of hardware to support
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, featureLevels, 1,
                                                D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, nullptr, &deviceContext);

    return !FAILED(hr);
}

bool CreateRenderTargetView(ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11RenderTargetView*& rtv)
{
    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
    {
        std::cerr << "Failed to get back buffer..." << std::endl;
        return false;
    }
    HRESULT hr = device->CreateRenderTargetView(backBuffer, nullptr, &rtv);
    backBuffer->Release();
    return !FAILED(hr);
}

bool CreateDepthStencil(ID3D11Device* device, UINT width, UINT height, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView)
{
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;  //Multisampled texture
    depthDesc.ArraySize = 1;  //Number of textures
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //24 bit depth and 8 bit stencil, 0.0 - 1.0
    depthDesc.SampleDesc.Count = 1;   //No MSAA
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.CPUAccessFlags = 0; //CPU is not needed
    depthDesc.MiscFlags = 0;

    if (FAILED(device->CreateTexture2D(&depthDesc, nullptr, &dsTexture)))
    {
        std::cerr << "Failed to create depth stencil texture..." << std::endl;
        return false;
    }

    HRESULT hr = device->CreateDepthStencilView(dsTexture, nullptr, &dsView);
    return !FAILED(hr);
}

void SetViewport(D3D11_VIEWPORT& viewport, UINT width, UINT height)
{
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)width;
    viewport.Height = (float)height;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
}

bool SetupD3D11(UINT width, UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& deviceContext,
                IDXGISwapChain*& swapChain, ID3D11RenderTargetView*& rtv, ID3D11Texture2D*& dsTexture, 
                ID3D11DepthStencilView*& dsView, D3D11_VIEWPORT& viewport)
{
    if (!CreateInterfaces(width, height, window, device, deviceContext, swapChain))
    {
        std::cerr << "Failed to create interface..." << std::endl;
        return false;
    }

    if (!CreateRenderTargetView(device, swapChain, rtv))
    {
        std::cerr << "Failed to create rendertargetview..." << std::endl;
        return false;
    }

    if (!CreateDepthStencil(device, width, height, dsTexture, dsView))
    {
        std::cerr << "Failed to create depth and stencil..." << std::endl;
        return false;
    }

    SetViewport(viewport, width, height);

    return true;
}
