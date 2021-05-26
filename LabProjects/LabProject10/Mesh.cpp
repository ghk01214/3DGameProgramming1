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

//==================================================================================================

CTriangleMesh::CTriangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
	// 삼각형 Mesh 정의
	m_nVertices								 = 3;
	m_nStride								 = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology					 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*Vertex(삼각형의 꼭지점)의 색상은 시계방향 순서대로 빨간색, 녹색, 파란색으로 지정. RGBA(Red, Green, Blue, Alpha)
	4개의 파라미터를 사용하여 색상 표현. 각 파라메터는 0.0~1.0 사이의 실수값을 가진다.*/
	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));

	// 삼각형 Mesh 리소스(Vertex 버퍼)로 생성
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// Vertex 버퍼 뷰 생성
	m_d3dVertexBufferView.BufferLocation	 = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		 = m_nStride;
	m_d3dVertexBufferView.SizeInBytes		 = m_nStride * m_nVertices;
}

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FLOAT fWidth, FLOAT fHeight, FLOAT fDepth)
{
	// 직육면체는 꼭지점(Vertex)이 8개
	m_nVertices				 = 8;
	m_nStride				 = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology	 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FLOAT fx{ fWidth * 0.5f };	 	// fWidth: 직육면체 가로(x-축) 길이
	FLOAT fy{ fHeight * 0.5f };		// fHeight: 직육면체 세로(y-축) 길이
	FLOAT fz{ fDepth * 0.5f };		// fDepth: 직육면체 깊이(z-축) 길이

	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다.
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
	
	// 정점 버퍼 뷰를 생성한다.
	m_d3dVertexBufferView.BufferLocation	 = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		 = m_nStride;
	m_d3dVertexBufferView.SizeInBytes		 = m_nStride * m_nVertices;

	// Index 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다. 삼각형 리스트로 직육면체를 표현할 것이므로
	// 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 Vertex가 필요. 즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인덱스를 가져야 한다
	m_nIndices = 36;

	UINT pnIndices[36];
	
	// 앞면(Front) 사각형의 위쪽 삼각형
	pnIndices[0] = 3;
	pnIndices[1] = 1;
	pnIndices[2] = 0;
	
	// 앞면(Front) 사각형의 아래쪽 삼각형
	pnIndices[3] = 2;
	pnIndices[4] = 1;
	pnIndices[5] = 3;
	
	// 윗면(Top) 사각형의 위쪽 삼각형
	pnIndices[6] = 0;
	pnIndices[7] = 5;
	pnIndices[8] = 4;
	
	// 윗면(Top) 사각형의 아래쪽 삼각형
	pnIndices[9] = 1;
	pnIndices[10] = 5;
	pnIndices[11] = 0;
	
	// 뒷면(Back) 사각형의 위쪽 삼각형
	pnIndices[12] = 3;
	pnIndices[13] = 4;
	pnIndices[14] = 7;
	
	// 뒷면(Back) 사각형의 아래쪽 삼각형
	pnIndices[15] = 0;
	pnIndices[16] = 4;
	pnIndices[17] = 3;
	
	// 아래면(Bottom) 사각형의 위쪽 삼각형
	pnIndices[18] = 1;
	pnIndices[19] = 6;
	pnIndices[20] = 5;
	
	// 아래면(Bottom) 사각형의 아래쪽 삼각형
	pnIndices[21] = 2;
	pnIndices[22] = 6;
	pnIndices[23] = 1;
	
	// 옆면(Left) 사각형의 위쪽 삼각형
	pnIndices[24] = 2;
	pnIndices[25] = 7;
	pnIndices[26] = 6;
	
	// 옆면(Left) 사각형의 아래쪽 삼각형
	pnIndices[27] = 3;
	pnIndices[28] = 7;
	pnIndices[29] = 2;
	
	// 옆면(Right) 사각형의 위쪽 삼각형
	pnIndices[30] = 6;
	pnIndices[31] = 4;
	pnIndices[32] = 5;
	
	// 옆면(Right) 사각형의 아래쪽 삼각형
	pnIndices[33] = 7;
	pnIndices[34] = 4;
	pnIndices[35] = 6;
	
	// Index 버퍼 생성
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	
	// Index 버퍼 View 생성
	m_d3dIndexBufferView.BufferLocation		 = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format				 = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes		 = sizeof(UINT) * m_nIndices;
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}
