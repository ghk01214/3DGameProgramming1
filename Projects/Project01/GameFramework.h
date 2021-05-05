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
public:
	BOOL OnCreate(HINSTANCE hInstance, HWND hWnd);
	void OnDestroy();
	void FrameAdvance();
public:
	void SetActive(BOOL bActive) { m_bActive = bActive; }
public:
	void BuildFrameBuffer();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer();
public:
	void BuildObjects();
	void ReleaseObjects();
	void ProcessInput();
public:
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE					m_hInstance = nullptr;
	HWND						m_hWnd = nullptr;
public:
	BOOL						m_bActive = TRUE;
public:
	CGameTimer					m_GameTimer;
public:
	HDC							m_hDCFrameBuffer = nullptr;
	HBITMAP						m_hBitmapFrameBuffer = nullptr;
public:
	CPlayer*					m_pPlayer = nullptr;
	CScene*						m_pScene = nullptr;
	CGameObject*				m_pSelectedObject = nullptr;
public:
	POINT						m_ptOldCursorPos;
public:
	_TCHAR						m_pszFrameRate[50];
};