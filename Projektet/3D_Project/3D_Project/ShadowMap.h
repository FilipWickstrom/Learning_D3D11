#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>

using namespace DirectX;

class ShadowMap
{
private:
	//Shadow resolution for now? Separate for the light?
	UINT m_width;
	UINT m_height;

	//View information
	XMFLOAT3 m_pos;
	XMFLOAT3 m_focus;
	XMFLOAT3 m_up;
	XMFLOAT4X4 m_viewMatrix;

	//Projection
	float m_fov;
	float m_nearPlane;		//near range of light
	float m_farPlane;		//far range of light
	XMFLOAT4X4 m_projectionMatrix;

	struct Spotlight
	{
		XMFLOAT3 position;		//from m_pos
		float range;			//from the far plane?
		XMFLOAT3 direction;		//from m_focus
		float exponent;
		XMFLOAT3 colour;
		float resolution;
	};
	Spotlight m_spotLight;
	ID3D11Buffer* m_SpotLightBuffer;

	//Depthbuffer
	ID3D11DepthStencilView* m_depthView;
	ID3D11ShaderResourceView* m_depthSRV;	//communicating with pixelshader later
	D3D11_VIEWPORT m_viewport;				//can be smaller than main resolution
	ID3D11VertexShader* m_shadowShader;

	//Constant buffer
	struct ShadowWVP
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	} m_shadowWVP;
	ID3D11Buffer* m_shadowWVPBuffer;

	ID3D11RasterizerState* m_depthBiasRasterizer;	//delete!!!
	ID3D11SamplerState* m_shadowSampler;

//Help functions
private:
	bool CreateSpotLightBuffer(ID3D11Device* device);
	bool CreateDepthBuffer(ID3D11Device* device);	//That uses 32 bit depth
	bool LoadVertexShader(ID3D11Device* device);
	bool CreateShadowWVPBuffer(ID3D11Device* device);

	bool CreateDepthBiasRasterizer(ID3D11Device* device);
	bool CreateShadowSampler(ID3D11Device* device);
public:
	ShadowMap();
	~ShadowMap();

	bool Initialize(ID3D11Device* device);

	void BindShadowVS(ID3D11DeviceContext* deviceContext);
	void UpdateShadowWVP(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 newWorld);

	void BindToPS(ID3D11DeviceContext* deviceContext);

	void DisableSRV(ID3D11DeviceContext* deviceContext);
};