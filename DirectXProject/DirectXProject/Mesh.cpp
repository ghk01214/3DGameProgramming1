#include "framework.h"
#include "Mesh.h"

CDiffusedVertex::CDiffusedVertex()
{
	m_xmf3Position		 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf4Diffuse		 = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

CDiffusedVertex::CDiffusedVertex(FLOAT x, FLOAT y, FLOAT z, XMFLOAT4 xmf4Diffuse)
{
	m_xmf3Position		 = XMFLOAT3(x, y, z);
	m_xmf4Diffuse		 = xmf4Diffuse;
}

CDiffusedVertex::CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse)
{
	m_xmf3Position		 = xmf3Position;
	m_xmf4Diffuse		 = xmf4Diffuse;
}

//==================================================================================================

CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CHAR* pstrFileName, BOOL bTextFile)
{
	if (pstrFileName)
		LoadMeshFromFile(pd3dDevice, pd3dCommandList, pstrFileName, bTextFile);
}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer)
		m_pd3dVertexBuffer->Release();

	if (m_pd3dVertexUploadBuffer)
		m_pd3dVertexUploadBuffer->Release();

	if (m_pd3dIndexBuffer)
		m_pd3dIndexBuffer->Release();

	if (m_pd3dIndexUploadBuffer)
		m_pd3dIndexUploadBuffer->Release();
}

void CMesh::Release()
{
	if (--m_nReferences <= 0)
		delete this;
}

void CMesh::ReleaseUploadBuffers()
{
	// Vertex ���۸� ���� ���ε� ���� �Ҹ�
	if (m_pd3dVertexUploadBuffer)
		m_pd3dVertexUploadBuffer->Release();

	if (m_pd3dIndexUploadBuffer)
		m_pd3dIndexUploadBuffer->Release();

	m_pd3dVertexUploadBuffer	 = nullptr;
	m_pd3dIndexUploadBuffer		 = nullptr;
}

