#include "framework.h"
#include "Player.h"
#include "Shader.h"

CPlayer::CPlayer()
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
	XMFLOAT3 xmf3Shift{ XMFLOAT3(0, 0, 0) };

	if (dwDirection)
	{
		// 화살표 키 '→'를 누르면 로컬 x축 방향으로 이동한다.'←'를 누르면 반대 방향으로 이동한다.
		if (dwDirection & DIR_RIGHT)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		
		if (dwDirection & DIR_LEFT)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);

		if (dwDirection & DIR_UP)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
	}

	xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);

	Move(xmf3Shift, bVelocity);
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

	// 카메라 모드가 3인칭이면 플레이어 객체를 렌더링
	if (dwCameraMode == THIRD_PERSON_CAMERA)
	{
		if (m_pShader)
			m_pShader->Render(pd3dCommandList, pCamera);

		CGameObject::Render(pd3dCommandList, pCamera);
	}
}

//=====================================================================================================================================================

CCarPlayer::CCarPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	// 비행기 Mesh 생성
	CMesh* pCarMesh{ new CCarMeshDiffused(pd3dDevice, pd3dCommandList, 6.0f, 6.0f, 6.0f, XMFLOAT4(0.0f, 255.0f, 0.0f, 0.0f)) };

	SetMesh(pCarMesh);

	// 플레이어의 카메라를 Spaceship 카메라로 변경(생성)
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	// 플레이어를 위한 Shader 변수 생성
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	// 플레이어의 위치 설정
	SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	// 플레이어(비행기) Mesh를 Rendering할 때 사용할 Shader 생성
	CPlayerShader* pShader{ new CPlayerShader() };

	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);

	SetShader(pShader);
}

CCarPlayer::~CCarPlayer()
{
}

// 카메라를 변경할 때 호출되는 함수이다.nNewCameraMode는 새로 설정할 카메라 모드이다.
CCamera* CCarPlayer::ChangeCamera(DWORD dwNewCameraMode, FLOAT fTimeElapsed)
{
	DWORD dwCurrentCameraMode{ (m_pCamera) ? m_pCamera->GetMode() : 0x00 };
	
	if (dwCurrentCameraMode == dwNewCameraMode)
		return m_pCamera;

	switch (dwNewCameraMode)
	{
	case THIRD_PERSON_CAMERA:
	{
		// 플레이어의 특성을 3인칭 카메라 모드에 맞게 변경(지연 효과와 카메라 Offset 설정)
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, 1.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);

		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, dwCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 30.0f, -25.0f));									// 카메라 위치 설정
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

		break;
	}
	default:
		break;
	}

	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));

	// 플레이어를 시간의 경과에 따라 갱신(위치와 방향을 변경: 속도, 마찰력, 중력 등을 처리)
	Update(fTimeElapsed);

	return m_pCamera;
}

void CCarPlayer::PrepareRender()
{
	CPlayer::PrepareRender();

	// 비행기 모델을 그리기 전에 x축으로 90도 회전
	XMMATRIX mtxRotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f) };
	
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}
