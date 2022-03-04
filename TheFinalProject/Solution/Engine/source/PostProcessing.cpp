#include "PCH.h"
#include "PostProcessing.h"

bool PostProcessing::CreateViews(ID3D11Device* device, IDXGISwapChain* swapChain)
{
	//Get the backbuffers texture and map it to this SRV's and UAV's
	ID3D11Texture2D* backBuffer = nullptr;
	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
	{
		std::cerr << "Failed to get back buffer..." << std::endl;
		return false;
	}

	HRESULT hr = device->CreateShaderResourceView(backBuffer, nullptr, &m_backbuffSRV);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create srv..." << std::endl;
		backBuffer->Release();
		return false;
	}

	hr = device->CreateUnorderedAccessView(backBuffer, nullptr, &m_backbuffUAV);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create uav..." << std::endl;
		backBuffer->Release();
		return false;
	}

	D3D11_TEXTURE2D_DESC backbuffDesc;
	backBuffer->GetDesc(&backbuffDesc);
	backBuffer->Release();

	//Create the middleground texture
	D3D11_TEXTURE2D_DESC textDesc = {};
	textDesc.Width = backbuffDesc.Width;
	textDesc.Height = backbuffDesc.Height;
	textDesc.MipLevels = 1;
	textDesc.ArraySize = 1;
	textDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textDesc.SampleDesc.Count = 1;
	textDesc.SampleDesc.Quality = 0;
	textDesc.Usage = D3D11_USAGE_DEFAULT;
	textDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textDesc.CPUAccessFlags = 0;
	textDesc.MiscFlags = 0;

	ID3D11Texture2D* middleGroundText;

	hr = device->CreateTexture2D(&textDesc, 0, &middleGroundText);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create texture..." << std::endl;
		if (middleGroundText)
			middleGroundText->Release();
		return false;
	}

	//Shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(middleGroundText, &srvDesc, &m_middlegroundSRV);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create SRV..." << std::endl;
		if (middleGroundText)
			middleGroundText->Release();
		return false;
	}

	//Unordered access view
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	hr = device->CreateUnorderedAccessView(middleGroundText, &uavDesc, &m_middlegroundUAV);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create UAV..." << std::endl;
		if (middleGroundText)
			middleGroundText->Release();
		return false;
	}

	middleGroundText->Release();
	return true;
}

bool PostProcessing::LoadShaders(ID3D11Device* device)
{
	std::string shaderData;
	std::ifstream reader;

	reader.open("Shaders/GaussianFilterCS.cso", std::ios::binary | std::ios::ate);  //ate: Begins at the end
	if (!reader.is_open())
	{
		std::cerr << "Could not open ComputeShader for gaussian filter..." << std::endl;
		return false;
	}

	shaderData.reserve((UINT)reader.tellg());  //How many bytes to reserve in string. Reads from the pointer where it is
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());  //Copy data from start until string is filled
	HRESULT hr = device->CreateComputeShader(shaderData.c_str(), shaderData.length(), nullptr, &m_gaussShader);
	reader.close();
	shaderData.clear();

	if (FAILED(hr))
	{
		std::cerr << "Failed to create compute shader for gaussian filter..." << std::endl;
		return false;
	}

	reader.open("Shaders/BilateralFilterCS.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open ComputeShader for bilateral filter..." << std::endl;
		return false;
	}

	shaderData.reserve((UINT)reader.tellg());
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
	hr = device->CreateComputeShader(shaderData.c_str(), shaderData.length(), nullptr, &m_bilateralShader);
	reader.close();
	shaderData.clear();

	if (FAILED(hr))
	{
		std::cerr << "Failed to create compute shader for bilateral filter..." << std::endl;
		return false;
	}

	return true;
}

