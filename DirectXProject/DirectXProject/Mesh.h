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

class CHeightMapImage
{
public:
	CHeightMapImage(LPCTSTR pFileName, INT nWidth, INT nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage();
public:
	FLOAT GetHeight(FLOAT fx, FLOAT fz);											// Height Map 이미지에서 (x, z) 위치의 Pixel 값에 기반한 지형의 높이 반환
	XMFLOAT3 GetHeightMapNormal(INT x, INT z);										// Height Map 이미지에서 (x, z) 위치의 법선 벡터 반환
	XMFLOAT3 GetScale() { return m_xmf3Scale; }
	BYTE* GetHeightMapPixels() { return m_pHeightMapPixels; }
	INT GetHeightMapWidth() { return m_nWidth; }
	INT GetHeightMapLength() { return m_nLength; }

private:
	BYTE*						 m_pHeightMapPixels;								// Height Map 이미지 Pixel(8 Bit)들의 이차원 배열. 각 Pixel은 0~255의 값을 갖는다.
private:
	INT							 m_nWidth;											// Height Map 이미지의 가로 크기
	INT							 m_nLength;											// Height Map 이미지의 세로 크기
private:
	XMFLOAT3					 m_xmf3Scale;										// Height Map 이미지를 실제로 몇 배 확대하여 사용할 것인가를 나타내는 Scale 벡터
};

class CCubeMeshDiffused : public CMesh
{
public:
	CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth);
	virtual ~CCubeMeshDiffused();

};

class CHeightMapGridMesh : public CMesh
{
public:
	CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		INT xStart, INT zStart, INT nWidth, INT nLength, XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void* pContext = nullptr);
	virtual ~CHeightMapGridMesh();
public:
	XMFLOAT3 GetScale() { return m_xmf3Scale; }
	INT GetWidth() { return m_nWidth; }
	INT GetLength() { return m_nLength; }
public:
	virtual FLOAT OnGetHeight(INT x, INT z, void* pContext);						// 격자의 좌표가 (x, z)일 때 교점(Vertex)의 높이를 반환
	virtual XMFLOAT4 OnGetColor(INT x, INT z, void* pContext);						// 격자의 좌표가 (x, z)일 때 교점(Vertex)의 색상을 반환

protected:
	INT							 m_nWidth;											// 격자의 가로: x방향 크기
	INT							 m_nLength;											// 격자의 세로: z방향 크기
public:
	/*격자의 Scale(가로: x방향, 세로: z방향, 높이: y방향) 벡터. 실제 격자 Mesh의 각 Vertex의 x좌표, y좌표, z좌표는
	Scale 벡터의 x좌표, y좌표, z좌표로 곱한 값을 갖는다. 즉, 실제 격자의 x축 방향의 간격은 1이 아니라 스케일 벡터의 x좌표가 된다.
	이렇게 하면 작은 격자(적은 Vertex)를 사용하더라도 큰 크기의 격자(지형)를 생성할 수 있다.*/
	XMFLOAT3					 m_xmf3Scale;
};
