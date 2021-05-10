#pragma once

#include "Camera.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CVertex
{
public:
	CVertex() { }
	CVertex(FLOAT x, FLOAT y, FLOAT z) { m_xmf3Position = XMFLOAT3(x, y, z); }

	XMFLOAT3					m_xmf3Position;
};

class CPolygon
{
public:
	CPolygon(INT nVertices);
	virtual ~CPolygon();

	INT							m_nVertices;
	CVertex*					m_pVertices;

	void SetVertex(INT nIndex, CVertex& vertex);
};

class CMesh
{
public:
	CMesh(INT nPolygons);
	virtual ~CMesh();

private:
	INT							m_nReferences;

	INT							m_nPolygons;
	CPolygon**					m_ppPolygons;

public:
	BoundingOrientedBox			m_xmOOBB;

public:
	void AddRef() { ++m_nReferences; }
	void Release() { --m_nReferences; if (m_nReferences <= 0) delete this; }

public:
	void SetPolygon(INT nIndex, CPolygon* pPolygon);
	virtual void Render(HDC hDCFrameBuffer, XMFLOAT4X4& xmf4x4World, CCamera* pCamera);

	BOOL RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, FLOAT* pfNearHitDistance);
	INT CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, FLOAT* pfNearHitDistance);
};

class CEnemyMesh : public CMesh
{
public:
	CEnemyMesh(FLOAT fWidth = 4.0f, FLOAT fHeight = 4.0f, FLOAT fDepth = 4.0f);
	virtual ~CEnemyMesh();
};

class CWallMesh : public CMesh
{
public:
	CWallMesh(FLOAT fWidth = 4.0f, FLOAT fHeight = 4.0f, FLOAT fDepth = 4.0f, INT nSubRects = 20);
	virtual ~CWallMesh();
};

class CCarMesh : public CMesh
{
public:
	CCarMesh(FLOAT fWidth = 20.0f, FLOAT fHeight = 20.0f, FLOAT fDepth = 4.0f);
	virtual ~CCarMesh();
};


