#pragma once
#include "GameObject.h"
#include "Camera.h"

// �� ��ǥ���� ���� ���� ��ȯ, ī�޶� ��ȯ, ���� ���� ��ȯ�� �����ϱ� ���� Ŭ����
class CGraphicsPipeline
{
private:
	static CGameObject* m_pGameObject;
	static CCamera*		m_pCamera;
public:
	static void SetGameObject(CGameObject* pGameObject) { m_pGameObject = pGameObject; }
	static void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
public:
	static CPoint3D ScreenTransform(CPoint3D& f3Projection);
	static CPoint3D Project(CPoint3D& f3Model);
};