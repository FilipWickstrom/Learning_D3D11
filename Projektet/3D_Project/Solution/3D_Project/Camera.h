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
	XMFLOAT3 m_eyePos;		//Posision
	XMFLOAT3 m_target;		//Focus point
	XMFLOAT3 m_up;		
	XMFLOAT3 m_rotation;	//Pitch, yaw, roll

	//Projection settings
	float m_fieldOfView;
	float m_aspectRatio;
	float m_nearPlane;
	float m_farPlane;

	//Using vectors to make calculations easier
	XMVECTOR m_posVector;

	//Constant default directions
	const XMVECTOR m_DEFAULT_FORWARD	= { 0.0f, 0.0f, 1.0f, 0.0f };
	const XMVECTOR m_DEFAULT_BACKWARD	= { 0.0f, 0.0f,-1.0f, 0.0f };
	const XMVECTOR m_DEFAULT_RIGHT		= { 1.0f, 0.0f, 0.0f, 0.0f };
	const XMVECTOR m_DEFAULT_LEFT		= {-1.0f, 0.0f, 0.0f, 0.0f };
	const XMVECTOR m_DEFAULT_UP			= { 0.0f, 1.0f, 0.0f, 0.0f };

	//Current direction
	XMVECTOR m_forwardDir;
	XMVECTOR m_backwardDir;
	XMVECTOR m_rightDir;
	XMVECTOR m_leftDir;

public:
	Camera();
	~Camera();

	void Reset();
	void Initialize(UINT winWidth, UINT winHeight, float fov, float nearPlane, float farPlane);

	void UpdateViewMatrix();
	void UpdateProjMatrix();

	//Movement
	void Move(const XMVECTOR& direction);
	void Rotate(float pitch, float yaw, float roll);

	//Get functions
	const XMFLOAT4X4& GetViewMatrix() const;
	const XMFLOAT4X4& GetProjMatrix() const;

	const XMVECTOR& GetForward();
	const XMVECTOR& GetBackward();
	const XMVECTOR& GetRight();
	const XMVECTOR& GetLeft();
	
	const XMFLOAT3& GetPosition() const;

};