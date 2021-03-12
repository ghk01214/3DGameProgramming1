#include "stdafx.h"
#include "Mesh.h"
#include "GraphicsPipeline.h"

CPolygon::CPolygon(INT nVertices)
{
	m_nVertices = nVertices;
	m_pVertices = new CVertex[nVertices];
}

CPolygon::~CPolygon()
{
	if (m_pVertices)
	{
		delete[] m_pVertices;
	}
}

void CPolygon::SetVertex(INT nIndex, CVertex vertex)
{
	if ((nIndex >= 0) && (nIndex < m_nVertices) && m_pVertices)
	{
		m_pVertices[nIndex] = vertex;
	}
}

CMesh::CMesh(INT nPolygons)
{
	m_nPolygons = nPolygons;
	m_ppPolygons = new CPolygon * [nPolygons];
}

CMesh::~CMesh()
{
	if (m_ppPolygons)
	{
		for (INT i = 0; i < m_nPolygons; ++i)
		{
			if (m_ppPolygons[i])
			{
				delete m_ppPolygons[i];
			}

			delete[] m_ppPolygons;
		}
	}
}

void CMesh::SetPolygon(INT nIndex, CPolygon* pPolygon)
{
	// 메쉬의 다각형을 설정한다
	if ((nIndex >= 0) && (nIndex < m_nPolygons))
	{
		m_ppPolygons[nIndex] = pPolygon;
	}
}

void CMesh::Render(HDC hDCFramebuffer)
{
	CPoint3D f3InitialProject, f3PreviousProject, f3Intersect;
	BOOL bPreviousInside = FALSE, bInitialInside = FALSE, bCurrentInside = FALSE, bIntersectInside = FALSE;

	// 메쉬를 구성하는 모든 다각형들을 렌더링한다
	for (INT i = 0; i < m_nPolygons; ++i)
	{
		INT nVertices = m_ppPolygons[i]->m_nVertices;
		CVertex* pVertices = m_ppPolygons[i]->m_pVertices;

		// 다각형의 첫 번째 정점을 원근 투영 변환한다
		f3PreviousProject = f3InitialProject = CGraphicsPipeline::Project(pVertices[0].m_f3Position);
		// 변환된 점이 투영 사각형에 포함되는 가를 계산한다
		bPreviousInside = bInitialInside = (f3InitialProject.x >= -1.0f) && (f3InitialProject.x <= 1.0f) && (f3InitialProject.y >= -1.0f) && (f3InitialProject.y <= 1.0f);

		// 다각형을 구성하는 모든 정점들을 원근 투영 변환하고 선분으로 렌더링한다
		for (INT j = 1; j < nVertices; ++j)
		{
			CPoint3D f3CurrentProject = CGraphicsPipeline::Project(pVertices[j].m_f3Position);]
			// 변환된 점이 투영 사각형에 포함되는 가를 계산한다
			bCurrentInside = (f3CurrentProject.x >= -1.0f) && (f3CurrentProject.x <= 1.0f) && (f3CurrentProject.y >= -1.0f) && (f3CurrentProject.y <= 1.0f);

			// 변환 점이 투영 사각형에 포함되면 이전 점과 현재 점을 선분으로 그린다
			if (((f3PreviousProject.z >= 0.0f) || (f3CurrentProject.z >= 0.0f)) && (bCurrentInside || bPreviousInside))
			{
				::Draw2DLine(hDCFramebuffer, f3PreviousProject, f3CurrentProject);
				
				f3PreviousProject = f3CurrentProject;
				bPreviousInside = bCurrentInside;
			}

			// 다각형의 마지막 정점과 다각형의 시작점을 선분으로 그린다
			if (((f3PreviousProject.z >= 0.0f) || (f3InitialProject.z >= 0.0f)) && (bInitialInside || bPreviousInside))
			{
				::Draw2DLine(hDCFramebuffer, f3PreviousProject, f3InitialProject);
			}
		}
	}
}

CCubeMesh::CCubeMesh(FLOAT fwidth, FLOAT fheight, FLOAT fDepth) : CMesh(6)
{
	FLOAT fHalfWidth = fwidth * 0.5f;
	FLOAT fHalfHeight = fheight * 0.5f;
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
}

CCubeMesh::~CCubeMesh()
{
}

void Draw2DLine(HDC hDCFrameBuffer, CPoint3D& f3PreviousProject, CPoint3D& f3CurrentProject)
{
	// 투영 좌표계의 2점을 화면 좌표계로 변환하고 변환된 두 점(픽셀)을 선분으로 그린다
	CPoint3D f3Previous = CGraphicsPipeline::ScreenTransform(f3PreviousProject);
	CPoint3D f3Current = CGraphicsPipeline::ScreenTransform(f3CurrentProject);

	::MoveToEx(hDCFrameBuffer, (LONG)f3Previous.x, (LONG)f3Previous.y, NULL);
	::LineTo(hDCFrameBuffer, (LONG)f3Current.x, (LONG)f3Current.y);
}