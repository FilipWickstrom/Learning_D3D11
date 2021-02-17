#pragma once
#pragma comment(lib, "DirectXTK.lib")

#include <DirectXMath.h>
#include <Windows.h>
#include "Camera.h"
#include <directxTK/Keyboard.h>
#include <directxTK/Mouse.h>

using namespace DirectX;

class Movement
{
private:
	std::unique_ptr<Keyboard> m_keyboard;
	std::unique_ptr<Mouse> m_mouse;
	float m_moveSpeed;
	float m_mouseSens;

private:
	void KeyboardInput(float dt, Camera& camera);
	void MouseInput(float dt, Camera& camera);

public:
	Movement();
	~Movement();

	void Initialize(HWND& window, float moveSpeed, float mouseSens);
	void CheckInput(float dt, Camera& camera);
};