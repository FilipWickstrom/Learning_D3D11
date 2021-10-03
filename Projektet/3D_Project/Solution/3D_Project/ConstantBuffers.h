#pragma once
#include <d3d11.h>
#include <DirectXMath.h> 
#include <iostream>
#include "Camera.h"
using namespace DirectX;

const int NROFLIGHTS = 3;

class ConstantBuffers
{
private:
	//	World view projection
	struct WVPMatrix
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};
	WVPMatrix m_WVPMatrix;
	ID3D11Buffer* m_WVPBuffer;

	//	Lights
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

	//	Camera position and lights on/off
	struct CamStruct
	{
		XMFLOAT3 camPos;
		UINT renderMode;
	};
	CamStruct m_camStruct;
	ID3D11Buffer* m_camBuffer;

	//	Material for current object
	struct Material
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
	};
	Material m_material;
	ID3D11Buffer* m_materialBuffer;

	/* Some changeable settings */
	// Let the main light follow the camera
	bool m_followCamera;

private:
	void UpdateWVP(ID3D11DeviceContext* deviceContext);
	//Creates all of the buffers
	bool CreateBuffers(ID3D11Device* device);

public:
	ConstantBuffers();
	~ConstantBuffers();

	bool Initialize(ID3D11Device* device, const Camera& camera);

	//Set the wvp to vertex shader and domain shader
	void SetWVP(ID3D11DeviceContext* deviceContext);
	
	//Update the struct
	void UpdateWorld(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 world);
	void UpdateView(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 view);
	void UpdateProjection(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 proj);
	
	//Light
	void SetLightsToPS(ID3D11DeviceContext* deviceContext);
	void UpdateLights(ID3D11DeviceContext* deviceContext, Camera& camera);

	//Camera
	void SetCamToPS(ID3D11DeviceContext* deviceContext);
	void SetCamToGS(ID3D11DeviceContext* deviceContext);
	void UpdateCam(ID3D11DeviceContext* deviceContext);
	void SetCamPos(XMFLOAT3 position);

	//Material
	void UpdateMaterial(ID3D11DeviceContext* deviceContext, XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular);
	void SetMaterialPS(ID3D11DeviceContext* deviceContext);

	//Light follow camera
	void SetFollowCamera(bool trueOrFalse);
	void SetRenderMode(UINT mode);

};

