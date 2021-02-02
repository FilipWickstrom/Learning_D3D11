#pragma once
#include <array>
#include <DirectXMath.h>

struct SimpleVertex
{
	float pos[3];
	float norm[3];
	float tex[2];

	SimpleVertex(const std::array<float, 3>& position, const std::array<float, 3>& normal, const std::array<float, 2>& texCoord)
	{
		for (int i = 0; i < 3; i++)
		{
			pos[i] = position[i];
			norm[i] = normal[i];
		}
		tex[0] = texCoord[0];
		tex[1] = texCoord[1];
	}
};

struct constantBufferWVP
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};
