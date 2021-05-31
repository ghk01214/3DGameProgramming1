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

//========================================================================================================================================

CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
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

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);			// Mesh�� Primitive ������ ����
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);	// Mesh�� Vertex ���� �並 ����

	if (m_pd3dIndexBuffer)
	{
		// Index ���۰� ������ Index ���۸� Pipeline(IA: �Է� ������)�� �����ϰ� Index�� ����Ͽ� Rendering
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);			// Mesh�� Vertex ���� �並 Rendering(Pipeline(�Է� ������)�� �۵��ϰ� �Ѵ�)
}

//========================================================================================================================================

CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, INT nWidth, INT nLength, XMFLOAT3 xmf3Scale)
{
	m_nWidth		 = nWidth;
	m_nLength		 = nLength;
	m_xmf3Scale		 = xmf3Scale;

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
	INT		 x{ (INT)fx};
	INT		 z{ (INT)fz};
	FLOAT	 fxPercent{ fx - x };
	FLOAT	 fzPercent{ fz - z };
	FLOAT	 fBottomLeft{ (FLOAT)m_pHeightMapPixels[x + (z * m_nWidth)] };
	FLOAT	 fBottomRight{ (FLOAT)m_pHeightMapPixels[(x + 1) + (z * m_nWidth)] };
	FLOAT	 fTopLeft{ (FLOAT)m_pHeightMapPixels[x + ((z + 1) * m_nWidth)] };
	FLOAT	 fTopRight{ (FLOAT)m_pHeightMapPixels[(x + 1) + ((z + 1) * m_nWidth)] };

#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	// z��ǥ�� 1, 3, 5, ...�� ��� Index�� �����ʿ��� �������� �����ȴ�.
	bool bRightToLeft = ((z % 2) != 0);
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
	FLOAT		 y1{ (FLOAT)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y };
	FLOAT		 y2{ (FLOAT)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y };
	FLOAT		 y3{ (FLOAT)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y };

	// xmf3Edge1�� (0, y3, m_xmf3Scale.z) - (0, y1, 0) �����̴�.
	XMFLOAT3	 xmf3Edge1{ XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z) };

	// xmf3Edge2�� (m_xmf3Scale.x, y2, 0) - (0, y1, 0) �����̴�.
	XMFLOAT3	 xmf3Edge2{ XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f) };

	// ���� ���ʹ� xmf3Edge1�� xmf3Edge2�� ������ ����ȭ�ϸ� �ȴ�.
	XMFLOAT3	 xmf3Normal{ Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, TRUE) };

	return xmf3Normal;
}

//========================================================================================================================================

CTriangleMesh::CTriangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
	// �ﰢ�� Mesh ����
	m_nVertices								 = 3;
	m_nStride								 = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology					 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*Vertex(�ﰢ���� ������)�� ������ �ð���� ������� ������, ���, �Ķ������� ����. RGBA(Red, Green, Blue, Alpha)
	4���� �Ķ���͸� ����Ͽ� ���� ǥ��. �� �Ķ���ʹ� 0.0~1.0 ������ �Ǽ����� ������.*/
	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));

	// �ﰢ�� Mesh ���ҽ�(Vertex ����)�� ����
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// Vertex ���� �� ����
	m_d3dVertexBufferView.BufferLocation	 = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		 = m_nStride;
	m_d3dVertexBufferView.SizeInBytes		 = m_nStride * m_nVertices;
}

