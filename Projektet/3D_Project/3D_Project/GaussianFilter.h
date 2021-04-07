#pragma once
#include <d3d11.h>
#include <string>
#include <fstream>
#include <iostream>

class GaussianFilter
{
private:
	ID3D11ComputeShader* m_computeShader;
	ID3D11UnorderedAccessView* m_unorderedAccessView;			//read from the backbuffer

	//ID3D11SamplerState* m_gaussSampler;					//no wrapping - use reflect instead
	//constbuf?
	/*
		width
		height
		generated filter
		vertical or horizontal blurring?
	*/
private:
	bool CreateComputeShader(ID3D11Device* device);
	bool CreateUnorderedAccessView(ID3D11Device* device, IDXGISwapChain* swapChain);
	
	//generate filter - update the constbuffer: return array of 5 floats?

public:
	GaussianFilter();
	~GaussianFilter();

	bool Initialize(ID3D11Device* device, IDXGISwapChain* swapChain);
	void Render(ID3D11DeviceContext* deviceContext);
};
