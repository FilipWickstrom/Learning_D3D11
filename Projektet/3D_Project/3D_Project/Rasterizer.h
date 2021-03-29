#pragma once
#include <d3d11.h>
#include <iostream>

class Rasterizer
{
private:
	ID3D11RasterizerState* m_defaultRS;
	ID3D11RasterizerState* m_wireframeRS;
	bool m_wireFrameON;

//Helpfunctions
private:
	bool CreateRasterizerStates(ID3D11Device* device);

public:
	Rasterizer();
	~Rasterizer();

	bool Initialize(ID3D11Device* device);
	void Bind(ID3D11DeviceContext* deviceContext);
	void UnBind(ID3D11DeviceContext* deviceContext);
	void TurnOnWireframe(bool onOrOff);

};