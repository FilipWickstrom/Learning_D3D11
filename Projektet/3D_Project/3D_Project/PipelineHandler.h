#pragma once
#include <d3d11.h>
#include <iostream>
#include <fstream>	//Read file

bool SetupPipeline(ID3D11Device* device, 
					ID3D11VertexShader*& vShader,
					ID3D11PixelShader*& pShader, 
					ID3D11InputLayout*& inputLayout,
					ID3D11SamplerState*& sampler);