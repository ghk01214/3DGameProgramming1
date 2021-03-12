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
	CVertex(FLOAT x, FLOAT y, FLOAT z) { m_f3Position = CPoint3D(x, y, z); }
	virtual ~CVertex() {};
};

class CPolygon
{
public:
	// 다각형(면)을 구성하는 정점들의 리스트이다
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
	// 인스턴싱(Instancing)을 위하여 메쉬는 게임 객체들에 공유될 수 있다
	// 다음 참조값(Reference Count)은 메쉬가 공유되는 게임 객체의 개수를 나타낸다
	INT m_nReferences = 1;
private:
	// 메쉬를 구성하는 다각형(면)들의 리스트이다.
	INT			m_nPolygons = 0;
	CPolygon**	m_ppPolygons = nullptr;
public:
	CMesh() {};
	CMesh(INT nPolygons);
	virtual ~CMesh();
public:
	// 메쉬가 게임 객체에 공유될 때마다 참조값을 1씩 증가시킨다
	void AddRef() { ++m_nReferences; }
	// 메쉬를 공유하는 게임 객체가 소멸될 때마다 참조값을 1씩 감소시킨다
	// 참조값이 0이 되면 메쉬를 소멸시킨다.
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
	//메쉬를 렌더링한다
	virtual void Render(HDC hDCFramebuffer);
};

// 직육면체 클래스를 선언한다
class CCubeMesh : public CMesh
{
public:
	CCubeMesh(FLOAT fwidth = 4.0f, FLOAT fheight = 4.0f, FLOAT fDepth = 4.0f);
	virtual ~CCubeMesh();
};