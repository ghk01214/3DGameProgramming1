#pragma once

#include "Mesh.h"
#include "Camera.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGameObject
{
public:
	CGameObject(CMesh* pMesh);
	CGameObject();
	virtual ~CGameObject();
public:
	void SetActive(BOOL bActive) { bActive = bActive; }
	void SetMesh(CMesh* pMesh) { pMesh = pMesh; if (pMesh) pMesh->AddRef(); }
	void SetColor(DWORD dwColor) { dwColor = dwColor; }
	void SetPosition(FLOAT x, FLOAT y, FLOAT z);
	void SetPosition(XMFLOAT3& xmf3Position);

	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(FLOAT fSpeed) { fMovingSpeed = fSpeed; }
	void SetMovingRange(FLOAT fRange) { fMovingRange = fRange; }

	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(FLOAT fSpeed) { fRotationSpeed = fSpeed; }

	void MoveStrafe(FLOAT fDistance = 1.0f);
	void MoveUp(FLOAT fDistance = 1.0f);
	void MoveForward(FLOAT fDistance = 1.0f);
	void Move(XMFLOAT3& vDirection, FLOAT fSpeed);

	void Rotate(FLOAT fPitch = 10.0f, FLOAT fYaw = 10.0f, FLOAT fRoll = 10.0f);
	void Rotate(XMFLOAT3& xmf3Axis, FLOAT fAngle);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up);
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	void CGameObject::UpdateBoundingBox();

	virtual void OnUpdateTransform() { }
	virtual void Animate(FLOAT fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

	void GenerateRayForPicking(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection);
	int PickObjectByRayIntersection(XMVECTOR& xmPickPosition, XMMATRIX& xmmtxView, FLOAT* pfHitDistance);

public:
	BOOL						bActive{ true };
public:
	CMesh*						pMesh{ nullptr };
	XMFLOAT4X4					xmf4x4World;
public:
	BoundingOrientedBox			xmOOBB;
public:
	CGameObject*				pObjectCollided{ nullptr };
public:
	DWORD						dwColor;
public:
	XMFLOAT3					xmf3MovingDirection;
	FLOAT						fMovingSpeed;
	FLOAT						fMovingRange;
public:
	XMFLOAT3					xmf3RotationAxis;
	FLOAT						fRotationSpeed;
};

class CExplosiveObject : public CGameObject
{
public:
	CExplosiveObject();
	virtual ~CExplosiveObject();

	BOOL						bBlowingUp{ FALSE };

	XMFLOAT4X4					pxmf4x4Transforms[EXPLOSION_DEBRISES];

	FLOAT						fElapsedTimes{ 0.0f };
	FLOAT						fDuration{ 2.5f };
	FLOAT						fExplosionSpeed{ 10.0f };
	FLOAT						fExplosionRotation{ 720.0f };

	virtual void Animate(FLOAT fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

public:
	static CMesh*				pExplosionMesh;
	static XMFLOAT3				pxmf3SphereVectors[EXPLOSION_DEBRISES];

	static void PrepareExplosion();
};

class CWallsObject : public CGameObject
{
public:
	CWallsObject();
	virtual ~CWallsObject();

public:
	BoundingOrientedBox			xmOOBBPlayerMoveCheck;
	XMFLOAT4					pxmf4WallPlanes[6];
};

class CBulletObject : public CGameObject
{
public:
	CBulletObject(FLOAT fEffectiveRange);
	virtual ~CBulletObject();

public:
	virtual void Animate(FLOAT fElapsedTime);

	FLOAT						fBulletEffectiveRange{ 50.0f };
	XMFLOAT3					xmf3FirePosition;
	FLOAT						fRotationAngle{ 0.0f };

	void SetFirePosition(XMFLOAT3 xmf3FirePosition);
};

