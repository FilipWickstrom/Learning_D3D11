#pragma once
#include <d3d11.h>
#include <iostream>			// Error handling
#include <vector>
#include <array>
#include <fstream>			// Reading from file
#include <sstream>			// For reading parts of strings
#include <DirectXMath.h>	// 4x4 matrix
#include "Tessellation.h"

using namespace DirectX;

class MeshObject
{
private:
	//Number of vertices and the buffer which holds them
	UINT m_vertexCount;
	ID3D11Buffer* m_vertexBuffer;
	
	//Diffuse texture information
	std::string m_diffuseFile;
	ID3D11Texture2D* m_diffuseTexture;
	ID3D11ShaderResourceView* m_diffuseTextureSRV;

	//Where th model is in the world
	XMFLOAT4X4 m_modelMatrix;
	std::string m_mtlfile;

	struct SimpleVertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
		XMFLOAT3 tangent;
	};

	struct Material
	{
		XMFLOAT4 ambient;		//Ka (float3)
		XMFLOAT4 diffuse;		//Kd (float3)
		XMFLOAT4 specular;		//Ks (float3) + Ns (float)
	};
	Material m_material;

	//Tessellation
	bool m_tessallated;
	ID3D11Texture2D* m_displacementMap;
	ID3D11ShaderResourceView* m_displacementMapSRV;

	//Normal mapping
	std::string m_normalFile;
	ID3D11Texture2D* m_normalMap;
	ID3D11ShaderResourceView* m_normalMapSRV;
	bool m_hasNormalMap;

	struct Settings 
	{
		UINT useNormalMap;
		UINT padding[3];
	} m_settings;
	ID3D11Buffer* m_settingsBuffer;

	//The mesh can rotate in realtime
	bool m_canRotate;
	float m_rotationSpeed;
	XMFLOAT3 m_position;
	XMFLOAT3 m_scale;
	XMFLOAT3 m_rotation;

private:
	bool LoadOBJ(ID3D11Device* device, std::string objfile);
	bool LoadTexture(ID3D11Device* device, std::string texture);
	bool LoadMaterial(ID3D11Device* device);
	bool LoadNormal(ID3D11Device* device);
	bool CreateSettingsBuff(ID3D11Device* device);

public:
	MeshObject();
	~MeshObject();

	//Loading in the model with file, texture and where and how it going to be placed
	bool Load(ID3D11Device* device, std::string obj, std::string material, 
			  std::array<float, 3>pos = { 0.0f,0.0f,0.0f },
			  std::array<float, 3>scl = { 1.0f,1.0f,1.0f },
			  std::array<float, 3>rot = { 0.0f,0.0f,0.0f });

	//Tessellated or not
	void SetTessellated(bool trueOrFalse);
	bool LoadDisplacementMap(ID3D11Device* device, std::string displacementMap);

	//Update the model matrix and uses some defaults if no input
	void UpdateModelMatrix(std::array<float, 3>pos = { 0.0f,0.0f,0.0f },
						   std::array<float, 3>scl = { 1.0f,1.0f,1.0f },
						   std::array<float, 3>rot = { 0.0f,0.0f,0.0f });

	const XMFLOAT4X4 GetModelMatrix() const;
	const Material GetMaterial() const;

	//Normal map on or off
	void UseNormalMap(ID3D11DeviceContext* deviceContext, bool trueOrFalse);

	//Set rotation and do rotation around two axis
	void SetRotate(bool on, float speed);
	void RotateY(float& dt);

	//Drawing what it got in the buffer
	void Render(ID3D11DeviceContext* deviceContext, Tessellation& tessellation);
	void RenderShadows(ID3D11DeviceContext* deviceContext);
};