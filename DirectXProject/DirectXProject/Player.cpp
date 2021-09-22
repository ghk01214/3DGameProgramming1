#include "framework.h"
#include "Player.h"
#include "Shader.h"

CPlayer::CPlayer(INT nMeshes) : CGameObject(nMeshes)
{
	m_pCamera				 = nullptr;

	m_xmf3Position			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right				 = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up				 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look				 = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ		 = 0.0f;
	m_fMaxVelocityY			 = 0.0f;
	m_fFriction				 = 0.0f;

	m_fPitch				 = 0.0f;
	m_fRoll					 = 0.0f;
	m_fYaw					 = 0.0f;

	m_pPlayerUpdatedContext	 = nullptr;
	m_pCameraUpdatedContext	 = nullptr;
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (m_pCamera)
		delete m_pCamera;
}

void CPlayer::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pCamera)
		m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::ReleaseShaderVariables()
{
	if (m_pCamera)
		m_pCamera->ReleaseShaderVariables();

	CGameObject::ReleaseShaderVariables();
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pCamera)
		m_pCamera->UpdateShaderVariables(pd3dCommandList);

	CGameObject::UpdateShaderVariables(pd3dCommandList);
}

void CPlayer::Move(DWORD dwDirection, FLOAT fDistance, BOOL bVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift{ XMFLOAT3(0, 0, 0) };

		// ȭ��ǥ Ű '��'�� ������ ���� z�� �������� �̵�(����)�Ѵ�.'��'�� ������ �ݴ� �������� �̵��Ѵ�.
		if (dwDirection & DIR_FORWARD)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);

		if (dwDirection & DIR_BACKWARD)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);

		// ȭ��ǥ Ű '��'�� ������ ���� x�� �������� �̵��Ѵ�.'��'�� ������ �ݴ� �������� �̵��Ѵ�.
		if (dwDirection & DIR_RIGHT)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);

		if (dwDirection & DIR_LEFT)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		// 'Page Up'�� ������ ���� y�� �������� �̵��Ѵ�.'Page Down'�� ������ �ݴ� �������� �̵��Ѵ�.
		if (dwDirection & DIR_UP)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);

		if (dwDirection & DIR_DOWN)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		// �÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵��Ѵ�.
		Move(xmf3Shift, bVelocity);
	}
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, BOOL bVelocity)
{
	if (bVelocity)
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);

		if (m_pCamera)
			m_pCamera->Move(const_cast<XMFLOAT3&>(xmf3Shift));
	}
}

void CPlayer::Move(FLOAT fxOffset, FLOAT fyOffset, FLOAT fzOffset)
{
}

void CPlayer::Rotate(FLOAT x, FLOAT y, FLOAT z)
{
	DWORD dwCurrentCameraMode{ m_pCamera->GetMode() };

	if ((dwCurrentCameraMode == FIRST_PERSON_CAMERA) || (dwCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;

			if (m_fPitch > +89.0f)
			{
				x -= (m_fPitch - 89.0f);
				m_fPitch = +89.0f;
			}

			if (m_fPitch < -89.0f)
			{
				x -= (m_fPitch + 89.0f);
				m_fPitch = -89.0f;
			}
		}
		if (y != 0.0f)
		{
			m_fYaw += y;

			if (m_fYaw > 360.0f)
				m_fYaw -= 360.0f;

			if (m_fYaw < 0.0f)
				m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f)
			{
				z -= (m_fRoll - 20.0f);
				m_fRoll = +20.0f;
			}

			if (m_fRoll < -20.0f)
			{
				z -= (m_fRoll + 20.0f);
				m_fRoll = -20.0f;
			}
		}

		m_pCamera->Rotate(x, y, z);

		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y)) };

			m_xmf3Look	 = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right	 = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (dwCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_pCamera->Rotate(x, y, z);

		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x)) };
			
			m_xmf3Look	 = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up	 = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}

		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y)) };
			
			m_xmf3Look	 = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right	 = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}

		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z)) };

			m_xmf3Up	 = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right	 = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	m_xmf3Look	 = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right	 = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, TRUE);
	m_xmf3Up	 = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, TRUE);
}

