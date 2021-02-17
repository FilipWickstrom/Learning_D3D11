#include "Renderer.h"

Renderer::Renderer(UINT winWidth, UINT winHeight)
	:m_winWidth(winWidth), m_winHeight(winHeight)
{
	//Directx
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_swapChain = nullptr;
	
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

	//Setting up the scene with all the objects in it
	if (!m_scene.Load(m_device))
	{
		std::cerr << "Failed to load the scene..." << std::endl;
		return false;
	}

	//Setting up camera and the movement
	m_camera.Initialize(m_winWidth, m_winHeight, XM_PI * 0.5f, 0.1f, 1000.0f);
	m_movement.Initialize(window, 0.005f, 0.004f);

	//Setting up WVP, light buffer and camera
	if (!m_constBuffers.Initialize(m_device, m_camera))
	{
		std::cerr << "Failed to initialize the constbuffers..." << std::endl;
		return false;
	}

	m_fpscounter.StartClock();

	return true;
}

void Renderer::Render()
{
	//First pass - Only for geometry - outputs data to the g-buffers. 
	//Binds the right targets and clears them before new information
	m_firstPass.Bind(m_deviceContext);

	//WVP
	m_constBuffers.SetWVPToVS(m_deviceContext);

	//Render all the objects in the scene
	m_scene.Render(m_deviceContext, m_constBuffers);

	//Lights
	m_constBuffers.UpdateLights(m_deviceContext, m_camera);
	m_constBuffers.UpdateCam(m_deviceContext, m_camera);
	m_constBuffers.SetLightsToPS(m_deviceContext);
	m_constBuffers.SetCamToPs(m_deviceContext);
	
	//Second pass - Only for lightning - output to the final render target
	//Uses the information saves in g-buffer and compute the lightning
	m_secondPass.Bind(m_deviceContext, m_firstPass.GetShaderResourceView(0), m_firstPass.GetShaderResourceView(1), m_firstPass.GetShaderResourceView(2));

	//Present the final result
	m_swapChain->Present(1, 0);
}

void Renderer::StartGameLoop(HWND& window)
{
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		m_deltatime = m_fpscounter.GetDeltatime();
		m_fpscounter.RestartClock();

		//Looks at the message and removes it from message queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
			
		//Check mouse and keyboard
		m_movement.CheckInput(m_deltatime, m_camera);

		//Update the view matrix
		m_constBuffers.UpdateView(m_deviceContext, m_camera.GetViewMatrix());

		//Draw the scene
		Render();
		
		m_fpscounter.DisplayInWindow(window);
;	}
}
