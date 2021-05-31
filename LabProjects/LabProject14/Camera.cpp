#include "framework.h"
#include "Camera.h"
#include "Player.h"

CCamera::CCamera()
{
	m_xmf4x4View			 = Matrix4x4::Identity();
	m_xmf4x4Projection		 = Matrix4x4::Identity();
	m_d3dViewport			 = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_d3dScissorRect		 = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
	m_xmf3Position			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right				 = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Look				 = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Up				 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fPitch				 = 0.0f;
	m_fRoll					 = 0.0f;
	m_fYaw					 = 0.0f;
	m_xmf3Offset			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fTimeLag				 = 0.0f;
	m_xmf3LookAtWorld		 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_dwMode					 = 0x00;
	m_pPlayer				 = NULL;
}

CCamera::CCamera(CCamera* pCamera)
{
	if (pCamera)
	{
		// 카메라가 이미 있으면 기존 카메라의 정보를 새로운 카메라에 복사
		*this = *pCamera;
	}
	else
	{
		// 카메라가 없으면 기본 정보 설정
		m_xmf4x4View			 = Matrix4x4::Identity();
		m_xmf4x4Projection		 = Matrix4x4::Identity();
		m_d3dViewport			 = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
		m_d3dScissorRect		 = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
		m_xmf3Position			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf3Right				 = XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_xmf3Look				 = XMFLOAT3(0.0f, 0.0f, 1.0f);
		m_xmf3Up				 = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_fPitch				 = 0.0f;
		m_fRoll					 = 0.0f;
		m_fYaw					 = 0.0f;
		m_xmf3Offset			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_fTimeLag				 = 0.0f;
		m_xmf3LookAtWorld		 = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_dwMode					 = 0x00;
		m_pPlayer				 = NULL;
	}
}

CCamera::~CCamera()
{
}

void CCamera::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CCamera::ReleaseShaderVariables()
{
}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4View;

	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4View, 0);							// Root 파라미터 인덱스 1의
	
	XMFLOAT4X4 xmf4x4Projection;

	XMStoreFloat4x4(&xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4Projection, 16);
}

void CCamera::GenerateViewMatrix()
{
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, m_xmf3LookAtWorld, m_xmf3Up);
}

void CCamera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf3Position		 = xmf3Position;
	m_xmf3LookAtWorld	 = xmf3LookAt;
	m_xmf3Up			 = xmf3Up;

	GenerateViewMatrix();
}

void CCamera::RegenerateViewMatrix()
{
	// 카메라의 z축을 기준으로 카메라의 좌표 축들이 직교하도록 카메라 변환 행렬 갱신
	m_xmf3Look	 = Vector3::Normalize(m_xmf3Look);							// 카메라의 z축 벡터 정규화
	m_xmf3Right	 = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, TRUE);		// 카메라의 z축과 y축에 수직인 벡터를 x축으로 설정한다.
	m_xmf3Up	 = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, TRUE);	// 카메라의 z축과 x축에 수직인 벡터를 y축으로 설정한다.
	
	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;

	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);
}

void CCamera::GenerateProjectionMatrix(FLOAT fNearPlaneDistance, FLOAT fFarPlaneDistance, FLOAT fAspectRatio, FLOAT fFOVAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}

void CCamera::SetViewport(INT xTopLeft, INT yTopLeft, INT nWidth, INT nHeight, FLOAT fMinZ, FLOAT fMaxZ)
{
	m_d3dViewport.TopLeftX	 = (FLOAT)xTopLeft;
	m_d3dViewport.TopLeftY	 = (FLOAT)yTopLeft;
	m_d3dViewport.Width		 = (FLOAT)nWidth;
	m_d3dViewport.Height	 = (FLOAT)nHeight;
	m_d3dViewport.MinDepth	 = fMinZ;
	m_d3dViewport.MaxDepth	 = fMaxZ;
}

void CCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left		 = xLeft;
	m_d3dScissorRect.top		 = yTop;
	m_d3dScissorRect.right		 = xRight;
	m_d3dScissorRect.bottom		 = yBottom;
}

void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

void CCamera::Move(XMFLOAT3& xmf3Shift)
{
	m_xmf3Position.x += xmf3Shift.x;
	m_xmf3Position.y += xmf3Shift.y; m_xmf3Position.z += xmf3Shift.z;
}

//================================================================================

CFirstPersonCamera::CFirstPersonCamera(CCamera* pCamera) : CCamera(pCamera)
{
	m_dwMode = FIRST_PERSON_CAMERA;

	if (pCamera)
	{
		/*1인칭 카메라로 변경하기 이전의 카메라가 Spaceship 카메라이면 카메라의 Up 벡터를 월드 좌표의 y축이 되도록 한다.
		이것은 Spaceship 카메라의 로컬 y축 벡터가 어떤 방향이든지 1인칭 카메라(대부분 사람인 경우)의 로컬 y축 벡터가
		월드 좌표의 y축이 되도록, 즉, 똑바로 서있는 형태로 설정한다는 의미이다.
		그리고 로컬 x축 벡터와 로컬 z축 벡터의 y좌표가 0.0f가 되도록 한다.
		이것은 다음 그림과 같이 로컬 x축 벡터와 로컬 z축 벡터를 xz평면(지면)으로 투영하는 것을 의미한다.
		즉, 1인칭 카메라의 로컬 x축 벡터와 로컬 z축 벡터는 xz평면에 평행하다.*/
		if (pCamera->GetMode() == SPACESHIP_CAMERA)
		{
			m_xmf3Up		 = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_xmf3Right.y	 = 0.0f;
			m_xmf3Look.y	 = 0.0f;
			m_xmf3Right		 = Vector3::Normalize(m_xmf3Right);
			m_xmf3Look		 = Vector3::Normalize(m_xmf3Look);
		}
	}
}

void CFirstPersonCamera::Rotate(FLOAT fPitch, FLOAT fYaw, FLOAT fRoll)
{
	if (fPitch != 0.0f)
	{
		// 카메라의 로컬 x축을 기준으로 회전하는 행렬을 생성(사람의 경우 고개를 끄떡이는 동작)
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch)) };

		// 카메라의 로컬 x축, y축, z축을 회전 행렬을 사용하여 회전
		m_xmf3Look	 = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up	 = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right	 = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}

	if (m_pPlayer && (fYaw != 0.0f))
	{
		// 플레이어의 로컬 y축을 기준으로 회전하는 행렬 생성
		XMFLOAT3 xmf3Up{ m_pPlayer->GetUpVector() };
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(fYaw)) };

		// 카메라의 로컬 x축, y축, z축을 회전 행렬을 사용하여 회전
		m_xmf3Look	 = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up	 = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right	 = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}

	if (m_pPlayer && (fRoll != 0.0f))
	{
		// 플레이어의 로컬 z축을 기준으로 회전하는 행렬 생성
		XMFLOAT3 xmf3Look{ m_pPlayer->GetLookVector() };
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look), XMConvertToRadians(fRoll)) };
		
		// 카메라의 위치 벡터를 플레이어 좌표계로 표현(Offset 벡터)
		m_xmf3Position = Vector3::Subtract(m_xmf3Position, m_pPlayer->GetPosition());
		// Offset 벡터 회전
		m_xmf3Position = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		// 회전한 카메라의 위치를 월드 좌표계로 표현
		m_xmf3Position = Vector3::Add(m_xmf3Position, m_pPlayer->GetPosition());

		// 카메라의 로컬 x축, y축, z축 회전
		m_xmf3Look	 = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up	 = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right	 = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
}

//================================================================================

