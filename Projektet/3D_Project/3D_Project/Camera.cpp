#include "Camera.h"

Camera::Camera()
{
	//View matrix - default values
	m_eye = { 0.0f, 0.0f, 0.0f };
	m_target = { 0.0f, 0.0f, 1.0f };
	m_up = { 0.0f, 1.0f, 0.0f };

	m_posVector = XMLoadFloat3(&m_eye);
	m_rotation = { 0.0f, 0.0f, 0.0f };
	m_rotVector = XMLoadFloat3(&m_rotation);
	UpdateViewMatrix();

	//Projection matrix - default values
	m_fieldOfView = XM_PI * 0.5f; //90 degrees
	m_aspectRatio = float(16 / 9);
	m_nearPlane = 0.1f;
	m_farPlane = 100.0f;
	UpdateProjMatrix();

	m_cameraSpeed = 1.0f;

}

Camera::~Camera()
{
}

void Camera::Initialize(UINT winWidth, UINT winHeight, float fov, float nearPlane, float farPlane)
{
	//Projection matrix
	m_fieldOfView = fov;			//90 degrees
	m_aspectRatio = float(winWidth / winHeight);
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	UpdateProjMatrix();
}

XMVECTOR Camera::Float3ToVector(XMFLOAT3& val)
{
	return XMLoadFloat3(&val);
}

XMFLOAT3 Camera::VectorToFloat3(XMVECTOR& vec)
{
	XMFLOAT3 newVal;
	XMStoreFloat3(&newVal, vec);
	return newVal;
}

XMFLOAT4X4 Camera::MatrixToFloat4x4(XMMATRIX& matrix)
{
	XMFLOAT4X4 newFloat4x4;
	XMStoreFloat4x4(&newFloat4x4, matrix);
	return newFloat4x4;
}

XMMATRIX Camera::Float4x4ToMatrix(XMFLOAT4X4& floatMatrix)
{
	return XMLoadFloat4x4(&floatMatrix);
}

XMFLOAT4X4 Camera::getViewMatrix()
{
	return MatrixToFloat4x4(m_viewMatrix);
}

XMFLOAT4X4 Camera::getProjMatrix()
{
	return MatrixToFloat4x4(m_projectionMatrix);
}

void Camera::UpdateViewMatrix()
{
	//
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

	XMVECTOR TargetVector = XMVector3TransformCoord(m_defaultForward, camRotationMatrix);
	TargetVector = XMVector3Normalize(TargetVector);

	TargetVector += m_posVector;

	XMVECTOR upDirection = XMVector3TransformCoord(m_defaultUp, camRotationMatrix);

	m_viewMatrix = XMMatrixLookAtLH(m_posVector, TargetVector, upDirection);
}

void Camera::UpdateProjMatrix()
{
	m_projectionMatrix = XMMatrixPerspectiveFovLH(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::Move(XMFLOAT3 direction)
{
	m_eye.x += direction.x;
	m_eye.y += direction.y;
	m_eye.z += direction.z;
	m_posVector = XMLoadFloat3(&m_eye);
	UpdateViewMatrix();
}

void Camera::RotateY(float angle)
{
	/*XMMATRIX rotationMatrix = XMMatrixRotationY(angle);
	m_up = XMVector3TransformNormal(m_up, rotationMatrix);
	m_target = XMVector3TransformNormal(m_target, rotationMatrix);

	UpdateViewMatrix();*/
}

