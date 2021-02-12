#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include "Camera.h"
using namespace DirectX;

class Movement
{
private:
	XMFLOAT3 m_dir;
	XMFLOAT3 m_rot;

public:
	Movement();
	~Movement();

	bool KeyboardInput(float deltatime, MSG msg, Camera &camera);


};