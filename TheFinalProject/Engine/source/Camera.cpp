#include "PCH.h"
#include "Camera.h"

Camera::Camera()
{
	//Setup the default position and rotation of the camera
	Reset();

	//Projection matrix - default values
	m_fieldOfView = XM_PI * 0.5f; //90 degrees
	m_aspectRatio = 16.0f / 9.0f;
	m_nearPlane = 0.1f;
	m_farPlane = 1000.0f;
	UpdateProjMatrix();
}

Camera::~Camera()
{
}

void Camera::Reset()
{
	//View matrix - default values
	m_eyePos = { 0.0f, 0.0f, 0.0f };
	m_target = { 0.0f, 0.0f, 1.0f };
	m_up = { 0.0f, 1.0f, 0.0f };
	m_rotation = { 0.0f, 0.0f, 0.0f };

	m_posVector = XMLoadFloat3(&m_eyePos);

	XMVECTOR zerovec = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_forwardDir = zerovec;
	m_backwardDir = zerovec;
	m_rightDir = zerovec;
	m_leftDir = zerovec;

	UpdateViewMatrix();
}

void Camera::Initialize(UINT winWidth, UINT winHeight, float fov, float nearPlane, float farPlane)
{
	//Projection matrix
	m_fieldOfView = fov;
	m_aspectRatio = (float)winWidth / (float)winHeight;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	UpdateProjMatrix();
}

void Camera::UpdateViewMatrix()
{
	//How the camera is rotated
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

	//Calculate the forward vector. Which direction is it pointing at
	XMVECTOR targetVector = XMVector3TransformCoord(m_DEFAULT_FORWARD, camRotationMatrix);

	//Adds the offset of the cameras position
	targetVector += m_posVector;

	//Calculates the up vector
	XMVECTOR upDirection = XMVector3TransformCoord(m_DEFAULT_UP, camRotationMatrix);

	//Finally update the viewmatrix
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(m_posVector, targetVector, upDirection));

	//Rotation only dependent on yaw. So that right is always right side
	XMMATRIX dirRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, m_rotation.y, 0.0f);
	m_forwardDir = XMVector3TransformCoord(m_DEFAULT_FORWARD, dirRotationMatrix);
	m_backwardDir = XMVector3TransformCoord(m_DEFAULT_BACKWARD, dirRotationMatrix);
	m_rightDir = XMVector3TransformCoord(m_DEFAULT_RIGHT, dirRotationMatrix);
	m_leftDir = XMVector3TransformCoord(m_DEFAULT_LEFT, dirRotationMatrix);
}

void Camera::UpdateProjMatrix()
{
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveFovLH(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane));
}

void Camera::Move(const XMVECTOR& direction)
{
	m_posVector += direction;
	XMStoreFloat3(&m_eyePos, m_posVector);
}

void Camera::Rotate(float pitch, float yaw, float roll)
{
	float maxPitch = XM_PI * 0.5;	//90 degrees

	//Should not be able to rotate around up or down
	if (m_rotation.x + pitch < maxPitch && m_rotation.x + pitch > -maxPitch)
	{
		m_rotation.x += pitch;
	}
	m_rotation.y += yaw;
	m_rotation.z += roll;
}

//Get functions
const XMFLOAT4X4& Camera::GetViewMatrix() const
{
	return m_viewMatrix;
}

const XMFLOAT4X4& Camera::GetProjMatrix() const
{
	return m_projectionMatrix;
}

const XMFLOAT3& Camera::GetPosition() const
{
	return m_eyePos;
}

const XMVECTOR& Camera::GetForward()
{
	return m_forwardDir;
}

const XMVECTOR& Camera::GetBackward()
{
	return m_backwardDir;
}

const XMVECTOR& Camera::GetRight()
{
	return m_rightDir;
}

const XMVECTOR& Camera::GetLeft()
{
	return m_leftDir;
}

