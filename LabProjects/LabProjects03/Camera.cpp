#include "StdAfx.h"
#include "Camera.h"
#include "Player.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCamera::CCamera()
{
	xmf3Position	= XMFLOAT3(0.0f, 0.0f, 0.0f);
	xmf3Right		= XMFLOAT3(1.0f, 0.0f, 0.0f);
	xmf3Up			= XMFLOAT3(0.0f, 1.0f, 0.0f);
	xmf3Look		= XMFLOAT3(0.0f, 0.0f, 1.0f);

	xmf4x4View			= Matrix4x4::Identity();
	xmf4x4Projection	= Matrix4x4::Identity();
	xmf4x4ViewProject	= Matrix4x4::Identity();

	d3dViewport = { 0, 0, CLIENT_WIDTH , CLIENT_HEIGHT, 0.0f, 1.0f };
}

CCamera::~CCamera()
{
}

void CCamera::GenerateViewMatrix()
{
	xmf3Look	= Vector3::Normalize(xmf3Look);
	xmf3Right	= Vector3::Normalize(Vector3::CrossProduct(xmf3Up, xmf3Look));
	xmf3Up		= Vector3::Normalize(Vector3::CrossProduct(xmf3Look, xmf3Right));

	xmf4x4View._11 = xmf3Right.x; xmf4x4View._12 = xmf3Up.x; xmf4x4View._13 = xmf3Look.x;
	xmf4x4View._21 = xmf3Right.y; xmf4x4View._22 = xmf3Up.y; xmf4x4View._23 = xmf3Look.y;
	xmf4x4View._31 = xmf3Right.z; xmf4x4View._32 = xmf3Up.z; xmf4x4View._33 = xmf3Look.z;
	xmf4x4View._41 = -Vector3::DotProduct(xmf3Position, xmf3Right);
	xmf4x4View._42 = -Vector3::DotProduct(xmf3Position, xmf3Up);
	xmf4x4View._43 = -Vector3::DotProduct(xmf3Position, xmf3Look);

	xmf4x4ViewProject = Matrix4x4::Multiply(xmf4x4View, xmf4x4Projection);

	xmf4x4InverseView._11 = xmf3Right.x; xmf4x4InverseView._12 = xmf3Right.y; xmf4x4InverseView._13 = xmf3Right.z;
	xmf4x4InverseView._21 = xmf3Up.x; xmf4x4InverseView._22 = xmf3Up.y; xmf4x4InverseView._23 = xmf3Up.z;
	xmf4x4InverseView._31 = xmf3Look.x; xmf4x4InverseView._32 = xmf3Look.y; xmf4x4InverseView._33 = xmf3Look.z;
	xmf4x4InverseView._41 = xmf3Position.x; xmf4x4InverseView._42 = xmf3Position.y; xmf4x4InverseView._43 = xmf3Position.z;

	xmFrustumView.Transform(xmFrustumWorld, XMLoadFloat4x4(&xmf4x4InverseView));
}

void CCamera::SetLookAt(XMFLOAT3& xmf3Position, XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	this->xmf3Position = xmf3Position;

	xmf4x4View		= Matrix4x4::LookAtLH(this->xmf3Position, xmf3LookAt, xmf3Up);
	xmf3Right		= Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	this->xmf3Up	= Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	xmf3Look		= Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void CCamera::SetLookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View{ Matrix4x4::LookAtLH(this->xmf3Position, xmf3LookAt, xmf3Up) };

	xmf3Right		 = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	this->xmf3Up	 = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	xmf3Look		 = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void CCamera::SetViewport(INT xTopLeft, INT yTopLeft, INT nWidth, INT nHeight, FLOAT fMinZ, FLOAT fMaxZ)
{
	d3dViewport.TopLeftX = FLOAT(xTopLeft);
	d3dViewport.TopLeftY = FLOAT(yTopLeft);

	d3dViewport.Width	= FLOAT(nWidth);
	d3dViewport.Height	= FLOAT(nHeight);

	d3dViewport.MinDepth = fMinZ;
	d3dViewport.MaxDepth = fMaxZ;
}

void CCamera::GenerateProjectionMatrix(FLOAT fNearPlaneDistance, FLOAT fFarPlaneDistance, FLOAT fFOVAngle)
{
	FLOAT fAspectRatio{ (FLOAT(d3dViewport.Width) / FLOAT(d3dViewport.Height)) };
	XMMATRIX xmmtxProjection{ XMMatrixPerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance) };

	XMStoreFloat4x4(&xmf4x4Projection, xmmtxProjection);

	BoundingFrustum::CreateFromMatrix(xmFrustumView, xmmtxProjection);
}

