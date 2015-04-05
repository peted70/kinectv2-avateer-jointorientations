#include "pch.h"
#include "DXGrid.h"
#include "Content\ShaderStructures.h"

using namespace Avateer;

DXGrid::DXGrid()
{


}

void DXGrid::Initialise(ID3D11Device *device)
{
	int gridWidth = 20;
	float cellWidth = 5.0f;
	float cellHeight = 5.0f;

	VertexPositionColor *vertices;
	unsigned long* indices;
	int index, i, j;
	float positionX, positionZ;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	int num = (gridWidth + 1) / 2;
	
	int numInRow = (num * 2 + 1);

	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = numInRow * numInRow;

	// Set the index count to the same as the vertex count.
	m_indexCount = numInRow * 4;

	// Create the vertex array.
	vertices = new VertexPositionColor[m_vertexCount];

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	index = 0;

	for (int i = -num; i <= num; i++)
	{
		for (int j = -num; j <= num; j++)
		{
			vertices[index].pos = XMFLOAT3(cellWidth * i, 0.0f, cellHeight * j);
			vertices[index].color = XMFLOAT3(0.4f, 0.4f, 0.4f);
			index++;
		}
	}

	index = 0;

	// first one direction...
	for (int k = 0; k < numInRow; k++)
	{
		indices[index] = k;
		index++;
		indices[index] = k + (numInRow * (numInRow - 1));
		index++;
	}

	// then the other...
	for (int k = 0; k < numInRow * numInRow; k+=numInRow)
	{
		indices[index] = k;
		index++;
		indices[index] = k + numInRow - 1;
		index++;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexPositionColor) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return;
	}

	D3D11_RASTERIZER_DESC rasterizerState;
	rasterizerState.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerState.CullMode = D3D11_CULL_NONE;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = false;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = false;
	device->CreateRasterizerState(&rasterizerState, &_pRasterState);

	// Release the arrays now that the buffers have been created and loaded.
	delete [] vertices;
	delete [] indices;
}

void DXGrid::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void DXGrid::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexPositionColor);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the render format to line list.
	// Set the type of primitive that should be rendered from this vertex buffer, in this case a line list.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	deviceContext->RSSetState(_pRasterState);
	return;
}

