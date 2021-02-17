#pragma once
#include <vector>
#include "MeshObject.h"
#include "ConstantBuffers.h"

class Scene
{
private:
	std::vector<MeshObject*> m_objects;

public:
	Scene();
	~Scene();

	//Loads in all the objects 
	bool Load(ID3D11Device* device);
	
	//Render all the mesh objects
	void Render(ID3D11DeviceContext* deviceContext, ConstantBuffers& constBuf);
};