void CMesh::LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CHAR* pstrFileName, BOOL bTextFile)
{
	char pstrToken[64] = { '\0' };

	if (bTextFile)
	{
		std::ifstream InFile{ pstrFileName };

		for (; ; )
		{
			InFile >> pstrToken;

			if (!InFile)
				break;

			if (!strcmp(pstrToken, "<Vertices>:"))
			{
				InFile >> m_nVertices;

				m_vxmf3Positions.reserve(m_nVertices);

				for (UINT i = 0; i < m_nVertices; i++)
				{
					m_vxmf3Positions.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));
					
					InFile >> m_vxmf3Positions[i].x >> m_vxmf3Positions[i].y >> m_vxmf3Positions[i].z;
				}
			}
			else if (!strcmp(pstrToken, "<Normals>:"))
			{
				InFile >> pstrToken;
			
				m_vxmf3Normals.reserve(m_nVertices);
				
				for (UINT i = 0; i < m_nVertices; i++)
				{
					m_vxmf3Normals.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));

					InFile >> m_vxmf3Normals[i].x >> m_vxmf3Normals[i].y >> m_vxmf3Normals[i].z;
				}
			}
			else if (!strcmp(pstrToken, "<TextureCoords>:"))
			{
				InFile >> pstrToken;
				
				m_vxmf2TextureCoords.reserve(m_nVertices);
				
				for (UINT i = 0; i < m_nVertices; i++)
				{
					m_vxmf2TextureCoords.push_back(XMFLOAT2(0.0f, 0.0f));

					InFile >> m_vxmf2TextureCoords[i].x >> m_vxmf2TextureCoords[i].y;
				}
			}
			else if (!strcmp(pstrToken, "<Indices>:"))
			{
				InFile >> m_nIndices;
				
				m_vnIndices.reserve(m_nIndices);
				
				for (UINT i = 0; i < m_nIndices; i++)
				{
					m_vnIndices.push_back(0);

					InFile >> m_vnIndices[i];
				}
			}
		}
	}
	else
	{
		FILE* pFile{ nullptr };
		::fopen_s(&pFile, pstrFileName, "rb");
		::rewind(pFile);

		char pstrToken[64] = { '\0' };

		BYTE nStrLength = 0;
		UINT nReads = 0;

		nReads = static_cast<UINT>(::fread(&nStrLength, sizeof(BYTE), 1, pFile));
		nReads = static_cast<UINT>(::fread(pstrToken, sizeof(CHAR), 14, pFile)); //"<BoundingBox>:"
		nReads = static_cast<UINT>(::fread(&m_xmBoundingBox.Center, sizeof(FLOAT), 3, pFile));
		nReads = static_cast<UINT>(::fread(&m_xmBoundingBox.Extents, sizeof(FLOAT), 3, pFile));

		nReads = static_cast<UINT>(::fread(&nStrLength, sizeof(BYTE), 1, pFile));
		nReads = static_cast<UINT>(::fread(pstrToken, sizeof(CHAR), 11, pFile)); //"<Vertices>:"
		nReads = static_cast<UINT>(::fread(&m_nVertices, sizeof(INT), 1, pFile));
		
		m_vxmf3Positions.reserve(m_nVertices);

		for (INT i = 0; i < m_nVertices; ++i)
		{
			m_vxmf3Positions.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));
		}

		nReads = static_cast<UINT>(::fread(&(*m_vxmf3Positions.data()), sizeof(FLOAT), 3 * m_nVertices, pFile));

		nReads = static_cast<UINT>(::fread(&nStrLength, sizeof(BYTE), 1, pFile));
		nReads = static_cast<UINT>(::fread(pstrToken, sizeof(CHAR), 10, pFile)); //"<Normals>:"
		nReads = static_cast<UINT>(::fread(&m_nVertices, sizeof(INT), 1, pFile));
		
		m_vxmf3Normals.reserve(m_nVertices);
		
		for (UINT i = 0; i < m_nVertices; i++)
		{
			m_vxmf3Normals.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));
		}

		nReads = static_cast<UINT>(::fread(&(*m_vxmf3Normals.data()), sizeof(FLOAT), 3 * m_nVertices, pFile));

		nReads = static_cast<UINT>(::fread(&nStrLength, sizeof(BYTE), 1, pFile));
		nReads = static_cast<UINT>(::fread(pstrToken, sizeof(CHAR), 16, pFile)); //"<TextureCoords>:"
		nReads = static_cast<UINT>(::fread(&m_nVertices, sizeof(INT), 1, pFile));
		
		m_vxmf2TextureCoords.reserve(m_nVertices);

		for (UINT i = 0; i < m_nVertices; i++)
		{
			m_vxmf2TextureCoords.push_back(XMFLOAT2(0.0f, 0.0f));
		}

		nReads = static_cast<UINT>(::fread(&(*m_vxmf2TextureCoords.data()), sizeof(FLOAT), 2 * m_nVertices, pFile));

		nReads = static_cast<UINT>(::fread(&nStrLength, sizeof(BYTE), 1, pFile));
		nReads = static_cast<UINT>(::fread(pstrToken, sizeof(CHAR), 10, pFile)); //"<Indices>:"
		nReads = static_cast<UINT>(::fread(&m_nIndices, sizeof(INT), 1, pFile));
		
		m_vnIndices.reserve(m_nIndices);

		for (UINT i = 0; i < m_nIndices; i++)
		{
			m_vnIndices.push_back(0);
		}

		nReads = static_cast<UINT>(::fread(&(*m_vnIndices.data()), sizeof(UINT), m_nIndices, pFile));

		::fclose(pFile);
	}

	m_pd3dPositionBuffer	 = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &(*m_vxmf3Positions.data()), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	m_pd3dNormalBuffer		 = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &(*m_vxmf3Normals.data()), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);
	m_pd3dTextureCoordBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &(*m_vxmf2TextureCoords.data()), sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoordUploadBuffer);

	m_nVertexBufferViews = 3;
	m_vd3dVertexBufferViews.reserve(m_nVertexBufferViews);

	for (INT i = 0; i < m_nVertexBufferViews; ++i)
	{
		D3D12_VERTEX_BUFFER_VIEW temp{};
		m_vd3dVertexBufferViews.push_back(temp);
	}

	m_vd3dVertexBufferViews[0].BufferLocation	 = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_vd3dVertexBufferViews[0].StrideInBytes	 = sizeof(XMFLOAT3);
	m_vd3dVertexBufferViews[0].SizeInBytes		 = sizeof(XMFLOAT3) * m_nVertices;

	m_vd3dVertexBufferViews[1].BufferLocation	 = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_vd3dVertexBufferViews[1].StrideInBytes	 = sizeof(XMFLOAT3);
	m_vd3dVertexBufferViews[1].SizeInBytes		 = sizeof(XMFLOAT3) * m_nVertices;

	m_vd3dVertexBufferViews[2].BufferLocation	 = m_pd3dTextureCoordBuffer->GetGPUVirtualAddress();
	m_vd3dVertexBufferViews[2].StrideInBytes	 = sizeof(XMFLOAT2);
	m_vd3dVertexBufferViews[2].SizeInBytes		 = sizeof(XMFLOAT2) * m_nVertices;

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &(*m_vnIndices.data()), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation			 = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format					 = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes			 = sizeof(UINT) * m_nIndices;
}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, m_nVertexBufferViews, &m_d3dVertexBufferView);

	if (m_pd3dIndexBuffer)
	{
		// Index ���۰� ������ Index ���۸� Pipeline(IA: �Է� ������)�� �����ϰ� �ε����� ����Ͽ� Rendering
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);			// Mesh�� Vertex ���� �並 Rendering(Pipeline(�Է� ������)�� �۵��ϰ� �Ѵ�)
}