CThirdPersonCamera::CThirdPersonCamera(CCamera* pCamera) : CCamera(pCamera)
{
	m_dwMode = THIRD_PERSON_CAMERA;

	if (pCamera)
	{
		/*3인칭 카메라로 변경하기 이전의 카메라가 Spaceship 카메라이면 카메라의 Up 벡터를 월드좌표의 y축이 되도록 한다.
		이것은 Spaceship 카메라의 로컬 y축 벡터가 어떤 방향이든지 3인칭 카메라(대부분 사람인 경우)의 로컬 y축 벡터가
		월드좌표의 y축이 되도록, 즉, 똑바로 서있는 형태로 설정한다는 의미.
		그리고 로컬 x축 벡터와 로컬 z축 벡터의 y좌표가 0.0f가 되도록 한다.
		이것은 로컬 x축 벡터와 로컬 z축 벡터를 xz평면(지면)으로 투영하는 것을 의미한다.
		즉, 3인칭 카메라의 로컬 x축 벡터와 로컬 z축 벡터는 xz평면에 평행하다.*/
		if (pCamera->GetMode() == SPACESHIP_CAMERA)
		{
			m_xmf3Up		 = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_xmf3Right.y	 = 0.0f;
			m_xmf3Look.y	 = 0.0f;
			m_xmf3Right		 = Vector3::Normalize(m_xmf3Right);
			m_xmf3Look		 = Vector3::Normalize(m_xmf3Look);
		}
	}
}

void CThirdPersonCamera::Update(XMFLOAT3& xmf3LookAt, FLOAT fTimeElapsed)
{
	// 플레이어가 있으면 플레이어의 회전에 따라 3인칭 카메라도 회전
	if (m_pPlayer)
	{
		XMFLOAT4X4	 xmf4x4Rotate{ Matrix4x4::Identity() };
		XMFLOAT3	 xmf3Right{ m_pPlayer->GetRightVector() };
		XMFLOAT3	 xmf3Up{ m_pPlayer->GetUpVector() };
		XMFLOAT3	 xmf3Look{ m_pPlayer->GetLookVector() };

		// 플레이어의 로컬 x축, y축, z축 벡터로부터 회전 행렬(플레이어와 같은 방향을 나타내는 행렬) 생성
		xmf4x4Rotate._11 = xmf3Right.x; xmf4x4Rotate._21 = xmf3Up.x; xmf4x4Rotate._31 = xmf3Look.x;
		xmf4x4Rotate._12 = xmf3Right.y; xmf4x4Rotate._22 = xmf3Up.y; xmf4x4Rotate._32 = xmf3Look.y;
		xmf4x4Rotate._13 = xmf3Right.z; xmf4x4Rotate._23 = xmf3Up.z; xmf4x4Rotate._33 = xmf3Look.z;

		// 카메라 Offset 벡터를 회전 행렬로 변환(회전)
		XMFLOAT3 xmf3Offset{ Vector3::TransformCoord(m_xmf3Offset, xmf4x4Rotate) };

		// 회전한 카메라의 위치는 플레이어의 위치에 회전한 카메라 Offset 벡터를 더한 것
		XMFLOAT3 xmf3Position{ Vector3::Add(m_pPlayer->GetPosition(), xmf3Offset) };

		// 현재의 카메라의 위치에서 회전한 카메라의 위치까지의 방향과 거리를 나타내는 벡터
		XMFLOAT3 xmf3Direction{ Vector3::Subtract(xmf3Position, m_xmf3Position) };		
		FLOAT fLength{ Vector3::Length(xmf3Direction) };

		xmf3Direction = Vector3::Normalize(xmf3Direction);

		/*3인칭 카메라의 Lag는 플레이어가 회전하더라도 카메라가 동시에 따라서 회전하지 않고 약간의 시차를 두고 회전하는 효과를 구현하기 위한 것이다.
		m_fTimeLag가 1보다 크면 fTimeLagScale이 작아지고 실제 회전(이동)이 적게 일어날 것이다.
		m_fTimeLag가 0이 아닌 경우 fTimeElapsed를 곱하고 있으므로 3인칭 카메라는 1초의 시간동안 (1.0f / m_fTimeLag)의 비율만큼 플레이어의 회전을 따라가게 될 것이다.*/
		FLOAT fTimeLagScale{ (m_fTimeLag) ? fTimeElapsed * (1.0f / m_fTimeLag) : 1.0f };
		FLOAT fDistance{ fLength * fTimeLagScale };

		if (fDistance > fLength)
			fDistance = fLength;

		if (fLength < 0.01f)
			fDistance = fLength;

		if (fDistance > 0)
		{
			// 실제로 카메라를 회전하지 않고 이동(회전의 각도가 작은 경우 회전 이동은 선형 이동과 거의 같다)
			m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Direction, fDistance);

			// 카메라가 플레이어를 바라보도록 설정
			SetLookAt(xmf3LookAt);
		}
	}
}

