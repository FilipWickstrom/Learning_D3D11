#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include <Keyboard.h>
#include <Mouse.h>
#include "Camera.h"
#include "Tessellation.h"
#include "Scene.h"
#include "ConstantBuffers.h"
#include "Rasterizer.h"
#include "BackFaceCulling.h"
#include "PostProcessing.h"

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
					   BackFaceCulling& culling,			//Use the geometry shader or not
					   PostProcessing& postProcess);					   

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
					BackFaceCulling& culling,
					PostProcessing& postProcess);
};