bool PostProcessing::CreateConstantBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(GaussSettings);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &m_gaussSettings;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&desc, &data, &m_gaussSettingsBuf)))
	{
		std::cerr << "Failed to create settingsbuffer for gauss..." << std::endl;
		return false;
	}

	desc.ByteWidth = sizeof(GaussWeights);
	data.pSysMem = &m_gaussWeights;
	if (FAILED(device->CreateBuffer(&desc, &data, &m_gaussWeightBuf)))
	{
		std::cerr << "Failed to create weights for gauss..." << std::endl;
		return false;
	}

	desc.ByteWidth = sizeof(BilateralSettings);
	data.pSysMem = &m_bilateralSettings;
	if (FAILED(device->CreateBuffer(&desc, &data, &m_bilateralSettingsBuf)))
	{
		std::cerr << "Failed to create settings for bilateral filter..." << std::endl;
		return false;
	}

	desc.ByteWidth = sizeof(BilateralWeights);
	data.pSysMem = &m_bilateralWeights;
	if (FAILED(device->CreateBuffer(&desc, &data, &m_bilateralWeightBuf)))
	{
		std::cerr << "Failed to create weights for bilateral filter..." << std::endl;
		return false;
	}

	return true;
}

void PostProcessing::UpdateSettingsBuffer(ID3D11DeviceContext* deviceContext, const Filter& filter)
{
	if (filter == Filter::GAUSSIAN)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		deviceContext->Map(m_gaussSettingsBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &m_gaussSettings, sizeof(GaussSettings));
		deviceContext->Unmap(m_gaussSettingsBuf, 0);
	}
	else if (filter == Filter::BILATERAL)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		deviceContext->Map(m_bilateralSettingsBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &m_bilateralSettings, sizeof(BilateralSettings));
		deviceContext->Unmap(m_bilateralSettingsBuf, 0);
	}
}

void PostProcessing::SwapDirection(ID3D11DeviceContext* deviceContext, const Filter& filter)
{
	if (filter == Filter::GAUSSIAN)
	{
		if (m_gaussSettings.doVertBlur)
			m_gaussSettings.doVertBlur = false;
		else
			m_gaussSettings.doVertBlur = true;
	}
	else if (filter == Filter::BILATERAL)
	{
		if (m_bilateralSettings.doVertFilter)
			m_bilateralSettings.doVertFilter = false;
		else
			m_bilateralSettings.doVertFilter = true;
	}
	UpdateSettingsBuffer(deviceContext, filter);
}

bool PostProcessing::GenerateGaussFilter(UINT radius, float sigma, const Filter& filter)
{
	if (radius > ((MAXWEIGHTSIZE / 2) - 1) || radius < 1)
	{
		std::cerr << "Radius has to be in the range of 1 - 15..." << std::endl;
		return false;
	}

	//If no input we calculate the sigma
	if (sigma == 0)
	{
		//Calculates the sigma value to fit with the bell curve
		//Will make the blur at the best it can be relative to the radius
		sigma = float(radius / 2.0f);
	}

	int x = radius * -1;
	int index = 0;
	float halfGaussWeights[(MAXWEIGHTSIZE / 2) + 1] = {};

	//Goes from the back to the main square, 0
	while (x <= 0)
	{
		//Formula from: https://en.wikipedia.org/wiki/Gaussian_blur
		float weight = (float)(1.0f / sqrt(double(2) * PI * pow(sigma, 2)) * exp(-pow(x, 2) / (2 * pow(sigma, 2))));
		halfGaussWeights[index++] = weight;
		x++;
	}

	//Calculate sum to normalize the values later
	float sum = 0.0f;
	for (UINT i = 0; i < radius; i++)
	{
		sum += halfGaussWeights[i];
	}
	sum *= 2;							//Only went through half of the total array
	sum += halfGaussWeights[radius];	//Add the middle

	int weights = radius * 2 + 1;
	int goBackIndex = radius - 1;

	//Fix the final weights so that it is symmetrical
	for (int i = 0; i < weights; i++)
	{
		if (filter == Filter::GAUSSIAN)
		{
			//Walk forward
			if (i <= (int)radius)
				m_gaussWeights.weights[i] = halfGaussWeights[i];
			//Walk backward
			else if (goBackIndex >= 0)
				m_gaussWeights.weights[i] = halfGaussWeights[goBackIndex--];

			//Normalize the values
			m_gaussWeights.weights[i] /= sum;
		}
		else if (filter == Filter::BILATERAL)
		{
			if (i <= (int)radius)
				m_bilateralWeights.weights[i] = halfGaussWeights[i];
			else if (goBackIndex >= 0)
				m_bilateralWeights.weights[i] = halfGaussWeights[goBackIndex--];

			m_bilateralWeights.weights[i] /= sum;
		}
	}

	return true;
}


