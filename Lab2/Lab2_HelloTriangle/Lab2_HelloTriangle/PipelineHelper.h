#pragma once
#include <array>
#include <d3d11.h>
#include <DirectXMath.h>

struct SimpleVertex
{
	float pos[3];
	float uv[2];

	SimpleVertex(const std::array<float, 3>& position, const std::array<float, 2> uvCoords/*, const std::array<float, 3> colour*/)
	{
		for (int i = 0; i < 3; ++i)
		{
			pos[i] = position[i];
		}
		uv[0] = uvCoords[0];
		uv[1] = uvCoords[1];
	}
};

struct ConstantBuffer
{
	DirectX::XMFLOAT4X4 World;
	//DirectX::XMFLOAT4X4 View;
	//DirectX::XMFLOAT4X4 Projection;
};

bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, 
				   ID3D11DeviceContext*& immediateContext, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV, ID3D11SamplerState*& sampler);