#pragma once
#include "Mesh.h"

class CViewport
{
public:
	INT m_nLeft;
	INT m_nTop;
	INT m_nWidth;
	INT m_nHeight;
public:
	CViewport(INT nLeft, INT nTop, INT nWidth, INT nHeight) : m_nLeft(nLeft), m_nTop(nTop), m_nWidth(nWidth), m_nHeight(nHeight) {}
	virtual ~CViewport() {}
};

class CCamera
{
private:
	// 카메라의 위치(월드 좌표계)
	FLOAT m_fxPosition{ 0.0f };
	FLOAT m_fyPosition{ 0.0f };
	FLOAT m_fzPosition{ 0.0f };
private:
	// 카메라의 회전(카메라 좌표계)
	FLOAT m_fxRotation{0.0f};
	FLOAT m_fyRotation{0.0f};
	FLOAT m_fzRotation{0.0f};
private:
	// 카메라의 시야각, 투영 사각형까지의 거리
	FLOAT m_fFOVAngle{ 90.0f };
	FLOAT m_fProjectRectDistance{ 1.0f };
private:
	// 뷰포트
	CViewport*	m_pViewport{ nullptr };
	//뷰포트의 가로 길이와 세로 길이의 비율(종횡비: Aspect Ratio)
	FLOAT		m_fAspectRatio{ FLOAT(FRAMEBUFFER_WIDTH) / FLOAT(FRAMEBUFFER_HEIGHT) };
public:
	CCamera() {}
	virtual ~CCamera()
	{
		if (m_pViewport)
		{
			delete m_pViewport;
		}
	}
public:
	// 카메라 변환, 투영 변환, 화면 변환을 수행한디
	CPoint3D CameraTransform(CPoint3D& f3World);
	CPoint3D ProjectionTransform(CPoint3D& f3Camera);
	CPoint3D ScreenTransform(CPoint3D& f3Projection);
public:
	void SetPosition(FLOAT x, FLOAT y, FLOAT z)
	{
		m_fxPosition = x;
		m_fyPosition = y;
		m_fzPosition = z;
	}
	void SetRotation(FLOAT x, FLOAT y, FLOAT z)
	{
		m_fxRotation = x;
		m_fyRotation = y;
		m_fzRotation = z;
	}
	// 카메라의 뷰포트와 시야각을 설정한다
	void SetViewport(INT xStart, INT yStart, INT nWidth, INT nHeight);
	void SetFOVAngle(FLOAT fFOVAngle);
public:
	// 카메라를 이동하고 이동하고 회전한다
	void Move(FLOAT x, FLOAT y, FLOAT z);
	void Rotate(FLOAT fPitch, FLOAT fYaw, FLOAT fRoll);
};