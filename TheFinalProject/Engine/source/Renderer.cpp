#include "PCH.h"
#include "Renderer.h"

Renderer::Renderer(UINT winWidth, UINT winHeight)
	:m_winWidth(winWidth), m_winHeight(winHeight)
{
	//Directx
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_swapChain = nullptr;
	m_deltatime = 0.0f;
	m_VSync = false;
	m_isRunning = true;
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

	if (!m_rasterizer.Initialize(m_device))
	{
		std::cerr << "Initialize() for rasterizer failed..." << std::endl;
		return false;
	}

	//Reads the ico-files for the shaders and saves to buffers
	if (!m_geometryPass.Initialize(m_device, m_winWidth, m_winHeight))
	{
		std::cerr << "Failed to initialize the first pass..." << std::endl;
		return false;
	}
	if (!m_lightingPass.Initialize(m_device, m_winWidth, m_winHeight, m_swapChain))
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
	m_camera.Initialize(m_winWidth, m_winHeight, XM_PI * 0.45f, 0.1f, 1000.0f);
	m_inputKeyboardMouse.Initialize(window, 0.005f, 0.004f);

	//Setting up WVP, light buffer and camera
	if (!m_constBuffers.Initialize(m_device, m_camera))
	{
		std::cerr << "Failed to initialize the constbuffers..." << std::endl;
		return false;
	}

	//Setting up the shadowmap
	if (!m_shadowMap.Initialize(m_device))
	{
		std::cerr << "Shadowmap: Initialize() failed..." << std::endl;
		return false;
	}
	
	//Back face culling
	if (!m_backFaceCulling.Initialize(m_device))
	{
		std::cerr << "BackFaceCulling: Initialize() failed..." << std::endl;
		return false;
	}

	//Filters with compute shader - Max radius is 15 in this implementation
	if (!m_postProcess.Initialize(m_device, m_swapChain, 5, 2.5, 5, 0.09f))
	{
		std::cerr << "Post process failed to initialize..." << std::endl;
		return false;
	}

	m_fpscounter.StartClock();

	return true;
}

void Renderer::Render()
{
	/* ------ First pass - only geometry - saves to gbuffers ------ */

	//Binds the right targets and clears them before new information
	m_geometryPass.Bind(m_deviceContext);

	//Rasterizer binding current state for it
	m_rasterizer.Bind(m_deviceContext);

	//Bind the geometry shader with back face culling
	m_backFaceCulling.Bind(m_deviceContext);

	//WVP - for vertex shader and domain shader
	m_constBuffers.SetWVP(m_deviceContext);

	//Material
	m_constBuffers.SetMaterialPS(m_deviceContext);

	//Render all the objects in the scene
	m_scene.Render(m_deviceContext, m_constBuffers, &m_geometryPass.GetTessellation(), m_deltatime);

	//Turning off things before the screenquad
	m_geometryPass.TurnOffTessellation(m_deviceContext);
	m_backFaceCulling.UnBind(m_deviceContext);
	m_rasterizer.UnBind(m_deviceContext);

	//Setups all the settings for the shadowmap
	m_shadowMap.BindShadowVS(m_deviceContext);
	
	//Render the scene but only where the objects are
	m_scene.RenderShadows(m_deviceContext, m_shadowMap);

	/*------------ First pass done ------------------*/


	/* ------ Second pass - lighting only - reads from gbuffers ----- */

	//Preparation for lights
	m_constBuffers.UpdateLights(m_deviceContext, m_camera);
	m_constBuffers.SetCamToPS(m_deviceContext);
	m_constBuffers.SetLightsToPS(m_deviceContext);
	
	//Second pass - Only for lightning - output to the final render target
	//Uses the information saves in g-buffer and compute the lightning
	m_lightingPass.Bind(m_deviceContext, m_geometryPass.GetShaderResourceViews());

	//Set the light and the depth buffer as a texture
	m_shadowMap.BindToPS(m_deviceContext);

	//Finally render to the screenquad
	m_lightingPass.Render(m_deviceContext);

	//SRV sets to nullptr
	m_shadowMap.DisableSRV(m_deviceContext);

	/*------------ Second pass done ------------------*/

	//Final postprocessing with compute shader
	m_postProcess.Render(m_deviceContext, m_winWidth, m_winHeight);

	//Present the final result
	Present();
}

void Renderer::Present()
{
	if (m_VSync)
		m_swapChain->Present(1, 0);
	else
		m_swapChain->Present(0, 0);
}

void Renderer::StartGameLoop(HWND& window)
{
	MSG msg = {};

	//Keep the window running as long as possible
	while (m_isRunning)
	{
		//Check if we are going to quit
		if (m_inputKeyboardMouse.CheckExit())
			m_isRunning = false;

		m_deltatime = m_fpscounter.GetDeltatime();
		m_fpscounter.RestartClock();

		//Looks at the message and removes it from message queue
		//While = takes all the message and checks them
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				m_isRunning = false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
			
		//Check mouse and keyboard
		m_inputKeyboardMouse.CheckInput(m_deltatime, 
										m_camera, 
										m_rasterizer, 
										m_geometryPass.GetTessellation(), 
										m_scene, 
										m_constBuffers, 
										m_deviceContext, 
										m_backFaceCulling, 
										m_postProcess);

		//Update the view matrix from camera
		m_constBuffers.UpdateView(m_deviceContext, m_camera.GetViewMatrix());
		m_constBuffers.UpdateCam(m_deviceContext);

		//Draw the scene
		Render();
		
		m_fpscounter.DisplayInWindow(window);
;	}
}
