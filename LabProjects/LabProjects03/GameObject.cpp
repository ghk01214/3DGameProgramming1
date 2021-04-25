#include "StdAfx.h"
#include "GameObject.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
inline FLOAT RandF(FLOAT fMin, FLOAT fMax)
{
	return (fMin + ((FLOAT)rand() / (FLOAT)RAND_MAX) * (fMax - fMin));
}

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne{ XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f) };
	XMVECTOR xmvZero{ XMVectorZero() };

	while (TRUE)
	{
		XMVECTOR v{ XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f) };

		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne))
		{
			return XMVector3Normalize(v);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject() 
{ 
	pMesh					 = nullptr; 
	xmf4x4World			 = Matrix4x4::Identity();

	dwColor				 = RGB(0, 0, 0);

	xmf3MovingDirection	 = XMFLOAT3(0.0f, 0.0f, 1.0f);
	fMovingSpeed			 = 0.0f;
	fMovingRange			 = 0.0f;

	xmf3RotationAxis		 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	fRotationSpeed		 = 0.05f;
}

CGameObject::CGameObject(CMesh *pMesh) : CGameObject()
{
	this->pMesh = pMesh; 
}

CGameObject::~CGameObject()
{
	if (pMesh)
	{
		pMesh->Release();
	}
}

void CGameObject::SetPosition(FLOAT x, FLOAT y, FLOAT z) 
{
	xmf4x4World._41 = x; 
	xmf4x4World._42 = y; 
	xmf4x4World._43 = z; 
}

void CGameObject::SetPosition(XMFLOAT3& xmf3Position) 
{ 
	xmf4x4World._41 = xmf3Position.x; 
	xmf4x4World._42 = xmf3Position.y; 
	xmf4x4World._43 = xmf3Position.z; 
}

XMFLOAT3 CGameObject::GetPosition() 
{ 
	return XMFLOAT3(xmf4x4World._41, xmf4x4World._42, xmf4x4World._43);
}

XMFLOAT3 CGameObject::GetLook() 
{ 	
	XMFLOAT3 xmf3LookAt(xmf4x4World._31, xmf4x4World._32, xmf4x4World._33);

	xmf3LookAt = Vector3::Normalize(xmf3LookAt);

	return xmf3LookAt;
}

XMFLOAT3 CGameObject::GetUp() 
{ 	
	XMFLOAT3 xmf3Up(xmf4x4World._21, xmf4x4World._22, xmf4x4World._23);

	xmf3Up = Vector3::Normalize(xmf3Up);

	return xmf3Up;
}

XMFLOAT3 CGameObject::GetRight()
{ 	
	XMFLOAT3 xmf3Right(xmf4x4World._11, xmf4x4World._12, xmf4x4World._13);

	xmf3Right = Vector3::Normalize(xmf3Right);

	return xmf3Right;
}

void CGameObject::MoveStrafe(FLOAT fDistance)
{
	XMFLOAT3 xmf3Position{ GetPosition() };
	XMFLOAT3 xmf3Right{ GetRight() };

	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Right, fDistance));

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(FLOAT fDistance)
{
	XMFLOAT3 xmf3Position{ GetPosition() };
	XMFLOAT3 xmf3Up{ GetUp() };

	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Up, fDistance));

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(FLOAT fDistance)
{
	XMFLOAT3 xmf3Position{ GetPosition() };
	XMFLOAT3 xmf3LookAt{ GetLook() };

	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3LookAt, fDistance));

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(FLOAT fPitch, FLOAT fYaw, FLOAT fRoll)
{
	XMFLOAT4X4 mtxRotate{ Matrix4x4::RotationYawPitchRoll(fPitch, fYaw, fRoll) };

	xmf4x4World = Matrix4x4::Multiply(mtxRotate, xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3& xmf3RotationAxis, FLOAT fAngle)
{
	XMFLOAT4X4 mtxRotate{ Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle) };

	xmf4x4World = Matrix4x4::Multiply(mtxRotate, xmf4x4World);
}

void CGameObject::Move(XMFLOAT3& vDirection, FLOAT fSpeed)
{
	SetPosition(xmf4x4World._41 + vDirection.x * fSpeed, xmf4x4World._42 + vDirection.y * fSpeed, xmf4x4World._43 + vDirection.z * fSpeed);
}

