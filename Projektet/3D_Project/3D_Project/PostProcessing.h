#pragma once
#include <d3d11.h>
#include <iostream>
#include <string>
#include <fstream>

const UINT MAXWEIGHTSIZE = 32;		//15 is max radius
const double PI = 3.14159265359f;

enum class Filter { GAUSSIAN, BILATERAL };

class PostProcessing
{
private:
	ID3D11UnorderedAccessView* m_unorderedAccessView;	//read from the backbuffer
	ID3D11ComputeShader* m_gaussShader;
	ID3D11ComputeShader* m_bilateralShader;
	ID3D11Buffer* m_gaussSettingsBuf;
	ID3D11Buffer* m_gaussWeightBuf;
	ID3D11Buffer* m_bilateralSettingsBuf;

	struct GaussSettings
	{
		UINT radius;
		bool doVertBlur;
		float padding[2];
	} m_gaussSettings;

	struct GaussWeights
	{
		float weights[MAXWEIGHTSIZE];
	} m_gaussWeights;

	struct BilateralSettings
	{
		UINT radius;
		bool doVertFilter;
		float sigma;
		float padding;
	} m_bilateralSettings;

	/*
	Weights for bilateral??? Or use the same as gauss uses
	*/

	bool m_useGauss;
	bool m_useBilateral;

private:
	//Creating
	bool CreateUAV(ID3D11Device* device, IDXGISwapChain* swapChain);
	bool LoadShaders(ID3D11Device* device);
	bool CreateConstantBuffers(ID3D11Device* device);

	//Updating
	void UpdateSettingsBuffer(ID3D11DeviceContext* deviceContext, const Filter& filter);
	void SwapDirection(ID3D11DeviceContext* deviceContext, const Filter& filter);

	//Generation
	bool GenerateGaussFilter(UINT radius, float sigma = 0);


	//EXTRA TO FIX LATER:
	//Update weights

public:
	PostProcessing();
	~PostProcessing();

	bool Initialize(ID3D11Device* device, 
					IDXGISwapChain* swapChain, 
					UINT gaussRadius = 3,
					UINT bilateralRadius = 3, 
					float bilateralSigma = 1.0f);

	void TurnOnGauss(bool toggle = true);
	void TurnOnBilateral(bool toggle = true);

	//Change settings??? sigma? Generate other filters???

	void Render(ID3D11DeviceContext* deviceContext, UINT winWidth, UINT winHeight);

};