#include "StdAfx.h"
#include "GameObject.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
inline FLOAT RandF(FLOAT fMin, FLOAT fMax)
{
	return fMin + ((FLOAT)rand() / (FLOAT)RAND_MAX) * (fMax - fMin);
}

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (TRUE)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);

		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne))
			return XMVector3Normalize(v);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject()
{
	m_pMesh = nullptr;
	m_xmf4x4World = Matrix4x4::Identity();

	m_dwColor = RGB(0, 0, 0);

	m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_fMovingSpeed = 0.0f;
	m_fMovingRange = 0.0f;

	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 0.05f;
}

CGameObject::CGameObject(CMesh* pMesh) : CGameObject()
{
	m_pMesh = pMesh;
}

CGameObject::~CGameObject()
{
	if (m_pMesh)
		m_pMesh->Release();
}

void CGameObject::SetPosition(FLOAT x, FLOAT y, FLOAT z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3& xmf3Position)
{
	m_xmf4x4World._41 = xmf3Position.x;
	m_xmf4x4World._42 = xmf3Position.y;
	m_xmf4x4World._43 = xmf3Position.z;
}

XMFLOAT3 CGameObject::GetPosition()
{
	return XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
}

XMFLOAT3 CGameObject::GetLook()
{
	XMFLOAT3 xmf3LookAt(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);

	xmf3LookAt = Vector3::Normalize(xmf3LookAt);

	return xmf3LookAt;
}

XMFLOAT3 CGameObject::GetUp()
{
	XMFLOAT3 xmf3Up(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);

	xmf3Up = Vector3::Normalize(xmf3Up);

	return xmf3Up;
}

XMFLOAT3 CGameObject::GetRight()
{
	XMFLOAT3 xmf3Right(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);

	xmf3Right = Vector3::Normalize(xmf3Right);

	return xmf3Right;
}

void CGameObject::MoveStrate(FLOAT fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();

	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Right, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(FLOAT fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();

	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Up, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(FLOAT fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3LookAt = GetLook();

	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3LookAt, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(FLOAT fPitch, FLOAT fYaw, FLOAT fRoll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(fPitch, fYaw, fRoll);

	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3& xmf3RotationAxis, FLOAT fAngle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle);

	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Move(XMFLOAT3& vDirection, FLOAT fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}

void CGameObject::LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(GetPosition(), xmf3LookTo, xmf3Up);

	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}

void CGameObject::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, xmf3Up);

	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}

void CGameObject::UpdateBoundingBox()
{
	if (m_pMesh)
	{
		m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}

void CGameObject::Animate(FLOAT fElapsedTime)
{
	if (m_fRotationSpeed != 0.0f)
		Rotate(m_xmf3RotationAxis, m_fRotationSpeed * fElapsedTime);

	if (m_fMovingSpeed != 0.0f)
		Move(m_xmf3MovingDirection, m_fMovingSpeed * fElapsedTime);

	UpdateBoundingBox();
}

void CGameObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	if (m_pMesh)
	{
		if (pCamera->IsInFrustum(m_xmOOBB))
		{
			HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
			HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

			m_pMesh->Render(hDCFrameBuffer, m_xmf4x4World, pCamera);

			::SelectObject(hDCFrameBuffer, hOldPen);
			::DeleteObject(hPen);
		}
	}
}

void CGameObject::GenerateRayForPicking(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection)
{
	XMMATRIX xmmtxToModel = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_xmf4x4World) * xmmtxView);
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);

	xmvPickRayOrigin = XMVector3TransformCoord(XMLoadFloat3(&xmf3CameraOrigin), xmmtxToModel);
	xmvPickRayDirection = XMVector3TransformCoord(xmvPickPosition, xmmtxToModel);
	xmvPickRayDirection = XMVector3Normalize(xmvPickRayDirection - xmvPickRayOrigin);
}

INT CGameObject::PickObjectByRayIntersection(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, FLOAT* pfHitDistance)
{
	INT nIntersected = 0;

	if (m_pMesh)
	{
		XMVECTOR xmvPickRayOrigin, xmvPickRayDirection;

		GenerateRayForPicking(xmvPickPosition, xmmtxView, xmvPickRayOrigin, xmvPickRayDirection);

		nIntersected = m_pMesh->CheckRayIntersection(xmvPickRayOrigin, xmvPickRayDirection, pfHitDistance);
	}

	return nIntersected;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CWallsObject::CWallsObject()
{
}

CWallsObject::~CWallsObject()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
XMFLOAT3 CEnemyObject::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh* CEnemyObject::m_pExplosionMesh = nullptr;

CEnemyObject::CEnemyObject()
{
}

CEnemyObject::~CEnemyObject()
{
}

void CEnemyObject::PrepareExplosion()
{
	for (INT i = 0; i < EXPLOSION_DEBRISES; ++i)
	{
		XMStoreFloat3(&m_pxmf3SphereVectors[i], ::RandomUnitVectorOnSphere());
	}

	m_pExplosionMesh = new CEnemyMesh(0.5f, 0.5f, 0.5f);
}

void CEnemyObject::Animate(FLOAT fElapsedTime)
{
	if (m_bBlowingUp)
	{
		m_fElapsedTimes += fElapsedTime;

		if (m_fElapsedTimes <= m_fDuration)
		{
			XMFLOAT3 xmf3Position = GetPosition();

			for (INT i = 0; i < EXPLOSION_DEBRISES; ++i)
			{
				m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
				m_pxmf4x4Transforms[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], m_fExplosionRotation * m_fElapsedTimes), m_pxmf4x4Transforms[i]);
			}
		}
		else
		{
			m_bBlowingUp = FALSE;
			m_fElapsedTimes = 0.0f;
		}
	}
	else
		CGameObject::Animate(fElapsedTime);
}

void CEnemyObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	if (m_bBlowingUp)
	{
		for (INT i = 0; i < EXPLOSION_DEBRISES; ++i)
		{
			if (m_pExplosionMesh)
			{
				HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
				HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

				m_pExplosionMesh->Render(hDCFrameBuffer, m_pxmf4x4Transforms[i], pCamera);

				::SelectObject(hDCFrameBuffer, hOldPen);
				::DeleteObject(hPen);
			}
		}
	}
	else
		CGameObject::Render(hDCFrameBuffer, pCamera);
}