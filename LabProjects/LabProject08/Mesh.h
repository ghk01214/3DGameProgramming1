#pragma once

// ���� ǥ�� Ŭ����
class CVertex
{
public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() {}
protected:
	XMFLOAT3 m_xmf3Position;														// ������ ��ġ ����(��� ������ �ּ��� ��ġ ���͸� ������ �Ѵ�)
};

class CDiffusedVertex : public CVertex
{
public:
	CDiffusedVertex();
	CDiffusedVertex(FLOAT x, FLOAT y, FLOAT z, XMFLOAT4 xmf4Diffuse);
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse);
	~CDiffusedVertex() {}
protected:
	XMFLOAT4 m_xmf4Diffuse;															// ������ ����
};

class CMesh
{
public:
	CMesh() {}
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMesh();
public:
	void AddReference() { ++m_nReferences; }
	void Release();
	void ReleaseUploadBuffers();
public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

private:
	int							 m_nReferences{ 0 };
protected:
	ID3D12Resource*				 m_pd3dVertexBuffer{ nullptr };
	ID3D12Resource*				 m_pd3dVertexUploadBuffer{ nullptr };
protected:
	D3D12_VERTEX_BUFFER_VIEW	 m_d3dVertexBufferView;
protected:
	D3D12_PRIMITIVE_TOPOLOGY	 m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT						 m_nSlot{ 0 };
	UINT						 m_nVertices{ 0 };
	UINT						 m_nStride{ 0 };
	UINT						 m_nOffset{ 0 };
};

class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CTriangleMesh() { }
};

class CCubeMeshDiffused : public CMesh
{
public:
	// ������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü Mesh ����
	CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth);
	virtual ~CCubeMeshDiffused();
};