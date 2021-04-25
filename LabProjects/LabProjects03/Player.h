#pragma once

#include "GameObject.h"

class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();
public:
	void SetPosition(FLOAT x, FLOAT y, FLOAT z);
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);
public:
	void Move(DWORD dwDirection, FLOAT fDistance);
	void Move(XMFLOAT3& xmf3Shift, BOOL bUpdateVelocity);
	void Move(FLOAT x, FLOAT y, FLOAT z);
	void Rotate(FLOAT fPitch = 0.0f, FLOAT fYaw = 0.0f, FLOAT fRoll = 0.0f);
	void SetCameraOffset(XMFLOAT3& xmf3CameraOffset);
	void Update(FLOAT fTimeElapsed = 0.016f);
public:
	virtual void OnUpdateTransform();
	virtual void Animate(FLOAT fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

public:
	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;
public:
	XMFLOAT3					m_xmf3CameraOffset;
	XMFLOAT3					m_xmf3Velocity;
	FLOAT						m_fFriction = 125.0f;
public:
	FLOAT           			m_fPitch = 0.0f;
	FLOAT           			m_fYaw = 0.0f;
	FLOAT           			m_fRoll = 0.0f;
public:
	CCamera						*m_pCamera = NULL;
};

#define BULLETS					30

class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer();
	virtual ~CAirplanePlayer();
public:
	void FireBullet(CGameObject* pSelectedObject);
public:
	virtual void OnUpdateTransform();
	virtual void Animate(FLOAT fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

public:
	FLOAT						m_fBulletEffectiveRange{ 150.0f };
	CBulletObject				*m_ppBullets[BULLETS];
};

