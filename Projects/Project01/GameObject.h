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
	BOOL						m_bActive = TRUE;

	CMesh*						m_pMesh = nullptr;
	XMFLOAT4X4					m_xmf4x4World;

	BoundingOrientedBox			m_xmOOBB;

	CGameObject*				m_pObjectCollided = nullptr;

	DWORD						m_dwColor;

	XMFLOAT3					m_xmf3MovingDirection;
	FLOAT						m_fMovingSpeed;
	FLOAT						m_fMovingRange;

	XMFLOAT3					m_xmf3RotationAxis;
	FLOAT						m_fRotationSpeed;

public:
	void SetActive(BOOL bActive) { m_bActive = bActive; }
	void SetMesh(CMesh* pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }
	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }
	void SetPosition(FLOAT x, FLOAT y, FLOAT z);
	void SetPosition(XMFLOAT3& xmf3Position);

	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(FLOAT fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingRange(FLOAT fRange) { m_fMovingRange = fRange; }

	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(FLOAT fSpeed) { m_fRotationSpeed = fSpeed; }

	void MoveStrate(FLOAT fDistance = 1.0f);
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
	INT PickObjectByRayIntersection(XMVECTOR& xmPickPosition, XMMATRIX& xmmtxView, FLOAT* pfHitDistance);
};

class CEnemyObject : public CGameObject
{
public:
	CEnemyObject();
	virtual ~CEnemyObject();

	BOOL						m_bBlowingUp = FALSE;

	XMFLOAT4X4					m_pxmf4x4Transforms[EXPLOSION_DEBRISES];

	FLOAT						m_fElapsedTimes = 0.0f;
	FLOAT						m_fDuration = 2.5f;
	FLOAT						m_fExplosionSpeed = 10.0f;
	FLOAT						m_fExplosionRotation = 720.0f;

	virtual void Animate(FLOAT fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

public:
	static CMesh*				m_pExplosionMesh;
	static XMFLOAT3				m_pxmf3SphereVectors[EXPLOSION_DEBRISES];

	static void PrepareExplosion();
};

class CWallsObject : public CGameObject
{
public:
	CWallsObject();
	virtual ~CWallsObject();

public:
	BoundingOrientedBox			m_xmOOBBPlayerMoveCheck;
	XMFLOAT4					m_pxmf4WallPlanes[6];
};