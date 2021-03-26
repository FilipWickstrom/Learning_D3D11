#include "MeshObject.h"

//Reading an image file
#define STB_IMAGE_IMPLEMENTATION
#include "Includes/stb/stb_image.h"

MeshObject::MeshObject()
	:m_vertexCount(0)
{
	m_vertexBuffer = nullptr;
	m_diffuseTexture = nullptr;
	m_diffuseTextureSRV = nullptr;

	XMStoreFloat4x4(&m_modelMatrix, XMMatrixIdentity());

	m_material = {};

	m_displacementMap = nullptr;
	m_displacementMapSRV = nullptr;
	m_normalMap = nullptr;
	m_normalMapSRV = nullptr;
	m_settings = {};
	m_settingsBuffer = nullptr;

	// Texture files
	m_diffuseFile = "";
	m_mtlfile = "";
	m_normalFile = "";

	m_tessallated = false;
	m_hasNormalMap = false;

	m_canRotate = false;
	m_rotationSpeed = 0.0f;
	m_position = { 0,0,0 };
	m_scale = { 1,1,1 };
	m_rotation = { 0,0,0 };
	m_visible = true;
}

MeshObject::~MeshObject()
{
	if (m_vertexBuffer)
		m_vertexBuffer->Release();
	if (m_diffuseTexture)
		m_diffuseTexture->Release();
	if (m_diffuseTextureSRV)
		m_diffuseTextureSRV->Release();
	if (m_displacementMap)
		m_displacementMap->Release();
	if (m_displacementMapSRV)
		m_displacementMapSRV->Release();

	if (m_normalMap)
		m_normalMap->Release();
	if (m_normalMapSRV)
		m_normalMapSRV->Release();
	if (m_settingsBuffer)
		m_settingsBuffer->Release();
}

