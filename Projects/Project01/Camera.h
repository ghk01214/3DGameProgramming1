#pragma once

class CPlayer;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCamera
{
public:
	CCamera();
	virtual ~CCamera();

	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	XMFLOAT4X4					m_xmf4x4View;
	XMFLOAT4X4					m_xmf4x4Projection;
	XMFLOAT4X4					m_xmf4x4ViewProject;

	D3D12_VIEWPORT				m_d3dViewport;

	BoundingFrustum				m_xmFrustumView;
	BoundingFrustum				m_xmFrustumWorld;
	XMFLOAT4X4					m_xmf4x4InverseView;

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(FLOAT fNearPlaneDistance, FLOAT fFarPlaneDistance, FLOAT fFOVAngle);
	void SetViewport(INT xTopLeft, INT yTopLeft, INT nWidth, INT nHeight, FLOAT fMinZ, FLOAT fMaxZ);

	void SetLookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);
	void SetLookAt(XMFLOAT3& vPosition, XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	void Move(XMFLOAT3& xmf3Shift);
	void Move(FLOAT x, FLOAT y, FLOAT z);
	void Rotate(FLOAT fPitch = 0.0f, FLOAT fYaw = 0.0f, FLOAT fRoll = 0.0f);
	void Update(CPlayer *pPlayer, XMFLOAT3& xmf3LookAt, FLOAT fTimeElapsed = 0.016f);

	BOOL IsInFrustum(BoundingOrientedBox& xmBoundingBox);
};