PostProcessing::PostProcessing()
{
	m_backbuffSRV = nullptr;
	m_middlegroundUAV = nullptr;
	m_middlegroundSRV = nullptr;
	m_backbuffUAV = nullptr;
	m_nullSRV = nullptr;
	m_nullUAV = nullptr;

	m_gaussShader = nullptr;
	m_bilateralShader = nullptr;
	m_gaussSettingsBuf = nullptr;
	m_gaussWeightBuf = nullptr;
	m_bilateralSettingsBuf = nullptr;
	m_bilateralWeightBuf = nullptr;

	m_gaussSettings = {};
	m_gaussWeights = {};
	m_bilateralSettings = {};
	m_bilateralWeights = {};

	m_useGauss = false;
	m_useBilateral = false;
}

PostProcessing::~PostProcessing()
{
	if (m_backbuffSRV)
		m_backbuffSRV->Release();
	if (m_middlegroundUAV)
		m_middlegroundUAV->Release();
	if (m_middlegroundSRV)
		m_middlegroundSRV->Release();
	if (m_backbuffUAV)
		m_backbuffUAV->Release();
	if (m_nullSRV)
		m_nullSRV->Release();
	if (m_nullUAV)
		m_nullUAV->Release();

	if (m_gaussShader)
		m_gaussShader->Release();
	if (m_bilateralShader)
		m_bilateralShader->Release();
	if (m_gaussSettingsBuf)
		m_gaussSettingsBuf->Release();
	if (m_gaussWeightBuf)
		m_gaussWeightBuf->Release();
	if (m_bilateralSettingsBuf)
		m_bilateralSettingsBuf->Release();
	if (m_bilateralWeightBuf)
		m_bilateralWeightBuf->Release();
}

bool PostProcessing::Initialize(ID3D11Device* device, 
								IDXGISwapChain* swapChain, 
								UINT gaussRadius, 
								float gaussSigma,
								UINT bilateralRadius, 
								float bilateralSigma)
{
	if (!CreateViews(device, swapChain))
	{
		std::cerr << "CreateViews() failed..." << std::endl;
		return false;
	}
	if (!LoadShaders(device))
	{
		std::cerr << "LoadShaders() failed..." << std::endl;
		return false;
	}

	m_gaussSettings.radius = gaussRadius;
	m_gaussSettings.doVertBlur = true;

	if (!GenerateGaussFilter(gaussRadius, gaussSigma, Filter::GAUSSIAN))
	{
		std::cerr << "GenerateGaussFilter() failed for gaussian blur..." << std::endl;
		return false;
	}

	m_bilateralSettings.radius = bilateralRadius;
	m_bilateralSettings.doVertFilter = true;
	m_bilateralSettings.sigma = bilateralSigma;
	
	if (!GenerateGaussFilter(bilateralRadius, 0, Filter::BILATERAL))
	{
		std::cerr << "GenerateGaussFilter() failed for bilateral filter..." << std::endl;
		return false;
	}

	if (!CreateConstantBuffers(device))
	{
		std::cerr << "CreateConstantBuffers() failed..." << std::endl;
		return false;
	}

	return true;
}

void PostProcessing::TurnOnGauss(bool toggle)
{
	m_useGauss = toggle;
}

void PostProcessing::TurnOnBilateral(bool toggle)
{
	m_useBilateral = toggle;
}

