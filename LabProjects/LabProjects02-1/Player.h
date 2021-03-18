#pragma once
#include "GameObject.h"
#include "Camera.h"

// 플레이어 객체도 게임 객체이므로 CGameObject 클래스에서 상속하여 클래스를 생성한다
class CPlayer : public CGameObject
{
private:
	// 플레이어 객체에 포함된 카메라이다
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