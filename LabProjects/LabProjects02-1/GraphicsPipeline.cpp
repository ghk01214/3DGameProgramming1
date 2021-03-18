#include "stdafx.h"
#include "GraphicsPipeline.h"

CGameObject* CGraphicsPipeline::m_pGameObject	{ nullptr };
CCamera*	 CGraphicsPipeline::m_pCamera		{ nullptr };

// 투영 좌표계의 점을 화면 변환을 수행한다
// 화면 변환은 카메라의 정보(뷰포트)가 필요하다
CPoint3D CGraphicsPipeline::ScreenTransform(CPoint3D& f3Projection)
{
	CPoint3D f3Screen{ m_pCamera->ScreenTransform(f3Projection) };

	return f3Screen;
}

// 모델 좌표계의 점을 월드 변환, 카메라 변환, 원근 투영 변환을 순차적으로 수행한다
// 월드 변환은 게임 객체의 정보(위치와 방향)가 필요하다
// 카메라 변환은 카메라의 정보(위치와 방향)가 필요하다
// 원근 투영 변환은 카메라의 정보(시야각과 종횡비)가 필요하다
CPoint3D CGraphicsPipeline::Project(CPoint3D& f3Model)
{
	CPoint3D f3World		{ m_pGameObject->WorldTransform(f3Model) };
	CPoint3D f3Camera		{ m_pCamera->CameraTransform(f3World) };
	CPoint3D f3Projection	{ m_pCamera->ProjectionTransform(f3Camera) };

	return f3Projection;
}