void PostProcessing::Render(ID3D11DeviceContext* deviceContext, UINT winWidth, UINT winHeight)
{
	if (m_useGauss || m_useBilateral)
	{
		//We do not won't anything to change the rendertarget when we are going to use it
		ID3D11RenderTargetView* nullrtv = nullptr;
		deviceContext->OMSetRenderTargets(1, &nullrtv, nullptr);

		//Does the filters in two passes - like a plus-sign - really efficient 
		if (m_useBilateral)
		{
			//Set shader and constantbuffers needed
			deviceContext->CSSetShader(m_bilateralShader, nullptr, 0);
			deviceContext->CSSetConstantBuffers(0, 1, &m_bilateralSettingsBuf);
			deviceContext->CSSetConstantBuffers(1, 1, &m_bilateralWeightBuf);

			//Bind the backbuffers SRV (input) and the middlegrounds UAV (output)
			deviceContext->CSSetShaderResources(0, 1, &m_backbuffSRV);
			deviceContext->CSSetUnorderedAccessViews(0, 1, &m_middlegroundUAV, nullptr);
			
			//Do the filter in one direction
			deviceContext->Dispatch(winWidth / 8, winHeight / 8, 1);
			SwapDirection(deviceContext, Filter::BILATERAL);

			//Unbind the used views
			deviceContext->CSSetShaderResources(0, 1, &m_nullSRV);
			deviceContext->CSSetUnorderedAccessViews(0, 1, &m_nullUAV, nullptr);

			//Bind the middleground SRV (input) and backbuffers UAV (output)
			deviceContext->CSSetShaderResources(0, 1, &m_middlegroundSRV);
			deviceContext->CSSetUnorderedAccessViews(0, 1, &m_backbuffUAV, nullptr);

			//Do the filter in other direction
			deviceContext->Dispatch(winWidth / 8, winHeight / 8, 1);
			SwapDirection(deviceContext, Filter::BILATERAL);

			//Unbind the used views
			deviceContext->CSSetShaderResources(0, 1, &m_nullSRV);
			deviceContext->CSSetUnorderedAccessViews(0, 1, &m_nullUAV, nullptr);
		}

		if (m_useGauss)
		{
			//Set shader and constantbuffers needed
			deviceContext->CSSetShader(m_gaussShader, nullptr, 0);
			deviceContext->CSSetConstantBuffers(0, 1, &m_gaussSettingsBuf);
			deviceContext->CSSetConstantBuffers(1, 1, &m_gaussWeightBuf);
			
			//Bind the backbuffers SRV (input) and the middlegrounds UAV (output)
			deviceContext->CSSetShaderResources(0, 1, &m_backbuffSRV);
			deviceContext->CSSetUnorderedAccessViews(0, 1, &m_middlegroundUAV, nullptr);

			//Blur in one direction
			deviceContext->Dispatch(winWidth / 8, winHeight / 8, 1);
			SwapDirection(deviceContext, Filter::GAUSSIAN);

			//Unbind the used views
			deviceContext->CSSetShaderResources(0, 1, &m_nullSRV);
			deviceContext->CSSetUnorderedAccessViews(0, 1, &m_nullUAV, nullptr);
			
			//Bind the middleground SRV (input) and backbuffers UAV (output)
			deviceContext->CSSetShaderResources(0, 1, &m_middlegroundSRV);
			deviceContext->CSSetUnorderedAccessViews(0, 1, &m_backbuffUAV, nullptr);

			//Blur in the other direction
			deviceContext->Dispatch(winWidth / 8, winHeight / 8, 1);
			SwapDirection(deviceContext, Filter::GAUSSIAN);

			//Unbind the used views
			deviceContext->CSSetShaderResources(0, 1, &m_nullSRV);
			deviceContext->CSSetUnorderedAccessViews(0, 1, &m_nullUAV, nullptr);
		}

		//Unbinding constant buffers
		ID3D11Buffer* nullCBuff = nullptr;
		deviceContext->CSSetConstantBuffers(0, 1, &nullCBuff);
		deviceContext->CSSetConstantBuffers(1, 1, &nullCBuff);
	}

}