bool MeshObject::LoadOBJ(ID3D11Device* device, std::string objfile)
{
	//Start reading from file
	std::ifstream file;
	file.open(objfile);
	if (file.is_open())
	{
		std::stringstream ss;
		std::string line = "";
		std::string prefix = "";
		XMFLOAT2 float2;	//Holds temporary
		XMFLOAT3 float3;	//Same
		std::vector <XMFLOAT3> positions;
		std::vector <XMFLOAT2> textureCoords;
		std::vector <XMFLOAT3> normals;
		std::vector <SimpleVertex> vertices;

		while (getline(file, line))
		{
			ss.clear();
			ss.str(line);
			ss >> prefix;

			// Check if positon
			if (prefix == "v")
			{
				ss >> float3.x >> float3.y >> float3.z;
				positions.push_back(float3);
			}
			// Check if texture coordinate (vt)
			else if (prefix == "vt")
			{
				ss >> float2.x >> float2.y;
				textureCoords.push_back(float2);
			}
			// Check if normal (vn)
			else if (prefix == "vn")
			{
				ss >> float3.x >> float3.y >> float3.z;
				normals.push_back(float3);
			}
			// Check if face (f)
			else if (prefix == "f")
			{
				std::string str;
				int v, vt, vn;
				while (!ss.eof())
				{
					ss >> str;									// Takes "1/1/1" and saves it
					v = stoi(str.substr(0, str.find('/')));		// Takes out first until '/'
					str.erase(0, str.find('/')+1);				// Remove from start until '/'
																// 1/1 left in this case
					vt = stoi(str.substr(0, str.find('/')));	// Takes out as before until '/'
					str.erase(0, str.find('/') + 1);			// Remove as before
					
					vn = stoi(str);								// Saves the last

					// Put into 
					SimpleVertex thevertex; 
					thevertex.position = positions[v - size_t(1)];
					thevertex.normal = normals[vn - size_t(1)];
					thevertex.texCoord = textureCoords[vt - size_t(1)];
					vertices.push_back(thevertex);
				}
			}
		}
		file.close();
		
		//Calculate the tangent for every face (3 vertices at a time)
		for (int i = 0; i < vertices.size(); i += 3)
		{
			//2 edges from the three points
			XMFLOAT3 edge1 = XMFLOAT3(vertices[i + 1].position.x - vertices[i].position.x,
									  vertices[i + 1].position.y - vertices[i].position.y,
									  vertices[i + 1].position.z - vertices[i].position.z);

			XMFLOAT3 edge2 = XMFLOAT3(vertices[i + 2].position.x - vertices[i].position.x,
									  vertices[i + 2].position.y - vertices[i].position.y,
									  vertices[i + 2].position.z - vertices[i].position.z);
			//	
			XMFLOAT2 uv1 = XMFLOAT2(vertices[i + 1].texCoord.x - vertices[i].texCoord.x,
									   vertices[i + 1].texCoord.y - vertices[i].texCoord.y);
			
			XMFLOAT2 uv2 = XMFLOAT2(vertices[i + 2].texCoord.x - vertices[i].texCoord.x,
									   vertices[i + 2].texCoord.y - vertices[i].texCoord.y);
			
			//The determinant for 
			float detUV = 1.0f / ((uv1.x * uv2.y) - (uv2.x * uv1.y));
			
			XMFLOAT3 tangent;
			tangent.x = detUV * ((uv2.y * edge1.x) - (uv1.y * edge2.x));
			tangent.y = detUV * ((uv2.y * edge1.y) - (uv1.y * edge2.y));
			tangent.z = detUV * ((uv2.y * edge1.z) - (uv1.y * edge2.z));

			vertices[i].tangent = tangent;
			vertices[i + 1].tangent = tangent;
			vertices[i + 2].tangent = tangent;
		}

		/* - - - - - Math behind the scenes - - - - - -
 		
			[edge1] = [uv1.x, uv1.y] * [ Tangent ]
			[edge2]	  [uv2.x, uv2.y]   [Bitangent]

			//Rewrite to get Tangent and Bitangent
			[ Tangent ] = [uv1.x, uv1.y]^-1 * [edge1]
			[Bitangent]	  [uv2.x, uv2.y]	  [edge2]	<==>

			[ Tangent ] = 1 / (uv1.x*uv2.y - uv1.y*uv2.x) * [ uv2.y -uv1.y] * [edge1]
			[Bitangent]										[-uv2.x  uv1.x]   [edge2]

			det = 1 / (uv1.x*uv2.y - uv1.y*uv2.x)

			[ Tangent ] = det * [uv2.y*edge1.x - uv1.y*edge2.x, uv2.y*edge1.y - uv1.y*edge2.y, uv2.y*edge1.z - uv1.y*edge2.z]
		
		- - - - - - - - - END - - - - - - - - -- - - */

		m_vertexCount = (UINT)vertices.size();

		//Vertex buffer - setting up description
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(SimpleVertex) * m_vertexCount;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = &vertices[0];
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		if (FAILED(device->CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer)))
		{
			std::cerr << "Failed to create vertex buffer" << std::endl;
			return false;
		}
	}
	else
	{
		std::cout << "Could not open the obj file..." << std::endl;
		return false;
	}
	return true;
}

bool MeshObject::LoadTexture(ID3D11Device* device, std::string texture)
{
	int textureWidth, textureHeight, channels;
	//Unsigned char because 1 byte (8 bits) which is good for format later on
	unsigned char* image = stbi_load(texture.c_str(), &textureWidth, &textureHeight, &channels, STBI_rgb_alpha);
	if (image == nullptr)
	{
		std::cerr << "Stbi_load() failed to find image..." << std::endl;
		return false;
	}

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = textureWidth;
	desc.Height = textureHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		//UNORM: 0 - 1
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;				//Only reads from GPU
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;    //Bind texture to shader
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &image[0];   //Pointer to data
	data.SysMemPitch = textureWidth * channels;     //Distance from one line to another
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateTexture2D(&desc, &data, &m_diffuseTexture);
	//Freeing memory
	stbi_image_free(image);

	if (FAILED(hr))
	{
		std::cerr << "Failed to create texture..." << std::endl;
		return false;
	}

	hr = device->CreateShaderResourceView(m_diffuseTexture, nullptr, &m_diffuseTextureSRV);
	return !FAILED(hr);
}