void CGameObject::LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View{ Matrix4x4::LookAtLH(GetPosition(), xmf3LookTo, xmf3Up) };

	xmf4x4World._11 = xmf4x4View._11;
	xmf4x4World._12 = xmf4x4View._21;
	xmf4x4World._13 = xmf4x4View._31;

	xmf4x4World._21 = xmf4x4View._12;
	xmf4x4World._22 = xmf4x4View._22;
	xmf4x4World._23 = xmf4x4View._32;

	xmf4x4World._31 = xmf4x4View._13;
	xmf4x4World._32 = xmf4x4View._23;
	xmf4x4World._33 = xmf4x4View._33;
}

void CGameObject::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View{ Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, xmf3Up) };

	xmf4x4World._11 = xmf4x4View._11;
	xmf4x4World._12 = xmf4x4View._21;
	xmf4x4World._13 = xmf4x4View._31;

	xmf4x4World._21 = xmf4x4View._12;
	xmf4x4World._22 = xmf4x4View._22;
	xmf4x4World._23 = xmf4x4View._32;

	xmf4x4World._31 = xmf4x4View._13;
	xmf4x4World._32 = xmf4x4View._23;
	xmf4x4World._33 = xmf4x4View._33;
}

void CGameObject::UpdateBoundingBox()
{
	if (pMesh)
	{
		pMesh->xmOOBB.Transform(xmOOBB, XMLoadFloat4x4(&xmf4x4World));

		XMStoreFloat4(&xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&xmOOBB.Orientation)));
	}
}

void CGameObject::Animate(FLOAT fElapsedTime)
{
	if (fRotationSpeed != 0.0f)
	{
		Rotate(xmf3RotationAxis, fRotationSpeed * fElapsedTime);
	}

	if (fMovingSpeed != 0.0f)
	{
		Move(xmf3MovingDirection, fMovingSpeed * fElapsedTime);
	}

	UpdateBoundingBox();
}

void CGameObject::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	if (pMesh)
	{
		if (pCamera->IsInFrustum(xmOOBB))
		{
			HPEN hPen{ ::CreatePen(PS_SOLID, 0, dwColor) };
			HPEN hOldPen{ (HPEN)::SelectObject(hDCFrameBuffer, hPen) };

			pMesh->Render(hDCFrameBuffer, xmf4x4World, pCamera);

			::SelectObject(hDCFrameBuffer, hOldPen);
			::DeleteObject(hPen);
		}
	}
}

void CGameObject::GenerateRayForPicking(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection)
{
	XMMATRIX xmmtxToModel{ XMMatrixInverse(nullptr, XMLoadFloat4x4(&xmf4x4World) * xmmtxView) };
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);

	xmvPickRayOrigin = XMVector3TransformCoord(XMLoadFloat3(&xmf3CameraOrigin), xmmtxToModel);
	xmvPickRayDirection = XMVector3TransformCoord(xmvPickPosition, xmmtxToModel);
	xmvPickRayDirection = XMVector3Normalize(xmvPickRayDirection - xmvPickRayOrigin);
}

