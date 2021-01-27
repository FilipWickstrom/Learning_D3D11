#include "MeshObject.h"

MeshObject::MeshObject()
	:m_vertexCount(0), m_indexCount(0)//, m_filename(filename)
{
	this->m_vertexBuffer = nullptr;
	this->m_indexBuffer = nullptr;
}

MeshObject::~MeshObject()
{
	if (this->m_vertexBuffer != nullptr)
		this->m_vertexBuffer->Release();
	if (this->m_indexBuffer != nullptr)
		this->m_indexBuffer->Release();
}

bool MeshObject::Load(ID3D11Device* device)
{
	//Load in the model from the file
	//IF CAN OPEN FILE - GOOD. ELSE - RETURN FALSE

	//TEMP
	SimpleVertex meshVertices[] =
	{
		{ {-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },    //Down left
		{ {-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },    //Up left
		{ { 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },     //Down right
		{ { 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }	  //Up right
	};

	unsigned short meshIndices[] =
	{
		0, 1, 2,
		2, 1, 3
	};
	m_vertexCount = sizeof(meshVertices) / sizeof(SimpleVertex);
	m_indexCount = sizeof(meshIndices) / sizeof(unsigned short);

	//Vertex buffer - setting up description
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(SimpleVertex) * m_vertexCount;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;	//GPU - change to default??
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = meshVertices;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer)))
	{
		std::cerr << "Failed to create vertex buffer" << std::endl;
		return false;
	}

	//Index buffer
	bufferDesc.ByteWidth = sizeof(unsigned short) * m_indexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	initData.pSysMem = meshIndices;

	if (FAILED(device->CreateBuffer(&bufferDesc, &initData, &m_indexBuffer)))
	{
		std::cerr << "Failed to create index buffer" << std::endl;
		return false;
	}

	return true;
}

void MeshObject::Render(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->DrawIndexed(m_indexCount, 0, 0);
}
