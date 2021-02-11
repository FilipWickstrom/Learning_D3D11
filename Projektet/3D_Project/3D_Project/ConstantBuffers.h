#pragma once
#include <d3d11.h>
#include <DirectXMath.h> 
#include <iostream>
using namespace DirectX;

class ConstantBuffers
{
private:
	struct WVPMatrix
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};
	WVPMatrix m_WVPMatrix;
	ID3D11Buffer* m_WVPBuffer;

	//Light things
	//struct
	//and buffer for it

private:
	bool CreateWVPBuffer(ID3D11Device* device);
	void UpdateWVP(ID3D11DeviceContext* deviceContext);

public:
	ConstantBuffers();
	~ConstantBuffers();

	bool InitializeWVP(ID3D11Device* device, XMFLOAT4X4 view, XMFLOAT4X4 proj);
	
	void SetWVPToVS(ID3D11DeviceContext* deviceContext);
	
	//Update the struct
	void UpdateWorld(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 world);
	void UpdateView(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 view);
	void UpdateProjection(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 proj);
	
	//InitalizeLight();

};

