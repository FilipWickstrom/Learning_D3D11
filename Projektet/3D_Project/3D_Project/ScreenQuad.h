#pragma once
#include <d3d11.h>
#include "Structures.h"

class ScreenQuad
{
private:
	ID3D11Buffer* m_vertexBuffer;
	int m_nrOfVertices;

public:
	ScreenQuad();
	~ScreenQuad();

	bool Initialize(ID3D11Device* device, UINT winWidth, UINT winHeight);
	void RenderBuffer(ID3D11DeviceContext* deviceContext);
	int GetNrOfVertices() const;
};