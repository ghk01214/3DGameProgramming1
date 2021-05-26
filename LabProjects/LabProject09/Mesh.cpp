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
		// Index ���۰� ������ Index ���۸� Pipeline(IA: �Է� ������)�� �����ϰ� �ε����� ����Ͽ� Rendering
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);			// Mesh�� Vertex ���� �並 Rendering(Pipeline(�Է� ������)�� �۵��ϰ� �Ѵ�)
}

//==================================================================================================

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

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth)
{
	// ������ü�� ������(Vertex)�� 8��
	m_nVertices				 = 8;
	m_nStride				 = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology	 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FLOAT fx{ fWidth * 0.5f };	 	// fWidth: ������ü ����(x-��) ����
	FLOAT fy{ fHeight * 0.5f };		// fHeight: ������ü ����(y-��) ����
	FLOAT fz{ fDepth * 0.5f };		// fDepth: ������ü ����(z-��) ����

	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������.
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
	
	// ���� ���� �並 �����Ѵ�.
	m_d3dVertexBufferView.BufferLocation	 = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		 = m_nStride;
	m_d3dVertexBufferView.SizeInBytes		 = m_nStride * m_nVertices;

	// Index ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. �ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���̹Ƿ�
	// �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� Vertex�� �ʿ�. ��, �ε��� ���۴� ��ü 36(=6*2*3)���� �ε����� ������ �Ѵ�
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
