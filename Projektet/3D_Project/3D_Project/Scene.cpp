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
	if (!floor->Load(device, "floor.obj", { 0,-2,0 }, { 10,1,10 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load the floor..." << std::endl;
		return false;
	}
	m_objects.push_back(floor);

	// Cat
	MeshObject* cat = new MeshObject();
	if (!cat->Load(device, "cat.obj", { -7,-0.5,-7 }, { 1,1,1 }, { 0, 135,0 }))
	{
		std::cerr << "Failed to load the cat..." << std::endl;
		return false;
	}
	m_objects.push_back(cat);

	// Cube - rotating
	MeshObject* cube = new MeshObject();
	if (!cube->Load(device, "brickCube.obj", { -6,0,0 }, { 1,1,1 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load the cube..." << std::endl;
		return false;
	}
	cube->SetRotate(true, 0.01f);
	m_objects.push_back(cube);

	// Noah - behind you! Ahhh!
	MeshObject* noah = new MeshObject();
	if (!noah->Load(device, "noah.obj", { 8, 0, -8 }, { 1, 1, 1 }, {0, 135, 0}))
	{
		std::cerr << "Failed to load Noah..." << std::endl;
		return false;
	}
	m_objects.push_back(noah);

	// Sphere
	MeshObject* sphere = new MeshObject();
	if (!sphere->Load(device, "sphere.obj", { -6, -0.5, 5 }, { 1, 1, 1 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load the sphere..." << std::endl;
		return false;
	}
	m_objects.push_back(sphere);

	// Tessellated object on grid with 10x10 squares
	MeshObject* tessGrid = new MeshObject();
	if (!tessGrid->Load(device, "wetRocks10x10_Disp.obj", { 0,1,10 }, { 3,1,3 }, {-90,0,0 }))
	{
		std::cerr << "Failed to load the tessellated object..." << std::endl;
		return false;
	}
	m_objects.push_back(tessGrid);

	// Tessellated object on plane
	MeshObject* tessPlane = new MeshObject();
	if (!tessPlane->Load(device, "wetRocks_Disp.obj", { -7,1,10 }, { 3,1,3 }, { -90,0,0 }))
	{
		std::cerr << "Failed to load the tessellated object..." << std::endl;
		return false;
	}
	m_objects.push_back(tessPlane);

	// Plane with wet rocks with normalmap
	MeshObject* wetRocks = new MeshObject();
	if (!wetRocks->Load(device, "wetRocks.obj", { 7, 1, 10 }, { 3, 1, 3 }, { -90, 0, 0 }))
	{
		std::cerr << "Failed to load the wet rocks wall..." << std::endl;
		return false;
	}
	m_objects.push_back(wetRocks);

	// Brick wall with normal map
	MeshObject* normalPlane = new MeshObject();
	if (!normalPlane->Load(device, "brickWall.obj", { 10, 1, 7 }, { 3, 1, 3 }, { -90, 90, 0 }))
	{
		std::cerr << "Failed to load the normal plane..." << std::endl;
		return false;
	}
	m_objects.push_back(normalPlane);

	// Wall on the left side to shadow on
	MeshObject* shadowWall = new MeshObject();
	if (!shadowWall->Load(device, "basicWall.obj", { -10, 1, 0 }, { 10, 1, 3 }, { 90, 90, 0 }))
	{
		std::cerr << "Failed to load the shadow wall..." << std::endl;
		return false;
	}
	m_objects.push_back(shadowWall);

	// Wall to test gaussian blur and bilateral filter
	MeshObject* detailedWall = new MeshObject();
	if (!detailedWall->Load(device, "feverDreamWall.obj", { 10, 1, -1.5f }, { 5.33f, 1, 3 }, { -90, 90, 0 }))
	{
		std::cerr << "Failed to load the gauss wall..." << std::endl;
		return false;
	}
	m_objects.push_back(detailedWall);

	// Water texture 
	MeshObject* waterWall = new MeshObject();
	waterWall->SetAnimatedTexture(true);
	waterWall->SetAnimationSpeed({ 0.0f, -0.0001f });
	if (!waterWall->Load(device, "waterFloor.obj", { 0,-2,-14 }, { 4,1,4 }, {0,180,0}))
	{
		std::cerr << "Failed to load water floor..." << std::endl;
		return false;
	}
	m_objects.push_back(waterWall);

	return true;
}

void Scene::Render(ID3D11DeviceContext* deviceContext, ConstantBuffers& constBuf, Tessellation* tessellation, float& dt)
{
	for (MeshObject* obj : m_objects)
	{
		//Rotate object that can do it
		obj->RotateY(dt);

		//Each object has it own world matrix that is needed to be set
		constBuf.UpdateWorld(deviceContext, obj->GetModelMatrix());

		//Each object has its own material
		constBuf.UpdateMaterial(deviceContext, obj->GetMaterial().ambient, obj->GetMaterial().diffuse, obj->GetMaterial().specular);

		//Can be toggled on and off for those objects that has normalmaps
		obj->UseNormalMap(deviceContext, m_useNormalMaps);

		//Render the object
		obj->Render(deviceContext, tessellation, dt);	
	}
}

void Scene::RenderShadows(ID3D11DeviceContext* deviceContext, ShadowMap& shadowMap)
{
	for (MeshObject* obj : m_objects)
	{
		if (obj->IsVisible())
		{
			shadowMap.UpdateShadowWVP(deviceContext, obj->GetModelMatrix());
			obj->RenderShadows(deviceContext);
		}
	}
}
