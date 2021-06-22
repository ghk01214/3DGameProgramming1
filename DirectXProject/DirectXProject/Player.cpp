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

		// 화살표 키 '↑'를 누르면 로컬 z축 방향으로 이동(전진)한다.'↓'를 누르면 반대 방향으로 이동한다.
		if (dwDirection & DIR_FORWARD)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);

		if (dwDirection & DIR_BACKWARD)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);

		// 화살표 키 '→'를 누르면 로컬 x축 방향으로 이동한다.'←'를 누르면 반대 방향으로 이동한다.
		if (dwDirection & DIR_RIGHT)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);

		if (dwDirection & DIR_LEFT)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		// 'Page Up'을 누르면 로컬 y축 방향으로 이동한다.'Page Down'을 누르면 반대 방향으로 이동한다.
		if (dwDirection & DIR_UP)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);

		if (dwDirection & DIR_DOWN)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		// 플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다.
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

/*카메라를 변경할 때 ChangeCamera() 함수에서 호출되는 함수
dwCurrentCameraMode는 현재 카메라의 모드이고 nNewCameraMode는 새로 설정할 카메라 모드이다.*/
CCamera* CPlayer::OnChangeCamera(DWORD dwNewCameraMode, DWORD dwCurrentCameraMode)
{
	// 새로운 카메라의 모드에 따라 카메라를 새로 생성한다.
	CCamera* pNewCamera{ new CThirdPersonCamera(m_pCamera) };

	if (pNewCamera)
	{
		pNewCamera->SetMode(dwNewCameraMode);

		// 현재 카메라를 사용하는 플레이어 객체 설정
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

/*플레이어의 위치와 회전축으로부터 월드 변환 행렬을 생성하는 함수.
플레이어의 Right 벡터가 월드 변환 행렬의 첫 번째 행 벡터, Up 벡터가 두 번째 행 벡터,
Look 벡터가 세 번째 행 벡터, 플레이어의 위치 벡터가 네 번째 행벡터가 된다.*/
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

	/*플레이어의 위치를 지형의 가운데(y축 좌표는 지형의 높이보다 1500 높게)로 설정.
	플레이어 위치 벡터의 y좌표가 지형의 높이보다 크고 중력이 작용하도록 플레이어를 설정하였으므로
	플레이어는 점차적으로 하강하게 된다.*/
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

	/*지형에서 플레이어의 현재 위치 (x, z)의 지형 높이(y)를 구한다.
	그리고 플레이어 Mesh의 높이가 12이고 플레이어의 중심이 직육면체의 가운데이므로
	y 값에 메쉬의 높이의 절반을 더하면 플레이어의 위치가 된다.*/
	FLOAT				 fHeight{ pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z) + 6.0f };

	/*플레이어의 위치 벡터의 y값이 음수이면(예를 들어, 중력이 적용되는 경우) 플레이어의 위치 벡터의 y값이 점점 작아지게 된다.
	이때 플레이어의 현재 위치 벡터의 y값이 지형의 높이(실제로 지형의 높이 + 6)보다 작으면 플레이어의 일부가 지형 아래에 있게 된다.
	이러한 경우를 방지하려면 플레이어의 속도 벡터의 y값을 0으로 만들고 플레이어의 위치 벡터의 y값을 지형의 높이(실제로 지형의 높이 + 6)로 설정한다.
	그러면 플레이어는 항상 지형 위에 있게 된다.*/
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

	/*Height Map에서 카메라의 현재 위치 (x, z)에 대한 지형의 높이(y 값)를 구한다.
	이 값이 카메라의 위치 벡터의 y값 보다 크면 카메라가 지형의 아래에 있게 된다.
	이렇게 되면 다음 그림의 왼쪽과 같이 지형이 그려지지 않는 경우가 발생한다(카메라가 지형 안에 있으므로 삼각형의 와인딩 순서가 바뀐다).
	이러한 경우가 발생하지 않도록 카메라의 위치 벡터의 y값의 최소값은 (지형의 높이 + 5)로 설정한다.
	카메라의 위치 벡터의 y값의 최소값은 지형의 모든 위치에서 카메라가 지형 아래에 위치하지 않도록 설정해야 한다.*/
	CHeightMapTerrain*	 pTerrain{ static_cast<CHeightMapTerrain*>(m_pCameraUpdatedContext) };
	FLOAT				 fHeight{ pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z) + 5.0f };

	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);

		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA)
		{
			// 3인칭 카메라의 경우 카메라 위치(y좌표)가 변경되었으므로 카메라가 플레이어를 바라보도록 한다.
			CThirdPersonCamera* p3rdPersonCamera{ static_cast<CThirdPersonCamera*>(m_pCamera) };

			p3rdPersonCamera->SetLookAt(GetPosition());
		}
	}
}