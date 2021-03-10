#pragma once
#include <vector>
#include "MeshObject.h"
#include "ConstantBuffers.h"
#include "Tessellation.h"
#include "ShadowMap.h"

class Scene
{
private:
	std::vector<MeshObject*> m_objects;
	bool m_useNormalMaps;

public:
	Scene();
	~Scene();

	//Toggle settings
	void UseNormalMaps(bool trueOrFalse);

	//Loads in all the objects 
	bool Load(ID3D11Device* device);
	
	//Render all the mesh objects
	void Render(ID3D11DeviceContext* deviceContext,
				ConstantBuffers& constBuf, 
				Tessellation& tessellation,
				float& dt);

	void RenderShadows(ID3D11DeviceContext* deviceContext, ShadowMap& shadowMap);
};