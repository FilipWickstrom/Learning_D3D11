#include "PassInitFunc.h"

bool InitializeShaders(ID3D11Device* device, ID3D11VertexShader*& vertexShader, std::string vsPath, ID3D11PixelShader*& pixelShader, std::string psPath, std::string& vsByteCode)
{
	std::string shaderData;
	std::ifstream reader;

	//Vertexshader
	reader.open("Shaders/" + vsPath, std::ios::binary | std::ios::ate);  //ate: Begins at the end
	if (!reader.is_open())
	{
		std::cerr << "Could not open vertex shader..." << std::endl;
		return false;
	}

	shaderData.reserve((unsigned int)reader.tellg());  //How many bytes to reserve in string. Reads from the pointer where it is
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());  //Copy data from start until string is filled

	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vertexShader)))
	{
		std::cerr << "Failed to create vertex shader..." << std::endl;
		return false;
	}

	vsByteCode = shaderData;
	shaderData.clear();
	reader.close();

	//Pixelshader
	reader.open("Shaders/" + psPath, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open pixel shader..." << std::endl;
		return false;
	}

	shaderData.reserve((unsigned int)reader.tellg());
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pixelShader)))
	{
		std::cerr << "Failed to create pixel shader..." << std::endl;
		return false;
	}
	return true;
}

bool InitializeAnisoSampler(ID3D11Device* device, ID3D11SamplerState*& sampler)
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

	HRESULT hr = device->CreateSamplerState(&samplerDesc, &sampler);
	return !FAILED(hr);
}

void InitializeViewport(D3D11_VIEWPORT& viewport, UINT winWidth, UINT winHeight)
{
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)winWidth;
	viewport.Height = (float)winHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}

bool InitializeDepthBuffer(ID3D11Device* device, ID3D11Texture2D*& depthTexture, ID3D11DepthStencilView*& depthView, UINT winWidth, UINT winHeight)
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.ArraySize = 1;                         //Only one texture
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;  //Bind for outputmerger
	textureDesc.CPUAccessFlags = 0;                    //No CPU involved 
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//24 bit depth and 8 bit stencil, 0.0 - 1.0
	textureDesc.Height = winHeight;
	textureDesc.Width = winWidth;
	textureDesc.MipLevels = 1;                         //Multisampled texture
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;                  //Only one sample per pixel
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;           //Read and write access to GPU

	HRESULT hr;
	hr = device->CreateTexture2D(&textureDesc, nullptr, &depthTexture);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create depth stencil texture..." << std::endl;
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
	viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;        //Same format as textures
	viewDesc.Texture2D.MipSlice = 0;                        //Index of mipmap to use
	viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; //How the resource can be accessed

	hr = device->CreateDepthStencilView(depthTexture, &viewDesc, &depthView);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create depth stencil view..." << std::endl;
		return false;
	}

	return true;
}
