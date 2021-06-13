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
	// Vertex 버퍼를 위한 업로드 버퍼 소멸
	if (m_pd3dVertexUploadBuffer)
		m_pd3dVertexUploadBuffer->Release();

	if (m_pd3dIndexUploadBuffer)
		m_pd3dIndexUploadBuffer->Release();

	m_pd3dVertexUploadBuffer	 = nullptr;
	m_pd3dIndexUploadBuffer		 = nullptr;
}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);			// Mesh의 Primitive 유형을 설정
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);	// Mesh의 Vertex 버퍼 뷰를 설정

	if (m_pd3dIndexBuffer)
	{
		// Index 버퍼가 있으면 Index 버퍼를 Pipeline(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 Rendering
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);			// Mesh의 Vertex 버퍼 뷰를 Rendering(Pipeline(입력 조립기)을 작동하게 한다)
}

//========================================================================================================================================

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth)
{
	// 직육면체는 꼭지점(Vertex)이 8개
	m_nVertices				 = 8;
	m_nStride				 = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology	 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FLOAT fx{ fWidth * 0.5f };	 	// fWidth: 직육면체 가로(x-축) 길이
	FLOAT fy{ fHeight * 0.5f };		// fHeight: 직육면체 세로(y-축) 길이
	FLOAT fz{ fDepth * 0.5f };		// fDepth: 직육면체 깊이(z-축) 길이

	// Vertex 버퍼는 직육면체의 꼭지점 8개에 대한 Vertex 데이터를 가진다.
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
	
	// Vertex 버퍼 뷰를 생성한다.
	m_d3dVertexBufferView.BufferLocation	 = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		 = m_nStride;
	m_d3dVertexBufferView.SizeInBytes		 = m_nStride * m_nVertices;

	// Index 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다. 삼각형 리스트로 직육면체를 표현할 것이므로
	// 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 Vertex가 필요. 즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인덱스를 가져야 한다
	m_nIndices = 36;

	std::vector<UINT> vnIndices;
	vnIndices.reserve(m_nIndices);
	
	// 윗면(Top) 사각형
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(3);

	// 아래면(Bottom) 사각형
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(6);

	// 앞면(Front) 사각형
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(2);

	// 뒷면(Back) 사각형
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(0);
	
	// 옆면(Left) 사각형
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(3);
	
	// 옆면(Right) 사각형
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(1);
	
	// Index 버퍼 생성
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, (&(*(vnIndices.data()))), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	
	// Index 버퍼 View 생성
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
	FLOAT fx{ fWidth * 0.5f };	 	// fWidth: 직육면체 가로(x-축) 길이
	FLOAT fy{ fHeight * 0.5f };		// fHeight: 직육면체 세로(y-축) 길이
	FLOAT fz{ fDepth * 0.5f };		// fDepth: 직육면체 깊이(z-축) 길이

	// Vertex 버퍼는 직육면체의 꼭지점 8개에 대한 Vertex 데이터를 가진다.
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

	// Vertex 버퍼 뷰를 생성한다.
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();

	// Index 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다. 삼각형 리스트로 직육면체를 표현할 것이므로
	// 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 Vertex가 필요. 즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인덱스를 가져야 한다

	std::vector<UINT> vnIndices;
	vnIndices.reserve(m_nIndices);

	// 윗면(Top) 사각형
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(3);

	// 아래면(Bottom) 사각형
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(6);

	// 앞면(Front) 사각형
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(2);

	// 뒷면(Back) 사각형
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(0);

	// 옆면(Left) 사각형
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(3);

	// 옆면(Right) 사각형
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(1);

	// Index 버퍼 생성
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, (&(*(vnIndices.data()))), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	// Index 버퍼 View 생성
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

	// 플레이어의 위쪽 육면체의 8개 꼭지점에 대한 Vertex 버퍼
	m_vVertices.emplace_back(XMFLOAT3(-fx, +fy, -(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, +fy, -(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, +fy, +(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, +fy, +(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, -fy, -(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, -fy, -(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(+fx, -fy, +(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_vVertices.emplace_back(XMFLOAT3(-fx, -fy, +(fz * 0.7f) - 2.1f), Vector4::Add(xmf4Color, RANDOM_COLOR));

	// 플레이어의 아래쪽 육면체의 8개 꼭지점에 대한 Vertex 버퍼
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

	// 윗부분(Top) 육면체
	// 윗면(Top) 사각형
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(0);

	// 아래면(Bottom) 사각형
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(2);

	// 앞면(Front) 사각형
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(3);

	// 뒷면(Back) 사각형
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(6);

	// 옆면(Left) 사각형
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(3);

	// 옆면(Right) 사각형
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(1);

	// 아래부분(Bottom) 육면체
	// 윗면(Top) 사각형
	vnIndices.emplace_back(0 + 8);
	vnIndices.emplace_back(5 + 8);
	vnIndices.emplace_back(4 + 8);
	vnIndices.emplace_back(1 + 8);
	vnIndices.emplace_back(5 + 8);
	vnIndices.emplace_back(0 + 8);

	// 아래면(Bottom) 사각형
	vnIndices.emplace_back(2 + 8);
	vnIndices.emplace_back(7 + 8);
	vnIndices.emplace_back(6 + 8);
	vnIndices.emplace_back(3 + 8);
	vnIndices.emplace_back(7 + 8);
	vnIndices.emplace_back(2 + 8);

	// 앞면(Front) 사각형
	vnIndices.emplace_back(3 + 8);
	vnIndices.emplace_back(1 + 8);
	vnIndices.emplace_back(0 + 8);
	vnIndices.emplace_back(2 + 8);
	vnIndices.emplace_back(1 + 8);
	vnIndices.emplace_back(3 + 8);

	// 뒷면(Back) 사각형
	vnIndices.emplace_back(6 + 8);
	vnIndices.emplace_back(4 + 8);
	vnIndices.emplace_back(5 + 8);
	vnIndices.emplace_back(7 + 8);
	vnIndices.emplace_back(4 + 8);
	vnIndices.emplace_back(6 + 8);

	// 옆면(Left) 사각형
	vnIndices.emplace_back(3 + 8);
	vnIndices.emplace_back(4 + 8);
	vnIndices.emplace_back(7 + 8);
	vnIndices.emplace_back(0 + 8);
	vnIndices.emplace_back(4 + 8);
	vnIndices.emplace_back(3 + 8);

	// 옆면(Right) 사각형
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

	// Mesh의 Bounding Box(모델 좌표계) 생성
	m_xmBoundingBox							 = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CCarMeshDiffused::~CCarMeshDiffused()
{
}

//========================================================================================================================================

CWallMeshDiffused::CWallMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, FLOAT fColor) : CMesh(pd3dDevice, pd3dCommandList)
{
	// 직육면체는 꼭지점(Vertex)이 8개
	m_nVertices				 = 8;
	m_nStride				 = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology	 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FLOAT fx{ fWidth * 0.5f };	 	// fWidth: 직육면체 가로(x-축) 길이
	FLOAT fy{ fHeight * 0.5f };		// fHeight: 직육면체 세로(y-축) 길이
	FLOAT fz{ fDepth * 0.5f };		// fDepth: 직육면체 깊이(z-축) 길이

	// Vertex 버퍼는 직육면체의 꼭지점 8개에 대한 Vertex 데이터를 가진다.
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
	
	// Vertex 버퍼 뷰를 생성한다.
	m_d3dVertexBufferView.BufferLocation	 = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		 = m_nStride;
	m_d3dVertexBufferView.SizeInBytes		 = m_nStride * m_nVertices;

	// Index 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다. 삼각형 리스트로 직육면체를 표현할 것이므로
	// 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 Vertex가 필요. 즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인덱스를 가져야 한다
	m_nIndices = 6 * 4;

	std::vector<UINT> vnIndices;
	vnIndices.reserve(m_nIndices);
	
	// 윗면(Top) 사각형
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(3);

	// 아래면(Bottom) 사각형
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(6);
	
	// 옆면(Left) 사각형
	vnIndices.emplace_back(3);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(7);
	vnIndices.emplace_back(0);
	vnIndices.emplace_back(4);
	vnIndices.emplace_back(3);
	
	// 옆면(Right) 사각형
	vnIndices.emplace_back(1);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(5);
	vnIndices.emplace_back(2);
	vnIndices.emplace_back(6);
	vnIndices.emplace_back(1);
	
	// Index 버퍼 생성
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, (&(*(vnIndices.data()))), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	
	// Index 버퍼 View 생성
	m_d3dIndexBufferView.BufferLocation		 = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format				 = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes		 = sizeof(UINT) * m_nIndices;

	// Mesh의 Bounding Box(모델 좌표계) 생성
	m_xmBoundingBox							 = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CWallMeshDiffused::~CWallMeshDiffused()
{
}