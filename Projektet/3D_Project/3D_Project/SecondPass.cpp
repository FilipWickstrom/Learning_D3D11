#include "SecondPass.h"

SecondPass::SecondPass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_anisoSampler = nullptr;
	m_inputLayout = nullptr;

	m_backbuffer = nullptr;

	m_depthTexture = nullptr;
	m_depthView = nullptr;
	m_viewport = {};
}

SecondPass::~SecondPass()
{
	if (m_vertexShader)
		m_vertexShader->Release();
	if (m_pixelShader)
		m_pixelShader->Release();
	if (m_anisoSampler)
		m_anisoSampler->Release();
	if (m_inputLayout)
		m_inputLayout->Release();

	if (m_backbuffer)
		m_backbuffer->Release();

	if (m_depthTexture)
		m_depthTexture->Release();
	if (m_depthView)
		m_depthView->Release();

}

bool SecondPass::InitInputLayout(ID3D11Device* device, std::string vsByteCode)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[2] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, 2, vsByteCode.c_str(), vsByteCode.length(), &m_inputLayout);
	return !FAILED(hr);
}

bool SecondPass::InitRenderTargetView(ID3D11Device* device, IDXGISwapChain* swapChain)
{
	ID3D11Texture2D* backBuffer = nullptr;
	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
	{
		std::cerr << "Failed to get back buffer..." << std::endl;
		return false;
	}
	HRESULT hr = device->CreateRenderTargetView(backBuffer, nullptr, &m_backbuffer);
	backBuffer->Release();
	return !FAILED(hr);
}

bool SecondPass::Initialize(ID3D11Device* device, UINT winWidth, UINT winHeight, IDXGISwapChain* swapChain)
{
	std::string vsByteCode = "";

	if (!InitializeShaders(device, m_vertexShader, "SecondPassVS.cso", m_pixelShader, "SecondPassPS.cso", vsByteCode))
	{
		std::cerr << "Failed to initialize vertex- or/and pixelshader..." << std::endl;
		return false;
	}

	if (!InitializeAnisoSampler(device, m_anisoSampler))
	{
		std::cerr << "Failed to initialize the sampler..." << std::endl;
		return false;
	}

	if (!InitInputLayout(device, vsByteCode))
	{
		std::cerr << "Failed to setup input layout..." << std::endl;
		return false;
	}

	//initialize the m_shaderResourceView
	if (!InitRenderTargetView(device, swapChain))
	{
		std::cerr << "Failed to initialize the render target view..." << std::endl;
		return false;
	}

	//Initialize depth buffer
	if (!InitializeDepthBuffer(device, m_depthTexture, m_depthView, winWidth, winHeight))
	{
		std::cerr << "Failed to initialize the depth buffer..." << std::endl;
		return false;
	}

	InitializeViewport(m_viewport, winWidth, winHeight);

	return true;
}

void SecondPass::Bind(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* srv1, ID3D11ShaderResourceView* srv2)
{
	deviceContext->IASetInputLayout(m_inputLayout);

	deviceContext->RSSetViewports(1, &m_viewport);

	deviceContext->OMSetRenderTargets(1, &m_backbuffer, m_depthView);
	
	deviceContext->ClearRenderTargetView(m_backbuffer, m_clearColour);

	deviceContext->ClearDepthStencilView(m_depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	deviceContext->PSSetSamplers(0, 1, &m_anisoSampler);

	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	deviceContext->PSSetShaderResources(0, 1, &srv1);
	deviceContext->PSSetShaderResources(1, 1, &srv2);
}

