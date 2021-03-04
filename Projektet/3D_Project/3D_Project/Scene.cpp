#include "Scene.h"

Scene::Scene()
{
	m_useNormalMaps = true;
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

void Scene::UseNormalMaps(bool trueOrFalse)
{
	m_useNormalMaps = trueOrFalse;
}

bool Scene::Load(ID3D11Device* device)
{
	// Floor
	MeshObject* floor = new MeshObject();
	if (!floor->Load(device, "plane.obj", "floor.mtl", { 0,-1.5,0 }, { 10,1,10 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load the floor..." << std::endl;
		return false;
	}
	m_objects.push_back(floor);

	// Cat
	MeshObject* cat = new MeshObject();
	if (!cat->Load(device, "smallcat.obj", "oldpaper.mtl", { -7,0,-7 }, { 1,1,1 }, { 0, 135,0 }))
	{
		std::cerr << "Failed to load the cat..." << std::endl;
		return false;
	}
	m_objects.push_back(cat);

	// Cube
	MeshObject* cube = new MeshObject();
	if (!cube->Load(device, "cube.obj", "brownBricks.mtl", { -8,1,0 }, { 1,1,1 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load the cube..." << std::endl;
		return false;
	}
	m_objects.push_back(cube);

	// Noah - behind you! Ahhh!
	MeshObject* noah = new MeshObject();
	if (!noah->Load(device, "betternoah.obj", "shinyDefault.mtl", { 7, 0.5, -7 }, { 1, 1, 1 }, {0, 135, 0}))
	{
		std::cerr << "Failed to load Noah..." << std::endl;
		return false;
	}
	m_objects.push_back(noah);

	// Sphere
	MeshObject* sphere = new MeshObject();
	if (!sphere->Load(device, "newSphere.obj", "default.mtl", { 0, 0, -7 }, { 1, 1, 1 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load the sphere..." << std::endl;
		return false;
	}
	m_objects.push_back(sphere);

	// Tessellated object on grid with 10x10 squares
	MeshObject* tessGrid = new MeshObject();
	if (!tessGrid->Load(device, "grid10x10.obj", "wetRocks.mtl", { 0,2,10 }, { 3,1,3 }, {-90,0,0 }))
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
	if (!tessPlane->Load(device, "plane.obj", "wetRocks.mtl", { -7,2,10 }, { 3,1,3 }, { -90,0,0 }))
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
	if (!normalPlane->Load(device, "plane.obj", "bricks.mtl", { 10, 2, -3 }, { 3, 1, 3 }, { -90, 90, 0 }))
	{
		std::cerr << "Failed to load the normal plane..." << std::endl;
		return false;
	}
	m_objects.push_back(normalPlane);

	MeshObject* wetRocks = new MeshObject();
	if (!wetRocks->Load(device, "plane.obj", "wetRocks.mtl", { 7, 2, 10 }, { 3, 1, 3 }, { -90, 0, 0 }))
	{
		std::cerr << "Failed to load the wet rocks wall..." << std::endl;
		return false;
	}
	m_objects.push_back(wetRocks);

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

		//Can be toggled on and off for those objects that has normalmaps
		obj->UseNormalMap(deviceContext, m_useNormalMaps);

		//Render the object
		obj->Render(deviceContext, tessellation);
	}
}