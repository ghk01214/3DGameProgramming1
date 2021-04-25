#include "stdafx.h"
#include "Mesh.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPolygon::CPolygon(INT nVertices)
{
	if (nVertices > 0)
	{
		this->nVertices = nVertices;
		this->pVertices = new CVertex[nVertices];
	}
}

CPolygon::~CPolygon()
{
	if (pVertices)
	{
		delete[] pVertices;
	}
}

void CPolygon::SetVertex(INT nIndex, CVertex& vertex)
{
	if ((0 <= nIndex) && (nIndex < nVertices) && pVertices)
	{
		pVertices[nIndex] = vertex;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMesh::CMesh(INT nPolygons)
{
	if (nPolygons > 0)
	{
		this->nPolygons = nPolygons;
		ppPolygons = new CPolygon*[nPolygons];
		nReferences = 0;
	}

	xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CMesh::~CMesh()
{
	if (ppPolygons)
	{
		for (INT i{ 0 }; i < nPolygons; ++i)
		{
			if (ppPolygons[i])
			{
				delete ppPolygons[i];
			}
		}

		delete[] ppPolygons;
	}
}

void CMesh::SetPolygon(INT nIndex, CPolygon *pPolygon)
{
	if ((0 <= nIndex) && (nIndex < nPolygons) && ppPolygons)
	{
		ppPolygons[nIndex] = pPolygon;
	}
}

void CMesh::Render(HDC hDCFrameBuffer, XMFLOAT4X4& xmf4x4World, CCamera *pCamera)
{
	XMFLOAT4X4 xmf4x4Transform{ Matrix4x4::Multiply(xmf4x4World, pCamera->xmf4x4ViewProject) };

	for (INT j{ 0 }; j < nPolygons; ++j)
	{
		INT			 nVertices{ ppPolygons[j]->nVertices };
		CVertex*	 pVertices{ ppPolygons[j]->pVertices };
		XMFLOAT3	 xmf3Previous(-1.0f, 0.0f, 0.0f);

		for (INT i{ 0 }; i <= nVertices; ++i)
		{
			CVertex		vertex{ pVertices[i % nVertices] };
			//World/View/Projection Transformation(Perspective Divide)
			XMFLOAT3	xmf3Current{ Vector3::TransformCoord(vertex.xmf3Position, xmf4x4Transform) };

			if ((xmf3Current.z >= 0.0f) && (xmf3Current.z <= 1.0f))
			{
				//Screen Transformation
				xmf3Current.x = +xmf3Current.x * (pCamera->d3dViewport.Width * 0.5f) + pCamera->d3dViewport.TopLeftX + (pCamera->d3dViewport.Width * 0.5f);
				xmf3Current.y = -xmf3Current.y * (pCamera->d3dViewport.Height * 0.5f) + pCamera->d3dViewport.TopLeftY + (pCamera->d3dViewport.Height * 0.5f);

				if (xmf3Previous.x >= 0.0f)
				{
					::MoveToEx(hDCFrameBuffer, (LONG)xmf3Previous.x, (LONG)xmf3Previous.y, nullptr);
					::LineTo(hDCFrameBuffer, (LONG)xmf3Current.x, (LONG)xmf3Current.y);
				}
				
				xmf3Previous = xmf3Current;
			}
		}
	}
}

BOOL CMesh::RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, FLOAT *pfNearHitDistance)
{
	FLOAT	 fHitDistance;
	BOOL	 bIntersected{ TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance) };

	if (bIntersected && (fHitDistance < *pfNearHitDistance))
	{
		*pfNearHitDistance = fHitDistance;
	}

	return bIntersected;
}

INT CMesh::CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, FLOAT *pfNearHitDistance)
{
	INT		 nIntersections{ 0 };
	BOOL	 bIntersected{ xmOOBB.Intersects(xmvPickRayOrigin, xmvPickRayDirection, *pfNearHitDistance) };

	if (bIntersected)
	{
		for (INT i{ 0 }; i < nPolygons; ++i)
		{
			switch (ppPolygons[i]->nVertices)
			{
				case 3:
				{
					XMVECTOR	 v0{ XMLoadFloat3(&(ppPolygons[i]->pVertices[0].xmf3Position)) };
					XMVECTOR	 v1{ XMLoadFloat3(&(ppPolygons[i]->pVertices[1].xmf3Position)) };
					XMVECTOR	 v2{ XMLoadFloat3(&(ppPolygons[i]->pVertices[2].xmf3Position)) };
					BOOL		 bIntersected{ RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance) };

					if (bIntersected)
					{
						++nIntersections;
					}

					break;
				}
				case 4:
				{
					XMVECTOR	 v0{ XMLoadFloat3(&(ppPolygons[i]->pVertices[0].xmf3Position)) };
					XMVECTOR	 v1{ XMLoadFloat3(&(ppPolygons[i]->pVertices[1].xmf3Position)) };
					XMVECTOR	 v2{ XMLoadFloat3(&(ppPolygons[i]->pVertices[2].xmf3Position)) };
					BOOL		 bIntersected{ RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance) };

					if (bIntersected)
					{
						++nIntersections;
					}

					v0 = XMLoadFloat3(&(ppPolygons[i]->pVertices[0].xmf3Position));
					v1 = XMLoadFloat3(&(ppPolygons[i]->pVertices[2].xmf3Position));
					v2 = XMLoadFloat3(&(ppPolygons[i]->pVertices[3].xmf3Position));
					bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);

					if (bIntersected)
					{
						++nIntersections;
					}

					break;
				}
			}
		}
	}
	return nIntersections;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMesh::CCubeMesh(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth) : CMesh(6)
{
	FLOAT fHalfWidth{ fWidth * 0.5f };
	FLOAT fHalfHeight{ fHeight * 0.5f };
	FLOAT fHalfDepth{ fDepth * 0.5f };

	CPolygon* pFrontFace{ new CPolygon(4) };
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(0, pFrontFace);

	CPolygon *pTopFace{ new CPolygon(4) };
	pTopFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pTopFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	SetPolygon(1, pTopFace);

	CPolygon *pBackFace{ new CPolygon(4) };
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(2, pBackFace);

	CPolygon *pBottomFace{ new CPolygon(4) };
	pBottomFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBottomFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(3, pBottomFace);

	CPolygon *pLeftFace{ new CPolygon(4) };
	pLeftFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pLeftFace->SetVertex(1, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(4, pLeftFace);

	CPolygon *pRightFace{ new CPolygon(4) };
	pRightFace->SetVertex(0, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pRightFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(5, pRightFace);

	xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CCubeMesh::~CCubeMesh()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CWallMesh::CWallMesh(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, INT nSubRects) : CMesh((4 * nSubRects * nSubRects)+2)
{
	FLOAT fHalfWidth{ fWidth * 0.5f };
	FLOAT fHalfHeight{ fHeight * 0.5f };
	FLOAT fHalfDepth{ fDepth * 0.5f };
	FLOAT fCellWidth{ fWidth * (1.0f / nSubRects) };
	FLOAT fCellHeight{ fHeight * (1.0f / nSubRects) };
	FLOAT fCellDepth{ fDepth * (1.0f / nSubRects) };

	INT k{ 0 };
;
	for (INT i{ 0 }; i < nSubRects; ++i)
	{
		for (INT j{ 0 }; j < nSubRects; ++j)
		{
			CPolygon* pLeftFace{ new CPolygon(4) };
			pLeftFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight+(i*fCellHeight), -fHalfDepth+(j*fCellDepth)));
			pLeftFace->SetVertex(1, CVertex(-fHalfWidth, -fHalfHeight+((i+1)*fCellHeight), -fHalfDepth+(j*fCellDepth)));
			pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight+((i+1)*fCellHeight), -fHalfDepth+((j+1)*fCellDepth)));
			pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight+(i*fCellHeight), -fHalfDepth+((j+1)*fCellDepth)));
			SetPolygon(++k, pLeftFace);
		}
	}

	
	for (INT i{ 0 }; i < nSubRects; ++i)
	{
		for (INT j{ 0 }; j < nSubRects; ++j)
		{
			CPolygon* pRightFace = new CPolygon(4);
			pRightFace->SetVertex(0, CVertex(+fHalfWidth, -fHalfHeight + (i*fCellHeight), -fHalfDepth + (j*fCellDepth)));
			pRightFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight + ((i + 1)*fCellHeight), -fHalfDepth + (j*fCellDepth)));
			pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight + ((i + 1)*fCellHeight), -fHalfDepth + ((j + 1)*fCellDepth)));
			pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight + (i*fCellHeight), -fHalfDepth + ((j + 1)*fCellDepth)));
			SetPolygon(++k, pRightFace);
		}
	}


	for (INT i{ 0 }; i < nSubRects; ++i)
	{
		for (INT j{ 0 }; j < nSubRects; ++j)
		{
			CPolygon* pTopFace = new CPolygon(4);
			pTopFace->SetVertex(0, CVertex(-fHalfWidth + (i*fCellWidth), +fHalfHeight, -fHalfDepth + (j*fCellDepth)));
			pTopFace->SetVertex(1, CVertex(-fHalfWidth + ((i + 1)*fCellWidth), +fHalfHeight, -fHalfDepth + (j*fCellDepth)));
			pTopFace->SetVertex(2, CVertex(-fHalfWidth + ((i + 1)*fCellWidth), +fHalfHeight, -fHalfDepth + ((j + 1)*fCellDepth)));
			pTopFace->SetVertex(3, CVertex(-fHalfWidth + (i*fCellWidth), +fHalfHeight, -fHalfDepth + ((j + 1)*fCellDepth)));
			SetPolygon(++k, pTopFace);
		}
	}


	for (INT i{ 0 }; i < nSubRects; ++i)
	{
		for (INT j{ 0 }; j < nSubRects; ++j)
		{
			CPolygon* pBottomFace = new CPolygon(4);
			pBottomFace->SetVertex(0, CVertex(-fHalfWidth + (i*fCellWidth), -fHalfHeight, -fHalfDepth + (j*fCellDepth)));
			pBottomFace->SetVertex(1, CVertex(-fHalfWidth + ((i + 1)*fCellWidth), -fHalfHeight, -fHalfDepth + (j*fCellDepth)));
			pBottomFace->SetVertex(2, CVertex(-fHalfWidth + ((i + 1)*fCellWidth), -fHalfHeight, -fHalfDepth + ((j + 1)*fCellDepth)));
			pBottomFace->SetVertex(3, CVertex(-fHalfWidth + (i*fCellWidth), -fHalfHeight, -fHalfDepth + ((j + 1)*fCellDepth)));
			SetPolygon(++k, pBottomFace);
		}
	}

	CPolygon *pFrontFace{ new CPolygon(4) };
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(++k, pFrontFace);

	CPolygon *pBackFace{ new CPolygon(4) };
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(++k, pBackFace);

	xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CWallMesh::~CWallMesh()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAirplaneMesh::CAirplaneMesh(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth) : CMesh(24)
{
	FLOAT fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	FLOAT x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1)*x2 + (fy - y3);
	INT i = 0;

	//Upper Plane
	CPolygon* pFace{ new CPolygon(3) };
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(2, CVertex(0.0f, 0.0f, -fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, -fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(+fx, -y3, -fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, -fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, -fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, -fz));
	SetPolygon(++i, pFace);

	//Lower Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(0.0f, 0.0f, +fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, +fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(0.0f, 0.0f, +fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, +fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(-fx, -y3, +fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, +fz));
	SetPolygon(++i, pFace);

	//Right Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(2, CVertex(+x2, +y2, -fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(2, CVertex(+x2, +y2, +fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(+x2, +y2, +fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, -fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+fx, -y3, -fz));
	pFace->SetVertex(1, CVertex(+x2, +y2, +fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, +fz));
	SetPolygon(++i, pFace);

	//Back/Right Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(+fx, -y3, -fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, +fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(+fx, -y3, +fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, +fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, +fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(0.0f, 0.0f, +fz));
	SetPolygon(++i, pFace);

	//Left Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(2, CVertex(-x2, +y2, -fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(-x2, +y2, -fz));
	pFace->SetVertex(2, CVertex(-x2, +y2, +fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(-x2, +y2, -fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, -fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(-fx, -y3, -fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, +fz));
	SetPolygon(++i, pFace);

	//Back/Left Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(0.0f, 0.0f, +fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, +fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, -fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, +fz));
	SetPolygon(++i, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(-fx, -y3, +fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, -fz));
	SetPolygon(++i, pFace);

	xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CAirplaneMesh::~CAirplaneMesh()
{
}