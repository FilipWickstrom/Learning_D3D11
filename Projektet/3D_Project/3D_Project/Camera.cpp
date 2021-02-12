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
	m_farPlane = 1000.0f;
	UpdateProjMatrix();

	m_cameraSpeed = 15.0f;

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

const XMFLOAT4X4& Camera::GetViewMatrix() const
{
	return m_viewMatrix;
}

const XMFLOAT4X4& Camera::GetProjMatrix() const
{
	return m_projectionMatrix;
}

void Camera::UpdateViewMatrix()
{
	//How the camera is rotated
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

	//Calculate the forward vector. Which direction is it pointing at
	XMVECTOR targetVector = XMVector3TransformCoord(m_defaultForward, camRotationMatrix);	//XMVector3Normalize

	targetVector += m_posVector;

	XMVECTOR upDirection = XMVector3TransformCoord(m_defaultUp, camRotationMatrix);

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(m_posVector, targetVector, upDirection));
}

void Camera::UpdateProjMatrix()
{
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveFovLH(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane));
}

void Camera::Move(XMFLOAT3 direction)
{
	m_eye.x += direction.x;
	m_eye.y += direction.y;
	m_eye.z += direction.z;
	m_posVector = XMLoadFloat3(&m_eye);
	UpdateViewMatrix();
}

void Camera::Rotate(XMFLOAT3 rotation)
{
	//LATER FIX. Should not be able to rotate 360 up or down
	m_rotation.x += rotation.x;
	m_rotation.y += rotation.y;
	m_rotation.z += rotation.z;

	UpdateViewMatrix();
}

float Camera::GetCamSpeed() const
{
	return m_cameraSpeed;
}

