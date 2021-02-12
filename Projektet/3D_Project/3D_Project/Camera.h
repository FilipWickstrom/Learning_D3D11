#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
private:
	//Matrixes
	XMFLOAT4X4 m_viewMatrix;
	XMFLOAT4X4 m_projectionMatrix;

	//View settings
	XMFLOAT3 m_eye;		//Posision
	XMFLOAT3 m_target;	//Focus point
	XMFLOAT3 m_up;		
	XMFLOAT3 m_rotation;//Pitch, yaw, roll

	//Projection settings
	float m_fieldOfView;
	float m_aspectRatio;
	float m_nearPlane;
	float m_farPlane;

	//Vector to make calculations easier
	XMVECTOR m_posVector;
	XMVECTOR m_rotVector;		//pitch, yaw, roll

	//Default direction vectors
	const XMVECTOR m_defaultForward = {0.0f, 0.0f, 1.0f, 0.0f};
	const XMVECTOR m_defaultUp = { 0.0f, 1.0f, 0.0f, 0.0f };

	//Camera settings
	float m_cameraSpeed;

private:
	//Conversion between types
	XMVECTOR Float3ToVector(XMFLOAT3& val);
	XMFLOAT3 VectorToFloat3(XMVECTOR& vec);
	XMFLOAT4X4 MatrixToFloat4x4(XMMATRIX& matrix);

public:
	Camera();
	~Camera();

	void Initialize(UINT winWidth, UINT winHeight, float fov, float nearPlane, float farPlane);

	//Get functions
	const XMFLOAT4X4& GetViewMatrix() const;
	const XMFLOAT4X4& GetProjMatrix() const;

	void UpdateViewMatrix();
	void UpdateProjMatrix();

	//Movement
	void Move(XMFLOAT3 direction);
	//void UpdateCamera();
	void Rotate(XMFLOAT3 rotation);

	//Camera speed
	float GetCamSpeed() const;

};