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
	if ((nIndex >= 0) && (nIndex < m_nPolygons))
	{
		m_ppPolygons[nIndex] = pPolygon;
	}
}

void CMesh::Render(HDC hDCFramebuffer)
{

}

CCubeMesh::CCubeMesh(FLOAT fwidth, FLOAT fheight, FLOAT fDepth)
{
}

CCubeMesh::~CCubeMesh()
{
}

void Draw2DLine(HDC hDCFrameBuffer, CPoint3D& f3PreviousProject, CPoint3D& f3CurrentProject)
{
	CPoint3D f3Previous = CGraphicsPipeline::ScreenTransform(f3PreviousProject);
	CPoint3D f3Current = CGraphicsPipeline::ScreenTransform(f3CurrentProject);

	::MoveToEx(hDCFrameBuffer, (LONG)f3Previous.x, (LONG)f3Previous.y, NULL);
	::LineTo(hDCFrameBuffer, (LONG)f3Current.x, (LONG)f3Current.y);
}