INT CGameObject::PickObjectByRayIntersection(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, FLOAT *pfHitDistance)
{
	INT nIntersected{ 0 };

	if (pMesh)
	{
		XMVECTOR xmvPickRayOrigin, xmvPickRayDirection;

		GenerateRayForPicking(xmvPickPosition, xmmtxView, xmvPickRayOrigin, xmvPickRayDirection);

		nIntersected = pMesh->CheckRayIntersection(xmvPickRayOrigin, xmvPickRayDirection, pfHitDistance);
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
XMFLOAT3 CExplosiveObject::pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh* CExplosiveObject::pExplosionMesh{ nullptr };

CExplosiveObject::CExplosiveObject()
{
}

CExplosiveObject::~CExplosiveObject()
{
}

void CExplosiveObject::PrepareExplosion()
{
	for (INT i = 0; i < EXPLOSION_DEBRISES; i++)
	{
		XMStoreFloat3(&pxmf3SphereVectors[i], ::RandomUnitVectorOnSphere());
	}

	pExplosionMesh = new CCubeMesh(0.5f, 0.5f, 0.5f);
}

void CExplosiveObject::Animate(FLOAT fElapsedTime)
{
	if (bBlowingUp)
	{
		this->fElapsedTimes += fElapsedTime;

		if (this->fElapsedTimes <= fDuration)
		{
			XMFLOAT3 xmf3Position{ GetPosition() };

			for (INT i = 0; i < EXPLOSION_DEBRISES; i++)
			{
				pxmf4x4Transforms[i] = Matrix4x4::Identity();

				pxmf4x4Transforms[i]._41 = xmf3Position.x + pxmf3SphereVectors[i].x * fExplosionSpeed * this->fElapsedTimes;
				pxmf4x4Transforms[i]._42 = xmf3Position.y + pxmf3SphereVectors[i].y * fExplosionSpeed * this->fElapsedTimes;
				pxmf4x4Transforms[i]._43 = xmf3Position.z + pxmf3SphereVectors[i].z * fExplosionSpeed * this->fElapsedTimes;

				pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(pxmf3SphereVectors[i], fExplosionRotation * this->fElapsedTimes), pxmf4x4Transforms[i]);
			}
		}
		else
		{
			bBlowingUp = FALSE;
			this->fElapsedTimes = 0.0f;
		}
	}
	else
	{
		CGameObject::Animate(fElapsedTime);
	}
}

void CExplosiveObject::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	if (bBlowingUp)
	{
		for (INT i = 0; i < EXPLOSION_DEBRISES; i++)
		{
			if (pExplosionMesh)
			{
				HPEN hPen{ ::CreatePen(PS_SOLID, 0, dwColor) };
				HPEN hOldPen{ (HPEN)::SelectObject(hDCFrameBuffer, hPen) };

				pExplosionMesh->Render(hDCFrameBuffer, pxmf4x4Transforms[i], pCamera);

				::SelectObject(hDCFrameBuffer, hOldPen);
				::DeleteObject(hPen);
			}
		}
	}
	else
	{
		CGameObject::Render(hDCFrameBuffer, pCamera);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CBulletObject::CBulletObject(FLOAT fEffectiveRange)
{
	fBulletEffectiveRange = fEffectiveRange;
}

CBulletObject::~CBulletObject()
{
}

void CBulletObject::SetFirePosition(XMFLOAT3 xmf3FirePosition)
{ 
	this->xmf3FirePosition = xmf3FirePosition; 

	SetPosition(xmf3FirePosition);
}

void CBulletObject::Animate(FLOAT fElapsedTime)
{
	FLOAT fDistance{ fMovingSpeed * fElapsedTime };

#ifdef _WITH_VECTOR_OPERATION
	XMFLOAT3 xmf3Position{ GetPosition() };

	fRotationAngle += fRotationSpeed * fElapsedTime;

	if (fRotationAngle > 360.0f)
	{
		fRotationAngle -= 360.0f;
	}

	XMFLOAT4X4 mtxRotate1{ Matrix4x4::RotationYawPitchRoll(0.0f, fRotationAngle, 0.0f) };

	XMFLOAT3	 xmf3RotationAxis{ Vector3::CrossProduct(xmf3RotationAxis, xmf3MovingDirection, TRUE) };
	FLOAT		 fDotProduct{ Vector3::DotProduct(xmf3RotationAxis, xmf3MovingDirection) };
	FLOAT		 fRotationAngle{ ::IsEqual(fDotProduct, 1.0f) ? 0.0f : (FLOAT)XMConvertToDegrees(acos(fDotProduct)) };
	XMFLOAT4X4	 mtxRotate2{ Matrix4x4::RotationAxis(xmf3RotationAxis, fRotationAngle) };

	xmf4x4World = Matrix4x4::Multiply(mtxRotate1, mtxRotate2);

	XMFLOAT3 xmf3Movement{ = Vector3::ScalarProduct(xmf3MovingDirection, fDistance, FALSE) };

	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement);
	SetPosition(xmf3Position);
#else
	XMFLOAT4X4 mtxRotate{ Matrix4x4::RotationYawPitchRoll(0.0f, fRotationSpeed * fElapsedTime, 0.0f) };

	xmf4x4World = Matrix4x4::Multiply(mtxRotate, xmf4x4World);

	XMFLOAT3 xmf3Movement{ Vector3::ScalarProduct(xmf3MovingDirection, fDistance, FALSE) };
	XMFLOAT3 xmf3Position{ GetPosition() };

	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement);
	SetPosition(xmf3Position);
#endif
	UpdateBoundingBox();

	if (Vector3::Distance(xmf3FirePosition, GetPosition()) > fBulletEffectiveRange)
	{
		SetActive(FALSE);
	}
}

