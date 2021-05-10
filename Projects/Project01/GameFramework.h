#pragma once

//-----------------------------------------------------------------------------
// File: GameFramework.h
//-----------------------------------------------------------------------------

#include "Timer.h"
#include "Scene.h"
#include "Player.h"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	BOOL OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();
	void FrameAdvance();

	void SetActive(BOOL bActive) { m_bActive = bActive; }

private:
	HINSTANCE					m_hInstance = nullptr;
	HWND						m_hWnd = nullptr;

    BOOL						m_bActive = TRUE;          

	CGameTimer					m_GameTimer;

	HDC							m_hDCFrameBuffer = nullptr;
    HBITMAP						m_hBitmapFrameBuffer = nullptr;

	CPlayer						*m_pPlayer = nullptr;
	CScene						*m_pScene = nullptr;
	CGameObject					*m_pSelectedObject = nullptr;

public:
	void BuildFrameBuffer();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer();

	void BuildObjects();
	void ReleaseObjects();
	void ProcessInput();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	POINT						m_ptOldCursorPos;

	_TCHAR						m_pszFrameRate[50];
};

