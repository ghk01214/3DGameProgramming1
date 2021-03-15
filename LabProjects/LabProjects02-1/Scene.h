#pragma once
#include "GameObject.h"
#include "Camera.h"

class CScene
{
private:
	//게임 객체들의 개수와 게임 객체들의 리스트(배열)이다
	INT				m_nObjects = 0;
	CGameObject**	m_ppObjects = nullptr;
public:
	CScene() {}
	virtual ~CScene() {}
public:
	// 게임 객체들을 생성하고 소멸시킨다
	virtual void BuildObjects();
	virtual void ReleaseObjects();
public:
	// 게임 객체들을 애니메이션한다
	virtual void Animate(FLOAT fElapsedTime);
	// 게임 객체들을 렌더링한다
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);
};