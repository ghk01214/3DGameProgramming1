#pragma once

class CVertex
{
public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() {}
protected:
	XMFLOAT3 m_xmf3Position;														// Vertex의 위치 벡터
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
	XMFLOAT4 m_xmf4Diffuse;															// Vertex의 색상
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
	ID3D12Resource*				 m_pd3dIndexBuffer{ nullptr };						// Index 버퍼 Interface 포인터(Vertex 버퍼(배열)에 대한 Index를 가짐)
	ID3D12Resource*				 m_pd3dIndexUploadBuffer{ nullptr };				// Index 버퍼 업로드 버퍼 Interface 포인터
	D3D12_INDEX_BUFFER_VIEW		 m_d3dIndexBufferView;
protected:
	UINT						 m_nIndices{ 0 };									// Index 버퍼에 포함되는 Index의 개수
	UINT						 m_nStartIndex{ 0 };								// Index 버퍼에서 Mesh를 그리기 위해 사용되는 시작 Index
	INT							 m_nBaseVertex{ 0 };								// Index 버퍼의 Index에 더해질 Index
public:
	BoundingOrientedBox			 m_xmBoundingBox;									// 모델 좌표계의 OOBB 바운딩 박스
};

class CCubeMeshDiffused : public CMesh
{
public:
	// 직육면체의 가로, 세로, 높이의 길이를 지정하여 직육면체 Mesh 생성
	CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth);
	virtual ~CCubeMeshDiffused();

};

class CFeverCubeMeshDiffused : public CCubeMeshDiffused
{
public:
	// 직육면체의 가로, 세로, 높이의 길이를 지정하여 직육면체 Mesh 생성
	CFeverCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth);
	virtual ~CFeverCubeMeshDiffused();
};

// 플레이어 및 다가오는 자동차의 Mesh
class CCarMeshDiffused : public CMesh
{
public:
	CCarMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, XMFLOAT4 xmf4Color);
	virtual ~CCarMeshDiffused();
};

// 플레이어와 다가오는 자동차 주변을 감싸고 있는 벽의 Mesh
class CWallMeshDiffused : public CMesh
{
public:
	CWallMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, FLOAT fColor);
	virtual ~CWallMeshDiffused();
};