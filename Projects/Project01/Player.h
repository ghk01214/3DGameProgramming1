#pragma once

#include "GameObject.h"

class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	XMFLOAT3					m_xmf3CameraOffset;
	XMFLOAT3					m_xmf3Velocity;
	FLOAT						m_fFriction = 125.0f;

	FLOAT           			m_fPitch = 0.0f;
	FLOAT           			m_fYaw = 0.0f;
	FLOAT           			m_fRoll = 0.0f;

	INT							m_iFeverStack = 0;
	BOOL						m_bFeverMode = FALSE;

	CCamera*					m_pCamera = nullptr;

	void SetPosition(FLOAT x, FLOAT y, FLOAT z);
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	void Move(DWORD dwDirection, FLOAT fDistance);
	void Move(XMFLOAT3& xmf3Shift, BOOL bUpdateVelocity);
	void Move(FLOAT x, FLOAT y, FLOAT z);
	void Rotate(FLOAT fPitch = 0.0f, FLOAT fYaw = 0.0f, FLOAT fRoll = 0.0f);
	void SetCameraOffset(XMFLOAT3& xmf3CameraOffset);
	void Update(FLOAT fTimeElapsed = 0.016f);

	virtual void OnUpdateTransform();
	virtual void Animate(FLOAT fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);
};

#define BULLETS					30

class CCarPlayer : public CPlayer
{
public:
	CCarPlayer();
	virtual ~CCarPlayer();

	void Jump();

	virtual void OnUpdateTransform();
	virtual void Animate(FLOAT fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

public:
	BOOL						m_bJump = FALSE;
	INT							m_iJumpState = 0;
};