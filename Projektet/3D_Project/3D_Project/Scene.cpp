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
	// Cat to the left
	MeshObject* mesh0 = new MeshObject();
	if (!mesh0->Load(device, "smallcat.obj", "oldpaper.png", { -2,0,0 }, { 1,1,1 }, { 0,-90,0 }))
	{
		std::cerr << "Failed to load the cat..." << std::endl;
		return false;
	}
	m_objects.push_back(mesh0);

	// Cube to the right
	MeshObject* mesh1 = new MeshObject();
	if (!mesh1->Load(device, "cube.obj", "techflip.png", { 2,1,0 }, { 1,1,1 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load the cube..." << std::endl;
		return false;
	}
	m_objects.push_back(mesh1);

	// Floor
	MeshObject* mesh2 = new MeshObject();
	if (!mesh2->Load(device, "plane.obj", "stoneGrass.png", { 0,-1.5,0 }, { 10,1,10 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load the floor..." << std::endl;
		return false;
	}
	m_objects.push_back(mesh2);

	// Noah - behind you! Ahhh!
	MeshObject* mesh3 = new MeshObject();
	if (!mesh3->Load(device, "betternoah.obj", "base.png", { 4, 0.5, -5 }, { 1, 1, 1 }, {0, 180, 0}))
	{
		std::cerr << "Failed to load Noah..." << std::endl;
		return false;
	}
	m_objects.push_back(mesh3);

	// Sphere
	MeshObject* mesh4 = new MeshObject();
	if (!mesh4->Load(device, "sphere.obj", "base.png", { -4, 0, -5 }, { 1, 1, 1 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load the sphere..." << std::endl;
		return false;
	}
	m_objects.push_back(mesh4);

	// Tessellated object
	MeshObject* mesh5 = new MeshObject();
	if (!mesh5->Load(device, "plane.obj", "pebbles.png", { 0,2,10 }, { 3,3,3 }, {-90,0,0 }))
	{
		std::cerr << "Failed to load the tessellated object..." << std::endl;
		return false;
	}
	mesh5->SetTessellated(true);
	mesh5->SetWireframe(true);
	m_objects.push_back(mesh5);

	return true;
}

void Scene::Render(ID3D11DeviceContext* deviceContext, ConstantBuffers& constBuf, Tessellation& tessellation)
{
	for (MeshObject* obj : m_objects)
	{
		//Each object has it own world matrix that is needed to be set
		constBuf.UpdateWorld(deviceContext, obj->GetModelMatrix());
		
		//Each object has its own material
		constBuf.UpdateMaterial(deviceContext, obj->GetMaterial().ambient, obj->GetMaterial().diffuse, obj->GetMaterial().specular);

		//Render the object
		obj->Render(deviceContext, tessellation);
	}
}
