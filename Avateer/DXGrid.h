#pragma once
class DXGrid
{
public:
	DXGrid();
	void Initialise(ID3D11Device *device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);
	int IndexCount() { return m_indexCount; };

private:
	int m_vertexCount;
	int m_indexCount;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	ID3D11RasterizerState * _pRasterState;
};

