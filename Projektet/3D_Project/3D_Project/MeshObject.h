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

	//Where th model is in the world
	XMFLOAT4X4 m_modelMatrix;

	std::string m_objfile;
	std::string m_texturefile;
	std::string m_mtlfile;

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

	struct Material
	{
		XMFLOAT4 ambient;		//Ka (float3)
		XMFLOAT4 diffuse;		//Kd (float3)
		XMFLOAT4 specular;		//Ks (float3) + Ns (float)
	};
	Material m_material;

private:
	bool LoadOBJ(ID3D11Device* device);
	bool LoadTextures(ID3D11Device* device);
	bool LoadMaterial(ID3D11Device* device);

public:
	MeshObject();
	~MeshObject();

	//Loading in the model with file, texture and where and how it going to be placed				//REMOVE TEXTURE FROM THIS?***
	bool Load(ID3D11Device* device, std::string obj, std::string texture, 
			  std::array<float, 3>pos = { 0.0f,0.0f,0.0f },
			  std::array<float, 3>scl = { 1.0f,1.0f,1.0f },
			  std::array<float, 3>rot = { 0.0f,0.0f,0.0f });

	//Update the model matrix and uses some defaults if no input
	void UpdateModelMatrix(std::array<float, 3>pos = { 0.0f,0.0f,0.0f },
						   std::array<float, 3>scl = { 1.0f,1.0f,1.0f },
						   std::array<float, 3>rot = { 0.0f,0.0f,0.0f });

	const XMFLOAT4X4 GetModelMatrix() const;

	const Material GetMaterial() const;

	//Drawing what it got in the buffer
	void Render(ID3D11DeviceContext* deviceContext);
};