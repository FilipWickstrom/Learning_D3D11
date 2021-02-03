#pragma once
#include <d3d11.h>
#include <string>
#include <fstream>
#include <iostream>

class ShaderClass
{
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11SamplerState* m_anisoSampler;

protected:
	//Can be accessed in the class that inherits from this
	ID3D11InputLayout* m_layout;
	std::string m_vsByteCode;

	//Initialize shader buffers
	bool InitializeShader(ID3D11Device* device, std::string vertexShaderPath, std::string pixelShaderPath);

	//Initialize sampler
	bool InitializeSampler(ID3D11Device* device);

	//Set all the pointers for IA, VS and PS
	void RenderShader(ID3D11DeviceContext* deviceContext);

public:
	ShaderClass();
	virtual ~ShaderClass();

	//Initialize input layout - Depends on how the input looks like
	virtual bool InitializeInputLayout(ID3D11Device* device) = 0;

	//Main initialization function
	virtual bool Initialize(ID3D11Device* device, std::string vsPath, std::string psPath) = 0;

	//Main renderer
	virtual void Render(ID3D11DeviceContext* deviceContext) = 0;

};