//========================================================================================================================================

CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, INT nWidth, INT nLength, XMFLOAT3 xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	BYTE* pHeightMapPixels{ new BYTE[m_nWidth * m_nLength] };

	// ������ ���� �д´�. Height Map �̹����� ���� Header�� ���� RAW �̹����̴�.
	HANDLE	 hFile{ ::CreateFile(pFileName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, nullptr) };
	DWORD	 dwBytesRead;

	::ReadFile(hFile, pHeightMapPixels, (m_nWidth * m_nLength), &dwBytesRead, nullptr);
	::CloseHandle(hFile);

	/*�̹����� y��� ������ z���� ������ �ݴ��̹Ƿ� �̹����� ���ϴ�Ī ���� ����.
	�׷��� �̹����� ��ǥ��� ������ ��ǥ���� ������ ��ġ�ϰ� �ȴ�.*/
	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];

	for (INT y = 0; y < m_nLength; ++y)
	{
		for (INT x = 0; x < m_nWidth; ++x)
		{
			m_pHeightMapPixels[x + ((m_nLength - 1 - y) * m_nWidth)] = pHeightMapPixels[x + (y * m_nWidth)];
		}
	}

	if (pHeightMapPixels)
		delete[] pHeightMapPixels;
}

CHeightMapImage::~CHeightMapImage()
{
	if (m_pHeightMapPixels)
		delete[] m_pHeightMapPixels;

	m_pHeightMapPixels = nullptr;
}

FLOAT CHeightMapImage::GetHeight(FLOAT fx, FLOAT fz)
{
	// ������ ��ǥ (fx, fz)�� �̹��� ��ǥ���̴�. Height Map�� x��ǥ�� z��ǥ�� Height Map�� ������ ����� ������ ���̴� 0�̴�.
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength))
		return 0.0f;

	// Height Map�� ��ǥ�� ���� �κа� �Ҽ� �κ��� ���
	INT		 x{ static_cast<INT>(fx) };
	INT		 z{ static_cast<INT>(fz) };
	FLOAT	 fxPercent{ fx - x };
	FLOAT	 fzPercent{ fz - z };
	FLOAT	 fBottomLeft{ static_cast<FLOAT>(m_pHeightMapPixels[x + (z * m_nWidth)]) };
	FLOAT	 fBottomRight{ static_cast<FLOAT>(m_pHeightMapPixels[(x + 1) + (z * m_nWidth)]) };
	FLOAT	 fTopLeft{ static_cast<FLOAT>(m_pHeightMapPixels[x + ((z + 1) * m_nWidth)]) };
	FLOAT	 fTopRight{ static_cast<FLOAT>(m_pHeightMapPixels[(x + 1) + ((z + 1) * m_nWidth)]) };