//========================================================================================================================================

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth)
{
	// ������ü�� ������(Vertex)�� 8��
	m_nVertices				 = 8;
	m_nStride				 = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology	 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FLOAT fx{ fWidth * 0.5f };	 	// fWidth: ������ü ����(x��) ����
	FLOAT fy{ fHeight * 0.5f };		// fHeight: ������ü ����(y��) ����
	FLOAT fz{ fDepth * 0.5f };		// fDepth: ������ü ����(z��) ����

	// Vertex ���۴� ������ü�� ������ 8���� ���� Vertex �����͸� ������.
	CDiffusedVertex pVertices[8];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	
	// Vertex ���� �並 �����Ѵ�.
	m_d3dVertexBufferView.BufferLocation	 = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		 = m_nStride;
	m_d3dVertexBufferView.SizeInBytes		 = m_nStride * m_nVertices;

	// Index ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. �ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���̹Ƿ�
	// �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� Vertex�� �ʿ�. ��, Index ���۴� ��ü 36(=6*2*3)���� Index�� ������ �Ѵ�
	m_nIndices = 36;

	UINT pnIndices[36];
	
	// �ո�(Front) �簢���� ���� �ﰢ��
	pnIndices[0] = 3;
	pnIndices[1] = 1;
	pnIndices[2] = 0;
	
	// �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	pnIndices[3] = 2;
	pnIndices[4] = 1;
	pnIndices[5] = 3;
	
	// ����(Top) �簢���� ���� �ﰢ��
	pnIndices[6] = 0;
	pnIndices[7] = 5;
	pnIndices[8] = 4;
	
	// ����(Top) �簢���� �Ʒ��� �ﰢ��
	pnIndices[9] = 1;
	pnIndices[10] = 5;
	pnIndices[11] = 0;
	
	// �޸�(Back) �簢���� ���� �ﰢ��
	pnIndices[12] = 3;
	pnIndices[13] = 4;
	pnIndices[14] = 7;
	
	// �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	pnIndices[15] = 0;
	pnIndices[16] = 4;
	pnIndices[17] = 3;
	
	// �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	pnIndices[18] = 1;
	pnIndices[19] = 6;
	pnIndices[20] = 5;
	
	// �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	pnIndices[21] = 2;
	pnIndices[22] = 6;
	pnIndices[23] = 1;
	
	// ����(Left) �簢���� ���� �ﰢ��
	pnIndices[24] = 2;
	pnIndices[25] = 7;
	pnIndices[26] = 6;
	
	// ����(Left) �簢���� �Ʒ��� �ﰢ��
	pnIndices[27] = 3;
	pnIndices[28] = 7;
	pnIndices[29] = 2;
	
	// ����(Right) �簢���� ���� �ﰢ��
	pnIndices[30] = 6;
	pnIndices[31] = 4;
	pnIndices[32] = 5;
	
	// ����(Right) �簢���� �Ʒ��� �ﰢ��
	pnIndices[33] = 7;
	pnIndices[34] = 4;
	pnIndices[35] = 6;
	
	// Index ���� ����
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	
	// Index ���� View ����
	m_d3dIndexBufferView.BufferLocation		 = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format				 = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes		 = sizeof(UINT) * m_nIndices;
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}

//========================================================================================================================================

CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices				 = 24 * 3;
	m_nStride				 = sizeof(CDiffusedVertex);
	m_nOffset				 = 0;
	m_nSlot					 = 0;
	m_d3dPrimitiveTopology	 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	FLOAT fx{ fWidth * 0.5f };
	FLOAT fy{ fHeight * 0.5f };
	FLOAT fz{ fDepth * 0.5f };

	//���� �׸��� ���� ����� �޽��� ǥ���ϱ� ���� Vertex �������̴�.
	CDiffusedVertex	 pVertices[24 * 3];
	FLOAT			 x1{ fx * 0.2f };
	FLOAT			 y1{ fy * 0.2f };
	FLOAT			 x2{ fx * 0.1f };
	FLOAT			 y3{ fy * 0.3f };
	FLOAT			 y2{ ((y1 - (fy - y3)) / x1) * x2 + (fy - y3) };
	INT				 i{ 0 };

	//����� �޽��� ���� ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));

	//����� �޽��� �Ʒ��� ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	
	//����� �޽��� ������ ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	
	//����� �޽��� ����/������ ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	
	//����� �޽��� ���� ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	
	//����� �޽��� ����/���� ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation	 = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		 = m_nStride;
	m_d3dVertexBufferView.SizeInBytes		 = m_nStride * m_nVertices;
}

