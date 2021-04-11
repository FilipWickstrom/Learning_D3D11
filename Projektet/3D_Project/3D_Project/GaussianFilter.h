#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <fstream>
#include <iostream>

#define MATH_PI 3.14159265359
#define MAXGAUSSWEIGHTS 32		//15 is max radius

class GaussianFilter
{
private:
	ID3D11ComputeShader* m_computeShader;
	ID3D11UnorderedAccessView* m_unorderedAccessView;			//read from the backbuffer	
	ID3D11Buffer* m_gaussBuffer;

	struct GaussStruct
	{
										//On GPU-side:
		UINT radius;					//4 bytes
		float winWidth;					//4 bytes
		float winHeight;				//4 bytes
		bool doVertBlur;				//4 bytes, true = vertical, false = horizontal
		float weights[MAXGAUSSWEIGHTS];	//32*4 = 128 bytes

	} m_gaussStruct;
	bool m_useGaussFilter;

private:
	bool CreateComputeShader(ID3D11Device* device);
	bool CreateUnorderedAccessView(ID3D11Device* device, IDXGISwapChain* swapChain);
	void SetGaussStruct(float width, float height, UINT radius = 2);
	bool CreateGaussConstBuff(ID3D11Device* device);
	void SwapBlurDirection(ID3D11DeviceContext* deviceContext);
	bool GenerateGaussFilter();
public:
	GaussianFilter();
	~GaussianFilter();

	bool Initialize(ID3D11Device* device, IDXGISwapChain* swapChain, float width, float height, UINT radius);
	void Render(ID3D11DeviceContext* deviceContext, UINT winWidth, UINT winHeight, int nrOfLoops = 1);
	void TurnOnGaussFilter(bool trueOrFalse);

};
