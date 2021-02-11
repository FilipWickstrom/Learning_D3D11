#include "MeshObject.h"

//Reading an image file
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

MeshObject::MeshObject()
	:m_vertexCount(0)
{
	m_vertexBuffer = nullptr;
	m_texture = nullptr;
	m_textureSRV = nullptr;

	XMStoreFloat4x4(&m_modelMatrix, XMMatrixIdentity());
}

MeshObject::~MeshObject()
{
	if (m_vertexBuffer != nullptr)
		m_vertexBuffer->Release();
	if (m_texture != nullptr)
		m_texture->Release();
	if (m_textureSRV != nullptr)
		m_textureSRV->Release();
}

//Some more work needs to be done...
bool MeshObject::LoadOBJ(std::string filepath, ID3D11Device* device)
{
	//Start reading from file
	std::ifstream file;
	file.open(filepath, std::ios::in);
	if (file.is_open())
	{
		std::stringstream ss;
		std::string line = "";
		std::string prefix = "";
		std::array<float, 2>array2;
		std::array<float, 3> array3;
		std::vector <std::array<float, 3>> positions;
		std::vector <std::array<float, 2>> textureCoords;
		std::vector <std::array<float, 3>> normals;

		std::vector <SimpleVertex> vertices;

		while (getline(file, line))
		{
			ss.clear();
			ss.str(line);
			ss >> prefix;

			// Check if positon
			if (prefix == "v")
			{
				ss >> array3[0] >> array3[1] >> array3[2];
				positions.push_back(array3);
			}
			// Check if texture coordinate (vt)
			else if (prefix == "vt")
			{
				ss >> array2[0] >> array2[1];
				textureCoords.push_back(array2);
			}
			// Check if normal (vn)
			else if (prefix == "vn")
			{
				ss >> array3[0] >> array3[1] >> array3[2];
				normals.push_back(array3);
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
					SimpleVertex thevertex(positions[v-1], normals[vn-1], textureCoords[vt-1]);
					vertices.push_back(thevertex);
				}
			}
			//If matllib???	*** FIX LATER
			//if usemtl
		}
		file.close();

		m_vertexCount = vertices.size();

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

bool MeshObject::LoadTextures(std::string filepath, ID3D11Device* device)
{
	int textureWidth, textureHeight, channels;
	//Unsigned char because 1 byte (8 bits) which is good for format later on
	unsigned char* image = stbi_load(filepath.c_str(), &textureWidth, &textureHeight, &channels, STBI_rgb_alpha);

	//Description
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = textureWidth;
	desc.Height = textureHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;   //UNORM: 0 - 1
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;     //Only reads from GPU
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;    //Bind texture to shader
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &image[0];   //Pointer to data
	data.SysMemPitch = textureWidth * channels;     //Distance from one line to another
	data.SysMemSlicePitch = 0;

	if (FAILED(device->CreateTexture2D(&desc, &data, &m_texture)))
	{
		std::cerr << "Failed to create texture..." << std::endl;
		stbi_image_free(image);
		return false;
	}

	//Freeing memory
	stbi_image_free(image);

	HRESULT hr = device->CreateShaderResourceView(m_texture, nullptr, &m_textureSRV);
	return !FAILED(hr);
}

void MeshObject::SetupModelMatrix(std::array<float, 3> pos, std::array<float, 3> scl, std::array<float, 3> rot)
{
	UpdateModelMatrix(pos, scl, rot);
}

bool MeshObject::Load(ID3D11Device* device, std::string obj, std::string texture, std::array<float, 3> pos, std::array<float, 3> scl, std::array<float, 3> rot)
{
	bool success = true;

	//Load the obj-file
	if (LoadOBJ("ObjFiles/" + obj, device))
	{
		//LATER FIX: Read from mtl file which texture to use	***

		//Load in texture
		if (!LoadTextures("Textures/"+ texture, device))
		{
			std::cerr << "Failed to load texture..." << std::endl;
			success = false;
		}
	}
	else
	{
		std::cerr << "Failed to load file... Can only take '.obj'-files..." << std::endl;
		success = false;
	}

	//Load in the modell matrix
	SetupModelMatrix(pos, scl, rot);

	return success;
}

void MeshObject::UpdateModelMatrix(std::array<float, 3> pos, std::array<float, 3> scl, std::array<float, 3> rot)
{
	float degToRad = XM_PI / 180;
	XMStoreFloat4x4(&m_modelMatrix, XMMatrixScaling(scl[0], scl[1], scl[2]) *
									XMMatrixRotationRollPitchYaw(rot[0]*degToRad, rot[1]*degToRad, rot[2]*degToRad) *
									XMMatrixTranslation(pos[0], pos[1], pos[2]));
}

XMFLOAT4X4 MeshObject::GetModelMatrix() const
{
	return m_modelMatrix;
}

void MeshObject::Render(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->PSSetShaderResources(0, 1, &m_textureSRV);
	deviceContext->Draw(m_vertexCount, 0);
}
