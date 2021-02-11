#pragma once
#include <d3d11.h>
#include <iostream>			// Error handling
#include <vector>
#include <array>
#include <fstream>			// Reading from file
#include <sstream>			// For reading parts of strings
#include <DirectXMath.h>	// 4x4 matrix
using namespace DirectX;

class MeshObject
{
private:
	//Number of vertices and the buffer which holds them
	UINT m_vertexCount;
	ID3D11Buffer* m_vertexBuffer;
	
	//Texture information
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureSRV;

	//Where model is in the world
	XMFLOAT4X4 m_modelMatrix;

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

private:
	bool LoadOBJ(std::string filepath, ID3D11Device* device);
	bool LoadTextures(std::string filepath, ID3D11Device* device);
	void SetupModelMatrix(std::array<float, 3>pos,std::array<float, 3>scl,std::array<float, 3>rot);

public:
	MeshObject();
	~MeshObject();

	//Loading in the model with file, texture and where and how it going to be placed
	bool Load(ID3D11Device* device, std::string obj, std::string texture, 
			  std::array<float, 3>pos = { 0.0f,0.0f,0.0f },
			  std::array<float, 3>scl = { 1.0f,1.0f,1.0f },
			  std::array<float, 3>rot = { 0.0f,0.0f,0.0f });

	//Update the model matrix and uses some defaults if no input
	void UpdateModelMatrix(std::array<float, 3>pos = { 0.0f,0.0f,0.0f },
						   std::array<float, 3>scl = { 1.0f,1.0f,1.0f },
						   std::array<float, 3>rot = { 0.0f,0.0f,0.0f });

	XMFLOAT4X4 GetModelMatrix() const;

	//Drawing what it got in the buffer
	void Render(ID3D11DeviceContext* deviceContext);
};