#include "InputKeyboardMouse.h"

InputKeyboardMouse::InputKeyboardMouse()
{
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
}

InputKeyboardMouse::~InputKeyboardMouse()
{
}

void InputKeyboardMouse::Initialize(HWND& window, float moveSpeed, float mouseSens)
{
	m_moveSpeed = moveSpeed;
	m_mouseSens = mouseSens;
	m_mouse->SetWindow(window);
}

void InputKeyboardMouse::KeyboardInput(float dt, Camera& camera, Tessellation& tessellator, 
									   Scene& theScene, ConstantBuffers& constBuffs, 
									   ID3D11DeviceContext* deviceContext)
{
	XMVECTOR movement = { 0.0f, 0.0f, 0.0f, 0.0f };
	XMVECTOR upOrDown = { 0.0f, 1.0f, 0.0f, 0.0f };

	//Getting the key
	auto kb = m_keyboard->GetState();

	//Move forward or backward
	if (kb.W)
	{
		if (kb.LeftShift)	//Sprint
			movement += (camera.GetForward() * 2);
		else
			movement += camera.GetForward();
	}
	if (kb.S)
	{
		if (kb.LeftShift)
			movement += (camera.GetBackward() * 2);
		else
			movement += camera.GetBackward();
	}
	//Move sideways
	if (kb.D)
	{
		if (kb.LeftShift)
			movement += (camera.GetRight() * 2);
		else
			movement += camera.GetRight();
	}
	if (kb.A)
	{
		if (kb.LeftShift)
			movement += (camera.GetLeft() * 2);
		else
			movement += camera.GetLeft();
	}
	//Move up or down
	if (kb.Space)
	{
		movement += upOrDown;
	}
	if (kb.LeftControl)
	{
		movement += (-upOrDown);
	}

	//Reset the camera
	if (kb.R)
	{
		camera.Reset();
	}

	camera.Move(movement * m_moveSpeed * dt);

	/*-------- Tessellation settings --------*/
	//Wireframe
	if (kb.V)
		tessellator.SetWireframe(true);
	else if (kb.B)
		tessellator.SetWireframe(false);
	
	//Level of detail of tessellation (1, 4, 8, 16, 32)
	if (kb.D1)
		tessellator.UpdateLOD(deviceContext, 1.0f);
	else if (kb.D2)
		tessellator.UpdateLOD(deviceContext, 4.0f);
	else if (kb.D3)
		tessellator.UpdateLOD(deviceContext, 8.0f);
	else if (kb.D4)
		tessellator.UpdateLOD(deviceContext, 16.0f);
	else if (kb.D5)
		tessellator.UpdateLOD(deviceContext, 32.0f);

	//How much of the displacementmap to use
	if (kb.D6)
		tessellator.UpdateDepth(deviceContext, 0.2f);
	else if (kb.D7)
		tessellator.UpdateDepth(deviceContext, 0.4f);
	else if (kb.D8)
		tessellator.UpdateDepth(deviceContext, 0.6f);
	else if (kb.D9)
		tessellator.UpdateDepth(deviceContext, 0.8f);
	else if (kb.D0)
		tessellator.UpdateDepth(deviceContext, 1.0f);

	/*--------Toggle on or off normalmapping--------*/
	if (kb.N)
		theScene.UseNormalMaps(true);
	else if (kb.M)
		theScene.UseNormalMaps(false);

	/* ------ Move main light ------*/
	if (kb.Q)
		constBuffs.SetFollowCamera(true);
	else if (kb.E)
		constBuffs.SetFollowCamera(false);
	
	/* --- Render mode: texture, lights on or normal mode --- */
	if (kb.NumPad0)
		constBuffs.SetRenderMode(0);
	else if (kb.NumPad1)
		constBuffs.SetRenderMode(1);
	else if (kb.NumPad2)
		constBuffs.SetRenderMode(2);
	else if (kb.NumPad3)
		constBuffs.SetRenderMode(3);

}

void InputKeyboardMouse::MouseInput(float dt, Camera& camera)
{
	auto mouse = m_mouse->GetState();

	//Set to the window mode when rightclick is held down
	if (mouse.rightButton)
		m_mouse->SetMode(Mouse::MODE_RELATIVE);
	else
		m_mouse->SetMode(Mouse::MODE_ABSOLUTE);
	
	if (mouse.positionMode == Mouse::MODE_RELATIVE)
	{
		//Rotate only around pitch and yaw. Inverted from x and y position
		camera.Rotate((float)mouse.y * m_mouseSens, (float)mouse.x * m_mouseSens, 0.0f); 
	}
}


void InputKeyboardMouse::CheckInput(float dt, Camera& camera, Tessellation& tessellator, 
									Scene& theScene, ConstantBuffers& constBuffs, 
									ID3D11DeviceContext* deviceContext)
{
	MouseInput(dt, camera);
	KeyboardInput(dt, camera, tessellator, theScene, constBuffs, deviceContext);
	camera.UpdateViewMatrix();
}