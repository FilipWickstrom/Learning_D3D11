#include "FirstPass.h"

FirstPass::FirstPass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_anisoSampler = nullptr;
	m_inputLayout = nullptr;

	for (int i = 0; i < NROFBUFFERS; i++)
	{
		m_renderTargetTextures[i] = nullptr;
		m_renderTargetViews[i] = nullptr;
		m_shaderResourceViews[i] = nullptr;
	}

	m_depthTexture = nullptr;
	m_depthView = nullptr;
	m_viewport = {};
}

FirstPass::~FirstPass()
{
	if (m_vertexShader)
		m_vertexShader->Release();
	if (m_pixelShader)
		m_pixelShader->Release();
	if (m_anisoSampler)
		m_anisoSampler->Release();
	if (m_inputLayout)
		m_inputLayout->Release();

	for (int i = 0; i < NROFBUFFERS; i++)
	{
		if (m_renderTargetTextures[i])
			m_renderTargetTextures[i]->Release();
		if (m_renderTargetViews[i])
			m_renderTargetViews[i]->Release();
		if (m_shaderResourceViews[i])
			m_shaderResourceViews[i]->Release();
	}

	if (m_depthTexture)
		m_depthTexture->Release();
	if (m_depthView)
		m_depthView->Release();

}

bool FirstPass::InitInputLayout(ID3D11Device* device, std::string vsByteCode)
{
	//The structure for the input in the vertex shader
	D3D11_INPUT_ELEMENT_DESC inputDesc[3] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, 3, vsByteCode.c_str(), vsByteCode.length(), &m_inputLayout);
	return !FAILED(hr);
}

bool FirstPass::InitRTtextures(ID3D11Device* device, UINT winWidth, UINT winHeight)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.ArraySize = 1;                             //Number of textures
	//Bind texture as a rendertarget for output-merger and bind the texture to shader stage
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;   //How many channels for texture and how accuret
	desc.Height = winHeight;
	desc.Width = winWidth;
	desc.MipLevels = 1;                             //1 = Multisampled texture
	desc.MiscFlags = 0;                             //No flags. Can be used for cube-map
	desc.SampleDesc.Count = 1;                      //Number of multisamples per pixel
	desc.Usage = D3D11_USAGE_DEFAULT;

	HRESULT hr;
	for (int i = 0; i < NROFBUFFERS; i++)
	{
		hr = device->CreateTexture2D(&desc, nullptr, &m_renderTargetTextures[i]);
		if (FAILED(hr))
		{
			return false;
		}
	}
	return true;
}

bool FirstPass::InitRTviews(ID3D11Device* device)
{
	D3D11_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;       //Data format
	desc.Texture2D.MipSlice = 0;                        //Index of mipmap level to use mip slice
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; //How rendertarget resource will be accessed

	HRESULT hr;
	for (int i = 0; i < NROFBUFFERS; i++)
	{
		hr = device->CreateRenderTargetView(m_renderTargetTextures[i], &desc, &m_renderTargetViews[i]);
		if (FAILED(hr))
		{
			std::cerr << "Failed to create render target view nr '" << i << "'..." << std::endl;
			return false;
		}
	}
	return true;
}

bool FirstPass::InitRTshaders(ID3D11Device* device)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;       //Data format
	desc.Texture2D.MipLevels = 1;                       //Max number of mipmap levels for view of the texture
	desc.Texture2D.MostDetailedMip = 0;                 //Index of most detailed mipmap
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; //Which shader resource view to use in the union

	HRESULT hr;
	for (int i = 0; i < NROFBUFFERS; i++)
	{
		hr = device->CreateShaderResourceView(m_renderTargetTextures[i], &desc, &m_shaderResourceViews[i]);
		if (FAILED(hr))
		{
			return false;
		}
	}
	return true;
}

bool FirstPass::Initialize(ID3D11Device* device, UINT winWidth, UINT winHeight)
{
	std::string vsByteCode = "";

	if (!InitializeShaders(device, m_vertexShader, "FirstPassVS.cso", m_pixelShader, "FirstPassPS.cso", vsByteCode))
	{
		std::cout << "Failed to initialize vertex- or/and pixelshader..." << std::endl;
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

	//---------Starting to prepare the G-buffers-----------
	if (!InitRTtextures(device, winWidth, winHeight))
	{
		std::cerr << "Failed to initialize textures for gbuffer..." << std::endl;
		return false;
	}
	if (!InitRTviews(device))
	{
		std::cerr << "Failed to initialize the views for gbuffer..." << std::endl;
		return false;
	}
	if (!InitRTshaders(device))
	{
		std::cerr << "Failed to initialize shader resource views for gbuffer..." << std::endl;
		return false;
	}
	//-----Done with the preparation of the G-buffers------

	//Initialize depth buffer
	if (!InitializeDepthBuffer(device, m_depthTexture, m_depthView, winWidth, winHeight))
	{
		std::cerr << "Failed to initialize the depth buffer..." << std::endl;
		return false;
	}

	InitializeViewport(m_viewport, winWidth, winHeight);

	return true;
}

void FirstPass::Bind(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetInputLayout(m_inputLayout);

	deviceContext->RSSetViewports(1, &m_viewport);

	deviceContext->OMSetRenderTargets(NROFBUFFERS, m_renderTargetViews, m_depthView);	//EFTER ETT VARV. S� BLIR DET KNAS H�R...

	//Clearing what to draw to
	for (int i = 0; i < NROFBUFFERS; i++)
	{
		deviceContext->ClearRenderTargetView(m_renderTargetViews[i], m_clearColour);
	}
	deviceContext->ClearDepthStencilView(m_depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	deviceContext->PSSetSamplers(0, 1, &m_anisoSampler);

	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
}

ID3D11ShaderResourceView* FirstPass::GetShaderResourceView(int index)
{
	return m_shaderResourceViews[index];
}
