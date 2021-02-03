#pragma once
#include "ShaderClass.h"

class FirstPassShader : public ShaderClass
{
private:
	//matrixbuffer?

public:
	FirstPassShader();
	~FirstPassShader();

	//Setup matrix buffer?
	//setShaderParameters(devicecontext, worldMatrix, view, proj, texture?)

	// Inherited via ShaderClass
	virtual bool InitializeInputLayout(ID3D11Device* device) override;
	virtual bool Initialize(ID3D11Device* device, std::string vsPath, std::string psPath) override;
	virtual void Render(ID3D11DeviceContext* deviceContext) override;
};