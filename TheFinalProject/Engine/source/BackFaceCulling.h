#pragma once

class BackFaceCulling
{
private:
	ID3D11GeometryShader* m_geometryShader;
	bool m_cullingOn;

private:
	bool LoadGeometryShader(ID3D11Device* device);

public:
	BackFaceCulling();
	~BackFaceCulling();

	bool Initialize(ID3D11Device* device);
	void Bind(ID3D11DeviceContext* deviceContext);
	void UnBind(ID3D11DeviceContext* deviceContext);
	void SetCullingOn(bool onOrOff);
};