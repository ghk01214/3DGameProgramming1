#pragma once
#include "Mesh.h"

class CGameObject
{
private:
	// ���� ��ü�� ��ġ(���� ��ǥ��)�̴�
	FLOAT m_fxPosition = 0.0f;
	FLOAT m_fyPosition = 0.0f;
	FLOAT m_fzPosition = 0.0f;
private:
	// ���� ��ü�� x��, y��, z�� ȸ�� ��(���� �������� �ݽð� ����)�̴�
	FLOAT m_fxRotation = 0.0f;
	FLOAT m_fyRotation = 0.0f;
	FLOAT m_fzRotation = 0.0f;
private:
	// ���� ��ü�� x��, y��, z�� ȸ�� ���̴�
	FLOAT m_fxRotationSpeed = 0.0f;
	FLOAT m_fyRotationSpeed = 0.0f;
	FLOAT m_fzRotationSpeed = 0.0f;
private:
	// ���� ��ü�� ���(�޽�, ��)�̴�
	CMesh* m_pMesh = nullptr;
	// ���� ��ü�� ����(������ ����)�̴�
	DWORD m_dwColor = RGB(255, 0, 0);
public:
	CGameObject() {};
	~CGameObject();
public:
	void SetMesh(CMesh* pMesh)
	{
		m_pMesh = pMesh;

		if (pMesh)
		{
			pMesh->AddRef();
		}
	}
	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }
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
	void SetRotationSpeed(FLOAT x, FLOAT y, FLOAT z)
	{
		m_fxRotationSpeed = x;
		m_fyRotationSpeed = y;
		m_fzRotationSpeed = z;
	}
public:
	// ���� ��ü�� x, y, z������ �̵��Ѵ�
	void Move(FLOAT x, FLOAT y, FLOAT z);
	// ���� ��ü�� x, y, z���� �������� ȸ���Ѵ�
	void Rotate(FLOAT x, FLOAT y, FLOAT z);
public:
	// �޽��� ���� �ϳ��� ���� ��ü�� ��ġ�� ������ ����Ͽ� ���� ��ǥ ��ȯ�� �Ѵ�
	CPoint3D WorldTransform(CPoint3D& f3Model);
	// ���� ��ü�� �ִϸ��̼� �Ѵ�
	virtual void Animate(FLOAT fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer);
};							  