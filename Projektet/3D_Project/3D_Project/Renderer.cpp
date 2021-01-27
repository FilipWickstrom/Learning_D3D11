#include "Renderer.h"

Renderer::Renderer(UINT winWidth, UINT winHeight)
	:m_winWidth(winWidth), m_winHeight(winHeight)
{
	//Directx
	this->m_device = nullptr;
	this->m_deviceContext = nullptr;
	this->m_swapChain = nullptr;
	this->m_renderTargetView = nullptr;
	this->m_depthStencilTexture = nullptr;
	this->m_depthStencilView = nullptr;
	this->m_viewport = {};
	
	//Shaders
	this->m_vertexShader = nullptr;
	this->m_pixelShader = nullptr;
	this->m_inputLayout = nullptr;

	//Textures
	this->m_2dtexture = nullptr;
	this->m_textureShaderResourceView = nullptr;
	this->m_samplerState = nullptr;

}

Renderer::~Renderer()
{
	this->m_device->Release();
	this->m_deviceContext->Release();
	this->m_swapChain->Release();
	this->m_renderTargetView->Release();
	this->m_depthStencilTexture->Release();
	this->m_depthStencilView->Release();
	this->m_vertexShader->Release();
	this->m_pixelShader->Release();
	this->m_inputLayout->Release();
	
	//TEMP
	if (this->m_2dtexture != nullptr)
		this->m_2dtexture->Release();
	if (this->m_textureShaderResourceView != nullptr)
		this->m_textureShaderResourceView->Release();
	if (this->m_samplerState != nullptr)
		this->m_samplerState->Release();
	
}

bool Renderer::Setup(HINSTANCE hInstance, int nCmdShow, HWND& window)
{
	//Setting up the window settings
	if (!SetupWindow(hInstance, m_winWidth, m_winHeight, nCmdShow, window))
	{
		std::cerr << "Failed to setup window..." << std::endl;
		return false;
	}

	//Setup the directx structures
	if (!SetupD3D11(m_winWidth, m_winHeight, window, m_device, m_deviceContext, m_swapChain,
					m_renderTargetView, m_depthStencilTexture, m_depthStencilView, m_viewport))
	{
		std::cerr << "Failed to setup directx..." << std::endl;
		return false;
	}

	//Setup pipeline
	if (!SetupPipeline(m_device, m_vertexShader, m_pixelShader, m_inputLayout,
					   m_2dtexture, m_textureShaderResourceView, m_samplerState))
	{
		std::cerr << "Failed to setup pipeline..." << std::endl;
		return false;
	}

	//Setup meshes
	m_mesh1.Load(m_device);

	//Setup wvp


	//Setup lights


	return true;
}

void Renderer::Update()
{
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		//Looks at the message and removes it from message queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//Testing reading messages
		if (msg.message == WM_KEYDOWN)
		{
			//Move forward
			if (msg.wParam == 'W')
			{
				std::cout << "Pressed W" << std::endl;
				//call move(forwards);
			}
			//Move backwards
			else if (msg.wParam == 'S')
			{
				std::cout << "Pressed S" << std::endl;
				//call move(backwards);
			}
			//Move left
			else if (msg.wParam == 'A')
			{
				std::cout << "Pressed A" << std::endl;
			}
			//Move right
			else if (msg.wParam == 'D')
			{
				std::cout << "Pressed D" << std::endl;
			}
		}

		//Draw the scene
		Draw();
	}
}

void Renderer::Draw()
{
	float clearColour[4] = { 0.0f,0.0f,0.0f,0.0f };	//Black
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColour);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);	//NEEDED?

	//m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		//FIX WITH INDEX BUFFER***
	m_deviceContext->IASetInputLayout(m_inputLayout);

	m_deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	//textures?

	//update wvp

	//update lights?

	m_deviceContext->RSSetViewports(1, &m_viewport);
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	m_mesh1.Render(m_deviceContext);
	
	
	m_swapChain->Present(1, 0);
}

