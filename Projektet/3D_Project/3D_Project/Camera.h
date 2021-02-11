#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
private:
	XMMATRIX m_viewMatrix;			//change to float4x4
	XMMATRIX m_projectionMatrix;

	XMFLOAT3 m_eye;		//Posision
	XMFLOAT3 m_target;	//Focus point
	XMFLOAT3 m_up;		
	XMFLOAT3 m_rotation;//Pitch, yaw, roll

	float m_fieldOfView;
	float m_aspectRatio;
	float m_nearPlane;
	float m_farPlane;

	XMVECTOR m_posVector;
	XMVECTOR m_rotVector;		//pitch, yaw, roll

	//Camera settings
	float m_cameraSpeed;

	const XMVECTOR m_defaultForward = {0.0f, 0.0f, 1.0f, 0.0f};
	const XMVECTOR m_defaultUp = { 0.0f, 1.0f, 0.0f, 0.0f };

private:
	//Conversion between types
	XMVECTOR Float3ToVector(XMFLOAT3& val);
	XMFLOAT3 VectorToFloat3(XMVECTOR& vec);
	XMFLOAT4X4 MatrixToFloat4x4(XMMATRIX& matrix);
	XMMATRIX Float4x4ToMatrix(XMFLOAT4X4& floatMatrix);	//ta bort?

public:
	Camera();
	~Camera();

	void Initialize(UINT winWidth, UINT winHeight, float fov, float nearPlane, float farPlane);

	//Get functions
	XMFLOAT4X4 getViewMatrix();
	XMFLOAT4X4 getProjMatrix();

	void UpdateViewMatrix();
	void UpdateProjMatrix();

	//Movement
	void Move(XMFLOAT3 direction);
	//void UpdateCamera();
	void RotateY(float angle);

};