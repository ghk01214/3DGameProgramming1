#pragma once

#include "GameObject.h"
#include "Player.h"

class CScene
{
public:
	CScene();
	virtual ~CScene();

	CPlayer*					m_pPlayer = nullptr;

	INT							m_nObjects = 0;
	CGameObject**				m_ppObjects = nullptr;

	CWallsObject*				m_pWallsObject = nullptr;

	BOOL						m_bGameOver = FALSE;

	virtual void BuildObjects();
	virtual void RespawnEnemyObjects();
	virtual void ReleaseObjects();
	virtual void ReleaseEnemyObjects(INT iObjectNum);

	void CheckObjectByObjectCollisions();
	void CheckObjectByWallCollisions();
	void CheckPlayerByWallCollision();
	void CheckPlayerByObjectCollision();
	void CheckObjectOutOfCamera();

	virtual void Animate(FLOAT fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};