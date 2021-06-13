#pragma once

class CVertex
{
public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() {}
protected:
	XMFLOAT3 m_xmf3Position;														// Vertex�� ��ġ ����
};

class CDiffusedVertex : public CVertex
{
public:
	CDiffusedVertex();
	CDiffusedVertex(FLOAT x, FLOAT y, FLOAT z, XMFLOAT4 xmf4Diffuse);
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse);
	~CDiffusedVertex() {}
public:
	XMFLOAT3 GetPosition() { return m_xmf3Position; }
protected:
	XMFLOAT4 m_xmf4Diffuse;															// Vertex�� ����
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
	BoundingOrientedBox GetBoundingBox() { return m_xmBoundingBox; }
public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

private:
	INT							 m_nReferences{ 0 };
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
	std::vector<CDiffusedVertex> m_vVertices;
protected:
	ID3D12Resource*				 m_pd3dIndexBuffer{ nullptr };						// Index ���� Interface ������(Vertex ����(�迭)�� ���� Index�� ����)
	ID3D12Resource*				 m_pd3dIndexUploadBuffer{ nullptr };				// Index ���� ���ε� ���� Interface ������
	D3D12_INDEX_BUFFER_VIEW		 m_d3dIndexBufferView;
protected:
	UINT						 m_nIndices{ 0 };									// Index ���ۿ� ���ԵǴ� Index�� ����
	UINT						 m_nStartIndex{ 0 };								// Index ���ۿ��� Mesh�� �׸��� ���� ���Ǵ� ���� Index
	INT							 m_nBaseVertex{ 0 };								// Index ������ Index�� ������ Index
public:
	BoundingOrientedBox			 m_xmBoundingBox;									// �� ��ǥ���� OOBB �ٿ�� �ڽ�
};

class CCubeMeshDiffused : public CMesh
{
public:
	// ������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü Mesh ����
	CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth);
	virtual ~CCubeMeshDiffused();

};

class CFeverCubeMeshDiffused : public CCubeMeshDiffused
{
public:
	// ������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü Mesh ����
	CFeverCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth);
	virtual ~CFeverCubeMeshDiffused();
};

// �÷��̾� �� �ٰ����� �ڵ����� Mesh
class CCarMeshDiffused : public CMesh
{
public:
	CCarMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, XMFLOAT4 xmf4Color);
	virtual ~CCarMeshDiffused();
};

// �÷��̾�� �ٰ����� �ڵ��� �ֺ��� ���ΰ� �ִ� ���� Mesh
class CWallMeshDiffused : public CMesh
{
public:
	CWallMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, FLOAT fColor);
	virtual ~CWallMeshDiffused();
};