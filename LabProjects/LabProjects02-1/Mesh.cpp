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
	// �޽��� �ٰ����� �����Ѵ�
	if ((nIndex >= 0) && (nIndex < m_nPolygons))
	{
		m_ppPolygons[nIndex] = pPolygon;
	}
}

void CMesh::Render(HDC hDCFramebuffer)
{
	CPoint3D f3InitialProject, f3PreviousProject, f3Intersect;
	BOOL bPreviousInside = FALSE, bInitialInside = FALSE, bCurrentInside = FALSE, bIntersectInside = FALSE;

	// �޽��� �����ϴ� ��� �ٰ������� �������Ѵ�
	for (INT i = 0; i < m_nPolygons; ++i)
	{
		INT nVertices = m_ppPolygons[i]->m_nVertices;
		CVertex* pVertices = m_ppPolygons[i]->m_pVertices;

		// �ٰ����� ù ��° ������ ���� ���� ��ȯ�Ѵ�
		f3PreviousProject = f3InitialProject = CGraphicsPipeline::Project(pVertices[0].m_f3Position);
		// ��ȯ�� ���� ���� �簢���� ���ԵǴ� ���� ����Ѵ�
		bPreviousInside = bInitialInside = (f3InitialProject.x >= -1.0f) && (f3InitialProject.x <= 1.0f) && (f3InitialProject.y >= -1.0f) && (f3InitialProject.y <= 1.0f);

		// �ٰ����� �����ϴ� ��� �������� ���� ���� ��ȯ�ϰ� �������� �������Ѵ�
		for (INT j = 1; j < nVertices; ++j)
		{
			CPoint3D f3CurrentProject = CGraphicsPipeline::Project(pVertices[j].m_f3Position);]
			// ��ȯ�� ���� ���� �簢���� ���ԵǴ� ���� ����Ѵ�
			bCurrentInside = (f3CurrentProject.x >= -1.0f) && (f3CurrentProject.x <= 1.0f) && (f3CurrentProject.y >= -1.0f) && (f3CurrentProject.y <= 1.0f);

			// ��ȯ ���� ���� �簢���� ���ԵǸ� ���� ���� ���� ���� �������� �׸���
			if (((f3PreviousProject.z >= 0.0f) || (f3CurrentProject.z >= 0.0f)) && (bCurrentInside || bPreviousInside))
			{
				::Draw2DLine(hDCFramebuffer, f3PreviousProject, f3CurrentProject);
				
				f3PreviousProject = f3CurrentProject;
				bPreviousInside = bCurrentInside;
			}

			// �ٰ����� ������ ������ �ٰ����� �������� �������� �׸���
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
	// ���� ��ǥ���� 2���� ȭ�� ��ǥ��� ��ȯ�ϰ� ��ȯ�� �� ��(�ȼ�)�� �������� �׸���
	CPoint3D f3Previous = CGraphicsPipeline::ScreenTransform(f3PreviousProject);
	CPoint3D f3Current = CGraphicsPipeline::ScreenTransform(f3CurrentProject);

	::MoveToEx(hDCFrameBuffer, (LONG)f3Previous.x, (LONG)f3Previous.y, NULL);
	::LineTo(hDCFrameBuffer, (LONG)f3Current.x, (LONG)f3Current.y);
}