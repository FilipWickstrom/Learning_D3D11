#pragma once
#include <d3d11.h>
#include <DirectXMath.h> 
#include <iostream>
#include "Camera.h"
using namespace DirectX;

class ConstantBuffers
{
private:
	struct WVPMatrix
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};
	WVPMatrix m_WVPMatrix;
	ID3D11Buffer* m_WVPBuffer;

	struct LightStruct
	{
		XMFLOAT3 lightPos;
		float padding;
		XMFLOAT4 lightColour;
		XMFLOAT3 camPos;
		float lightRange;
	};
	LightStruct m_lights;
	ID3D11Buffer* m_lightsBuffer;

private:
	bool CreateWVPBuffer(ID3D11Device* device);
	void UpdateWVP(ID3D11DeviceContext* deviceContext);

	bool CreateLightsBuffer(ID3D11Device* device);

public:
	ConstantBuffers();
	~ConstantBuffers();

	bool InitializeWVP(ID3D11Device* device, XMFLOAT4X4 view, XMFLOAT4X4 proj);
	
	//Set to the vertex shader
	void SetWVPToVS(ID3D11DeviceContext* deviceContext);
	
	//Update the struct
	void UpdateWorld(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 world);
	void UpdateView(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 view);
	void UpdateProjection(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 proj);
	
	//Light
	bool InitializeLights(ID3D11Device* device);

	void SetLightToPS(ID3D11DeviceContext* deviceContext);

	void UpdateLights(ID3D11DeviceContext* deviceContext, Camera& camera);

};

