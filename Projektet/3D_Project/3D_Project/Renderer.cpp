#include "Renderer.h"

bool Renderer::setupWVP_CB()
{
	D3D11_BUFFER_DESC cb_desc = {};
	cb_desc.ByteWidth = sizeof(constantBufferWVP);
	cb_desc.Usage = D3D11_USAGE_DYNAMIC;
	cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb_desc.MiscFlags = 0;
	cb_desc.StructureByteStride = 0;

	//-------TESTING-----------------------------------------------
	//View matrix
	DirectX::XMVECTOR eyePos = { 0.0f, 0.0f, 0.0f };
	DirectX::XMVECTOR lookAtPos = { 0.0f, 0.0f, 1.0f };
	DirectX::XMVECTOR upDir = { 0.0f, 1.0f, 0.0f };
	XMStoreFloat4x4(&m_cbWVP.view, XMMatrixLookAtLH(eyePos, lookAtPos, upDir));

	//Projection matrix
	float fieldOfView = XM_PI * 0.5f;			//90 degrees
	float aspectRatio = float(m_winWidth / m_winHeight);
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	XMStoreFloat4x4(&m_cbWVP.projection, XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearPlane, farPlane));

	//XMStoreFloat4x4(&m_cbWVP.world, XMMatrixIdentity());	// * DirectX::XMMatrixTranslation(0, 0, 1)
	//---------------------------------------------------------------


	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = &m_cbWVP;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	HRESULT hr = m_device->CreateBuffer(&cb_desc, &initData, &m_WVPBuffer);
	return !FAILED(hr);
}

Renderer::Renderer(UINT winWidth, UINT winHeight)
	:m_winWidth(winWidth), m_winHeight(winHeight)
{
	//Directx
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_swapChain = nullptr;
	m_renderTargetView = nullptr;
	m_depthStencilTexture = nullptr;
	m_depthStencilView = nullptr;
	m_viewport = {};
	
	//Shaders
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_inputLayout = nullptr;

	m_samplerState = nullptr;

	//m_camera = nullptr;
	m_WVPBuffer = nullptr;
	m_cbWVP = {};

	//TESTING
	m_rotationtest = 0.0f;
}

Renderer::~Renderer()
{
	m_device->Release();
	m_deviceContext->Release();
	m_swapChain->Release();
	m_renderTargetView->Release();
	m_depthStencilTexture->Release();
	m_depthStencilView->Release();
	m_vertexShader->Release();
	m_pixelShader->Release();
	m_inputLayout->Release();
	
	//TEMP
	if (m_samplerState != nullptr)
		m_samplerState->Release();

	if (m_WVPBuffer != nullptr)
		m_WVPBuffer->Release();

	//delete m_camera; //still gives some leaks
	
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
	if (!SetupPipeline(m_device, m_vertexShader, m_pixelShader, m_inputLayout, m_samplerState))
	{
		std::cerr << "Failed to setup pipeline..." << std::endl;
		return false;
	}

	//Setup meshes
	if (!m_mesh1.Load(m_device))
	{
		std::cerr << "Failed to load mesh..." << std::endl;
		return false;
	}

	//m_camera = new Camera(m_winWidth, m_winHeight);

	//Setup the world view projection matrixes
	if (!setupWVP_CB())
	{
		std::cerr << "Failed to setup world view matrixes..." << std::endl;
		return false;
	}

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

		//KEYBOARD MOVEMENT - FIX SEPERATE FUNCTION FOR THIS????****
		if (msg.message == WM_KEYDOWN)
		{
			bool moveKeyPressed = false;

			//Move forward
			if (msg.wParam == 'W')
			{
				std::cout << "Pressed W" << std::endl;
				//m_camera->Move(DirectX::XMFLOAT3(0.0f, 0.0f, 0.01f));
				moveKeyPressed = true;
			}
			//Move backwards
			else if (msg.wParam == 'S')
			{
				std::cout << "Pressed S" << std::endl;
				//m_camera->Move(DirectX::XMFLOAT3(0.0f, 0.0f, -0.01f));
				moveKeyPressed = true;
			}
			//Move up
			else if (msg.wParam == 'Q')
			{
				std::cout << "Pressed Q" << std::endl;
				//m_camera->Move(DirectX::XMFLOAT3(0.0f, 0.01f, 0.0f));
				moveKeyPressed = true;
			}
			//Move down
			else if (msg.wParam == 'E')
			{
				std::cout << "Pressed E" << std::endl;
				//m_camera->Move(DirectX::XMFLOAT3(0.0f, -0.01f, 0.0f));
				moveKeyPressed = true;
			}
			//Move left
			if (msg.wParam == 'A')
			{
				std::cout << "Pressed A" << std::endl;
				//m_camera->Move(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
				moveKeyPressed = true;
			}
			//Move right
			else if (msg.wParam == 'D')
			{
				std::cout << "Pressed D" << std::endl;
				//m_camera->Move(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
				moveKeyPressed = true;
			}


			/*if (moveKeyPressed)
			{
				m_rotationtest += 0.1f;
				DirectX::XMStoreFloat4x4(&m_cbWVP.world, DirectX::XMMatrixRotationY(DirectX::XM_PI) * DirectX::XMMatrixTranslation(0, 0, 1) *
					DirectX::XMMatrixRotationY(m_rotationtest) * DirectX::XMMatrixTranslation(0, 0, 2));

				//m_cbWVP.view = m_camera->getViewMatrix();
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				m_deviceContext->Map(m_WVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
				memcpy(mappedResource.pData, &m_cbWVP, sizeof(constantBufferWVP));
				m_deviceContext->Unmap(m_WVPBuffer, 0);
			}*/
		}

		m_rotationtest += 0.005f;
		DirectX::XMStoreFloat4x4(&m_cbWVP.world, DirectX::XMMatrixRotationY(m_rotationtest) * DirectX::XMMatrixTranslation(0, 0, 6));

		//m_cbWVP.view = m_camera->getViewMatrix();
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		m_deviceContext->Map(m_WVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &m_cbWVP, sizeof(constantBufferWVP));
		m_deviceContext->Unmap(m_WVPBuffer, 0);

		//Draw the scene
		Draw();
	}
}

void Renderer::Draw()
{
	//Clearing the screen
	float clearColour[4] = { 0.0f,0.0f,0.0f,0.0f };	//Black
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColour);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);

	//Set viewport and what to render to
	m_deviceContext->RSSetViewports(1, &m_viewport);
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_deviceContext->IASetInputLayout(m_inputLayout);

	m_deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);

	//WVP
	m_deviceContext->VSSetConstantBuffers(0, 1, &m_WVPBuffer);

	//update lights?
	//m_device->CreateDeferredContext()

	m_mesh1.Render(m_deviceContext);

	//FIRST PASS - GEOMETRY
	//	only outputs the geometric and matrial data to the g-buffer



	//SECOND PASS - LIGHTNING
	//	uses the g-buffer information
	//	compute lightning
	
	m_swapChain->Present(1, 0);
}

