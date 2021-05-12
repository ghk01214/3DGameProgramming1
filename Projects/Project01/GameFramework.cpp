//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	_tcscpy_s(m_pszFrameRate, _T("LabProject ("));
}

CGameFramework::~CGameFramework()
{
}

BOOL CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	::srand(timeGetTime());

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	BuildFrameBuffer();

	BuildObjects();

	return(TRUE);
}

void CGameFramework::BuildFrameBuffer()
{
	HDC hDC = ::GetDC(m_hWnd);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, (rcClient.right - rcClient.left) + 1, (rcClient.bottom - rcClient.top) + 1);
	::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);

	::ReleaseDC(m_hWnd, hDC);
	::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
	HBRUSH hBrush = ::CreateSolidBrush(dwColor);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, INT(m_pPlayer->m_pCamera->m_d3dViewport.TopLeftX), INT(m_pPlayer->m_pCamera->m_d3dViewport.TopLeftY), INT(m_pPlayer->m_pCamera->m_d3dViewport.Width), INT(m_pPlayer->m_pCamera->m_d3dViewport.Height));
	::SelectObject(m_hDCFrameBuffer, hOldBrush);
	::DeleteObject(hBrush);
}

void CGameFramework::PresentFrameBuffer()
{
	HDC hDC = ::GetDC(m_hWnd);
	::BitBlt(hDC, INT(m_pPlayer->m_pCamera->m_d3dViewport.TopLeftX), INT(m_pPlayer->m_pCamera->m_d3dViewport.TopLeftY), INT(m_pPlayer->m_pCamera->m_d3dViewport.Width), INT(m_pPlayer->m_pCamera->m_d3dViewport.Height), m_hDCFrameBuffer, INT(m_pPlayer->m_pCamera->m_d3dViewport.TopLeftX), INT(m_pPlayer->m_pCamera->m_d3dViewport.TopLeftY), SRCCOPY);
	::ReleaseDC(m_hWnd, hDC);
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			::PostQuitMessage(0);

			break;
		}
		case VK_CONTROL:
		{
			((CCarPlayer*)m_pPlayer)->m_bJump = TRUE;

			break;
		}
		case VK_RETURN:
			break;
		default:
		{
			m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

			break;
		}
		}

		break;
	}
	default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
			m_GameTimer.Stop();
		else
			m_GameTimer.Start();

		break;
	}
	case WM_KEYDOWN: case WM_KEYUP:
	{
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

		break;
	}
	case WM_SIZE:
		break;
	}
	return(0);
}

void CGameFramework::BuildObjects()
{
	CCarMesh* pCarMesh = new CCarMesh(6.0f, 6.0f, 1.0f);

	m_pPlayer = new CCarPlayer();
	m_pPlayer->SetPosition(0.0f, 0.0f, 0.0f);
	m_pPlayer->SetMesh(pCarMesh);
	m_pPlayer->SetColor(RGB(107, 153, 0));
	m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 20.0f, -20.0f));

	m_pScene = new CScene();
	m_pScene->BuildObjects();

	m_pScene->m_pPlayer = m_pPlayer;
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->ReleaseObjects();

		delete m_pScene;
	}

	if (m_pPlayer)
		delete m_pPlayer;
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_hBitmapFrameBuffer)
		::DeleteObject(m_hBitmapFrameBuffer);

	if (m_hDCFrameBuffer)
		::DeleteDC(m_hDCFrameBuffer);

	if (m_hWnd)
		DestroyWindow(m_hWnd);
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;

	if (GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer[VK_LEFT] & 0xF0)
			dwDirection |= DIR_LEFT;

		if (pKeyBuffer[VK_RIGHT] & 0xF0)
			dwDirection |= DIR_RIGHT;
	}

	m_pPlayer->Move(dwDirection, 0.5f);
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::FrameAdvance()
{
	if (!m_bActive)
		return;

	m_GameTimer.Tick(0.0f);

	ProcessInput();

	FLOAT fTimeElapsed = m_GameTimer.GetTimeElapsed();

	m_pPlayer->Animate(fTimeElapsed);
	m_pScene->Animate(fTimeElapsed);

	ClearFrameBuffer(RGB(255, 255, 255));

	m_pScene->Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);
	m_pPlayer->Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);

	PresentFrameBuffer();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);

	if (m_pScene->m_bGameOver)
	{
		ReleaseObjects();
		BuildObjects();

		auto time = std::chrono::high_resolution_clock::now();
		while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - time).count() < 3);

		m_pScene->m_bGameOver = FALSE;
	}
}