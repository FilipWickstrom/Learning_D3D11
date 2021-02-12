#include "Movement.h"

Movement::Movement()
{
}

Movement::~Movement()
{
}

bool Movement::KeyboardInput(float dt, MSG msg, Camera& camera)
{
	bool moved = false;
	const float camSpeed = camera.GetCamSpeed();

	//Forwards or backwards
	if (msg.wParam == 'W')
	{
		camera.Move(camera.GetForward() * camSpeed * dt);
		moved = true;
	}
	else if (msg.wParam == 'S')
	{
		camera.Move(camera.GetBackward() * camSpeed * dt);
		moved = true;
	}

	//Sideways
	else if (msg.wParam == 'D')
	{
		camera.Move(camera.GetRight() * camSpeed * dt);
		moved = true;
	}
	else if (msg.wParam == 'A')
	{
		camera.Move(camera.GetLeft() * camSpeed * dt);
		moved = true;
	}

	//Move up or down
	else if (msg.wParam == 'Q')
	{
		XMVECTOR upVec = { 0.0f, camSpeed, 0.0f };
		camera.Move(upVec);
		moved = true;
	}
	else if (msg.wParam == 'E')
	{
		XMVECTOR downVec = { 0.0f, -camSpeed, 0.0f };
		camera.Move(downVec);
		moved = true;
	}

	////DEBUGGING - ROTATING CAMERA
	else if (msg.wParam == 'L')
	{
		camera.Rotate(0.0f, camSpeed * dt, 0.0f);
		moved = true;
	}
	else if (msg.wParam == 'J')
	{
		camera.Rotate(0.0f, -camSpeed * dt, 0.0f);
		moved = true;
	}
	else if (msg.wParam == 'I')
	{
		camera.Rotate(-camSpeed * dt, 0.0f, 0.0f);
		moved = true;
	}
	else if (msg.wParam == 'K')
	{
		camera.Rotate(camSpeed*dt, 0.0f, 0.0f);
		moved = true;
	}

	else if (msg.wParam == 'R')
	{
		camera.Reset();
		moved = true;
	}

	return moved;
}
