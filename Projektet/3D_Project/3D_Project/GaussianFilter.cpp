#include "GaussianFilter.h"

GaussianFilter::GaussianFilter()
{
	m_computeShader = nullptr;
	m_unorderedAccessView = nullptr;
}

GaussianFilter::~GaussianFilter()
{
	if (m_computeShader)
		m_computeShader->Release();
	if (m_unorderedAccessView)
		m_unorderedAccessView->Release();

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

bool GaussianFilter::Initialize(ID3D11Device* device, IDXGISwapChain* swapChain)
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

	return true;
}

void GaussianFilter::Render(ID3D11DeviceContext* deviceContext)
{
	//We do not won't anything to change the rendertarget when are going to use it
	ID3D11RenderTargetView* nullrtv = nullptr;
	deviceContext->OMSetRenderTargets(1, &nullrtv, nullptr);

	//LATER IF GAUSS IS ON OR OFF***

	//Binding
	deviceContext->CSSetShader(m_computeShader, nullptr, 0);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_unorderedAccessView, nullptr);

	//Use the compute shader
	deviceContext->Dispatch(32, 32, 1);		//om detta skalas upp får man en större  variant på displayen

	//Unbind the unordered access view
	ID3D11UnorderedAccessView* nulluav = nullptr;
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nulluav, nullptr);
}
