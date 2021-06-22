#include "framework.h"
#include "GameObject.h"

CGameObject::CGameObject(INT nMeshes)
{
	m_xmf4x4World	 = Matrix4x4::Identity();
	m_nMeshes		 = nMeshes;

	if (m_nMeshes > 0)
	{
		m_vpMeshes.reserve(m_nMeshes);

		for (INT i = 0; i < m_nMeshes; ++i)
		{
			m_vpMeshes.push_back(nullptr);
		}
	}
}

CGameObject::~CGameObject()
{
	if (!m_vpMeshes.empty())
	{
		for (auto iter = m_vpMeshes.begin(); iter != m_vpMeshes.end(); ++iter)
		{
			if (*iter)
				(*iter)->Release();
		}

		m_vpMeshes.clear();
		m_vpMeshes.shrink_to_fit();
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
	if (!m_vpMeshes.empty())
	{
		for (auto iter = m_vpMeshes.begin(); iter != m_vpMeshes.end(); ++iter)
		{
			if (*iter)
				(*iter)->ReleaseUploadBuffers();
		}
	}
}

void CGameObject::SetMesh(INT nIndex, CMesh* pMesh)
{
	if (!m_vpMeshes.empty())
	{
		if (m_vpMeshes[nIndex])
			m_vpMeshes[nIndex]->Release();

		m_vpMeshes[nIndex] = pMesh;

		if (pMesh)
			pMesh->AddReference();
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

	// ��ü�� ���� ��ȯ ����� Root ���(32��Ʈ ��)�� ���Ͽ� Shader ����(��� ����)�� ����
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

void CGameObject::SetCollidedObject(CGameObject* pObjectCollided)
{
	if (this == nullptr)
		return;

	m_pObjectCollided = pObjectCollided;
}

void CGameObject::UpdateBoundingBox()
{
	//if (m_pMesh)
	//{
	//	m_pMesh->m_xmBoundingBox.Transform(m_xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));

	//	XMStoreFloat4(&m_xmBoundingBox.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmBoundingBox.Orientation)));
	//}
}

void CGameObject::MoveStrate(FLOAT fDistance)
{
	XMFLOAT3 xmf3Position{ GetPosition() };
	XMFLOAT3 xmf3Right{ GetRight() };

	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);

	SetPosition(xmf3Position);
}

void CGameObject::MoveUp(FLOAT fDistance)
{
	XMFLOAT3 xmf3Position{ GetPosition() };
	XMFLOAT3 xmf3Up{ GetUp() };

	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);

	SetPosition(xmf3Position);
}

void CGameObject::MoveForward(FLOAT fDistance)
{
	XMFLOAT3 xmf3Position{ GetPosition() };
	XMFLOAT3 xmf3Look{ GetLook() };

	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);

	SetPosition(xmf3Position);
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

void CGameObject::Animate(FLOAT fTimeElapsed, CCamera* pCamera)
{
	UpdateBoundingBox();
}

void CGameObject::PrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	// ��ü�� ������ Shader ����(��� ����)�� ����
	UpdateShaderVariables(pd3dCommandList);

	if (m_pShader)
		m_pShader->Render(pd3dCommandList, pCamera);

	if (!m_vpMeshes.empty())
	{
		for (auto iter = m_vpMeshes.cbegin(); iter != m_vpMeshes.cend(); ++iter)
		{
			if (*iter)
				(*iter)->Render(pd3dCommandList);
		}
	}
}

//========================================================================================

CRotatingObject::CRotatingObject(INT nMeshes) : CGameObject(nMeshes)
{
	m_xmf3RotationAxis	 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed	 = 15.0f;
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(FLOAT fTimeElapsed, CCamera* pCamera)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}

//========================================================================================

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, INT nWidth, INT nLength, INT nBlockWidth, INT nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(0)
{
	// ������ ����� Height Map�� ����, ������ ũ���̴�.
	m_nWidth			 = nWidth;
	m_nLength			 = nLength;

	/*���� ��ü�� ���� Mesh���� �迭�� ���� ���̴�. nBlockWidth, nBlockLength�� ���� Mesh �ϳ��� ����, ���� ũ���̴�.
	cxQuadsPerBlock, czQuadsPerBlock�� ���� Mesh�� ���� ����� ���� ���� �簢���� �����̴�.*/
	INT cxQuadsPerBlock{ nBlockWidth - 1 };
	INT czQuadsPerBlock{ nBlockLength - 1 };

	// xmf3Scale�� ������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ����.
	m_xmf3Scale			 = xmf3Scale;
	m_pHeightMapImage	 = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	// �������� ���� ����, ���� �������� ���� Mesh�� �� ���� �ִ� ���� ��Ÿ����.
	LONG cxBlocks{ (m_nWidth - 1) / cxQuadsPerBlock };
	LONG czBlocks{ (m_nLength - 1) / czQuadsPerBlock };

	m_nMeshes = cxBlocks * czBlocks;
	m_vpMeshes.reserve(m_nMeshes);

	for (INT i = 0; i < m_nMeshes; ++i)
	{
		m_vpMeshes.push_back(nullptr);
	}

	CHeightMapGridMesh* pHeightMapGridMesh{ nullptr };

	for (INT z = 0, zStart = 0; z < czBlocks; ++z)
	{
		for (INT x = 0, xStart = 0; x < cxBlocks; ++x)
		{
			// ������ �Ϻκ��� ��Ÿ���� ���� Mesh�� ���� ��ġ(��ǥ)
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);

			//������ �Ϻκ��� ��Ÿ���� ���� Mesh�� �����Ͽ� ���� Mesh�� �����Ѵ�.
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);

			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}

	// ������ Rendering�ϱ� ���� Shader ����
	CTerrainShader* pShader{ new CTerrainShader() };

	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);

	SetShader(pShader);
}

CHeightMapTerrain::~CHeightMapTerrain()
{
	if (m_pHeightMapImage)
		delete m_pHeightMapImage;
}