#include "Movement.h"

Movement::Movement()
{
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
}

Movement::~Movement()
{
}

void Movement::Initialize(HWND& window, float moveSpeed, float mouseSens)
{
	m_moveSpeed = moveSpeed;
	m_mouseSens = mouseSens;
	m_mouse->SetWindow(window);
}

void Movement::KeyboardInput(float dt, Camera& camera)
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
	if (kb.Q)
	{
		movement += upOrDown;
	}
	if (kb.E)
	{
		movement += (-upOrDown);
	}

	//Reset the camera
	if (kb.R)
	{
		camera.Reset();
	}

	camera.Move(movement * m_moveSpeed * dt);
}

void Movement::MouseInput(float dt, Camera& camera)
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


void Movement::CheckInput(float dt, Camera& camera)
{
	MouseInput(dt, camera);
	KeyboardInput(dt, camera);
	camera.UpdateViewMatrix();
}