#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	// z��ǥ�� 1, 3, 5, ...�� ��� Index�� �����ʿ��� �������� �����ȴ�.
	BOOL bRightToLeft = ((z % 2) != 0);
	if (bRightToLeft)
	{
		/*������ �ﰢ������ �����ʿ��� ���� �������� �����Ǵ� ����̴�.
		���� �׸��� �������� (fzPercent < fxPercent) �� ����̴�.
		�� ��� TopLeft�� Pixel ���� (fTopLeft = fTopRight + (fBottomLeft - fBottomRight))�� �ٻ��Ѵ�.
		���� �׸��� ������ (fzPercent �� fxPercent)�� ����̴�.
		�� ��� BottomRight�� Pixel ���� (fBottomRight = fBottomLeft + (fTopRight - fTopLeft))�� �ٻ��Ѵ�.*/
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		/*������ �ﰢ������ ���ʿ��� ������ �������� �����Ǵ� ����̴�.
		���� �׸��� ������ (fzPercent < (1.0f - fxPercent))�� ����̴�.
		�� ��� TopRight�� Pixel ���� (fTopRight = fTopLeft + (fBottomRight - fBottomLeft))�� �ٻ��Ѵ�.
		���� �׸��� �������� (fzPercent �� (1.0f - fxPercent))�� ����̴�.
		�� ��� BottomLeft�� Pixel ���� (fBottomLeft = fTopLeft + (fBottomRight - fTopRight))�� �ٻ��Ѵ�.*/
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif

	// �簢���� �� ���� �����Ͽ� ����(Pixel ��) ���
	FLOAT fTopHeight{ fTopLeft * (1 - fxPercent) + fTopRight * fxPercent };
	FLOAT fBottomHeight{ fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent };
	FLOAT fHeight{ fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent };

	return fHeight;
}

XMFLOAT3 CHeightMapImage::GetHeightMapNormal(INT x, INT z)
{
	// x��ǥ�� z��ǥ�� Height Map�� ������ ����� ������ ���� ���ʹ� y�� ���� ����
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength))
		return XMFLOAT3(0.0f, 1.0f, 0.0f);

	// Height Map���� (x, z) ��ǥ�� Pixel ���� ������ �� ���� �� (x + 1, z), (z, z + 1)�� ���� Pixel ���� ����Ͽ� ���� ���� ���
	INT			 nHeightMapIndex{ x + (z * m_nWidth) };
	INT			 xHeightMapAdd{ (x < (m_nWidth - 1)) ? 1 : -1 };
	INT			 zHeightMapAdd{ (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth };

	// (x, z), (x + 1, z), (z, z + 1)�� Pixel���� ������ ���� ���
	FLOAT		 y1{ static_cast<FLOAT>(m_pHeightMapPixels[nHeightMapIndex]) * m_xmf3Scale.y };
	FLOAT		 y2{ static_cast<FLOAT>(m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd]) * m_xmf3Scale.y };
	FLOAT		 y3{ static_cast<FLOAT>(m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd]) * m_xmf3Scale.y };

	// xmf3Edge1�� (0, y3, m_xmf3Scale.z) - (0, y1, 0) �����̴�.
	XMFLOAT3	 xmf3Edge1{ XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z) };

	// xmf3Edge2�� (m_xmf3Scale.x, y2, 0) - (0, y1, 0) �����̴�.
	XMFLOAT3	 xmf3Edge2{ XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f) };

	// ���� ���ʹ� xmf3Edge1�� xmf3Edge2�� ������ ����ȭ�ϸ� �ȴ�.
	XMFLOAT3	 xmf3Normal{ Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, TRUE) };

	return xmf3Normal;
}