bool MeshObject::LoadMaterial(ID3D11Device* device)
{
	std::ifstream file;
	file.open(m_mtlfile, std::ios::in);
	if (file.is_open())
	{
		std::stringstream ss;
		std::string line = "";
		std::string prefix = "";

		while (getline(file, line))
		{
			ss.clear();
			ss.str(line);
			ss >> prefix;

			//Ambient
			if (prefix == "Ka")
			{
				ss >> m_material.ambient.x >> m_material.ambient.y >> m_material.ambient.z;
			}
			//Diffuse
			else if (prefix == "Kd")
			{
				ss >> m_material.diffuse.x >> m_material.diffuse.y >> m_material.diffuse.z;
			}
			//Specular
			else if (prefix == "Ks")
			{
				ss >> m_material.specular.x >> m_material.specular.y >> m_material.specular.z;
			}
			//Shiniess
			else if (prefix == "Ns")
			{
				ss >> m_material.specular.w;
			}

			//Diffuse texture
			else if (prefix == "map_Kd")
			{
				ss >> m_diffuseFile;
			}
			//Normalmap
			else if (prefix == "map_Bump")
			{
				ss >> m_normalFile;
			}

			//Others getting ignored
		}
		file.close();
	}
	else
	{
		std::cout << "Failed to open mtl file..." << std::endl;
		return false;
	}


	return true;
}

bool MeshObject::LoadNormal(ID3D11Device* device)
{
	int textureWidth, textureHeight, channels;
	//Unsigned char because 1 byte (8 bits) which is good for format later on
	unsigned char* image = stbi_load(m_normalFile.c_str(), &textureWidth, &textureHeight, &channels, STBI_rgb_alpha);
	if (image == nullptr)
	{
		std::cerr << "Stbi_load() failed to find image..." << std::endl;
		return false;
	}

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = textureWidth;
	desc.Height = textureHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		//UNORM: 0 - 1
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;				//Only reads from GPU
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;    //Bind texture to shader
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &image[0];   //Pointer to data
	data.SysMemPitch = textureWidth * channels;     //Distance from one line to another
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateTexture2D(&desc, &data, &m_normalMap);
	//Freeing memory
	stbi_image_free(image);

	if (FAILED(hr))
	{
		std::cerr << "Failed to create texture..." << std::endl;
		return false;
	}

	hr = device->CreateShaderResourceView(m_normalMap, nullptr, &m_normalMapSRV);
	return !FAILED(hr);
}

bool MeshObject::CreateSettingsBuff(ID3D11Device* device)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(Settings);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &m_settings;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &data, &m_settingsBuffer);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create settingsbuffer..." << std::endl;
		return false;
	}
	return true;
}

bool MeshObject::Load(ID3D11Device* device, std::string obj, std::string material,
					  std::array<float, 3> pos, std::array<float, 3> scl, std::array<float, 3> rot)
{
	// Load in the obj-file. If failed dont do anything more
	if (!LoadOBJ(device, "ObjFiles/" + obj))
	{
		std::cerr << "Failed to load in " << obj << "..." << std::endl;
		return false;
	}

	// Check the input of the material
	if (material != "")
		m_mtlfile = "Materials/" + material;
	else
		m_mtlfile = "Materials/default.mtl";

	// Loading in the material information and name of textures
	if (!LoadMaterial(device))
	{
		std::cerr << "Failed to load in " << m_mtlfile << std::endl;
		return false;
	}

	// Check if file has not been set
	if (m_diffuseFile == "")
	{
		m_diffuseFile = "defualt.png";
	}

	// Load the diffuse texture
	if (!LoadTexture(device, "Textures/" + m_diffuseFile))
	{
		std::cerr << "Failed to load in the diffuse texture..." << std::endl;
		return false;
	}

	//Load in the normal map if it exist
	if (m_normalFile != "")
	{
		m_normalFile = "Textures/" + m_normalFile;
		if (!LoadNormal(device))
		{
			std::cerr << "Failed to load in " << m_normalFile << std::endl;
			return false;
		}
		else
		{
			m_hasNormalMap = true;
			m_settings.useNormalMap = 1;
			if (!CreateSettingsBuff(device))
			{
				std::cerr << "Failed to create cbuffer for settings..." << std::endl;
				return false;
			}
		}
	}

	//Load in the modell matrix
	UpdateModelMatrix(XMFLOAT3(pos[0], pos[1], pos[2]), XMFLOAT3(scl[0], scl[1], scl[2]), XMFLOAT3(rot[0], rot[1], rot[2]));

	return true;
}

const XMFLOAT3 MeshObject::GetPosition() const
{
	return m_position;
}

const XMFLOAT3 MeshObject::GetScale() const
{
	return m_scale;
}

const XMFLOAT3 MeshObject::GetRotation() const
{
	return m_rotation;
}

