#pragma once

using namespace DirectX;

class ShadowMap
{
private:
	//Resolution of the shadowmap
	UINT m_width;
	UINT m_height;

	//View information
	XMFLOAT3 m_pos;
	XMFLOAT3 m_focus;
	XMFLOAT3 m_up;
	XMFLOAT4X4 m_viewMatrix;

	//Projection
	float m_fov;
	float m_nearPlane;
	float m_farPlane;
	XMFLOAT4X4 m_projectionMatrix;

	struct Spotlight
	{
		XMFLOAT3 position;
		float range;
		XMFLOAT3 direction;
		float outerAngle;
		XMFLOAT3 colour;
		float innerAngle;
	};
	Spotlight m_spotLight;
	ID3D11Buffer* m_SpotLightBuffer;

	//Depthbuffer
	ID3D11DepthStencilView* m_depthView;
	ID3D11ShaderResourceView* m_depthSRV;	//Communicates with pixelshader
	D3D11_VIEWPORT m_viewport;
	ID3D11VertexShader* m_shadowShader;

	//Constant buffer
	struct ShadowWVP
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	} m_shadowWVP;
	ID3D11Buffer* m_shadowWVPBuffer;

//Help functions
private:
	bool CreateSpotLightBuffer(ID3D11Device* device);
	bool CreateDepthBuffer(ID3D11Device* device);	//Uses 32 bit depth
	bool LoadVertexShader(ID3D11Device* device);
	bool CreateShadowWVPBuffer(ID3D11Device* device);

public:
	ShadowMap();
	~ShadowMap();

	bool Initialize(ID3D11Device* device);

	void BindShadowVS(ID3D11DeviceContext* deviceContext);
	void UpdateShadowWVP(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 newWorld);

	void BindToPS(ID3D11DeviceContext* deviceContext);

	void DisableSRV(ID3D11DeviceContext* deviceContext);
};