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
	ID3D11Buffer* m_gaussBuffer;

	struct GaussStruct
	{
		float sigma;
		float radius;		//start with 2
		float winWidth;
		float winHeight;
		bool doVertBlur;	//true = vertical, false = horizontal
		float padding[3];	//bool on CPU = 1byte, and GPU = 4bytes
	} m_gaussStruct;

	bool m_useGaussFilter;

	//float gereratedFilter[radius * 2 + 1] ?
	//float padding to fill out ? some array[? ? ? ]

private:
	bool CreateComputeShader(ID3D11Device* device);
	bool CreateUnorderedAccessView(ID3D11Device* device, IDXGISwapChain* swapChain);
	void SetGaussStruct(float width, float height, float sigma = 1.0f, float radius = 2.0f);
	bool CreateGaussConstBuff(ID3D11Device* device);
	void SwapBlurDirection(ID3D11DeviceContext* deviceContext);

	//generate filter - update the constbuffer: return array of 5 floats?

public:
	GaussianFilter();
	~GaussianFilter();

	bool Initialize(ID3D11Device* device, IDXGISwapChain* swapChain, float width, float height, float sigma, float radius);
	void Render(ID3D11DeviceContext* deviceContext, UINT winWidth, UINT winHeight);
	void TurnOnGaussFilter(bool trueOrFalse);

};