void CThirdPersonCamera::SetLookAt(XMFLOAT3& xmf3LookAt)
{
	// 현재 카메라의 위치에서 플레이어를 바라보기 위한 카메라 변환 행렬 생성
	XMFLOAT4X4 mtxLookAt{ Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, m_pPlayer->GetUpVector()) };
	
	// 카메라 변환 행렬에서 카메라의 x축, y축, z축 계산
	m_xmf3Right	 = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_xmf3Up	 = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_xmf3Look	 = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}

//================================================================================

CSpaceShipCamera::CSpaceShipCamera(CCamera* pCamera) : CCamera(pCamera)
{
	m_dwMode = SPACESHIP_CAMERA;
}

// Spaceship 카메라를 플레이어의 로컬 x축(Right), y축(Up), z축(Look)을 기준으로 회전하는 함수
void CSpaceShipCamera::Rotate(FLOAT fPitch, FLOAT fYaw, FLOAT fRoll)
{
	if (m_pPlayer && (fPitch != 0.0f))
	{
		// 플레이어의 로컬 x축에 대한 x각도의 회전 행렬 계산
		XMFLOAT3 xmf3Right{ m_pPlayer->GetRightVector() };
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&xmf3Right), XMConvertToRadians(fPitch)) };

		// 카메라의 로컬 x축, y축, z축 회전
		m_xmf3Right	 = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up	 = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look	 = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);

		// 카메라의 위치 벡터에서 플레이어의 위치 벡터를 뺀다. 결과는 플레이어 위치를 기준(원점)으로 한 카메라의 위치 벡터
		m_xmf3Position = Vector3::Subtract(m_xmf3Position, m_pPlayer->GetPosition());

		// 플레이어의 위치를 중심으로 카메라의 위치 벡터(플레이어를 기준으로 한) 회전
		m_xmf3Position = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		
		// 회전시킨 카메라의 위치 벡터에 플레이어의 위치를 더하여 카메라의 위치 벡터를 구한다.
		m_xmf3Position = Vector3::Add(m_xmf3Position, m_pPlayer->GetPosition());
	}

	if (m_pPlayer && (fYaw != 0.0f))
	{
		XMFLOAT3 xmf3Up{ m_pPlayer->GetUpVector() };
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(fYaw)) };

		m_xmf3Right		 = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up		 = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look		 = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Position	 = Vector3::Subtract(m_xmf3Position, m_pPlayer->GetPosition());
		m_xmf3Position	 = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		m_xmf3Position	 = Vector3::Add(m_xmf3Position, m_pPlayer->GetPosition());
	}

	if (m_pPlayer && (fRoll != 0.0f))
	{
		XMFLOAT3 xmf3Look{ m_pPlayer->GetLookVector() };
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look), XMConvertToRadians(fRoll)) };
		
		m_xmf3Right		 = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up		 = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look		 = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Position	 = Vector3::Subtract(m_xmf3Position, m_pPlayer->GetPosition());
		m_xmf3Position	 = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		m_xmf3Position	 = Vector3::Add(m_xmf3Position, m_pPlayer->GetPosition());
	}
}
