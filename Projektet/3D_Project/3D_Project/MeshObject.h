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

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
		XMFLOAT3 tangent;
	};

	std::string m_mtlfile;
	struct Material
	{
		XMFLOAT4 ambient  = {};		//Ka (float3)
		XMFLOAT4 diffuse  = {};		//Kd (float3)
		XMFLOAT4 specular = {};		//Ks (float3) + Ns (float)
	};
	Material m_material;

	//Tessellation
	bool m_tessallated;
	std::string m_displaceFile;
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

	//Render the mesh or not
	bool m_visible;

	//Water effect
	bool m_hasAnimTexture;
	std::vector<Vertex> m_animVertices;
	std::vector<XMFLOAT2> m_origTexCoords;
	DirectX::XMFLOAT2 m_animSpeed;
	DirectX::XMFLOAT2 m_animOffset;

private:
	bool LoadOBJ(ID3D11Device* device, std::string objfile);
	bool LoadMaterial(ID3D11Device* device);
	//Load textures
	bool LoadDiffuse(ID3D11Device* device);
	bool LoadNormal(ID3D11Device* device);
	bool LoadDisplacement(ID3D11Device* device);

	bool CreateSettingsBuff(ID3D11Device* device);

public:
	MeshObject();
	~MeshObject();

	//Loading in the model with file, texture and where and how it going to be placed
	bool Load(ID3D11Device* device, 
			  std::string objfile,
			  std::array<float, 3>pos = { 0.0f,0.0f,0.0f },
			  std::array<float, 3>scl = { 1.0f,1.0f,1.0f },
			  std::array<float, 3>rot = { 0.0f,0.0f,0.0f });

	//Get orientation
	const XMFLOAT3 GetPosition() const;
	const XMFLOAT3 GetScale() const;
	const XMFLOAT3 GetRotation() const;

	//Update the model matrix and uses some defaults if no input
	void UpdateModelMatrix(XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 rot);

	const XMFLOAT4X4 GetModelMatrix() const;
	const Material GetMaterial() const;

	//Normal map on or off
	void UseNormalMap(ID3D11DeviceContext* deviceContext, bool trueOrFalse);

	//Set rotation and do rotation around two axis
	void SetRotate(bool on, float speed);
	void RotateY(float& dt);

	//Hide
	void SetVisible(bool trueOrFalse);
	const bool IsVisible() const;

	//Water effect
	void SetAnimatedTexture(bool toggle = true);
	void UpdateTextureAnim(ID3D11DeviceContext* deviceContext, const float& dt);
	void SetAnimationSpeed(XMFLOAT2 speed);

	//Drawing what it got in the buffer
	void Render(ID3D11DeviceContext* deviceContext, Tessellation* tessellation, const float& dt);
	void RenderShadows(ID3D11DeviceContext* deviceContext);
};