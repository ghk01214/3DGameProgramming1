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

class CHeightMapImage
{
public:
	CHeightMapImage(LPCTSTR pFileName, INT nWidth, INT nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage();
public:
	FLOAT GetHeight(FLOAT fx, FLOAT fz);											// Height Map �̹������� (x, z) ��ġ�� Pixel ���� ����� ������ ���� ��ȯ
	XMFLOAT3 GetHeightMapNormal(INT x, INT z);										// Height Map �̹������� (x, z) ��ġ�� ���� ���� ��ȯ
	XMFLOAT3 GetScale() { return m_xmf3Scale; }
	BYTE* GetHeightMapPixels() { return m_pHeightMapPixels; }
	INT GetHeightMapWidth() { return m_nWidth; }
	INT GetHeightMapLength() { return m_nLength; }

private:
	BYTE*						 m_pHeightMapPixels;								// Height Map �̹��� Pixel(8 Bit)���� ������ �迭. �� Pixel�� 0~255�� ���� ���´�.
private:
	INT							 m_nWidth;											// Height Map �̹����� ���� ũ��
	INT							 m_nLength;											// Height Map �̹����� ���� ũ��
private:
	XMFLOAT3					 m_xmf3Scale;										// Height Map �̹����� ������ �� �� Ȯ���Ͽ� ����� ���ΰ��� ��Ÿ���� Scale ����
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
	virtual FLOAT OnGetHeight(INT x, INT z, void* pContext);						// ������ ��ǥ�� (x, z)�� �� ����(Vertex)�� ���̸� ��ȯ
	virtual XMFLOAT4 OnGetColor(INT x, INT z, void* pContext);						// ������ ��ǥ�� (x, z)�� �� ����(Vertex)�� ������ ��ȯ

protected:
	INT							 m_nWidth;											// ������ ����: x���� ũ��
	INT							 m_nLength;											// ������ ����: z���� ũ��
public:
	/*������ Scale(����: x����, ����: z����, ����: y����) ����. ���� ���� Mesh�� �� Vertex�� x��ǥ, y��ǥ, z��ǥ��
	Scale ������ x��ǥ, y��ǥ, z��ǥ�� ���� ���� ���´�. ��, ���� ������ x�� ������ ������ 1�� �ƴ϶� ������ ������ x��ǥ�� �ȴ�.
	�̷��� �ϸ� ���� ����(���� Vertex)�� ����ϴ��� ū ũ���� ����(����)�� ������ �� �ִ�.*/
	XMFLOAT3					 m_xmf3Scale;
};
