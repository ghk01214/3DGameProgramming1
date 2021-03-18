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
	// ī�޶��� ��ġ(���� ��ǥ��)
	FLOAT m_fxPosition{ 0.0f };
	FLOAT m_fyPosition{ 0.0f };
	FLOAT m_fzPosition{ 0.0f };
private:
	// ī�޶��� ȸ��(ī�޶� ��ǥ��)
	FLOAT m_fxRotation{0.0f};
	FLOAT m_fyRotation{0.0f};
	FLOAT m_fzRotation{0.0f};
private:
	// ī�޶��� �þ߰�, ���� �簢�������� �Ÿ�
	FLOAT m_fFOVAngle{ 90.0f };
	FLOAT m_fProjectRectDistance{ 1.0f };
private:
	// ����Ʈ
	CViewport*	m_pViewport{ nullptr };
	//����Ʈ�� ���� ���̿� ���� ������ ����(��Ⱦ��: Aspect Ratio)
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
	// ī�޶� ��ȯ, ���� ��ȯ, ȭ�� ��ȯ�� �����ѵ�
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
	// ī�޶��� ����Ʈ�� �þ߰��� �����Ѵ�
	void SetViewport(INT xStart, INT yStart, INT nWidth, INT nHeight);
	void SetFOVAngle(FLOAT fFOVAngle);
public:
	// ī�޶� �̵��ϰ� �̵��ϰ� ȸ���Ѵ�
	void Move(FLOAT x, FLOAT y, FLOAT z);
	void Rotate(FLOAT fPitch, FLOAT fYaw, FLOAT fRoll);
};