#include "Movement.h"

Movement::Movement()
{
	m_dir = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

Movement::~Movement()
{
}

bool Movement::KeyboardInput(float deltatime, MSG msg, Camera& camera)
{
	bool moved = false;
	m_dir = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//Forwards or backwards
	if (msg.wParam == 'W')
	{
		m_dir.z = 1;
		moved = true;
	}
	else if (msg.wParam == 'S')
	{
		m_dir.z = -1;
		moved = true;
	}

	//Sideways
	else if (msg.wParam == 'D')
	{
		m_dir.x = 1;
		moved = true;
	}
	else if (msg.wParam == 'A')
	{
		m_dir.x = -1;
		moved = true;
	}

	//Move up or down
	else if (msg.wParam == 'Q')
	{
		m_dir.y = 1;
		moved = true;
	}
	else if (msg.wParam == 'E')
	{
		m_dir.y = -1;
		moved = true;
	}

	//DEBUGGING - ROTATING CAMERA
	else if (msg.wParam == 'L')
	{
		m_rot.y = 1;
		moved = true;
	}
	else if (msg.wParam == 'J')
	{
		m_rot.y = -1;
		moved = true;
	}
	else if (msg.wParam == 'I')
	{
		m_rot.x = 1;
		moved = true;
	}
	else if (msg.wParam == 'K')
	{
		m_rot.x = -1;
		moved = true;
	}



	if (moved == true)
	{
		float dtSpeed = deltatime * camera.GetCamSpeed();
		m_dir = XMFLOAT3(m_dir.x * dtSpeed, m_dir.y * dtSpeed, m_dir.z * dtSpeed);
		camera.Move(m_dir);

		//TEMP
		m_rot = XMFLOAT3(m_rot.x * dtSpeed, m_rot.y * dtSpeed, m_rot.z * dtSpeed);
		camera.Rotate(m_rot);

		//Instead of move nad rotate calling the update function
		//use update last here
	}

	return moved;
}