BOOL CCamera::IsInFrustum(BoundingOrientedBox& xmBoundingBox)
{
	return xmFrustumWorld.Intersects(xmBoundingBox);
}

void CCamera::Move(XMFLOAT3& xmf3Shift)
{
	xmf3Position = Vector3::Add(xmf3Position, xmf3Shift);
}

void CCamera::Move(FLOAT x, FLOAT y, FLOAT z)
{
	Move(XMFLOAT3(x, y, z));
}

void CCamera::Rotate(FLOAT fPitch, FLOAT fYaw, FLOAT fRoll)
{
	if (fPitch != 0.0f)
	{
		XMMATRIX mtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&xmf3Right), XMConvertToRadians(fPitch)) };

		xmf3Look	= Vector3::TransformNormal(xmf3Look, mtxRotate);
		xmf3Up		= Vector3::TransformNormal(xmf3Up, mtxRotate);
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX mtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(fYaw)) };

		xmf3Look	= Vector3::TransformNormal(xmf3Look, mtxRotate);
		xmf3Right	= Vector3::TransformNormal(xmf3Right, mtxRotate);
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX mtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look), XMConvertToRadians(fRoll)) };

		xmf3Up		 = Vector3::TransformNormal(xmf3Up, mtxRotate);
		xmf3Right	 = Vector3::TransformNormal(xmf3Right, mtxRotate);
	}
}

void CCamera::Update(CPlayer *pPlayer, XMFLOAT3& xmf3LookAt, FLOAT fTimeElapsed)
{
	XMFLOAT4X4 mtxRotate{ Matrix4x4::Identity() };
	mtxRotate._11 = pPlayer->m_xmf3Right.x; mtxRotate._21 = pPlayer->m_xmf3Up.x; mtxRotate._31 = pPlayer->m_xmf3Look.x;
	mtxRotate._12 = pPlayer->m_xmf3Right.y; mtxRotate._22 = pPlayer->m_xmf3Up.y; mtxRotate._32 = pPlayer->m_xmf3Look.y;
	mtxRotate._13 = pPlayer->m_xmf3Right.z; mtxRotate._23 = pPlayer->m_xmf3Up.z; mtxRotate._33 = pPlayer->m_xmf3Look.z;

	XMFLOAT3 xmf3Offset{ Vector3::TransformCoord(pPlayer->m_xmf3CameraOffset, mtxRotate) };
	XMFLOAT3 xmf3Position{ Vector3::Add(pPlayer->m_xmf3Position, xmf3Offset) };
	XMFLOAT3 xmf3Direction{ Vector3::Subtract(xmf3Position, xmf3Position) };
	FLOAT fLength{ Vector3::Length(xmf3Direction) };

	xmf3Direction = Vector3::Normalize(xmf3Direction);

	FLOAT fTimeLagScale{ fTimeElapsed * (1.0f / 0.25f) };
	FLOAT fDistance{ fLength * fTimeLagScale };

	if (fDistance > fLength)
	{
		fDistance = fLength;
	}

	if (fLength < 0.01f)
	{
		fDistance = fLength;
	}
	
	if (fDistance > 0)
	{
		xmf3Position = Vector3::Add(xmf3Position, xmf3Direction, fDistance);

		SetLookAt(pPlayer->m_xmf3Position, pPlayer->m_xmf3Up);
	}
}