#include "AltCamera.h"

AltCamera::AltCamera()
{
	m_eyePos =	  { 0.0f, 0.0f, 0.0f };
	m_targetPos = { 0.0f, 0.0f, 1.0f };
	m_up =		  { 0.0f, 1.0f, 0.0f };
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixIdentity());
}

AltCamera::~AltCamera()
{
}

void AltCamera::Initialize(XMFLOAT3 pos, XMFLOAT3 focus, float fov, float aspectRatio, float nearPlane, float farPlane)
{
	//Setting up the view matrix
	m_eyePos = pos;
	m_targetPos = focus;
	m_up = { 0.0f, 1.0f, 0.0f };
	XMVECTOR eyePos = XMLoadFloat3(&pos);
	XMVECTOR focusPos = XMLoadFloat3(&focus);
	XMVECTOR upPos = XMLoadFloat3(&m_up);
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(eyePos, focusPos, upPos));

	//Projection
	float fieldOfView = fov * (XM_PI/180.0f);
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearPlane, farPlane));
}

const XMFLOAT4X4& AltCamera::GetViewMatrix() const
{
	return m_viewMatrix;
}

const XMFLOAT4X4& AltCamera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

void AltCamera::UpdateView(Camera& mainCamera)
{
	//Idea:
		//get the position of the main camera.
		//get the position of the alt camera.
		//also targetPos???
		//calc a new, view matrix with this. Add the positions? 
		//update the view matrix with this

		//altCamera.updateView(camera);

	XMFLOAT3 mainCamPos = mainCamera.GetPosition();
	XMFLOAT3 newPos = XMFLOAT3(mainCamPos.x + m_eyePos.x,
							   mainCamPos.y + m_eyePos.y,
							   mainCamPos.z + m_eyePos.z);

	XMVECTOR eyePos = XMLoadFloat3(&newPos);
	XMVECTOR focusPos = XMLoadFloat3(&m_targetPos);
	XMVECTOR upPos = XMLoadFloat3(&m_up);
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(eyePos, focusPos, upPos));
}
