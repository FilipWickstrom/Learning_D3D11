#pragma once
#include <d3d11.h>
#include <array>

class ScreenQuad
{
private:
	ID3D11Buffer* m_vertexBuffer;
	int m_nrOfVertices;

	struct ScreenVertex
	{
		float pos[3];
		float tex[2];

		ScreenVertex(const std::array<float, 3>& position, const std::array<float, 2>& texCoord)
		{
			for (int i = 0; i < 3; i++)
				pos[i] = position[i];
			tex[0] = texCoord[0];
			tex[1] = texCoord[1];
		}
	};

public:
	ScreenQuad();
	~ScreenQuad();

	bool Initialize(ID3D11Device* device, UINT winWidth, UINT winHeight);
	void Render(ID3D11DeviceContext* deviceContext);
	int GetNrOfVertices() const;
};