#include "framework.h"
#include "GameObject.h"

CGameObject::CGameObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CGameObject::~CGameObject()
{
	if (m_pMesh)
		m_pMesh->Release();

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
	if (m_pMesh)
		m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh)
		m_pMesh->Release();

	m_pMesh = pMesh;

	if (m_pMesh)
		m_pMesh->AddReference();
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

BOOL CGameObject::IsVisible(CCamera* pCamera)
{
	PrepareRender();

	BOOL				 bIsVisible{ FALSE };
	BoundingOrientedBox	 xmBoundingBox{ m_pMesh->GetBoundingBox() };

	// 모델 좌표계의 바운딩 박스를 월드 좌표계로 변환
	xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));

	if (pCamera)
		bIsVisible = pCamera->IsInFrustum(xmBoundingBox);

	return bIsVisible;
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
	XMFLOAT3 xmf3Position{ GetPosition() };
	XMFLOAT3 xmf3Look{ GetLook() };

	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Animate(FLOAT fTimeElapsed, CCamera* pCamera)
{

}

void CGameObject::PrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (IsVisible(pCamera))
	{
		// 객체의 정보를 Shader 변수(상수 버퍼)로 복사
		UpdateShaderVariables(pd3dCommandList);

		if (m_pShader)
			m_pShader->Render(pd3dCommandList, pCamera);

		if (m_pMesh)
			m_pMesh->Render(pd3dCommandList);
	}
}

//========================================================================================

CWallObject::CWallObject(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth) : m_fWidth(fWidth), m_fHeight(fHeight), m_fDepth(fDepth)
{
	m_xmf3Position = CGameObject::GetPosition();
}

CWallObject::~CWallObject()
{
}

void CWallObject::Animate(FLOAT fTimeElapsed, CCamera* pCamera)
{
	if (m_xmf4x4World._43 + m_fDepth < pCamera->GetPosition().z)
	{
		CGameObject::SetPosition(0.0f, 0.0f, m_xmf4x4World._43 + m_fDepth * 4);
	}
}