void CPlayer::Update(FLOAT fTimeElapsed)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, FALSE));

	FLOAT fLength{ sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z) };
	FLOAT fMaxVelocityXZ{ m_fMaxVelocityXZ * fTimeElapsed };

	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}

	FLOAT fMaxVelocityY{ m_fMaxVelocityY * fTimeElapsed };

	fLength = ::sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);

	if (fLength > m_fMaxVelocityY)
		m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	XMFLOAT3 xmf3Velocity{ Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, FALSE) };
	
	Move(xmf3Velocity, FALSE);

	UpdateBoundingBox();

	if (m_pPlayerUpdatedContext)
		OnPlayerUpdateCallback(fTimeElapsed);

	m_pCamera->Update(m_xmf3Position, fTimeElapsed);

	if (m_pCameraUpdatedContext)
		OnCameraUpdateCallback(fTimeElapsed);

	m_pCamera->SetLookAt(m_xmf3Position);
	m_pCamera->RegenerateViewMatrix();

	fLength = Vector3::Length(m_xmf3Velocity);
	
	FLOAT fDeceleration{ (m_fFriction * fTimeElapsed) };
	
	if (fDeceleration > fLength)
		fDeceleration = fLength;

	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, TRUE));
}

CCamera* CPlayer::OnChangeCamera(DWORD dwNewCameraMode, DWORD dwCurrentCameraMode)
{
	CCamera* pNewCamera{ nullptr };

	switch (dwNewCameraMode)
	{
	//case FIRST_PERSON_CAMERA:
	//	pNewCamera = new CFirstPersonCamera(m_pCamera);
	//	break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	//case SPACESHIP_CAMERA:
	//	pNewCamera = new CSpaceShipCamera(m_pCamera);
	//	break;
	}

	if (dwCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_xmf3Right	 = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up	 = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look	 = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));

		m_fPitch	 = 0.0f;
		m_fRoll		 = 0.0f;
		m_fYaw		 = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);

		if (m_xmf3Look.x < 0.0f)
			m_fYaw = -m_fYaw;
	}
	else if ((dwNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		m_xmf3Right	 = m_pCamera->GetRightVector();
		m_xmf3Up	 = m_pCamera->GetUpVector();
		m_xmf3Look	 = m_pCamera->GetLookVector();
	}

	if (pNewCamera)
		pNewCamera->SetPlayer(this);

	if (m_pCamera)
		delete m_pCamera;

	return pNewCamera;
}

void CPlayer::Animate(FLOAT fTimeElapsed)
{
	Update(fTimeElapsed);
}

/*�÷��̾��� ��ġ�� ȸ�������κ��� ���� ��ȯ ����� �����ϴ� �Լ�.
�÷��̾��� Right ���Ͱ� ���� ��ȯ ����� ù ��° �� ����, Up ���Ͱ� �� ��° �� ����,
Look ���Ͱ� �� ��° �� ����, �÷��̾��� ��ġ ���Ͱ� �� ��° �຤�Ͱ� �ȴ�.*/
void CPlayer::PrepareRender()
{
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._41 = m_xmf3Position.x;
	m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._42 = m_xmf3Position.y;
	m_xmf4x4World._13 = m_xmf3Right.z; m_xmf4x4World._23 = m_xmf3Up.z; m_xmf4x4World._33 = m_xmf3Look.z; m_xmf4x4World._43 = m_xmf3Position.z;
}

void CPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	DWORD dwCameraMode{ (pCamera) ? pCamera->GetMode() : 0x00 };

	if (dwCameraMode == THIRD_PERSON_CAMERA)
	{
		if (m_pShader)
			m_pShader->Render(pd3dCommandList, pCamera);

		CGameObject::Render(pd3dCommandList, pCamera);
	}
}

//=====================================================================================================================================================

CTerrainPlayer::CTerrainPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, INT nMeshes) : CPlayer(nMeshes)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	CHeightMapTerrain* pTerrain{ static_cast<CHeightMapTerrain*>(pContext) };

	/*�÷��̾��� ��ġ�� ������ ���(y�� ��ǥ�� ������ ���̺��� 1500 ����)�� ����.
	�÷��̾� ��ġ ������ y��ǥ�� ������ ���̺��� ũ�� �߷��� �ۿ��ϵ��� �÷��̾ �����Ͽ����Ƿ�
	�÷��̾�� ���������� �ϰ��ϰ� �ȴ�.*/
	FLOAT fHeight{ pTerrain->GetHeight(pTerrain->GetWidth() * 0.5f, pTerrain->GetLength() * 0.5f) };

	SetPlayerUpdatedContext(pTerrain);
	SetCameraUpdatedContext(pTerrain);

	CMesh* pAirplaneMesh{ new CMesh(pd3dDevice, pd3dCommandList, "Models/FlyerPlayership.bin", FALSE) };

	SetMesh(0, pAirplaneMesh);
	SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetColor(XMFLOAT3(0.0f, 0.25f, 0.875f));

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CPseudoLightingShader* pShader{ new CPseudoLightingShader() };

	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
