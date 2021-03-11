#pragma once

class CPoint3D
{
public:
	FLOAT x = 0.0f;
	FLOAT y = 0.0f;
	FLOAT z = 0.0f;
public:
	CPoint3D() {};
	CPoint3D(FLOAT x, FLOAT y, FLOAT z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	virtual ~CPoint3D() {};
};

class CVertex
{
public:
	CPoint3D m_f3Position;
public:
	CVertex() {};
	CVertex(FLOAT x, FLOAT y, FLOAT z)
	{
		m_f3Position = CPoint3D(x, y, z);
	}
	virtual ~CVertex() {};
};

class CPolygon
{
public:
	INT			m_nVertices = 0;
	CVertex*	m_pVertices = nullptr;
public:
	CPolygon() {};
	CPolygon(INT nVertices);
	virtual ~CPolygon();
public:
	void SetVertex(INT nIndex, CVertex vertex);
};

class CMesh
{
private:
	INT m_nReferences = 1;
private:
	INT			m_nPolygons = 0;
	CPolygon**	m_ppPolygons = nullptr;
public:
	CMesh() {};
	CMesh(INT nPolygons);
	virtual ~CMesh();
public:
	void AddRef() { ++m_nReferences; }
	void Release()
	{
		--m_nReferences;

		if (m_nReferences <= 0)
		{
			delete this;
		}
	}
public:
	void SetPolygon(INT nIndex, CPolygon* pPolygon);
	virtual void Render(HDC hDCFramebuffer);
};

class CCubeMesh : public CMesh
{
public:
	CCubeMesh(FLOAT fwidth = 4.0f, FLOAT fheight = 4.0f, FLOAT fDepth = 4.0f);
	virtual ~CCubeMesh();
};