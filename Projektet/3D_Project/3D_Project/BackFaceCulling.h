#pragma once
#include <d3d11.h>
#include <iostream>
#include <fstream>

class BackFaceCulling
{
private:
	ID3D11GeometryShader* m_geometryShader;

	//ON ELLER OFF?
	bool m_cullingOn;

//Helpfunctions
private:
	bool LoadGeometryShader(ID3D11Device* device);

public:
	BackFaceCulling();
	~BackFaceCulling();

	bool Initialize(ID3D11Device* device);

	void Bind(ID3D11DeviceContext* deviceContext);
	void UnBind(ID3D11DeviceContext* deviceContext);

};