//========================================================================================================================================

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth)
{
	// ������ü�� ������(Vertex)�� 8��
	m_nVertices				 = 8;
	m_nStride				 = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology	 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FLOAT fx{ fWidth * 0.5f };	 	// fWidth: ������ü ����(x-��) ����
	FLOAT fy{ fHeight * 0.5f };		// fHeight: ������ü ����(y-��) ����
	FLOAT fz{ fDepth * 0.5f };		// fDepth: ������ü ����(z-��) ����

	// Vertex ���۴� ������ü�� ������ 8���� ���� Vertex �����͸� ������.
	std::vector<CDiffusedVertex> vVertices;
	vVertices.reserve(8);

	vVertices.emplace_back(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	vVertices.emplace_back(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	vVertices.emplace_back(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	vVertices.emplace_back(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	vVertices.emplace_back(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	vVertices.emplace_back(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	vVertices.emplace_back(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	vVertices.emplace_back(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &(*vVertices.data()), m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	
	// Vertex ���� �並 �����Ѵ�.
	m_d3dVertexBufferView.BufferLocation	 = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		 = m_nStride;
	m_d3dVertexBufferView.SizeInBytes		 = m_nStride * m_nVertices;

	// Index ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. �ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���̹Ƿ�
	// �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� Vertex�� �ʿ�. ��, �ε��� ���۴� ��ü 36(=6*2*3)���� �ε����� ������ �Ѵ�
	m_nIndices = 36;

	std::vector<UINT> vnIndices;
	vnIndices.reserve(m_nIndices);
	
	// ����(Top) �簢��
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(3);

	// �Ʒ���(Bottom) �簢��
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(6);

	// �ո�(Front) �簢��
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(2);

	// �޸�(Back) �簢��
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(0);
	
	// ����(Left) �簢��
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(3);
	
	// ����(Right) �簢��
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(1);
	
	// Index ���� ����
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &(*vnIndices.data()), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	
	// Index ���� View ����
	m_d3dIndexBufferView.BufferLocation		 = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format				 = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes		 = sizeof(UINT) * m_nIndices;

	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	vVertices.clear();
	vnIndices.clear();

	vVertices.shrink_to_fit();
	vnIndices.shrink_to_fit();
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}

//========================================================================================================================================

CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, INT xStart, INT zStart, INT nWidth, INT nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void* pContext)
{
	// ������ ����(Vertex)�� ������ (nWidth * nLength)
	m_nVertices				 = nWidth * nLength;
	m_nStride				 = sizeof(CDiffusedVertex);

	// ���ڴ� �ﰢ�� ��Ʈ������ ����
	m_d3dPrimitiveTopology	 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_nWidth				 = nWidth;
	m_nLength				 = nLength;
	m_xmf3Scale				 = xmf3Scale;

	std::vector<CDiffusedVertex> vVertices;
	vVertices.reserve(m_nVertices);

	/*xStart�� zStart�� ������ ���� ��ġ(x��ǥ�� z��ǥ)�� ��Ÿ����. Ŀ�ٶ� ������ ���ڵ��� ������ �迭�� ����
	�ʿ䰡 �ֱ� ������ ��ü �������� �� ������ ���� ��ġ�� ��Ÿ���� ������ �ʿ��ϴ�.*/
	FLOAT fHeight{ 0.0f };
	FLOAT fMinHeight{ +FLT_MAX };
	FLOAT fMaxHeight{ -FLT_MAX };

	for (INT z = zStart; z < (zStart + nLength); ++z)
	{
		for (INT x = xStart; x < (xStart + nWidth); ++x)
		{
			//Vertex�� ���̿� ������ Height Map���κ��� ���Ѵ�.
			XMFLOAT3 xmf3Position{ XMFLOAT3((x * m_xmf3Scale.x), OnGetHeight(x, z, pContext), (z * m_xmf3Scale.z)) };
			XMFLOAT4 xmf3Color{ Vector4::Add(OnGetColor(x, z, pContext), xmf4Color) };

			vVertices.emplace_back(xmf3Position, xmf3Color);

			if (fHeight < fMinHeight)
				fMinHeight = fHeight;

			if (fHeight > fMaxHeight)
				fMaxHeight = fHeight;
		}
	}

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &(*vVertices.data()), m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation	 = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		 = m_nStride;
	m_d3dVertexBufferView.SizeInBytes		 = m_nStride * m_nVertices;

	m_nIndices = ((nWidth * 2) * (nLength - 1)) + ((nLength - 1) - 1);

	//UINT* pnIndices{ new UINT[m_nIndices] };
	std::vector<UINT> vnIndices;
	vnIndices.reserve(m_nIndices);

	for (INT z = 0; z < nLength - 1; ++z)
	{
		if ((z % 2) == 0)
		{
			// Ȧ�� ��° ���̹Ƿ�(z = 0, 2, 4, ...) Index�� ���� ������ ���ʿ��� ������ ����
			for (INT x = 0; x < nWidth; ++x)
			{
				// ù ��° ���� �����ϰ� ���� �ٲ� ������(x == 0) ù ��° Index �߰�
				if ((x == 0) && (z > 0))
					vnIndices.push_back(static_cast<UINT>(x + (z * nWidth)));

				// �Ʒ�(x, z), ��(x, z + 1)�� ������ Index �߰�
				vnIndices.push_back(static_cast<UINT>(x + (z * nWidth)));
				vnIndices.push_back(static_cast<UINT>((x + (z * nWidth)) + nWidth));
			}
		}
		else
		{
			// ¦�� ��° ���̹Ƿ�(z = 1, 3, 5, ...) Index�� ���� ������ �����ʿ��� ���� ����
			for (INT x = nWidth - 1; x >= 0; --x)
			{
				// ���� �ٲ� ������(x == (nWidth - 1)) ù ��° Index �߰�
				if (x == (nWidth - 1))
					vnIndices.push_back(static_cast<UINT>(x + (z * nWidth)));

				// �Ʒ�(x, z), ��(x, z + 1)�� ������ Index �߰�
				vnIndices.push_back(static_cast<UINT>(x + (z * nWidth)));
				vnIndices.push_back(static_cast<UINT>((x + (z * nWidth)) + nWidth));
			}
		}
	}

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &(*vnIndices.data()), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	
	m_d3dIndexBufferView.BufferLocation	 = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format			 = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes	 = sizeof(UINT) * m_nIndices;

	vVertices.clear();
	vnIndices.clear();

	vVertices.shrink_to_fit();
	vnIndices.shrink_to_fit();
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

// Height Map �̹����� Pixel ���� ������ ���̷� ��ȯ
FLOAT CHeightMapGridMesh::OnGetHeight(INT x, INT z, void* pContext)
{
	CHeightMapImage*	 pHeightMapImage{ static_cast<CHeightMapImage*>(pContext) };
	BYTE*				 pHeightMapPixels{ pHeightMapImage->GetHeightMapPixels() };
	XMFLOAT3			 xmf3Scale{ pHeightMapImage->GetScale() };
	INT					 nWidth{ pHeightMapImage->GetHeightMapWidth() };
	FLOAT				 fHeight{ pHeightMapPixels[x + (z * nWidth)] * xmf3Scale.y };

	return fHeight;
}

XMFLOAT4 CHeightMapGridMesh::OnGetColor(INT x, INT z, void* pContext)
{
	// ������ ���� ����(Vertex���� ��������� ����)
	XMFLOAT3			 xmf3LightDirection{ XMFLOAT3(-1.0f, 1.0f, 1.0f) };
	xmf3LightDirection = Vector3::Normalize(xmf3LightDirection);

	CHeightMapImage*	 pHeightMapImage{ static_cast<CHeightMapImage*>(pContext) };
	XMFLOAT3			 xmf3Scale{ pHeightMapImage->GetScale() };

	// ������ ����(����, ���)
	XMFLOAT4			 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);
	/*Vertex (x, z)���� ������ �ݻ�Ǵ� ��(����)�� Vertex (x, z)�� ���� ���Ϳ� ������ ���� ������ ����(cos)�� ������
	3���� Vertex (x+1, z), (x, z+1), (x+1, z+1)�� ���� ���Ϳ� ������ ���� ������ ������ ����Ͽ� ���Ѵ�.
	Vertex (x, z)�� ������ ���� ����(����)�� �ݻ�Ǵ� ��(����)�� ���� ���̴�.*/
	FLOAT				 fScale{ Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z), xmf3LightDirection) };

	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z + 1), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z + 1), xmf3LightDirection);
	fScale = (fScale / 4.0f) + 0.05f;

	if (fScale > 1.0f)
		fScale = 1.0f;

	if (fScale < 0.25f)
		fScale = 0.25f;

	// fScale�� ���� ����(���)�� �ݻ�Ǵ� ����
	XMFLOAT4			 xmf4Color{ Vector4::Multiply(fScale, xmf4IncidentLightColor) };

	return xmf4Color;
}