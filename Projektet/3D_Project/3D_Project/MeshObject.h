#pragma once
#include <d3d11.h>
#include <string>
#include <array>
#include <iostream>	//Error handling

class MeshObject
{
private:
	//std::string m_filename;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	UINT m_vertexCount;
	UINT m_indexCount;

	struct SimpleVertex		//CHANGE NAME OF IT?
	{
		float pos[3];
		float col[3];
		float norm[3];

		SimpleVertex(const std::array<float, 3>& position, const std::array<float, 3>& colour, const std::array<float, 3>& normal)
		{
			for (int i = 0; i < 3; i++)
			{
				pos[i] = position[i];
				col[i] = colour[i];
				norm[i] = normal[i];
			}
		}
	};

	//USE THIS ONE INSTEAD LATER ON
	/*struct SimpleVertex
	{
		float pos[3];
		float uv[2];
		float norm[3];

		SimpleVertex(const std::array<float, 3>& position, const std::array<float, 2>& uvCoords, const std::array<float, 3>& normal)
		{
			for (int i = 0; i < 3; i++)
			{
				pos[i] = position[i];
				norm[i] = normal[i];
			}
			uv[0] = uvCoords[0];
			uv[1] = uvCoords[1];
		}
	};*/

public:
	MeshObject();
	~MeshObject();

	bool Load(ID3D11Device* device);
	//LOAD TEXTURE FOR IT???

	void Render(ID3D11DeviceContext* deviceContext);
};