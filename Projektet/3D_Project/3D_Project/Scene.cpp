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
	// Floor
	MeshObject* floor = new MeshObject();
	if (!floor->Load(device, "plane.obj", "stoneGrass.png", { 0,-1.5,0 }, { 10,1,10 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load the floor..." << std::endl;
		return false;
	}
	m_objects.push_back(floor);

	// Cat
	MeshObject* cat = new MeshObject();
	if (!cat->Load(device, "smallcat.obj", "oldpaper.png", { -7,0,-7 }, { 1,1,1 }, { 0, 135,0 }))
	{
		std::cerr << "Failed to load the cat..." << std::endl;
		return false;
	}
	m_objects.push_back(cat);

	// Cube
	MeshObject* cube = new MeshObject();
	if (!cube->Load(device, "cube.obj", "base.png", { -8,1,0 }, { 1,1,1 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load the cube..." << std::endl;
		return false;
	}
	m_objects.push_back(cube);

	// Noah - behind you! Ahhh!
	MeshObject* noah = new MeshObject();
	if (!noah->Load(device, "betternoah.obj", "base.png", { 7, 0.5, -7 }, { 1, 1, 1 }, {0, 135, 0}))
	{
		std::cerr << "Failed to load Noah..." << std::endl;
		return false;
	}
	m_objects.push_back(noah);

	// Sphere
	MeshObject* sphere = new MeshObject();
	if (!sphere->Load(device, "newSphere.obj", "base.png", { 0, 0, -7 }, { 1, 1, 1 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load the sphere..." << std::endl;
		return false;
	}
	m_objects.push_back(sphere);

	// Tessellated object on grid with 10x10 squares
	MeshObject* tessGrid = new MeshObject();
	if (!tessGrid->Load(device, "grid10x10.obj", "wetRocks.png", { 4,2,10 }, { 3,1,3 }, {-90,0,0 }))
	{
		std::cerr << "Failed to load the tessellated object..." << std::endl;
		return false;
	}
	tessGrid->SetTessellated(true);
	if (!tessGrid->LoadDisplacementMap(device, "wetRocksHeightMap.png"))
	{
		std::cerr << "Failed to load diplacement texture for mesh5..." << std::endl;
		return false;
	}
	m_objects.push_back(tessGrid);

	// Tessellated object on plane
	MeshObject* tessPlane = new MeshObject();
	if (!tessPlane->Load(device, "plane.obj", "wetRocks.png", { -4,2,10 }, { 3,1,3 }, { -90,0,0 }))
	{
		std::cerr << "Failed to load the tessellated object..." << std::endl;
		return false;
	}
	tessPlane->SetTessellated(true);
	if (!tessPlane->LoadDisplacementMap(device, "wetRocksHeightMap.png"))
	{
		std::cerr << "Failed to load diplacement texture for mesh5..." << std::endl;
		return false;
	}
	m_objects.push_back(tessPlane);

	MeshObject* normalPlane = new MeshObject();
	if (!normalPlane->Load(device, "plane.obj", "brick.png", { 10, 2, 2 }, { 3, 1, 3 }, { 90, -90, 0 }))
	{
		std::cerr << "Failed to load the sphere..." << std::endl;
		return false;
	}
	m_objects.push_back(normalPlane);

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
