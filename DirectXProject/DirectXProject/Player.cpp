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
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);

	if (m_pCamera)
		m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::ReleaseShaderVariables()
{
	CGameObject::ReleaseShaderVariables();

	if (m_pCamera)
		m_pCamera->ReleaseShaderVariables();
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
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

/*ī�޶� ������ �� ChangeCamera() �Լ����� ȣ��Ǵ� �Լ�
dwCurrentCameraMode�� ���� ī�޶��� ����̰� nNewCameraMode�� ���� ������ ī�޶� ����̴�.*/
CCamera* CPlayer::OnChangeCamera(DWORD dwNewCameraMode, DWORD dwCurrentCameraMode)
{
	// ���ο� ī�޶��� ��忡 ���� ī�޶� ���� �����Ѵ�.
	CCamera* pNewCamera{ new CThirdPersonCamera(m_pCamera) };

	if (pNewCamera)
	{
		pNewCamera->SetMode(dwNewCameraMode);

		// ���� ī�޶� ����ϴ� �÷��̾� ��ü ����
		pNewCamera->SetPlayer(this);
	}

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

	if (m_pShader)
		m_pShader->Render(pd3dCommandList, pCamera);

	CGameObject::Render(pd3dCommandList, pCamera);
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

	SetPosition(XMFLOAT3(pTerrain->GetWidth() * 0.5f, fHeight + 1500.0f, pTerrain->GetLength() * 0.5f));
	SetPlayerUpdatedContext(pTerrain);
	SetCameraUpdatedContext(pTerrain);

	CCubeMeshDiffused* pCubeMesh{ new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 4.0f, 12.0f, 4.0f) };

	SetMesh(0, pCubeMesh);

	CPlayerShader* pShader{ new CPlayerShader() };

	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	
	SetShader(pShader);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CTerrainPlayer::~CTerrainPlayer()
{
}

CCamera* CTerrainPlayer::ChangeCamera(DWORD nNewCameraMode, FLOAT fTimeElapsed)
{
	DWORD dwCurrentCameraMode{ (m_pCamera) ? m_pCamera->GetMode() : 0x00 };

	if (dwCurrentCameraMode == nNewCameraMode)
		return m_pCamera;

	switch (nNewCameraMode)
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