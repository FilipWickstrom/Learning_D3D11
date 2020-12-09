#pragma once
#include <array>
#include <d3d11.h>
#include <DirectXMath.h>

struct SimpleVertex
{
	float pos[3];
	float uv[2];
	float norm[3];

	SimpleVertex(const std::array<float, 3>& position, const std::array<float, 2> uvCoords, const std::array<float, 3>& normal)
	{
		for (int i = 0; i < 3; ++i)
		{
			pos[i] = position[i];
			norm[i] = normal[i];
		}
		uv[0] = uvCoords[0];
		uv[1] = uvCoords[1];
	}
};

bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, 
				   ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV, ID3D11SamplerState*& sampler);