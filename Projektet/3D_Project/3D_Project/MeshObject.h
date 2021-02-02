#pragma once
#include <d3d11.h>
#include <iostream>	// Error handling
#include <vector>
#include <array>
#include <fstream>	// Reading from file
#include <sstream>	// For reading parts of strings
#include "Structures.h"

class MeshObject
{
private:
	// Number of vertices and the buffer which holds them
	UINT m_vertexCount;
	ID3D11Buffer* m_vertexBuffer;
	
	//Texture information
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureSRV;

private:
	bool LoadOBJ(std::string filepath, ID3D11Device* device);
	bool LoadTextures(std::string filepath, ID3D11Device* device);

public:
	MeshObject();
	~MeshObject();

	bool Load(ID3D11Device* device, std::string objfile, std::string texturefile);
	void Render(ID3D11DeviceContext* deviceContext);
};