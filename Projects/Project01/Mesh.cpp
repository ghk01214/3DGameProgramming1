#include "stdafx.h"
#include "Mesh.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPolygon::CPolygon(INT nVertices)
{
	if (nVertices > 0)
	{
		m_nVertices = nVertices;
		m_pVertices = new CVertex[nVertices];
	}
}

CPolygon::~CPolygon()
{
	if (m_pVertices) delete[] m_pVertices;
}

void CPolygon::SetVertex(INT nIndex, CVertex& vertex)
{
	if ((0 <= nIndex) && (nIndex < m_nVertices) && m_pVertices)
	{
		m_pVertices[nIndex] = vertex;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMesh::CMesh(INT nPolygons)
{
	if (nPolygons > 0)
	{
		m_nPolygons = nPolygons;
		m_ppPolygons = new CPolygon * [nPolygons];
		m_nReferences = 0;
	}
	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CMesh::~CMesh()
{
	if (m_ppPolygons)
	{
		for (INT i = 0; i < m_nPolygons; ++i) if (m_ppPolygons[i]) delete m_ppPolygons[i];
		delete[] m_ppPolygons;
	}
}

void CMesh::SetPolygon(INT nIndex, CPolygon* pPolygon)
{
	if ((0 <= nIndex) && (nIndex < m_nPolygons) && m_ppPolygons)
	{
		m_ppPolygons[nIndex] = pPolygon;
	}
}

void CMesh::Render(HDC hDCFrameBuffer, XMFLOAT4X4& xmf4x4World, CCamera* pCamera)
{
	XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Multiply(xmf4x4World, pCamera->m_xmf4x4ViewProject);
	for (INT j = 0; j < m_nPolygons; ++j)
	{
		INT nVertices = m_ppPolygons[j]->m_nVertices;
		CVertex* pVertices = m_ppPolygons[j]->m_pVertices;
		XMFLOAT3 xmf3Previous(-1.0f, 0.0f, 0.0f);
		for (INT i = 0; i <= nVertices; ++i)
		{
			CVertex vertex = pVertices[i % nVertices];
			//World/View/Projection Transformation(Perspective Divide)
			XMFLOAT3 xmf3Current = Vector3::TransformCoord(vertex.m_xmf3Position, xmf4x4Transform);
			if ((xmf3Current.z >= 0.0f) && (xmf3Current.z <= 1.0f))
			{
				//Screen Transformation
				xmf3Current.x = +xmf3Current.x * (pCamera->m_d3dViewport.Width * 0.5f) + pCamera->m_d3dViewport.TopLeftX + (pCamera->m_d3dViewport.Width * 0.5f);
				xmf3Current.y = -xmf3Current.y * (pCamera->m_d3dViewport.Height * 0.5f) + pCamera->m_d3dViewport.TopLeftY + (pCamera->m_d3dViewport.Height * 0.5f);

				if (xmf3Previous.x >= 0.0f)
				{
					::MoveToEx(hDCFrameBuffer, (long)xmf3Previous.x, (long)xmf3Previous.y, nullptr);
					::LineTo(hDCFrameBuffer, (long)xmf3Current.x, (long)xmf3Current.y);
				}
				xmf3Previous = xmf3Current;
			}
		}
	}
}

BOOL CMesh::RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, FLOAT* pfNearHitDistance)
{
	FLOAT fHitDistance;
	BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
	if (bIntersected && (fHitDistance < *pfNearHitDistance)) *pfNearHitDistance = fHitDistance;
	return(bIntersected);
}

INT CMesh::CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, FLOAT* pfNearHitDistance)
{
	INT nIntersections = 0;
	BOOL bIntersected = m_xmOOBB.Intersects(xmvPickRayOrigin, xmvPickRayDirection, *pfNearHitDistance);
	if (bIntersected)
	{
		for (INT i = 0; i < m_nPolygons; ++i)
		{
			switch (m_ppPolygons[i]->m_nVertices)
			{
			case 3:
			{
				XMVECTOR v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				XMVECTOR v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[1].m_xmf3Position));
				XMVECTOR v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				
				if (bIntersected)
					++nIntersections;

				break;
			}
			case 4:
			{
				XMVECTOR v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				XMVECTOR v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[1].m_xmf3Position));
				XMVECTOR v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				
				if (bIntersected)
					++nIntersections;
				
				v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[3].m_xmf3Position));
				bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				
				if (bIntersected)
					++nIntersections;

				break;
			}
			}
		}
	}
	return(nIntersections);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CEnemyMesh::CEnemyMesh(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth) : CMesh(6)
{
	FLOAT fHalfWidth = fWidth * 0.5f;
	FLOAT fHalfHeight = fHeight * 0.5f;
	FLOAT fHalfDepth = fDepth * 0.5f;

	CPolygon* pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(0, pFrontFace);

	CPolygon* pTopFace = new CPolygon(4);
	pTopFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pTopFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	SetPolygon(1, pTopFace);

	CPolygon* pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(2, pBackFace);

	CPolygon* pBottomFace = new CPolygon(4);
	pBottomFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBottomFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(3, pBottomFace);

	CPolygon* pLeftFace = new CPolygon(4);
	pLeftFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pLeftFace->SetVertex(1, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(4, pLeftFace);

	CPolygon* pRightFace = new CPolygon(4);
	pRightFace->SetVertex(0, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pRightFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(5, pRightFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CEnemyMesh::~CEnemyMesh()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CWallMesh::CWallMesh(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, INT nSubRects) : CMesh((4 * nSubRects * nSubRects) + 2)
{
	// 벽의 크기
	FLOAT fHalfWidth = fWidth * 0.5f;
	FLOAT fHalfHeight = fHeight * 0.5f;
	FLOAT fHalfDepth = fDepth * 0.5f;

	// 벽을 구성하는 작은 메쉬들의 크기
	FLOAT fCellWidth = fWidth * (1.0f / nSubRects);
	FLOAT fCellHeight = fHeight * (1.0f / nSubRects);
	FLOAT fCellDepth = fDepth * (1.0f / nSubRects);

	INT k = 0;
	CPolygon* pLeftFace;

	for (INT i = 0; i < nSubRects; ++i)
	{
		for (INT j = 0; j < nSubRects; ++j)
		{
			pLeftFace = new CPolygon(4);
			pLeftFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pLeftFace->SetVertex(1, CVertex(-fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pLeftFace);
		}
	}

	CPolygon* pRightFace;

	for (INT i = 0; i < nSubRects; ++i)
	{
		for (INT j = 0; j < nSubRects; ++j)
		{
			pRightFace = new CPolygon(4);
			pRightFace->SetVertex(0, CVertex(+fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pRightFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pRightFace);
		}
	}

	CPolygon* pTopFace;

	for (INT i = 0; i < nSubRects; ++i)
	{
		for (INT j = 0; j < nSubRects; ++j)
		{
			pTopFace = new CPolygon(4);
			pTopFace->SetVertex(0, CVertex(-fHalfWidth + (i * fCellWidth), +fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pTopFace->SetVertex(1, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), +fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pTopFace->SetVertex(2, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), +fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			pTopFace->SetVertex(3, CVertex(-fHalfWidth + (i * fCellWidth), +fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pTopFace);
		}
	}

	CPolygon* pBottomFace;

	for (INT i = 0; i < nSubRects; ++i)
	{
		for (INT j = 0; j < nSubRects; ++j)
		{
			pBottomFace = new CPolygon(4);
			pBottomFace->SetVertex(0, CVertex(-fHalfWidth + (i * fCellWidth), -fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pBottomFace->SetVertex(1, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), -fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pBottomFace->SetVertex(2, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), -fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			pBottomFace->SetVertex(3, CVertex(-fHalfWidth + (i * fCellWidth), -fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pBottomFace);
		}
	}

	CPolygon* pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(k++, pFrontFace);

	CPolygon* pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(k++, pBackFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CWallMesh::~CWallMesh()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCarMesh::CCarMesh(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth) : CMesh(9)
{
	FLOAT fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	INT i = 0;

	// Top/Top
	CPolygon* pFace = new CPolygon(4);
	pFace->SetVertex(0, CVertex(-fx, +(fy * 0.5f + 0.3f), +(fz * 0.5f)));
	pFace->SetVertex(1, CVertex(+fx, +(fy * 0.5f + 0.3f), +(fz * 0.5f)));
	pFace->SetVertex(2, CVertex(+fx, -(fy * 0.5f + 0.3f), +(fz * 0.5f)));
	pFace->SetVertex(3, CVertex(-fx, -(fy * 0.5f + 0.3f), +(fz * 0.5f)));
	SetPolygon(i++, pFace);

	// Top/Front
	pFace = new CPolygon(4);
	pFace->SetVertex(0, CVertex(+fx, +(fy * 0.5f + 0.3f), +(fz * 0.5f)));
	pFace->SetVertex(1, CVertex(-fx, +(fy * 0.5f + 0.3f), +(fz * 0.5f)));
	pFace->SetVertex(2, CVertex(-fx, +(fy * 0.5f + 1.5f), 3.0f));
	pFace->SetVertex(3, CVertex(+fx, +(fy * 0.5f + 1.5f), 3.0f));
	SetPolygon(i++, pFace);

	// Top/Back
	pFace = new CPolygon(4);
	pFace->SetVertex(0, CVertex(-fx, -(fy * 0.5f + 0.3f), +(fz * 0.5f)));
	pFace->SetVertex(1, CVertex(+fx, -(fy * 0.5f + 0.3f), +(fz * 0.5f)));
	pFace->SetVertex(2, CVertex(+fx, -(fy * 0.5f + 1.5f), 3.0f));
	pFace->SetVertex(3, CVertex(-fx, -(fy * 0.5f + 1.5f), 3.0f));
	SetPolygon(i++, pFace);

	// Top/Left
	pFace = new CPolygon(4);
	pFace->SetVertex(0, CVertex(-fx, +(fy * 0.5f + 0.3f), +(fz * 0.5f)));
	pFace->SetVertex(1, CVertex(-fx, -(fy * 0.5f + 0.3f), +(fz * 0.5f)));
	pFace->SetVertex(2, CVertex(-fx, -(fy * 0.5f + 1.5f), 3.0f));
	pFace->SetVertex(3, CVertex(-fx, +(fy * 0.5f + 1.5f), 3.0f));
	SetPolygon(i++, pFace);

	// Top/Right
	pFace = new CPolygon(4);
	pFace->SetVertex(0, CVertex(+fx, -(fy * 0.5f + 0.3f), +(fz * 0.5f)));
	pFace->SetVertex(1, CVertex(+fx, +(fy * 0.5f + 0.3f), +(fz * 0.5f)));
	pFace->SetVertex(2, CVertex(+fx, +(fy * 0.5f + 1.5f), 3.0f));
	pFace->SetVertex(3, CVertex(+fx, -(fy * 0.5f + 1.5f), 3.0f));
	SetPolygon(i++, pFace);

	// Bottom/Top
	pFace = new CPolygon(4);
	pFace->SetVertex(0, CVertex(-fx, +(fy * 0.5f + 4.0f), 3.0f));
	pFace->SetVertex(1, CVertex(+fx, +(fy * 0.5f + 4.0f), 3.0f));
	pFace->SetVertex(2, CVertex(+fx, -(fy * 0.5f + 4.0f), 3.0f));
	pFace->SetVertex(3, CVertex(-fx, -(fy * 0.5f + 4.0f), 3.0f));
	SetPolygon(i++, pFace);

	// Bottom/Bottom
	pFace = new CPolygon(4);
	pFace->SetVertex(0, CVertex(-fx, +(fy * 0.5f + 4.0f), 5.5f));
	pFace->SetVertex(1, CVertex(+fx, +(fy * 0.5f + 4.0f), 5.5f));
	pFace->SetVertex(2, CVertex(+fx, -(fy * 0.5f + 4.0f), 5.5f));
	pFace->SetVertex(3, CVertex(-fx, -(fy * 0.5f + 4.0f), 5.5f));
	SetPolygon(i++, pFace);

	// Bottom/Left
	pFace = new CPolygon(4);
	pFace->SetVertex(0, CVertex(+fx, -(fy * 0.5f + 4.0f), 3.0f));
	pFace->SetVertex(1, CVertex(+fx, +(fy * 0.5f + 4.0f), 3.0f));
	pFace->SetVertex(2, CVertex(+fx, +(fy * 0.5f + 4.0f), 5.5f));
	pFace->SetVertex(3, CVertex(+fx, -(fy * 0.5f + 4.0f), 5.5f));
	SetPolygon(i++, pFace);

	// Bottom/Right
	pFace = new CPolygon(4);
	pFace->SetVertex(0, CVertex(-fx, +(fy * 0.5f + 4.0f), 3.0f));
	pFace->SetVertex(1, CVertex(-fx, -(fy * 0.5f + 4.0f), 3.0f));
	pFace->SetVertex(2, CVertex(-fx, -(fy * 0.5f + 4.0f), 5.5f));
	pFace->SetVertex(3, CVertex(-fx, +(fy * 0.5f + 4.0f), 5.5f));
	SetPolygon(i++, pFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CCarMesh::~CCarMesh()
{
}

