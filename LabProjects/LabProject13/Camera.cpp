#include "framework.h"
#include "Camera.h"

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
	m_nMode					 = 0x00;

	m_pPlayer				 = nullptr;
}

CCamera::CCamera(CCamera* pCamera)
{
	if (pCamera)
		// ī�޶� �̹� ������ ���� ī�޶��� ������ ���ο� ī�޶� �����Ѵ�.
		*this = *pCamera;
	else
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
		m_nMode					 = 0x00;

		m_pPlayer				 = nullptr;
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
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4View, 0);							// Root �Ķ���� �ε��� 1��
	
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
	// ī�޶��� z���� �������� ī�޶��� ��ǥ����� �����ϵ��� ī�޶� ��ȯ ����� �����Ѵ�.
	// ī�޶��� z�� ���͸� ����ȭ�Ѵ�.
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	// ī�޶��� z��� y�࿡ ������ ���͸� x������ �����Ѵ�.
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	// ī�޶��� z��� x�࿡ ������ ���͸� y������ �����Ѵ�.
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 =
		m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 =
		m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 =
		m_xmf3Look.z;
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

//=========================================================================================

CSpaceShipCamera::CSpaceShipCamera(CCamera* pCamera) : CCamera(pCamera)
{
	m_nMode = SPACESHIP_CAMERA;
}

void CSpaceShipCamera::Rotate(FLOAT fPitch, FLOAT fYaw, FLOAT fRoll)
{
	if (m_pPlayer && (fPitch != 0.0f))
	{
		// �÷��̾��� ���� x�࿡ ���� x ������ ȸ�� ��� ���
		XMFLOAT3 xmf3Right{ m_pPlayer->GetRightVector() };
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&xmf3Right), XMConvertToRadians(fPitch)) };

		// ī�޶��� ���� x��, y��, z�� ȸ��
		m_xmf3Right		 = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up		 = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look		 = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);

		// ī�޶��� ��ġ ���Ϳ��� �÷��̾��� ��ġ ���͸� ����. ��� = �÷��̾� ��ġ�� ����(����)���� �� ī�޶��� ��ġ ����
		m_xmf3Position = Vector3::Subtract(m_xmf3Position, m_pPlayer->GetPosition());
		// �÷��̾��� ��ġ�� �߽����� ī�޶��� ��ġ ����(�÷��̾ �������� ��) ȸ��
		m_xmf3Position = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		// ȸ����Ų ī�޶��� ��ġ ���Ϳ� �÷��̾��� ��ġ�� ���Ͽ� ī�޶��� ��ġ ���� ���
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

//=========================================================================================

CFirstPersonCamera::CFirstPersonCamera(CCamera* pCamera)
{
	m_nMode = FIRST_PERSON_CAMERA;

	if (pCamera)
	{
		/*1��Ī ī�޶�� �����ϱ� ������ ī�޶� Spaceship ī�޶��̸� ī�޶��� Up ���͸� ���� ��ǥ�� y���� �ǵ��� �Ѵ�.
		�̰��� Spaceship ī�޶��� ���� y�� ���Ͱ� � �����̵��� 1��Ī ī�޶�(��κ� ����� ���)�� ���� y�� ���Ͱ�
		������ǥ�� y���� �ǵ���, ��, �ȹٷ� ���ִ� ���·� �����Ѵٴ� �ǹ��̴�.
		�׸��� ���� x�� ���Ϳ� ���� z�� ������ y��ǥ�� 0.0f�� �ǵ��� �Ѵ�.
		�̰��� ���� x�� ���Ϳ� ���� z�� ���͸� xz���(����)���� �����ϴ� ���� �ǹ��Ѵ�.
		��, 1��Ī ī�޶��� ���� x�� ���Ϳ� ���� z�� ���ʹ� xz��鿡 �����ϴ�.*/

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
		// ī�޶��� ���� x���� �������� ȸ���ϴ� ��� ����. ����� ��� ������ �����̴� ����.
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch)) };

		// ī�޶��� ���� x��, y��, z���� ȸ�� ����� ����Ͽ� ȸ��
		m_xmf3Look	 = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up	 = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right	 = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}

	if (m_pPlayer && (fYaw != 0.0f))
	{
		// �÷��̾��� ���� y���� �������� ȸ���ϴ� ����� �����Ѵ�.
		XMFLOAT3 xmf3Up{ m_pPlayer->GetUpVector() };
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(fYaw)) };

		// ī�޶��� ���� x��, y��, z���� ȸ�� ����� ����Ͽ� ȸ��
		m_xmf3Look	 = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up	 = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right	 = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}

	if (m_pPlayer && (fRoll != 0.0f))
	{
		// �÷��̾��� ���� z���� �������� ȸ���ϴ� ����� �����Ѵ�.
		XMFLOAT3 xmf3Look{ m_pPlayer->GetLookVector() };
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look), XMConvertToRadians(fRoll)) };

		// ī�޶��� ��ġ ���͸� �÷��̾� ��ǥ��� ǥ��(Offset ����)
		m_xmf3Position = Vector3::Subtract(m_xmf3Position, m_pPlayer->GetPosition());
		// Offset ���� ���͸� ȸ��
		m_xmf3Position = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		// ȸ���� ī�޶��� ��ġ�� ���� ��ǥ��� ǥ��
		m_xmf3Position = Vector3::Add(m_xmf3Position, m_pPlayer->GetPosition());
		
		// ī�޶��� ���� x��, y��, z�� ȸ��
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
}

//=========================================================================================

