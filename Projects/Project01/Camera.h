#pragma once

class CPlayer;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCamera
{
public:
	CCamera();
	virtual ~CCamera();
public:
	void GenerateViewMatrix();
	void GenerateProjectionMatrix(FLOAT fNearPlaneDistance, FLOAT fFarPlaneDistance, FLOAT fFOVAngle);
	void SetViewport(INT xTopLeft, INT yTopLeft, INT nWidth, INT nHeight, FLOAT fMinZ, FLOAT fMaxZ);
public:
	void SetLookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);
	void SetLookAt(XMFLOAT3& vPosition, XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);
public:
	void Move(XMFLOAT3& xmf3Shift);
	void Move(FLOAT x, FLOAT y, FLOAT z);
	void Rotate(FLOAT fPitch = 0.0f, FLOAT fYaw = 0.0f, FLOAT fRoll = 0.0f);
	void Update(CPlayer* pPlayer, XMFLOAT3& xmf3LookAt, FLOAT fTimeElapsed = 0.016f);
public:
	BOOL IsInFrustum(BoundingOrientedBox& xmBoundingBox);

public:
	// 카메라 좌표 벡터
	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;
public:
	// 카메라 변환 행려라
	XMFLOAT4X4					m_xmf4x4View;
	XMFLOAT4X4					m_xmf4x4Projection;
	XMFLOAT4X4					m_xmf4x4ViewProject;
public:
	// 뷰포트
	D3D12_VIEWPORT				m_d3dViewport;
public:
	BoundingFrustum				m_xmFrustumView;
	BoundingFrustum				m_xmFrustumWorld;
	XMFLOAT4X4					m_xmf4x4InverseView;
};