//	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	SetShader(pShader);
}

CTerrainPlayer::~CTerrainPlayer()
{
}

CCamera* CTerrainPlayer::ChangeCamera(DWORD dwNewCameraMode, FLOAT fTimeElapsed)
{
	DWORD dwCurrentCameraMode{ (m_pCamera) ? m_pCamera->GetMode() : 0x00 };

	if (dwCurrentCameraMode == dwNewCameraMode)
		return m_pCamera;

	switch (dwNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
	{
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);

		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, dwCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);

		break;
	}
	case SPACESHIP_CAMERA:
	{
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);

		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, dwCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);

		break;
	}
	case THIRD_PERSON_CAMERA:
	{
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);

		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, dwCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -50.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);


		break;
	}
	default:
		break;
	}

	Update(fTimeElapsed);

	return m_pCamera;
}

void CTerrainPlayer::OnPlayerUpdateCallback(FLOAT fTimeElapsed)
{
	XMFLOAT3			 xmf3PlayerPosition{ GetPosition() };
	CHeightMapTerrain*	 pTerrain{ static_cast<CHeightMapTerrain*>(m_pPlayerUpdatedContext) };

	/*�������� �÷��̾��� ���� ��ġ (x, z)�� ���� ����(y)�� ���Ѵ�.
	�׸��� �÷��̾� Mesh�� ���̰� 12�̰� �÷��̾��� �߽��� ������ü�� ����̹Ƿ�
	y ���� �޽��� ������ ������ ���ϸ� �÷��̾��� ��ġ�� �ȴ�.*/
	FLOAT				 fHeight{ pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z) + 6.0f };

	/*�÷��̾��� ��ġ ������ y���� �����̸�(���� ���, �߷��� ����Ǵ� ���) �÷��̾��� ��ġ ������ y���� ���� �۾����� �ȴ�.
	�̶� �÷��̾��� ���� ��ġ ������ y���� ������ ����(������ ������ ���� + 6)���� ������ �÷��̾��� �Ϻΰ� ���� �Ʒ��� �ְ� �ȴ�.
	�̷��� ��츦 �����Ϸ��� �÷��̾��� �ӵ� ������ y���� 0���� ����� �÷��̾��� ��ġ ������ y���� ������ ����(������ ������ ���� + 6)�� �����Ѵ�.
	�׷��� �÷��̾�� �׻� ���� ���� �ְ� �ȴ�.*/
	if (xmf3PlayerPosition.y < fHeight)
	{
		XMFLOAT3 xmf3PlayerVelocity{ GetVelocity() };

		xmf3PlayerVelocity.y = 0.0f;
		SetVelocity(xmf3PlayerVelocity);

		xmf3PlayerPosition.y = fHeight;
		SetPosition(xmf3PlayerPosition);
	}
}

void CTerrainPlayer::OnCameraUpdateCallback(FLOAT fTimeElapsed)
{
	XMFLOAT3			 xmf3CameraPosition{ m_pCamera->GetPosition() };

	/*Height Map���� ī�޶��� ���� ��ġ (x, z)�� ���� ������ ����(y ��)�� ���Ѵ�.
	�� ���� ī�޶��� ��ġ ������ y�� ���� ũ�� ī�޶� ������ �Ʒ��� �ְ� �ȴ�.
	�̷��� �Ǹ� ���� �׸��� ���ʰ� ���� ������ �׷����� �ʴ� ��찡 �߻��Ѵ�(ī�޶� ���� �ȿ� �����Ƿ� �ﰢ���� ���ε� ������ �ٲ��).
	�̷��� ��찡 �߻����� �ʵ��� ī�޶��� ��ġ ������ y���� �ּҰ��� (������ ���� + 5)�� �����Ѵ�.
	ī�޶��� ��ġ ������ y���� �ּҰ��� ������ ��� ��ġ���� ī�޶� ���� �Ʒ��� ��ġ���� �ʵ��� �����ؾ� �Ѵ�.*/
	CHeightMapTerrain*	 pTerrain{ static_cast<CHeightMapTerrain*>(m_pCameraUpdatedContext) };
	FLOAT				 fHeight{ pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z) + 5.0f };

	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);

		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA)
		{
			// 3��Ī ī�޶��� ��� ī�޶� ��ġ(y��ǥ)�� ����Ǿ����Ƿ� ī�޶� �÷��̾ �ٶ󺸵��� �Ѵ�.
			CThirdPersonCamera* p3rdPersonCamera{ static_cast<CThirdPersonCamera*>(m_pCamera) };

			p3rdPersonCamera->SetLookAt(GetPosition());
		}
	}
}