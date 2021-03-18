#pragma once
#include "GameObject.h"
#include "Camera.h"

// �÷��̾� ��ü�� ���� ��ü�̹Ƿ� CGameObject Ŭ�������� ����Ͽ� Ŭ������ �����Ѵ�
class CPlayer : public CGameObject
{
private:
	// �÷��̾� ��ü�� ���Ե� ī�޶��̴�
	CCamera* m_pCamera{ nullptr };
public:
	CPlayer() {}
	virtual ~CPlayer()
	{
		if (m_pCamera)
		{
			delete m_pCamera;
		}
	}
public:
	void SetPosition(FLOAT x, FLOAT y, FLOAT z);
	void SetRotation(FLOAT x, FLOAT y, FLOAT z);
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
public:
	CCamera* GetCamera() { return m_pCamera; }
public:
	void Move(FLOAT x, FLOAT y, FLOAT z);
	void Rotate(FLOAT fPitch, FLOAT fYaw, FLOAT fRoll);
};