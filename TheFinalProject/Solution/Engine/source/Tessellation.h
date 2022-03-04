#pragma once

class Tessellation
{
private:
	ID3D11HullShader* m_hullShader;
	ID3D11DomainShader* m_domainShader;

	//Constant buffer with settings
	struct tessellSettings
	{
		float level;
		float depth;
		float padding[2];
	} m_tessellSettings;
	ID3D11Buffer* m_tessellationBuffer;

private:
	bool LoadShaders(ID3D11Device* device);
	bool CreateTessellSettings(ID3D11Device* device);

public:
	Tessellation();
	~Tessellation();

	bool Initialize(ID3D11Device* device);
	void SetShaders(ID3D11DeviceContext* deviceContext, 
					bool useTessellation,  
					ID3D11ShaderResourceView* displaceMapSRV);

	//For screen quad
	void TurnOff(ID3D11DeviceContext* deviceContext);

	//For updating tessellation in cbuffer
	void UpdateLOD(ID3D11DeviceContext* deviceContext, float level);
	void UpdateDepth(ID3D11DeviceContext* deviceContext, float depth);
};