CAirplaneMeshDiffused::~CAirplaneMeshDiffused()
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

	CDiffusedVertex* pVertices{ new CDiffusedVertex[m_nVertices] };

	/*xStart�� zStart�� ������ ���� ��ġ(x��ǥ�� z��ǥ)�� ��Ÿ����. Ŀ�ٶ� ������ ���ڵ��� ������ �迭�� ����
	�ʿ䰡 �ֱ� ������ ��ü �������� �� ������ ���� ��ġ�� ��Ÿ���� ������ �ʿ��ϴ�.*/
	FLOAT fHeight{ 0.0f };
	FLOAT fMinHeight{ +FLT_MAX };
	FLOAT fMaxHeight{ -FLT_MAX };

	for (INT i = 0, z = zStart; z < (zStart + nLength); ++z)
	{
		for (INT x = xStart; x < (xStart + nWidth); ++x, ++i)
		{
			//Vertex�� ���̿� ������ Height Map���κ��� ���Ѵ�.
			XMFLOAT3 xmf3Position{ XMFLOAT3((x * m_xmf3Scale.x), OnGetHeight(x, z, pContext), (z * m_xmf3Scale.z)) };
			XMFLOAT4 xmf3Color{ Vector4::Add(OnGetColor(x, z, pContext), xmf4Color) };

			pVertices[i] = CDiffusedVertex(xmf3Position, xmf3Color);

			if (fHeight < fMinHeight)
				fMinHeight = fHeight;

			if (fHeight > fMaxHeight)
				fMaxHeight = fHeight;
		}
	}

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation	 = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		 = m_nStride;
	m_d3dVertexBufferView.SizeInBytes		 = m_nStride * m_nVertices;

	delete[] pVertices;

	/*���ڴ� �簢������ �����̰� �簢���� �� ���� �ﰢ������ �����ǹǷ� ���ڴ� ���� �׸��� ���� �ﰢ������ �����̶�� �� �� �ִ�.
	���ڸ� ǥ���ϱ� ���Ͽ� ������ �ﰢ������ Vertex ������ Index�� ǥ���ؾ� �Ѵ�.
	�ﰢ�� ��Ʈ���� ����Ͽ� �ﰢ������ ǥ���ϱ� ���Ͽ� �ﰢ������ �簢���� �� ������ �Ʒ����� ���� ��������(z��) �����Ѵ�.
	ù ��° �簢�� ���� �ﰢ������ ���ʿ��� ����������(x��) �����Ѵ�.
	�� ��° ���� �ﰢ������ �����ʿ��� ���� �������� �����Ѵ�.
	��, �簢���� ���� �ٲ� ������ ���� ������ �ٲ�� �Ѵ�.
	���� �׸��� ���ڿ� ���Ͽ� �ﰢ�� ��Ʈ���� ����Ͽ� �ﰢ������ ǥ���ϱ� ���� Index�� ������ ������ ���� ������ m��° �ٰ�
	(m + 1)��° ����	Vertex ��ȣ�� �簢���� ���� ���⿡ ���� ������ �Ʒ�, ��, �Ʒ�, ��, ... ������ �����ϸ� �ȴ�.
		0, 6, 1, 7, 2, 8, 3, 9, 4, 10, 5, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12
	�̷��� Index�� �����ϸ� �ﰢ�� ��Ʈ���� ����� ���̹Ƿ� ���� �׷����� �ﰢ������ Index�� ������ ����.
		(0, 6, 1), (1, 6, 7), (1, 7, 2), (2, 7, 8), (2, 8, 3), (3, 8, 9), ...
	�׷��� �̷��� Index�� ����ϸ� ù ��° ���� �����ϰ� �ﰢ������ ����� �׷����� �ʴ´�.
	�ֳ��ϸ� �ﰢ�� ��Ʈ�������� ������ 2���� Vertex�� ���ο� �ϳ��� Vertex�� ����Ͽ� ���ο� �ﰢ���� �׸���.
	�׸��� Ȧ�� ��° �ﰢ���� Vertex ���� ����(���ε� ����)�� �ð�����̰� ¦�� ��° �ﰢ���� ���ε� ������ �ݽð� �����̾�� �Ѵ�.
	������ �簢���� �� �ٿ��� �� ���� �ִ��� ������� �� ���� ������ �ﰢ���� ¦�� ��° �ﰢ���̰� ���ε� ������ �ݽð� �����̴�.
	�ֳ��ϸ� �簢���� �� ���� �ﰢ������ ���������� �����̴�.
	ù ��° �ٿ��� �� ��° ���� Index ������ ���� �׷����� �ﰢ������ Index�� ���캸��.
		..., 4, 10, 5, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12, ...
		..., (4, 10, 5), (5, 10, 11), (5, 11, 11), (11, 11, 17), (11, 17, 10), ...
	�ﰢ�� (5, 10, 11)�� ù ��° ���� ������ �ﰢ���̰� ¦�� ��°�̴�.
	�ﰢ�� (11, 17, 10)�� �� ��° ���� ù ��° �ﰢ���̰� Ȧ�� ��°�̴�.
	Ȧ�� ��°�̹Ƿ� ���ε� ������ �ð�����̾�� �ϴµ� ���� ���ε� ������ �ݽð� �����̹Ƿ� �׷����� ���� ���̴�.
	�翬�� ���� �ﰢ���� ���ε� ������ ���� �����Ƿ� �׷����� ���� ���̴�.
	�ﰢ�� (11, 17, 10)�� ���ε� ������ �ݽð�����̹Ƿ� �׷������� �Ϸ��� �� �ﰢ���� ¦�� ��° �ﰢ���� �ǵ��� �ؾ� �Ѵ�.
	�̸� ���ؼ� ���� �ٲ� ������ ������ Vertex�� Index�� �߰��ϵ��� ����.
	�׷��� ���� �ٲ� ù ��° �ﰢ���� ¦�� ��° �ﰢ���� �ȴ�.
	������ �������� 11�� �߰��� ������ Vertex�� Index�̴�.
	�̷��� �ϸ� �ﰢ���� ������ �� ��� �׷����� �ʴ� �ﰢ���� �� �ٸ��� 3���� ����� �ȴ�.
		..., 4, 10, 5, 11, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12, ...
		..., (5, 10, 11), (5, 11, 11), (11, 11, 11), (11, 11, 17), (11, 17, 10), ...
	�� ���� �ﰢ�� (5, 11, 11), (11, 11, 11), (11, 11, 17)�� �ﰢ���� ������ �� �����Ƿ� ������ �׷����� �ʴ´�.
	�̷��� Index�� �����ϸ� Index ���۴� ((nWidth * 2) * (nLength - 1)) + ((nLength - 1) - 1)���� Index�� ���´�.
	�簢�� ���� ������ (nLength - 1)�̰� �� �ٿ��� (nWidth * 2)���� Index�� ���´�.
	�׸��� ���� �ٲ� ������ Index�� �ϳ� �߰��ϹǷ� (nLength - 1) - 1���� Index�� �߰��� �ʿ��ϴ�.*/

	m_nIndices = ((nWidth * 2) * (nLength - 1)) + ((nLength - 1) - 1);

	UINT* pnIndices{ new UINT[m_nIndices] };

	for (INT j = 0, z = 0; z < nLength - 1; ++z)
	{
		if ((z % 2) == 0)
		{
			// Ȧ�� ��° ���̹Ƿ�(z = 0, 2, 4, ...) Index�� ���� ������ ���ʿ��� ������ ����
			for (INT x = 0; x < nWidth; ++x)
			{
				// ù ��° ���� �����ϰ� ���� �ٲ� ������(x == 0) ù ��° Index �߰�
				if ((x == 0) && (z > 0))
					pnIndices[j++] = (UINT)(x + (z * nWidth));

				// �Ʒ�(x, z), ��(x, z + 1)�� ������ Index �߰�
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			// ¦�� ��° ���̹Ƿ�(z = 1, 3, 5, ...) Index�� ���� ������ �����ʿ��� ���� ����
			for (INT x = nWidth - 1; x >= 0; --x)
			{
				// ���� �ٲ� ������(x == (nWidth - 1)) ù ��° Index �߰�
				if (x == (nWidth - 1))
					pnIndices[j++] = (UINT)(x + (z * nWidth));

				// �Ʒ�(x, z), ��(x, z + 1)�� ������ Index �߰�
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	
	m_d3dIndexBufferView.BufferLocation	 = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format			 = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes	 = sizeof(UINT) * m_nIndices;

	delete[] pnIndices;
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

// Height Map �̹����� Pixel ���� ������ ���̷� ��ȯ
FLOAT CHeightMapGridMesh::OnGetHeight(INT x, INT z, void* pContext)
{
	CHeightMapImage*	 pHeightMapImage{ (CHeightMapImage*)pContext };
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

	CHeightMapImage*	 pHeightMapImage{ (CHeightMapImage*)pContext };
	XMFLOAT3			 xmf3Scale{ pHeightMapImage->GetScale() };

	// ������ ����(����, ���)
	XMFLOAT4			 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);
	/*Vertex (x, z)���� ������ �ݻ�Ǵ� ��(����)�� Vertex (x, z)�� ���� ���Ϳ� ������ ���� ������ ����(cos)�� ������
	3���� Vertex (x+1, z), (x, z+1), (x+1, z+1)�� ���� ���Ϳ� ������ ���� ������ ������ ����Ͽ� ���Ѵ�.
	Vertex (x, z)�� ������ ���� ����(����)�� �ݻ�Ǵ� ��(����)�� ���� ���̴�.*/
	FLOAT fScale{ Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z), xmf3LightDirection) };

	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z + 1), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z + 1), xmf3LightDirection);
	fScale = (fScale / 4.0f) + 0.05f;

	if (fScale > 1.0f)
		fScale = 1.0f;

	if (fScale < 0.25f)
		fScale = 0.25f;

	// fScale�� ���� ����(���)�� �ݻ�Ǵ� ����
	XMFLOAT4 xmf4Color{ Vector4::Multiply(fScale, xmf4IncidentLightColor) };

	return xmf4Color;
}
