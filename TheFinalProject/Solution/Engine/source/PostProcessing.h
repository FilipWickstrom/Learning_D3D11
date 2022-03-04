#pragma once

//Max radius at this none optimal way is 15
//Not the best way as I'm saving the alot of weights
//twice. Made an inprovement in "Lilla Spel"
//where I'm only saving the necessary parts :)
const UINT MAXWEIGHTSIZE = 32;
const double PI = 3.14159265359f;

enum class Filter { GAUSSIAN, BILATERAL };

class PostProcessing
{
private:
	ID3D11ShaderResourceView* m_backbuffSRV;		//First pass
	ID3D11UnorderedAccessView* m_middlegroundUAV;	//First pass
	ID3D11ShaderResourceView* m_middlegroundSRV;	//Second pass
	ID3D11UnorderedAccessView* m_backbuffUAV;		//Second pass
	ID3D11ShaderResourceView* m_nullSRV;			//Zeroing between passes
	ID3D11UnorderedAccessView* m_nullUAV;			//Zeroing between passes

	ID3D11ComputeShader* m_gaussShader;
	ID3D11ComputeShader* m_bilateralShader;
	ID3D11Buffer* m_gaussSettingsBuf;
	ID3D11Buffer* m_gaussWeightBuf;
	ID3D11Buffer* m_bilateralSettingsBuf;
	ID3D11Buffer* m_bilateralWeightBuf;

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

	//Also some gauss weights but others from gaussians filter
	struct BilateralWeights
	{
		float weights[MAXWEIGHTSIZE];
	} m_bilateralWeights;

	//ON or OFF settings
	bool m_useGauss;
	bool m_useBilateral;

private:
	//Creating
	bool CreateViews(ID3D11Device* device, IDXGISwapChain* swapChain);
	bool LoadShaders(ID3D11Device* device);
	bool CreateConstantBuffers(ID3D11Device* device);

	//Updating
	void UpdateSettingsBuffer(ID3D11DeviceContext* deviceContext, const Filter& filter);
	void SwapDirection(ID3D11DeviceContext* deviceContext, const Filter& filter);

	bool GenerateGaussFilter(UINT radius, 
							 float sigma = 0, 
							 const Filter& filter = Filter::GAUSSIAN);

public:
	PostProcessing();
	~PostProcessing();

	bool Initialize(ID3D11Device* device, 
					IDXGISwapChain* swapChain, 
					UINT gaussRadius = 3,
					float gaussSigma = 0,
					UINT bilateralRadius = 3, 
					float bilateralSigma = 0.09f);	//Gives best result

	void TurnOnGauss(bool toggle = true);
	void TurnOnBilateral(bool toggle = true);

	void Render(ID3D11DeviceContext* deviceContext, UINT winWidth, UINT winHeight);

};