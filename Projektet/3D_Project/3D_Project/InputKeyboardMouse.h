#pragma once
#pragma comment(lib, "DirectXTK.lib")

#include <DirectXMath.h>
#include <Windows.h>
#include <directxTK/Keyboard.h>
#include <directxTK/Mouse.h>
#include "Camera.h"
#include "Tessellation.h"
#include "Scene.h"
#include "ConstantBuffers.h"
#include "Rasterizer.h"

using namespace DirectX;

class InputKeyboardMouse
{
private:
	std::unique_ptr<Keyboard> m_keyboard;
	std::unique_ptr<Mouse> m_mouse;
	float m_moveSpeed;
	float m_mouseSens;

private:
	void KeyboardInput(float dt, 
					   Camera& camera, 
					   Rasterizer& rasterizer,
					   Tessellation& tessellator, 
					   Scene& theScene,						//Using normal maps or not
					   ConstantBuffers& constBuffs,			//For updating light position
					   ID3D11DeviceContext* deviceContext,	//For updating the constant buffer
					   MeshObject& cameraMesh);				//Showing where the main camera is when second is active

	void MouseInput(float dt, Camera& camera);

public:
	InputKeyboardMouse();
	~InputKeyboardMouse();

	void Initialize(HWND& window, float moveSpeed, float mouseSens);
	void CheckInput(float dt, 
					Camera& camera, 
					Rasterizer& rasterizer,
					Tessellation& tessellator, 
					Scene& theScene,
					ConstantBuffers& constBuffs,
					ID3D11DeviceContext* deviceContext,
					MeshObject& cameraMesh);
};