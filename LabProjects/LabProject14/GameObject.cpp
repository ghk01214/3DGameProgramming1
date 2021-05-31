#include "framework.h"
#include "GameObject.h"

CGameObject::CGameObject(INT nMeshes)
{
	m_xmf4x4World	 = Matrix4x4::Identity();
	m_nMeshes		 = nMeshes;
	m_ppMeshes		 = nullptr;

	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh * [m_nMeshes];

		for (INT i = 0; i < m_nMeshes; ++i)
			m_ppMeshes[i] = nullptr;
	}
}

CGameObject::~CGameObject()
{
	if (m_ppMeshes)
	{
		for (INT i = 0; i < m_nMeshes; ++i)
		{
			if (m_ppMeshes[i])
				m_ppMeshes[i]->Release();

			m_ppMeshes[i] = nullptr;
		}

		delete[] m_ppMeshes;
	}

	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CGameObject::Release()
{
	if (--m_nReferences <= 0)
		delete this;
}

void CGameObject::ReleaseUploadBuffers()
{
	// Vertex 버퍼를 위한 업로드 버퍼 소멸
	if (m_ppMeshes)
	{
		for (INT i = 0; i < m_nMeshes; ++i)
		{
			if (m_ppMeshes[i])
				m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}
}

void CGameObject::SetMesh(INT nIndex, CMesh* pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex])
			m_ppMeshes[nIndex]->Release();

		m_ppMeshes[nIndex] = pMesh;

		if (pMesh
			) pMesh->AddReference();
	}
}

void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader)
		m_pShader->Release();

	m_pShader = pShader;

	if (m_pShader)
		m_pShader->AddReference();
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;

	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	
	// 객체의 월드 변환 행렬을 Root 상수(32비트 값)를 통하여 Shader 변수(상수 버퍼)로 복사
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::SetPosition(FLOAT x, FLOAT y, FLOAT z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::MoveStrate(FLOAT fDistance)
{
	XMFLOAT3 xmf3Position{ GetPosition() };
	XMFLOAT3 xmf3Right{ GetRight() };
	
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(FLOAT fDistance)
{
	XMFLOAT3 xmf3Position{ GetPosition() };
	XMFLOAT3 xmf3Up{ GetUp() };

	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(FLOAT fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();

	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(FLOAT fPitch, FLOAT fYaw, FLOAT fRoll)
{
	XMMATRIX mtxRotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll)) };
	
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, FLOAT fAngle)
{
	XMMATRIX mtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle)) };

	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Animate(FLOAT fTimeElapsed)
{
}

void CGameObject::PrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	PrepareRender();

	// 객체의 정보를 Shader 변수(상수 버퍼)로 복사
	UpdateShaderVariables(pd3dCommandList);

	if (m_pShader)
		m_pShader->Render(pd3dCommandList, pCamera);

	//게임 객체가 포함하는 모든 Mesh를 Rendering한다.
	if (m_ppMeshes)
	{
		for (INT i = 0; i < m_nMeshes; ++i)
		{
			if (m_ppMeshes[i])
				m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}

//========================================================================================================

CRotatingObject::CRotatingObject(INT nMeshes) : CGameObject(nMeshes)
{
	m_xmf3RotationAxis	 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed	 = 15.0f;
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(FLOAT fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}

//========================================================================================================

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, INT nWidth, INT nLength, INT nBlockWidth, INT nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(0)
{
	// 지형에 사용할 Height Map의 가로, 세로의 크기이다.
	m_nWidth			 = nWidth;
	m_nLength			 = nLength;

	/*지형 객체는 격자 Mesh들의 배열로 만들 것이다. nBlockWidth, nBlockLength는 격자 Mesh 하나의 가로, 세로 크기이다.
	cxQuadsPerBlock, czQuadsPerBlock은 격자 Mesh의 가로 방향과 세로 방향 사각형의 개수이다.*/
	INT cxQuadsPerBlock{ nBlockWidth - 1 };
	INT czQuadsPerBlock{ nBlockLength - 1 };

	// xmf3Scale는 지형을 실제로 몇 배 확대할 것인가를 나타낸다.
	m_xmf3Scale			 = xmf3Scale;

	// 지형에 사용할 Height Map 생성
	m_pHeightMapImage	 = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	// 지형에서 가로 방향, 세로 방향으로 격자 Mesh가 몇 개가 있는 가를 나타낸다.
	LONG cxBlocks{ (m_nWidth - 1) / cxQuadsPerBlock };
	LONG czBlocks{ (m_nLength - 1) / czQuadsPerBlock };

	// 지형 전체를 표현하기 위한 격자 Mesh의 개수이다.
	m_nMeshes = cxBlocks * czBlocks;

	// 지형 전체를 표현하기 위한 격자 Mesh에 대한 포인터 배열 생성
	m_ppMeshes = new CMesh * [m_nMeshes];

	for (INT i = 0; i < m_nMeshes; ++i)
	{
		m_ppMeshes[i] = nullptr;
	}

	CHeightMapGridMesh* pHeightMapGridMesh{ nullptr };

	for (INT z = 0, zStart = 0; z < czBlocks; ++z)
	{
		for (INT x = 0, xStart = 0; x < cxBlocks; ++x)
		{
			// 지형의 일부분을 나타내는 격자 Mesh의 시작 위치(좌표)
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);

			//지형의 일부분을 나타내는 격자 Mesh를 생성하여 지형 Mesh에 저장한다.
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);

			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}

	// 지형을 Rendering하기 위한 Shader 생성
	CTerrainShader* pShader{ new CTerrainShader() };

	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);

	SetShader(pShader);
}

CHeightMapTerrain::~CHeightMapTerrain()
{
	if (m_pHeightMapImage)
		delete m_pHeightMapImage;
}
