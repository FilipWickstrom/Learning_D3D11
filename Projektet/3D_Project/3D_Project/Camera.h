#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

//HAVE TO FIX THIS ONE... NOT DONE AND NOT GOOD...

class Camera
{
private:
	XMFLOAT4X4 m_viewMatrix;
	XMFLOAT4X4 m_projectionMatrix;

	XMFLOAT3 m_eyePos;	//Position of camera
	XMFLOAT3 m_lookAtPos;
	XMFLOAT3 m_upDir;

	float m_fieldOfView;
	float m_aspectRatio;
	float m_nearPlane;
	float m_farPlane;

	float m_cameraYawAngle;
	float m_cameraPitchAngle;

	//World(probably not)
private:
	//Conversion between types
	XMVECTOR Float3ToVector(XMFLOAT3& val);
	XMFLOAT3 VectorToFloat3(XMVECTOR& vec);
	XMFLOAT4X4 MatrixToFloat4x4(XMMATRIX& matrix);
	XMMATRIX Float4x4ToMatrix(XMFLOAT4X4& floatMatrix);

public:
	Camera(UINT winWidth, UINT winHeight);
	~Camera();

	//Get functions
	const XMFLOAT4X4 getViewMatrix();
	const XMFLOAT4X4 getProjMatrix();

	void UpdateViewMatrix();
	void UpdateProjMatrix();

	
	//LookDirection(float3())
	void Move(XMFLOAT3 direction);
	void Rotate(XMFLOAT3 axis, float degrees);	//Have to fix


};