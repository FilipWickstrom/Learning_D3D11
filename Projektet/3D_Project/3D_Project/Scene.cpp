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
	if (!mesh1->Load(device, "cube.obj", "techflip.png", { 2,0,0 }, { 1,1,1 }, { 0, 45, 0 }))
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
	if (!mesh3->Load(device, "noah.obj", "base.png", { 4, 0.5, -5 }, { 1, 1, 1 }, {0, 0, 0}))	//{0,180,0}
	{
		std::cerr << "Failed to load mesh3..." << std::endl;
	}
	m_objects.push_back(mesh3);

	MeshObject* mesh4 = new MeshObject();
	if (!mesh4->Load(device, "sphere.obj", "base.png", { -4, 0, -5 }, { 1, 1, 1 }, { 0, 0, 0 }))
	{
		std::cerr << "Failed to load mesh3..." << std::endl;
	}
	m_objects.push_back(mesh4);

	return true;
}

void Scene::Render(ID3D11DeviceContext* deviceContext, ConstantBuffers& constBuf)
{
	static float rotation = 0.0f;
	if (rotation > 360.0f)
		rotation = 0.0f;

	bool first = true;	//*****

	for (MeshObject* obj : m_objects)
	{
		//DELETE LATER****
		if (first)
		{
			XMFLOAT4X4 modelFloat = obj->GetModelMatrix();
			XMMATRIX modelMatrix = XMLoadFloat4x4(&modelFloat);
			XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(0.0f, rotation, 0.0f);
			XMMATRIX multiplied = XMMatrixMultiply(rotMatrix, modelMatrix);
			XMFLOAT4X4 final;
			XMStoreFloat4x4(&final, multiplied);

			constBuf.UpdateWorld(deviceContext, final);
			first = false;
		}
		//Each object has it own world matrix that is needed to be set
		else
		{
			constBuf.UpdateWorld(deviceContext, obj->GetModelMatrix());
		}

		//Render the object
		obj->Render(deviceContext);
	}

	rotation += 0.01f;	//*****
}
