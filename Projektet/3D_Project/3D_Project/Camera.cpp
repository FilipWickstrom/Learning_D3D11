#include "Camera.h"

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

Camera::Camera(UINT winWidth, UINT winHeight)
{
	//View matrix
	m_eyePos = { 0.0f, 0.0f, 0.0f };
	m_lookAtPos = { 0.0f, 0.0f, 1.0f };
	m_upDir = { 0.0f, 1.0f, 0.0f };
	UpdateViewMatrix();

	//Projection matrix
	m_fieldOfView = XM_PI * 0.5f;			//90 degrees
	m_aspectRatio = float(winWidth / winHeight);
	m_nearPlane = 0.1f;
	m_farPlane = 100.0f;
	UpdateProjMatrix();

	m_cameraYawAngle = XM_PI * 0.25f;
	m_cameraPitchAngle = XM_PI;
}

Camera::~Camera()
{
}

const XMFLOAT4X4 Camera::getViewMatrix()
{
	XMMATRIX transposed = XMMatrixTranspose(Float4x4ToMatrix(m_viewMatrix));
	return MatrixToFloat4x4(transposed);
}

const XMFLOAT4X4 Camera::getProjMatrix()
{
	XMMATRIX transposed = XMMatrixTranspose(Float4x4ToMatrix(m_projectionMatrix));
	return MatrixToFloat4x4(transposed);
}

void Camera::UpdateViewMatrix()
{
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(XMLoadFloat3(&m_eyePos), XMLoadFloat3(&m_lookAtPos), XMLoadFloat3(&m_upDir)));
}

void Camera::UpdateProjMatrix()
{
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveLH(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane));
}

void Camera::Move(XMFLOAT3 direction)
{
	//Makes a matrix of the new direction
	XMMATRIX directionMatrix = XMMatrixTranslation(direction.x, direction.y, direction.z);

	//Fix the position of the eyes
	XMVECTOR newPos = XMVector3Transform(Float3ToVector(m_eyePos), directionMatrix);
	m_eyePos = VectorToFloat3(newPos);
	
	//Fix the position looking at
	XMVECTOR newLookPos = XMVector3Transform(Float3ToVector(m_lookAtPos), directionMatrix);
	m_lookAtPos = VectorToFloat3(newLookPos);

	//Fix up
	XMVECTOR newUp = XMVector3Transform(Float3ToVector(m_upDir), directionMatrix);
	m_upDir = VectorToFloat3(newUp);

	UpdateViewMatrix();
}

void Camera::Rotate(XMFLOAT3 axis, float degrees)
{
	//if (the same axis or degreess is 0.0f) - Dont rotate

}