void MeshObject::SetTessellated(bool trueOrFalse)
{
	m_tessallated = trueOrFalse;
}

bool MeshObject::LoadDisplacementMap(ID3D11Device* device, std::string displacementMap)
{
	displacementMap = "Textures/" + displacementMap;

	int textureWidth, textureHeight, channels;
	unsigned char* image = stbi_load(displacementMap.c_str(), &textureWidth, &textureHeight, &channels, STBI_rgb_alpha);
	
	if (image == nullptr)
	{
		std::cerr << "Stbi_load() failed to find image..." << std::endl;
		return false;
	}

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = textureWidth;
	desc.Height = textureHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		//UNORM: 0 - 1
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;				//Only reads from GPU
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE| D3D11_BIND_UNORDERED_ACCESS;    //Bind texture to shader
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &image[0];						//Pointer to data
	data.SysMemPitch = textureWidth * channels;     //Distance from one line to another
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateTexture2D(&desc, &data, &m_displacementMap);
	stbi_image_free(image);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create displacement texture..." << std::endl;
		return false;
	}

	hr = device->CreateShaderResourceView(m_displacementMap, nullptr, &m_displacementMapSRV);
	return !FAILED(hr);
}

void MeshObject::UpdateModelMatrix(XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 rot)
{
	m_position = pos;
	m_scale = scl;
	m_rotation = rot;

	float degToRad = XM_PI / 180;
	XMStoreFloat4x4(&m_modelMatrix, XMMatrixScaling(scl.x, scl.y, scl.z) *
									XMMatrixRotationRollPitchYaw(rot.x*degToRad, rot.y*degToRad, rot.z*degToRad) *
									XMMatrixTranslation(pos.x, pos.y, pos.z));
}

const XMFLOAT4X4 MeshObject::GetModelMatrix() const
{
	return m_modelMatrix;
}

const MeshObject::Material MeshObject::GetMaterial() const
{
	return m_material;
}

void MeshObject::UseNormalMap(ID3D11DeviceContext* deviceContext, bool trueOrFalse)
{
	//Can only be use on objects that has normalmaps
	if (m_hasNormalMap)
	{
		if(trueOrFalse)
			m_settings.useNormalMap = 1;
		else
			m_settings.useNormalMap = 0;

		//Update the settings for this
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		deviceContext->Map(m_settingsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &m_settings, sizeof(Settings));
		deviceContext->Unmap(m_settingsBuffer, 0);
	}
}

void MeshObject::SetRotate(bool on, float speed)
{
	m_canRotate = on;
	m_rotationSpeed = speed;
}

void MeshObject::RotateY(float& dt)
{
	//If it can rotate it does it around the y-axis
	if (m_canRotate)
	{
		//Reset after 360 degrees
		if (m_rotation.y >= 360.0f)
			m_rotation.y = 0.0f;	

		m_rotation.y += (m_rotationSpeed * dt);
		
		UpdateModelMatrix({ m_position.x, m_position.y, m_position.z },
						  { m_scale.x,	  m_scale.y,	m_scale.z	 },
						  { m_rotation.x, m_rotation.y, m_rotation.z });
	}
	//Else just skips it
}

void MeshObject::SetVisible(bool trueOrFalse)
{
	m_visible = trueOrFalse;
}

const bool MeshObject::IsVisible() const
{
	return m_visible;
}

void MeshObject::Render(ID3D11DeviceContext* deviceContext, Tessellation* tessellation)
{
	//Only render if it is visible
	if (m_visible)
	{
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

		//Use tessellation for objects that use it
		if (tessellation != nullptr)
			tessellation->SetShaders(deviceContext, m_tessallated, m_displacementMapSRV);

		//Main texture that is going to be used
		deviceContext->PSSetShaderResources(0, 1, &m_diffuseTextureSRV);

		//Setting to nullptr if not exist
		deviceContext->PSSetShaderResources(1, 1, &m_normalMapSRV);

		//Can be different settings for objects
		deviceContext->VSSetConstantBuffers(1, 1, &m_settingsBuffer);
		deviceContext->PSSetConstantBuffers(1, 1, &m_settingsBuffer);

		deviceContext->Draw(m_vertexCount, 0);
	}
}

void MeshObject::RenderShadows(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->Draw(m_vertexCount, 0);
}