CThirdPersonCamera::CThirdPersonCamera(CCamera* pCamera) : CCamera(pCamera)
{
	m_nMode = THIRD_PERSON_CAMERA;

	if (pCamera)
	{
		/*3��Ī ī�޶�� �����ϱ� ������ ī�޶� Spaceship ī�޶��̸� ī�޶��� Up ���͸� ������ǥ�� y���� �ǵ��� �Ѵ�.
		�̰��� Spaceship ī�޶��� ���� y�� ���Ͱ� � �����̵��� 3��Ī ī�޶�(��κ� ����� ���)�� ���� y�� ���Ͱ�
		������ǥ�� y���� �ǵ���, ��, �ȹٷ� ���ִ� ���·� �����Ѵٴ� �ǹ��̴�.
		�׸��� ���� x�� ���Ϳ� ���� z�� ������ y��ǥ�� 0.0f�� �ǵ��� �Ѵ�.
		�̰��� ���� x�� ���Ϳ� ���� z�� ���͸� xz���(����)���� �����ϴ� ���� �ǹ��Ѵ�.
		��, 3��Ī ī�޶��� ���� x�� ���Ϳ� ���� z�� ���ʹ� xz��鿡 �����ϴ�.*/

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
	// �÷��̾ ������ �÷��̾��� ȸ���� ���� 3��Ī ī�޶� ȸ���ؾ� �Ѵ�
	if (m_pPlayer)
	{
		XMFLOAT4X4	 xmf4x4Rotate{ Matrix4x4::Identity() };
		XMFLOAT3	 xmf3Right{ m_pPlayer->GetRightVector() };
		XMFLOAT3	 xmf3Up{ m_pPlayer->GetUpVector() };
		XMFLOAT3	 xmf3Look{ m_pPlayer->GetLookVector() };

		// �÷��̾��� ���� x��, y��, z�� ���ͷκ��� ȸ�� ���(�÷��̾�� ���� ������ ��Ÿ���� ���) ����
		xmf4x4Rotate._11 = xmf3Right.x; xmf4x4Rotate._21 = xmf3Up.x; xmf4x4Rotate._31 = xmf3Look.x;
		xmf4x4Rotate._12 = xmf3Right.y; xmf4x4Rotate._22 = xmf3Up.y; xmf4x4Rotate._32 = xmf3Look.y;
		xmf4x4Rotate._13 = xmf3Right.z; xmf4x4Rotate._23 = xmf3Up.z; xmf4x4Rotate._33 = xmf3Look.z;

		// ī�޶� Offset ���͸� ȸ�� ��ķ� ��ȯ(ȸ��)
		XMFLOAT3 xmf3Offset{ Vector3::TransformCoord(m_xmf3Offset, xmf4x4Rotate) };
		// ȸ���� ī�޶��� ��ġ�� �÷��̾��� ��ġ�� ȸ���� ī�޶� Offset ���͸� ���� ��
		XMFLOAT3 xmf3Position{ Vector3::Add(m_pPlayer->GetPosition(), xmf3Offset) };
		// ������ ī�޶��� ��ġ���� ȸ���� ī�޶��� ��ġ������ ����� �Ÿ��� ��Ÿ���� ����
		XMFLOAT3 xmf3Direction{ Vector3::Subtract(xmf3Position, m_xmf3Position) };

		FLOAT fLength{ Vector3::Length(xmf3Direction) };
		xmf3Direction = Vector3::Normalize(xmf3Direction);

		/*3��Ī ī�޶��� Lag�� �÷��̾ ȸ���ϴ��� ī�޶� ���ÿ� ���� ȸ������ �ʰ� �ణ�� ������ �ΰ� ȸ���ϴ� ȿ���� �����ϱ� ���� ���̴�.
		m_fTimeLag�� 1���� ũ�� fTimeLagScale�� �۾����� ���� ȸ��(�̵�)�� ���� �Ͼ ���̴�.
		m_fTimeLag�� 0�� �ƴ� ��� fTimeElapsed�� ���ϰ� �����Ƿ� 3��Ī ī�޶�� 1���� �ð����� (1.0f / m_fTimeLag)�� ������ŭ �÷��̾��� ȸ���� ���󰡰� �� ���̴�.*/

		FLOAT fTimeLagScale{ (m_fTimeLag) ? fTimeElapsed * (1.0f / m_fTimeLag) : 1.0f };
		FLOAT fDistance{ fLength * fTimeLagScale };

		if (fDistance > fLength)
			fDistance = fLength;

		if (fLength < 0.01f)
			fDistance = fLength;

		if (fDistance > 0)
		{
			// ������ ī�޶� ȸ������ �ʰ� �̵�(ȸ���� ������ ���� ��� ȸ�� �̵��� ���� �̵��� ���� ����).
			m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Direction, fDistance);
			
			// ī�޶� �÷��̾ �ٶ󺸵��� �Ѵ�.
			SetLookAt(xmf3LookAt);
		}
	}
}

void CThirdPersonCamera::SetLookAt(XMFLOAT3& xmf3LookAt)
{
	// ���� ī�޶��� ��ġ���� �÷��̾ �ٶ󺸱� ���� ī�޶� ��ȯ ��� ����
	XMFLOAT4X4 mtxLookAt{ Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, m_pPlayer->GetUpVector()) };

	// ī�޶� ��ȯ ��Ŀ��� ī�޶��� x��, y��, z���� ���Ѵ�.
	m_xmf3Right	 = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_xmf3Up	 = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_xmf3Look	 = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}