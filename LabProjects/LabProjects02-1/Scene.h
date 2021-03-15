#pragma once
#include "GameObject.h"
#include "Camera.h"

class CScene
{
private:
	//���� ��ü���� ������ ���� ��ü���� ����Ʈ(�迭)�̴�
	INT				m_nObjects = 0;
	CGameObject**	m_ppObjects = nullptr;
public:
	CScene() {}
	virtual ~CScene() {}
public:
	// ���� ��ü���� �����ϰ� �Ҹ��Ų��
	virtual void BuildObjects();
	virtual void ReleaseObjects();
public:
	// ���� ��ü���� �ִϸ��̼��Ѵ�
	virtual void Animate(FLOAT fElapsedTime);
	// ���� ��ü���� �������Ѵ�
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);
};