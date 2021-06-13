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
	m_vVertices.reserve(8);

	m_vVertices.emplace_back(XMFLOAT3(-fx, +fy, -fz), Vector4::Add(XMFLOAT4(-0.5f, -0.5f, -0.5f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, +fy, -fz), Vector4::Add(XMFLOAT4(-0.5f, -0.5f, -0.5f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, +fy, +fz), Vector4::Add(XMFLOAT4(-0.5f, -0.5f, -0.5f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, +fy, +fz), Vector4::Add(XMFLOAT4(-0.5f, -0.5f, -0.5f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, -fy, -fz), Vector4::Add(XMFLOAT4(-0.5f, -0.5f, -0.5f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, -fy, -fz), Vector4::Add(XMFLOAT4(-0.5f, -0.5f, -0.5f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, -fy, +fz), Vector4::Add(XMFLOAT4(-0.5f, -0.5f, -0.5f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, -fy, +fz), Vector4::Add(XMFLOAT4(-0.5f, -0.5f, -0.5f, 0.0f), RANDOM_COLOR));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &(*(m_vVertices.data())), m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	
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
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, (&(*(vnIndices.data()))), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	
	// Index ���� View ����
	m_d3dIndexBufferView.BufferLocation		 = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format				 = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes		 = sizeof(UINT) * m_nIndices;

	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}

//========================================================================================================================================


CFeverCubeMeshDiffused::CFeverCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth) : CCubeMeshDiffused(pd3dDevice, pd3dCommandList, fWidth, fHeight, fDepth)
{
	FLOAT fx{ fWidth * 0.5f };	 	// fWidth: ������ü ����(x-��) ����
	FLOAT fy{ fHeight * 0.5f };		// fHeight: ������ü ����(y-��) ����
	FLOAT fz{ fDepth * 0.5f };		// fDepth: ������ü ����(z-��) ����

	// Vertex ���۴� ������ü�� ������ 8���� ���� Vertex �����͸� ������.
	m_vVertices.clear();

	m_vVertices.emplace_back(XMFLOAT3(-fx, +fy, -fz), Vector4::Add(XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, +fy, -fz), Vector4::Add(XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, +fy, +fz), Vector4::Add(XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, +fy, +fz), Vector4::Add(XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, -fy, -fz), Vector4::Add(XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, -fy, -fz), Vector4::Add(XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, -fy, +fz), Vector4::Add(XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, -fy, +fz), Vector4::Add(XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), RANDOM_COLOR));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &(*(m_vVertices.data())), m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// Vertex ���� �並 �����Ѵ�.
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();

	// Index ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. �ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���̹Ƿ�
	// �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� Vertex�� �ʿ�. ��, �ε��� ���۴� ��ü 36(=6*2*3)���� �ε����� ������ �Ѵ�

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
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, (&(*(vnIndices.data()))), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	// Index ���� View ����
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();

	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CFeverCubeMeshDiffused::~CFeverCubeMeshDiffused()
{
}

//========================================================================================================================================

CCarMeshDiffused::CCarMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices				 = 8 * 2 * 2;
	m_nStride				 = sizeof(CDiffusedVertex);
	m_nOffset				 = 0;
	m_nSlot					 = 0;
	m_d3dPrimitiveTopology	 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	FLOAT fx{ fWidth * 0.5f };
	FLOAT fy{ fHeight * 0.5f };
	FLOAT fz{ fDepth * 0.5f };

	m_vVertices.reserve(m_nVertices);

	// �÷��̾��� ���� ����ü�� 8�� �������� ���� Vertex ����
	m_vVertices.emplace_back(XMFLOAT3(-fx, +fy, -(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, +fy, -(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, +fy, +(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, +fy, +(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, -fy, -(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, -fy, -(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, -fy, +(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, -fy, +(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));

	// �÷��̾��� �Ʒ��� ����ü�� 8�� �������� ���� Vertex ����
	m_vVertices.emplace_back(XMFLOAT3(-fx, +(fy * 2.0f), -(fz * 0.8f) + 2.4f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, +(fy * 2.0f), -(fz * 0.8f) + 2.4f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, +(fy * 2.0f), +(fz * 0.8f) + 2.4f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, +(fy * 2.0f), +(fz * 0.8f) + 2.4f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, -(fy * 2.0f), -(fz * 0.8f) + 2.4f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, -(fy * 2.0f), -(fz * 0.8f) + 2.4f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, -(fy * 2.0f), +(fz * 0.8f) + 2.4f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, -(fy * 2.0f), +(fz * 0.8f) + 2.4f), Vector4::Add(xmf4Color, RANDOM_COLOR));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &(*(m_vVertices.data())), m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation	 = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		 = m_nStride;
	m_d3dVertexBufferView.SizeInBytes		 = m_nStride * m_nVertices;

	m_nIndices = 8 * 5 * 2;

	std::vector<UINT> vnIndices;
	vnIndices.reserve(m_nIndices);

	// ���κ�(Top) ����ü
	// ����(Top) �簢��
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(0);

	// �Ʒ���(Bottom) �簢��
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(2);

	// �ո�(Front) �簢��
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(3);

	// �޸�(Back) �簢��
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(6);

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

	// �Ʒ��κ�(Bottom) ����ü
	// ����(Top) �簢��
	vnIndices.emplace_back(0 + 8);
	vnIndices.emplace_back(5 + 8);
	vnIndices.emplace_back(4 + 8);
	vnIndices.emplace_back(1 + 8);
	vnIndices.emplace_back(5 + 8);
	vnIndices.emplace_back(0 + 8);

	// �Ʒ���(Bottom) �簢��
	vnIndices.emplace_back(2 + 8);
	vnIndices.emplace_back(7 + 8);
	vnIndices.emplace_back(6 + 8);
	vnIndices.emplace_back(3 + 8);
	vnIndices.emplace_back(7 + 8);
	vnIndices.emplace_back(2 + 8);

	// �ո�(Front) �簢��
	vnIndices.emplace_back(3 + 8);
	vnIndices.emplace_back(1 + 8);
	vnIndices.emplace_back(0 + 8);
	vnIndices.emplace_back(2 + 8);
	vnIndices.emplace_back(1 + 8);
	vnIndices.emplace_back(3 + 8);

	// �޸�(Back) �簢��
	vnIndices.emplace_back(6 + 8);
	vnIndices.emplace_back(4 + 8);
	vnIndices.emplace_back(5 + 8);
	vnIndices.emplace_back(7 + 8);
	vnIndices.emplace_back(4 + 8);
	vnIndices.emplace_back(6 + 8);

	// ����(Left) �簢��
	vnIndices.emplace_back(3 + 8);
	vnIndices.emplace_back(4 + 8);
	vnIndices.emplace_back(7 + 8);
	vnIndices.emplace_back(0 + 8);
	vnIndices.emplace_back(4 + 8);
	vnIndices.emplace_back(3 + 8);

	// ����(Right) �簢��
	vnIndices.emplace_back(1 + 8);
	vnIndices.emplace_back(6 + 8);
	vnIndices.emplace_back(5 + 8);
	vnIndices.emplace_back(2 + 8);
	vnIndices.emplace_back(6 + 8);
	vnIndices.emplace_back(1 + 8);

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, (&(*(vnIndices.data()))), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation		 = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format				 = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes		 = sizeof(UINT) * m_nIndices;

	// Mesh�� Bounding Box(�� ��ǥ��) ����
	m_xmBoundingBox							 = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CCarMeshDiffused::~CCarMeshDiffused()
{
}

//========================================================================================================================================

CWallMeshDiffused::CWallMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, FLOAT fColor) : CMesh(pd3dDevice, pd3dCommandList)
{
	// ������ü�� ������(Vertex)�� 8��
	m_nVertices				 = 8;
	m_nStride				 = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology	 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FLOAT fx{ fWidth * 0.5f };	 	// fWidth: ������ü ����(x-��) ����
	FLOAT fy{ fHeight * 0.5f };		// fHeight: ������ü ����(y-��) ����
	FLOAT fz{ fDepth * 0.5f };		// fDepth: ������ü ����(z-��) ����

	// Vertex ���۴� ������ü�� ������ 8���� ���� Vertex �����͸� ������.
	m_vVertices.reserve(m_nVertices);
	m_vVertices.emplace_back(XMFLOAT3(-fx, +fy, -fz), Vector4::Add(XMFLOAT4(fColor, 0.0f, 0.0f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, +fy, -fz), Vector4::Add(XMFLOAT4(fColor, 0.0f, 0.0f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, +fy, +fz), Vector4::Add(XMFLOAT4(fColor, 0.0f, 0.0f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, +fy, +fz), Vector4::Add(XMFLOAT4(fColor, 0.0f, 0.0f, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, -fy, -fz), Vector4::Add(XMFLOAT4(0.0f, 0.0f, fColor, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, -fy, -fz), Vector4::Add(XMFLOAT4(0.0f, 0.0f, fColor, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, -fy, +fz), Vector4::Add(XMFLOAT4(0.0f, 0.0f, fColor, 0.0f), RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, -fy, +fz), Vector4::Add(XMFLOAT4(0.0f, 0.0f, fColor, 0.0f), RANDOM_COLOR));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &(*(m_vVertices.data())), m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	
	// Vertex ���� �並 �����Ѵ�.
	m_d3dVertexBufferView.BufferLocation	 = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		 = m_nStride;
	m_d3dVertexBufferView.SizeInBytes		 = m_nStride * m_nVertices;

	// Index ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. �ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���̹Ƿ�
	// �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� Vertex�� �ʿ�. ��, �ε��� ���۴� ��ü 36(=6*2*3)���� �ε����� ������ �Ѵ�
	m_nIndices = 6 * 4;

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
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, (&(*(vnIndices.data()))), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	
	// Index ���� View ����
	m_d3dIndexBufferView.BufferLocation		 = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format				 = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes		 = sizeof(UINT) * m_nIndices;

	// Mesh�� Bounding Box(�� ��ǥ��) ����
	m_xmBoundingBox							 = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CWallMeshDiffused::~CWallMeshDiffused()
{
}