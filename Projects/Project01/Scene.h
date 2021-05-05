#pragma once

#include "GameObject.h"
#include "Player.h"

class CScene
{
public:
	CScene();
	virtual ~CScene();
public:
	virtual void BuildObjects();
	virtual void ReleaseObjects();
public:
	void CheckObjectByObjectCollisions();
	void CheckObjectByWallCollisions();
	void CheckPlayerByWallCollision();
	void CheckObjectByBulletCollisions();
public:
	virtual void Animate(FLOAT fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);
public:
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
public:
	CGameObject *PickObjectPointedByCursor(INT xClient, INT yClient, CCamera *pCamera);

public:
	CPlayer*					m_pPlayer = nullptr;
public:
	INT							m_nObjects = 0;
	CGameObject**				m_ppObjects = nullptr;
public:
	CWallsObject*				m_pWallsObject = nullptr;
};