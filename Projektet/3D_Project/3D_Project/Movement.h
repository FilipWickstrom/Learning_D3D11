#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include "Camera.h"

using namespace DirectX;

class Movement
{
private:
	

public:
	Movement();
	~Movement();

	bool KeyboardInput(float dt, MSG msg, Camera &camera);


};