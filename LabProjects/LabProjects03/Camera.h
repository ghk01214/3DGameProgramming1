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
	void Update(CPlayer *pPlayer, XMFLOAT3& xmf3LookAt, FLOAT fTimeElapsed = 0.016f);
public:
	BOOL IsInFrustum(BoundingOrientedBox& xmBoundingBox);

public:
	XMFLOAT3					xmf3Position;
	XMFLOAT3					xmf3Right;
	XMFLOAT3					xmf3Up;
	XMFLOAT3					xmf3Look;
public:
	XMFLOAT4X4					xmf4x4View;
	XMFLOAT4X4					xmf4x4Projection;
	XMFLOAT4X4					xmf4x4ViewProject;
public:
	D3D12_VIEWPORT				d3dViewport;
public:
	BoundingFrustum				xmFrustumView;
	BoundingFrustum				xmFrustumWorld;
	XMFLOAT4X4					xmf4x4InverseView;
};