#include "GaussianFilter.h"

GaussianFilter::GaussianFilter()
{
	m_computeShader = nullptr;
	m_unorderedAccessView = nullptr;
	m_gaussBuffer = nullptr;
	m_gaussStruct = {};
	m_useGaussFilter = false;
}

GaussianFilter::~GaussianFilter()
{
	if (m_computeShader)
		m_computeShader->Release();
	if (m_unorderedAccessView)
		m_unorderedAccessView->Release();
	if (m_gaussBuffer)
		m_gaussBuffer->Release();
}

bool GaussianFilter::CreateComputeShader(ID3D11Device* device)
{
	std::string shaderData;
	std::ifstream reader;

	reader.open("Shaders/GaussianFilterCS.cso", std::ios::binary | std::ios::ate);  //ate: Begins at the end
	if (!reader.is_open())
	{
		std::cerr << "Could not open vertex shader..." << std::endl;
		return false;
	}

	shaderData.reserve((unsigned int)reader.tellg());  //How many bytes to reserve in string. Reads from the pointer where it is
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());  //Copy data from start until string is filled

	return !FAILED(device->CreateComputeShader(shaderData.c_str(), shaderData.length(), nullptr, &m_computeShader));
}

bool GaussianFilter::CreateUnorderedAccessView(ID3D11Device* device, IDXGISwapChain* swapChain)
{
	ID3D11Texture2D* backBuffer = nullptr;
	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
	{
		std::cerr << "Failed to get back buffer..." << std::endl;
		return false;
	}
	HRESULT hr = device->CreateUnorderedAccessView(backBuffer, nullptr, &m_unorderedAccessView);
	backBuffer->Release();
	return !FAILED(hr);
}

void GaussianFilter::SetGaussStruct(float width, float height, UINT radius)
{
	m_gaussStruct.winWidth = width;
	m_gaussStruct.winHeight = height;
	m_gaussStruct.radius = radius;
	m_gaussStruct.doVertBlur = true;
}

bool GaussianFilter::CreateGaussConstBuff(ID3D11Device* device)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(GaussStruct);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &m_gaussStruct;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	return !FAILED(device->CreateBuffer(&desc, &data, &m_gaussBuffer));
}

void GaussianFilter::SwapBlurDirection(ID3D11DeviceContext* deviceContext)
{
	if (m_gaussStruct.doVertBlur)
		m_gaussStruct.doVertBlur = false;
	else
		m_gaussStruct.doVertBlur = true;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext->Map(m_gaussBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &m_gaussStruct, sizeof(GaussStruct));
	deviceContext->Unmap(m_gaussBuffer, 0);
}

bool GaussianFilter::GenerateGaussFilter()
{
	if (m_gaussStruct.radius > ((MAXGAUSSWEIGHTS / 2) - 1) )
	{
		std::cerr << "Reached weight limit..." << std::endl;
		return false;
	}

	UINT radius = m_gaussStruct.radius;

	//Calculates the sigma value to fit with the bell curve
	float sigma = float(radius / 2.0f);
	int x = radius * -1;
	int index = 0;
	float halfGaussWeights[MAXGAUSSWEIGHTS] = {};

	//Goes from the back to the main square, 0
	while (x <= 0)
	{
		//Formula from: https://en.wikipedia.org/wiki/Gaussian_blur
		halfGaussWeights[index++] = (float)(1.0f / sqrt(2 * MATH_PI * pow(sigma, 2)) * exp(-pow(x, 2) / (2 * pow(sigma, 2))));
		x++;
	}

	int weights = radius * 2 + 1;
	int goBackIndex = radius - 1;

	//Fix the final weights so that it is symmetrical
	for (int i = 0; i < weights; i++)
	{
		//Walk forward
		if (i <= (int)radius)
			m_gaussStruct.weights[i] = halfGaussWeights[i];
		//Walk backward
		else
			m_gaussStruct.weights[i] = halfGaussWeights[goBackIndex--];
	}

	return true;
}

bool GaussianFilter::Initialize(ID3D11Device* device, IDXGISwapChain* swapChain, float width, float height, UINT radius)
{
	if (!CreateComputeShader(device))
	{
		std::cerr << "CreateComputeShader() failed..." << std::endl;
		return false;
	}

	if (!CreateUnorderedAccessView(device, swapChain))
	{
		std::cerr << "CreateUnorderedAccessView() failed..." << std::endl;
		return false;
	}

	SetGaussStruct(width, height, radius);

	if (!GenerateGaussFilter())
	{
		std::cerr << "GenerateGaussFilter() failed..." << std::endl;
		return false;
	}

	if (!CreateGaussConstBuff(device))
	{
		std::cerr << "CreateGaussConstBuff() failed..." << std::endl;
		return false;
	}

	return true;
}

void GaussianFilter::Render(ID3D11DeviceContext* deviceContext, UINT winWidth, UINT winHeight, int nrOfLoops)
{
	if (m_useGaussFilter)
	{
		//We do not won't anything to change the rendertarget when are going to use it
		ID3D11RenderTargetView* nullrtv = nullptr;
		deviceContext->OMSetRenderTargets(1, &nullrtv, nullptr);

		//Binding
		deviceContext->CSSetShader(m_computeShader, nullptr, 0);
		deviceContext->CSSetUnorderedAccessViews(0, 1, &m_unorderedAccessView, nullptr);
		deviceContext->CSSetConstantBuffers(0, 1, &m_gaussBuffer);

		//How many times to blur a frame
		while (nrOfLoops > 0)
		{
			//Blur in one direction - either vertical or horizontal
			deviceContext->Dispatch(winWidth / 8, winHeight / 8, 1);

			//Swap direction to blur
			SwapBlurDirection(deviceContext);

			//Blur in the other direction
			deviceContext->Dispatch(winWidth / 8, winHeight / 8, 1);

			nrOfLoops--;
		}

		//Unbind the unordered access view
		ID3D11UnorderedAccessView* nulluav = nullptr;
		deviceContext->CSSetUnorderedAccessViews(0, 1, &nulluav, nullptr);
	}
}

void GaussianFilter::TurnOnGaussFilter(bool trueOrFalse)
{
	m_useGaussFilter = trueOrFalse;
}
