#include "stdafx.h"
#include "Camera.h"

// ���� ��ǥ���� ���� ī�޶� ��ǥ��� ��ȯ�ϴ� �Լ�
CPoint3D CCamera::CameraTransform(CPoint3D& f3World)
{
	// ī�޶� ���� ��ǥ���� �������� �̵��Ѵ�
	CPoint3D f3Camera{ f3World };
	f3Camera.x -= m_fxPosition;
	f3Camera.y -= m_fyPosition;
	f3Camera.z -= m_fzPosition;

	FLOAT fPitch{ DegreeToRadian(-m_fxRotation) };
	FLOAT fYaw{ DegreeToRadian(-m_fyRotation) };
	FLOAT fRoll{ DegreeToRadian(-m_fzRotation) };

	// ī�����l ���� ��ǥ���� ��� ��ġ�ϵ��� ȸ���Ѵ�
	CPoint3D f3Rotated{ f3Camera };

	if (fPitch != 0.0f)
	{
		f3Rotated.y = FLOAT(f3Camera.y * cos(fPitch) - f3Camera.z * sin(fPitch));
		f3Rotated.z = FLOAT(f3Camera.y * sin(fPitch) + f3Camera.z * cos(fPitch));
		
		f3Camera.y = f3Rotated.y;
		f3Camera.z = f3Rotated.z;
	}

	if (fYaw != 0.0f)
	{
		f3Rotated.x = FLOAT(+f3Camera.x * cos(fYaw) + f3Camera.z * sin(fYaw));
		f3Rotated.z = FLOAT(-f3Camera.x * sin(fYaw) + f3Camera.z * cos(fYaw));

		f3Camera.x = f3Rotated.x;
		f3Camera.z = f3Rotated.z;
	}

	if (fRoll != 0.0f)
	{
		f3Rotated.x = FLOAT(f3Camera.x * cos(fRoll) - f3Camera.y * sin(fRoll));
		f3Rotated.y = FLOAT(f3Camera.x * sin(fRoll) + f3Camera.y * cos(fRoll));

		f3Camera.x = f3Rotated.x;
		f3Camera.y = f3Rotated.y;
	}

	return f3Camera;
}

// ī�޶� ��ǥ���� ���� ���� ��ǥ��� ��ȯ�ϴ� �Լ�
CPoint3D CCamera::ProjectionTransform(CPoint3D& f3Camera)
{
	CPoint3D f3Project{ f3Camera };

	if (f3Camera.z != 0.0f)
	{
		//ī�޶��� �þ߰��� 90���� �ƴ� ��� ���� �簢�������� �Ÿ��� ���Ѵ�
		f3Project.x = FLOAT((f3Camera.x * m_fProjectRectDistance) / (f3Camera.z * m_fAspectRatio));
		f3Project.y = FLOAT((f3Camera.y * m_fProjectRectDistance) / f3Camera.z);
		// ���� ��ǥ��� 2�����̹Ƿ� z��ǥ�� ī�޶� ��ǥ�� z��ǥ�� �����Ѵ�
		f3Project.z = f3Camera.z;
	}

	return f3Project;
}

// ���� ��ǥ���� ���� ȭ�� ��ǥ��� ��ȯ�ϴ� �Լ�
CPoint3D CCamera::ScreenTransform(CPoint3D& f3Projection)
{
	CPoint3D f3Screen{ f3Projection };

	FLOAT fHalfWidth{ m_pViewport->m_nWidth * 0.5f };
	FLOAT fHalfHeight{ m_pViewport->m_nHeight * 0.5f };
	
	f3Screen.x = (+f3Projection.x * fHalfWidth) + m_pViewport->m_nLeft + fHalfWidth;
	f3Screen.y = (-f3Projection.y * fHalfHeight) + m_pViewport->m_nTop + fHalfHeight;

	return f3Screen;
}

void CCamera::SetViewport(INT xStart, INT yStart, INT nWidth, INT nHeight)
{
	m_pViewport = new CViewport(xStart, yStart, nWidth, nHeight);
	m_fAspectRatio = FLOAT(m_pViewport->m_nWidth) / FLOAT(m_pViewport->m_nHeight);
}

void CCamera::SetFOVAngle(FLOAT fFOVAngle)
{
	m_fFOVAngle = fFOVAngle;
	m_fProjectRectDistance = FLOAT(1.0f / tan(DegreeToRadian(fFOVAngle * 0.5f)));
}

void CCamera::Move(FLOAT x, FLOAT y, FLOAT z)
{
	m_fxPosition += x;
	m_fyPosition += y;
	m_fzPosition += z;
}

void CCamera::Rotate(FLOAT fPitch, FLOAT fYaw, FLOAT fRoll)
{
	m_fxRotation += fPitch;
	m_fyRotation += fYaw;
	m_fzRotation += fRoll;
}