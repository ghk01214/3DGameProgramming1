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

	// Game Object에 Shder 객체가 연결되어 있으면 Shader 상태 객체 설정
	if (m_pShader)
	{
		// Game Object의 월드 변환 행렬을 Shder의 상수 버퍼로 전달(복사)
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
		m_pShader->Render(pd3dCommandList, pCamera);
	}

	// Game Object에 Mesh가 연결되어 있으면 Mesh Render
	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList);
}

CRotatingObject::CRotatingObject()
{
	m_xmf3RotationAxis	 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed	 = 90.0f;
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(FLOAT fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}
