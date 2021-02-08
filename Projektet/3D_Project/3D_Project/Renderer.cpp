#include "Renderer.h"

Renderer::Renderer(UINT winWidth, UINT winHeight)
	:m_winWidth(winWidth), m_winHeight(winHeight)
{
	//Directx
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_swapChain = nullptr;
	
	//m_camera = nullptr;
	m_WVPBuffer = nullptr;
	m_cbWVP = {};

	m_rotationtest = 0.0f;	//*****for testing
}

Renderer::~Renderer()
{
	//Releases all which is pointing at something
	if (m_device)
		m_device->Release();
	if (m_deviceContext)
		m_deviceContext->Release();
	if (m_swapChain)
		m_swapChain->Release();

	//
	if (m_WVPBuffer)
		m_WVPBuffer->Release();

	//delete m_camera; //still gives some leaks

	//Cleaning up objects. Do not won't memory leaks
	for (MeshObject* obj : m_objects)
	{
		delete obj;
		obj = nullptr;
	}
	
}

bool Renderer::CreateDeviceAndSwapChain(HWND window)
{
	DXGI_SWAP_CHAIN_DESC desc = {};
	desc.BufferDesc.Width = m_winWidth;
	desc.BufferDesc.Height = m_winHeight;
	desc.BufferDesc.RefreshRate.Numerator = 0;				//Refreshrate limiter. 0 = no limit
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//8 bits for every R, G, B, A - with values from 0.0 - 1.0
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.SampleDesc.Count = 1;								//Nr of multisamples per pixel. 1 = does not use it
	desc.SampleDesc.Quality = 0;							//Quality of anti-alising
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 2;									//Need to have two buffers for ...flip_discard
	desc.OutputWindow = window;
	desc.Windowed = true;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		//Better performance and lower power usage (https://devblogs.microsoft.com/directx/dxgi-flip-model/)
	desc.Flags = 0;

	//In debugging mode, show messages
	UINT flags = 0;
	if (_DEBUG)
		flags = D3D11_CREATE_DEVICE_DEBUG;

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 }; //Versions of hardware to support
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, featureLevels, 1,
		D3D11_SDK_VERSION, &desc, &m_swapChain, &m_device, nullptr, &m_deviceContext);

	return !FAILED(hr);
}

bool Renderer::SetupWVP_CB()
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

bool Renderer::Setup(HINSTANCE hInstance, int nCmdShow, HWND& window)
{
	//Setting up the window settings
	if (!SetupWindow(hInstance, m_winWidth, m_winHeight, nCmdShow, window))
	{
		std::cerr << "Failed to setup window..." << std::endl;
		return false;
	}

	//Prepares and setups the swapchain with device and devicecontext
	if (!CreateDeviceAndSwapChain(window))
	{
		std::cerr << "Failed to create device and swap chain..." << std::endl;
		return false;
	}

	//Reads the ico-files for the shaders and saves to buffers
	if (!m_firstPass.Initialize(m_device, m_winWidth, m_winHeight))
	{
		std::cerr << "Failed to initialize the first pass..." << std::endl;
		return false;
	}
	if (!m_secondPass.Initialize(m_device, m_winWidth, m_winHeight, m_swapChain))
	{
		std::cerr << "Failed to initialize the second pass..." << std::endl;
		return false;
	}

	if (!m_screenQuad.Initialize(m_device, m_winWidth, m_winHeight))
	{
		std::cerr << "Failed to initialize the screenquad..." << std::endl;
		return false;
	}

	//---------FIX SEPARATE FUNCTION FOR ALL LOADING OF OBJECTS----------------
	MeshObject* mesh0 = new MeshObject();
	if (!mesh0->Load(m_device, "smallcat.obj", "Grey.png"))
	{
		std::cerr << "Failed to load mesh0..." << std::endl;
		return false;
	}
	m_objects.push_back(mesh0);

	MeshObject* mesh1 = new MeshObject();
	if (!mesh1->Load(m_device, "cube.obj", "TechFlip.png"))
	{
		std::cerr << "Failed to load mesh1..." << std::endl;
		return false;
	}
	m_objects.push_back(mesh1);
	//-------------------------------------------------------------------------

	//m_camera = new Camera(m_winWidth, m_winHeight);

	//Setup the world view projection matrixes
	if (!SetupWVP_CB())
	{
		std::cerr << "Failed to setup world view matrixes..." << std::endl;
		return false;
	}

	//Setup lights


	return true;
}

void Renderer::Render()
{
	//First pass - Only for geometry - outputs data to the g-buffers. 
	//Binds the right targets and clears them before new information
	m_firstPass.Bind(m_deviceContext);

	//WVP
	m_deviceContext->VSSetConstantBuffers(0, 1, &m_WVPBuffer);


	//Render all objects - sents to 
	for (MeshObject* obj : m_objects)
	{
		obj->Render(m_deviceContext);
	}
	
	//Second pass - Only for lightning - output to the final render target
	//Uses the information saves in g-buffer and compute the lightning
	m_secondPass.Bind(m_deviceContext, m_firstPass.GetShaderResourceView(0), m_firstPass.GetShaderResourceView(1));

	//Render to the screen quad
	m_screenQuad.RenderBuffer(m_deviceContext);


	m_swapChain->Present(1, 0);
}

void Renderer::StartGameLoop()
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
		Render();
	}
}
