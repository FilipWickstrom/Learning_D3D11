#include "Camera.h"
#include <iostream>	//*****

Camera::Camera()
{
	//Setup the default position and rotation of the camera
	Reset();

	//Projection matrix - default values
	m_fieldOfView = XM_PI * 0.5f; //90 degrees
	m_aspectRatio = float(16 / 9);
	m_nearPlane = 0.1f;
	m_farPlane = 1000.0f;
	UpdateProjMatrix();

	m_cameraSpeed = 0.01f;
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
	m_fieldOfView = fov;			//90 degrees
	m_aspectRatio = float(winWidth / winHeight);
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

	targetVector += m_posVector;

	XMVECTOR upDirection = XMVector3TransformCoord(m_DEFAULT_UP, camRotationMatrix);

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(m_posVector, targetVector, upDirection));

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
	UpdateViewMatrix();
}

void Camera::Rotate(float pitch, float yaw, float roll)
{
	float maxPitch = XM_PI * 0.5;	//90 degrees

	//FIX THIS A BIT MORE NICER
	if (m_rotation.x + pitch > maxPitch)
	{
		std::cout << "Lower limit" << std::endl;
		std::cout << "Current: " << m_rotation.x << std::endl;
	}
	else if (m_rotation.x + pitch < -maxPitch)
	{
		std::cout << "Upper limit" << std::endl;
		std::cout << "Current: " << m_rotation.x << std::endl;
	}
	else
	{
		m_rotation.x += pitch;
	}
	m_rotation.y += yaw;
	m_rotation.z += roll;
			
	UpdateViewMatrix();
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

const float Camera::GetCamSpeed() const
{
	return m_cameraSpeed;
}

