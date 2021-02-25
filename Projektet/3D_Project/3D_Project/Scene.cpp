#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
	//Cleaning up objects. Do not won't memory leaks
	for (MeshObject* obj : m_objects)
	{
		delete obj;
		obj = nullptr;
	}
}

bool Scene::Load(ID3D11Device* device)
{
	MeshObject* mesh0 = new MeshObject();
	if (!mesh0->Load(device, "smallcat.obj", "oldpaper.png", { -2,0,0 }, { 1,1,1 }, { 0,-90,0 }))
	{
		std::cerr << "Failed to load mesh0..." << std::endl;
		return false;
	}
	m_objects.push_back(mesh0);

	MeshObject* mesh1 = new MeshObject();
	if (!mesh1->Load(device, "cube.obj", "techflip.png", { 2,1,0 }, { 1,1,1 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load mesh1..." << std::endl;
		return false;
	}
	m_objects.push_back(mesh1);

	MeshObject* mesh2 = new MeshObject();
	if (!mesh2->Load(device, "plane.obj", "stoneGrass.png", { 0,-1.5,0 }, { 10,1,10 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load mesh2..." << std::endl;
		return false;
	}
	m_objects.push_back(mesh2);

	MeshObject* mesh3 = new MeshObject();
	if (!mesh3->Load(device, "betternoah.obj", "base.png", { 4, 0.5, -5 }, { 1, 1, 1 }, {0, 180, 0}))
	{
		std::cerr << "Failed to load mesh3..." << std::endl;
	}
	m_objects.push_back(mesh3);

	MeshObject* mesh4 = new MeshObject();
	if (!mesh4->Load(device, "sphere.obj", "base.png", { -4, 0, -5 }, { 1, 1, 1 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load mesh4..." << std::endl;
	}
	m_objects.push_back(mesh4);

	return true;
}

void Scene::Render(ID3D11DeviceContext* deviceContext, ConstantBuffers& constBuf)
{
	for (MeshObject* obj : m_objects)
	{
		//Each object has it own world matrix that is needed to be set
		constBuf.UpdateWorld(deviceContext, obj->GetModelMatrix());
		
		//Each object has its own material
		constBuf.UpdateMaterial(deviceContext, obj->GetMaterial().ambient, obj->GetMaterial().diffuse, obj->GetMaterial().specular);

		//Render the object
		obj->Render(deviceContext);
	}
}
