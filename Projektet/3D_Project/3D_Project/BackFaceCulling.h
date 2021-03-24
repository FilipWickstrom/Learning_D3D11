#pragma once
#include <d3d11.h>
#include <iostream>
#include <fstream>

class BackFaceCulling
{
private:
	ID3D11GeometryShader* m_geometryShader;
	ID3D11RasterizerState* m_rasterizerState;

	//ON ELLER OFF?

//Helpfunctions
private:
	bool LoadGeometryShader(ID3D11Device* device);
	bool CreateRasterizerState(ID3D11Device* device);

public:
	BackFaceCulling();
	~BackFaceCulling();

	bool Initialize(ID3D11Device* device);

	void Bind(ID3D11DeviceContext* deviceContext);
	void UnBind(ID3D11DeviceContext* deviceContext);

};