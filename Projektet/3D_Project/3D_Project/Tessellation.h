#pragma once
#include <d3d11.h>
#include <string>
#include <fstream>
#include <iostream>

class Tessellation
{
private:
	ID3D11HullShader* m_hullShader;
	ID3D11DomainShader* m_domainShader;

	//Wireframe
	ID3D11RasterizerState* m_rasterizerState;
	bool m_wireframeOn;

private:
	bool LoadShaders(ID3D11Device* device);
	bool CreateRasterizerState(ID3D11Device* device);
	
public:
	Tessellation();
	~Tessellation();

	bool Initialize(ID3D11Device* device);
	void SetShaders(ID3D11DeviceContext* deviceContext, 
					bool useTessellation,  
					ID3D11ShaderResourceView* displaceMapSRV);
	
	void SetWireframe(bool trueorfalse);

	//For screen quad
	void TurnOff(ID3D11DeviceContext* deviceContext);
};