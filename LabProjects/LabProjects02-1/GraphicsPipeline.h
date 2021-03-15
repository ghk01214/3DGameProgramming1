#pragma once
#include "GameObject.h"
#include "Camera.h"

// 모델 좌표계의 점을 월드 변환, 카메라 변환, 원근 투영 변환을 수행하기 위한 클래스
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