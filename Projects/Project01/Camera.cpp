#include "StdAfx.h"
#include "Camera.h"
#include "Player.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCamera::CCamera()
{
	m_xmf3Position			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right				 = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up				 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look				 = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf4x4View			 = Matrix4x4::Identity();
	m_xmf4x4Projection		 = Matrix4x4::Identity();
	m_xmf4x4ViewProject		 = Matrix4x4::Identity();

	m_d3dViewport			 = { 0, 0, CLIENT_WIDTH , CLIENT_HEIGHT, 0.0f, 1.0f };
}

CCamera::~CCamera()
{
}

void CCamera::GenerateViewMatrix()
{
	m_xmf3Look					= Vector3::Normalize(m_xmf3Look);
	m_xmf3Right					= Vector3::Normalize(Vector3::CrossProduct(m_xmf3Up, m_xmf3Look));
	m_xmf3Up					= Vector3::Normalize(Vector3::CrossProduct(m_xmf3Look, m_xmf3Right));
	m_xmf4x4View._11			= m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21			= m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31			= m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;
	m_xmf4x4View._41			= -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42			= -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43			= -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);

	m_xmf4x4ViewProject			= Matrix4x4::Multiply(m_xmf4x4View, m_xmf4x4Projection);

	m_xmf4x4InverseView._11		= m_xmf3Right.x; m_xmf4x4InverseView._12 = m_xmf3Right.y; m_xmf4x4InverseView._13 = m_xmf3Right.z;
	m_xmf4x4InverseView._21		= m_xmf3Up.x; m_xmf4x4InverseView._22 = m_xmf3Up.y; m_xmf4x4InverseView._23 = m_xmf3Up.z;
	m_xmf4x4InverseView._31		= m_xmf3Look.x; m_xmf4x4InverseView._32 = m_xmf3Look.y; m_xmf4x4InverseView._33 = m_xmf3Look.z;
	m_xmf4x4InverseView._41		= m_xmf3Position.x; m_xmf4x4InverseView._42 = m_xmf3Position.y; m_xmf4x4InverseView._43 = m_xmf3Position.z;

	m_xmFrustumView.Transform(m_xmFrustumWorld, XMLoadFloat4x4(&m_xmf4x4InverseView));
}

void CCamera::SetLookAt(XMFLOAT3& xmf3Position, XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	m_xmf3Position	 = xmf3Position;
	m_xmf4x4View	 = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right		 = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._11, m_xmf4x4View._21, m_xmf4x4View._31));
	m_xmf3Up		 = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._12, m_xmf4x4View._22, m_xmf4x4View._32));
	m_xmf3Look		 = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._13, m_xmf4x4View._23, m_xmf4x4View._33));
}

void CCamera::SetLookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View	 = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right				 = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	m_xmf3Up				 = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	m_xmf3Look				 = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void CCamera::SetViewport(INT xTopLeft, INT yTopLeft, INT nWidth, INT nHeight, FLOAT fMinZ, FLOAT fMaxZ)
{
	m_d3dViewport.TopLeftX	 = FLOAT(xTopLeft);
	m_d3dViewport.TopLeftY	 = FLOAT(yTopLeft);
	m_d3dViewport.Width		 = FLOAT(nWidth);
	m_d3dViewport.Height	 = FLOAT(nHeight);
	m_d3dViewport.MinDepth	 = fMinZ;
	m_d3dViewport.MaxDepth	 = fMaxZ;
}

void CCamera::GenerateProjectionMatrix(FLOAT fNearPlaneDistance, FLOAT fFarPlaneDistance, FLOAT fFOVAngle)
{
	FLOAT fAspectRatio = (FLOAT(m_d3dViewport.Width) / FLOAT(m_d3dViewport.Height));
	XMMATRIX xmmtxProjection =  XMMatrixPerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);

	XMStoreFloat4x4(&m_xmf4x4Projection, xmmtxProjection);
	BoundingFrustum::CreateFromMatrix(m_xmFrustumView, xmmtxProjection);
}

BOOL CCamera::IsInFrustum(BoundingOrientedBox& xmBoundingBox)
{
	return m_xmFrustumWorld.Intersects(xmBoundingBox);
}

void CCamera::Move(XMFLOAT3& xmf3Shift)
{
	m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
}

void CCamera::Move(FLOAT x, FLOAT y, FLOAT z)
{
	Move(XMFLOAT3(x, y, z));
}

void CCamera::Rotate(FLOAT fPitch, FLOAT fYaw, FLOAT fRoll)
{
	if (fPitch != 0.0f)
	{
		XMMATRIX mtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch)) };

		m_xmf3Look		 = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Up		 = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX mtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw)) };

		m_xmf3Look		 = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Right		 = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX mtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll)) };

		m_xmf3Up		 = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
		m_xmf3Right		 = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
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
	XMFLOAT3 xmf3Direction{ Vector3::Subtract(xmf3Position, m_xmf3Position) };

	FLOAT fLength{ Vector3::Length(xmf3Direction) };
	xmf3Direction = Vector3::Normalize(xmf3Direction);

	FLOAT fTimeLagScale{ fTimeElapsed * (1.0f / 0.25f) };
	FLOAT fDistance{ fLength * fTimeLagScale };

	if (fDistance > fLength)
		fDistance = fLength;

	if (fLength < 0.01f)
		fDistance = fLength;

	if (fDistance > 0)
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Direction, fDistance);
		SetLookAt(pPlayer->m_xmf3Position, pPlayer->m_xmf3Up);
	}
}