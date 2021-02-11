#include "Renderer.h"

Renderer::Renderer(UINT winWidth, UINT winHeight)
	:m_winWidth(winWidth), m_winHeight(winHeight)
{
	//Directx
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_swapChain = nullptr;
	
	m_rotationtest = 0.0f;	//*****for testing
	m_deltatime = 0.0f;
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

	//Cleaning up objects. Do not won't memory leaks
	for (MeshObject* obj : m_objects)
	{
		delete obj;
		obj = nullptr;
	}
	
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
	if (!CreateDeviceAndSwapChain(window, m_winWidth, m_winHeight, m_swapChain, m_device, m_deviceContext))
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

	//---------FIX SEPARATE FUNCTION FOR ALL LOADING OF OBJECTS----------------
	MeshObject* mesh0 = new MeshObject();
	if (!mesh0->Load(m_device, "smallcat.obj", "Grey.png", { -2,0,0 }, { 1,1,1 }, {0,-90,0}))
	{
		std::cerr << "Failed to load mesh0..." << std::endl;
		return false;
	}
	m_objects.push_back(mesh0);

	MeshObject* mesh1 = new MeshObject();
	if (!mesh1->Load(m_device, "cube.obj", "TechFlip.png", { 2,0,0 }, { 1,1,1 }, {0, 45, 0}))
	{
		std::cerr << "Failed to load mesh1..." << std::endl;
		return false;
	}
	m_objects.push_back(mesh1);
	//-------------------------------------------------------------------------

	m_camera.Initialize(m_winWidth, m_winHeight, XM_PI * 0.5f, 0.1f, 1000.0f);

	//Setup the world view projection matrixes
	if (!m_constBuffers.InitializeWVP(m_device, m_camera.getViewMatrix(), m_camera.getProjMatrix()))
	{
		std::cerr << "Failed to initialize the world view projection matrix..." << std::endl;
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
	m_constBuffers.SetWVPToVS(m_deviceContext);

	//Render all objects - sents to 
	for (MeshObject* obj : m_objects)
	{
		//Each object has it own world matrix that is needed to be set
		m_constBuffers.UpdateWorld(m_deviceContext, obj->GetModelMatrix());

		//Render the object
		obj->Render(m_deviceContext);
	}
	
	//Second pass - Only for lightning - output to the final render target
	//Uses the information saves in g-buffer and compute the lightning
	m_secondPass.Bind(m_deviceContext, m_firstPass.GetShaderResourceView(0), m_firstPass.GetShaderResourceView(1));

	//Present the final result
	m_swapChain->Present(1, 0);
}

void Renderer::StartGameLoop(HWND& window)
{
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		m_deltatime = m_fpscounter.GetDeltatime();

		//Looks at the message and removes it from message queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//FUNCTION FOR INPUT WITH KEYBOARD AND MOUSE - void Movement()

		//SEND THE DELTATIME TO THE MOVEMENT
		//KEYBOARD MOVEMENT - FIX SEPERATE FUNCTION FOR THIS????**** 
		if (msg.message == WM_KEYDOWN)
		{
			bool moveKeyPressed = false;

			//Move forward
			if (msg.wParam == 'W')
			{
				std::cout << "Pressed W" << std::endl;
				m_camera.Move({0.0f, 0.0f, 1.0f });
				moveKeyPressed = true;
			}
			//Move backwards
			else if (msg.wParam == 'S')
			{
				std::cout << "Pressed S" << std::endl;
				m_camera.Move({ 0.0f, 0.0f, -1.0f });
				moveKeyPressed = true;
			}
			//Move up
			else if (msg.wParam == 'Q')
			{
				std::cout << "Pressed Q" << std::endl;
				m_camera.Move({ 0.0f, 1.0f, 0.0f });
				moveKeyPressed = true;
			}
			//Move down
			else if (msg.wParam == 'E')
			{
				std::cout << "Pressed E" << std::endl;
				m_camera.Move({ 0.0f, -1.0f, 0.0f });
				moveKeyPressed = true;
			}
			//Move left
			if (msg.wParam == 'A')
			{
				std::cout << "Pressed A" << std::endl;
				m_camera.Move({ -1.0f, 0.0f, 0.0f });
				moveKeyPressed = true;
			}
			//Move right
			else if (msg.wParam == 'D')
			{
				std::cout << "Pressed D" << std::endl;
				m_camera.Move({ 1.0f, 0.0f, 0.0f });
				moveKeyPressed = true;
			}
			

			else if (msg.wParam == 'L')
			{
				m_camera.RotateY(1);
				moveKeyPressed = true;
			}
			else if (msg.wParam == 'J')
			{
				m_camera.RotateY(-1);
				moveKeyPressed = true;
			}

			//Only update the view matrix when we have moved the camera
			if (moveKeyPressed)
			{
				m_constBuffers.UpdateView(m_deviceContext, m_camera.getViewMatrix());
			}
		}

		

		//Draw the scene
		Render();
		
		m_fpscounter.DisplayInWindow(window);
;	}
}
