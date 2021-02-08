#include "ScreenQuad.h"

ScreenQuad::ScreenQuad()
{
	m_vertexBuffer = nullptr;
	m_nrOfVertices = 6;
}

ScreenQuad::~ScreenQuad()
{
	if (m_vertexBuffer)
		m_vertexBuffer->Release();
}

bool ScreenQuad::Initialize(ID3D11Device* device, UINT winWidth, UINT winHeight)
{
	//Screen window size is [1280 x 720] in my example. The middle of the screen is (0,0)

	//Screen coordinates of the sides for the window. 
	float right = (float)(winWidth / 2);	//[ 640 ]
	float left = right - winWidth;			//[-640 ]
	float top = (float)(winHeight / 2);		//[ 360 ]
	float bottom = top - winHeight;			//[-360 ]

	ScreenVertex quad[] =
	{
		//First triangle
		{ {left, bottom, 0.0f},  {0.0f, 1.0f} },	//Bottom left
		{ {left, top, 0.0f},	 {0.0f, 0.0f} },	//Top left
		{ {right, bottom, 0.0f}, {1.0f, 1.0f} },	//Bottom right
		//Second triangle
		{ {left, top, 0.0f},	 {0.0f, 0.0f} },	//Top left
		{ {right, top, 0.0f},	 {1.0f, 0.0f} },	//Top right
		{ {right, bottom, 0.0f}, {1.0f, 1.0f} }		//Bottom right
	};
	
	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(quad);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = quad;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &data, &m_vertexBuffer);
	return !FAILED(hr);
}

void ScreenQuad::RenderBuffer(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(ScreenVertex);
	UINT offset = 0;

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->Draw(m_nrOfVertices, 0);
}

int ScreenQuad::GetNrOfVertices() const
{
	return m_nrOfVertices;
}
