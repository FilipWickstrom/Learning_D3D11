#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Camera.h"			//'***

using namespace DirectX;

class AltCamera
{
private:
	XMFLOAT4X4 m_viewMatrix;
	XMFLOAT4X4 m_projectionMatrix;
	
	XMFLOAT3 m_eyePos;
	XMFLOAT3 m_targetPos;
	XMFLOAT3 m_up;

public:
	AltCamera();
	~AltCamera();

	void Initialize(XMFLOAT3 pos, XMFLOAT3 focus, float fov = 90.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 100.0f);

	const XMFLOAT4X4& GetViewMatrix() const;
	const XMFLOAT4X4& GetProjectionMatrix() const;

	void UpdateView(Camera& mainCamera);

	//Set to active?

};
