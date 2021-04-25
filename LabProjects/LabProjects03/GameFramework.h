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
	BOOL OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();
	void FrameAdvance();
public:
	void SetActive(BOOL bActive) { bActive = bActive; }

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
	HINSTANCE					hInstance{ nullptr };
	HWND						hWnd{ nullptr };
public:
	BOOL						bActive{ TRUE };
public:
	CGameTimer					GameTimer;
public:
	HDC							hDCFrameBuffer{ nullptr };
    HBITMAP						hBitmapFrameBuffer{ nullptr };
public:
	CPlayer						*pPlayer{ nullptr };
	CScene						*pScene{ nullptr };
	CGameObject					*pSelectedObject{ nullptr };
public:
	POINT						ptOldCursorPos;
	_TCHAR						pszFrameRate[50];
};

