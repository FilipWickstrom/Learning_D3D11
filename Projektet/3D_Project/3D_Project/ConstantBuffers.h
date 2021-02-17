#pragma once
#include <d3d11.h>
#include <DirectXMath.h> 
#include <iostream>
#include "Camera.h"
using namespace DirectX;

const int NROFLIGHTS = 2;

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

	struct Light
	{
		XMFLOAT4 position;
		XMFLOAT4 colour;
		float range;
		float padding[3];
	};

	struct LightStruct
	{
		Light pointlights[NROFLIGHTS];
	};
	LightStruct m_lights;
	ID3D11Buffer* m_lightsBuffer;

	struct CamStruct
	{
		XMFLOAT3 camPos;
		float padding;
	};
	CamStruct m_camStruct;
	ID3D11Buffer* m_camBuffer;

private:
	void UpdateWVP(ID3D11DeviceContext* deviceContext);
	//Creates all of the buffers
	bool CreateBuffers(ID3D11Device* device);

public:
	ConstantBuffers();
	~ConstantBuffers();

	bool Initialize(ID3D11Device* device, const Camera& camera);

	//Set to the vertex shader
	void SetWVPToVS(ID3D11DeviceContext* deviceContext);
	
	//Update the struct
	void UpdateWorld(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 world);
	void UpdateView(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 view);
	void UpdateProjection(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 proj);
	
	//Light
	void SetLightsToPS(ID3D11DeviceContext* deviceContext);
	void UpdateLights(ID3D11DeviceContext* deviceContext, Camera& camera);

	//Camera
	void SetCamToPs(ID3D11DeviceContext* deviceContext);
	void UpdateCam(ID3D11DeviceContext* deviceContext, Camera& camera);

};

