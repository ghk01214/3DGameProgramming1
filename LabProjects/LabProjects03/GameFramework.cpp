//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	_tcscpy_s(pszFrameRate, _T("LabProject ("));
}

CGameFramework::~CGameFramework()
{
}

BOOL CGameFramework::OnCreate(HINSTANCE hInstance, HWND hWnd)
{
	//::srand(timeGetTime());

	this->hInstance = hInstance;
	this->hWnd = hWnd;

	BuildFrameBuffer();

	BuildObjects();

	return TRUE;
}

void CGameFramework::BuildFrameBuffer()
{
	HDC hDC = ::GetDC(hWnd);

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, (rcClient.right - rcClient.left) + 1, (rcClient.bottom - rcClient.top) + 1);
	::SelectObject(hDCFrameBuffer, hBitmapFrameBuffer);

	::ReleaseDC(hWnd, hDC);
	::SetBkMode(hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
	HBRUSH hBrush{ ::CreateSolidBrush(dwColor) };
	HBRUSH hOldBrush{ (HBRUSH)::SelectObject(hDCFrameBuffer, hBrush) };

	::Rectangle(hDCFrameBuffer, INT(pPlayer->m_pCamera->d3dViewport.TopLeftX), INT(pPlayer->m_pCamera->d3dViewport.TopLeftY), INT(pPlayer->m_pCamera->d3dViewport.Width), INT(pPlayer->m_pCamera->d3dViewport.Height));
	::SelectObject(hDCFrameBuffer, hOldBrush);
	::DeleteObject(hBrush);
}

void CGameFramework::PresentFrameBuffer()
{
	HDC hDC{ ::GetDC(hWnd) };

	::BitBlt(hDC, INT(pPlayer->m_pCamera->d3dViewport.TopLeftX), INT(pPlayer->m_pCamera->d3dViewport.TopLeftY), INT(pPlayer->m_pCamera->d3dViewport.Width), INT(pPlayer->m_pCamera->d3dViewport.Height), hDCFrameBuffer, INT(pPlayer->m_pCamera->d3dViewport.TopLeftX), INT(pPlayer->m_pCamera->d3dViewport.TopLeftY), SRCCOPY);
	::ReleaseDC(hWnd, hDC);
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
	{
		pSelectedObject = pScene->PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam), pPlayer->m_pCamera);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		::SetCapture(hWnd);
		::GetCursorPos(&ptOldCursorPos);

		break;
	}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	{
		::ReleaseCapture();
		break;
	}
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
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
		case VK_RETURN:
			break;
		case VK_CONTROL:
		{
			((CAirplanePlayer*)pPlayer)->FireBullet(pSelectedObject);
			break;
		}
		default:
		{
			pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
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
		{
			GameTimer.Stop();
		}
		else
		{
			GameTimer.Start();
		}

		break;
	}
	case WM_SIZE:
		break;
	case WM_LBUTTONDOWN: case WM_LBUTTONUP:
	case WM_RBUTTONDOWN: case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	{
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	}

	return 0;
}

void CGameFramework::BuildObjects()
{
	CAirplaneMesh* pAirplaneMesh{ new CAirplaneMesh(6.0f, 6.0f, 1.0f) };

	pPlayer = new CAirplanePlayer();
	pPlayer->SetPosition(0.0f, 0.0f, 0.0f);
	pPlayer->SetMesh(pAirplaneMesh);
	pPlayer->SetColor(RGB(0, 0, 255));
	pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));

	pScene = new CScene();
	pScene->BuildObjects();

	pScene->m_pPlayer = pPlayer;
}

void CGameFramework::ReleaseObjects()
{
	if (pScene)
	{
		pScene->ReleaseObjects();

		delete pScene;
	}

	if (pPlayer)
	{
		delete pPlayer;
	}
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (hBitmapFrameBuffer)
	{
		::DeleteObject(hBitmapFrameBuffer);
	}
	
	if (hDCFrameBuffer)
	{
		::DeleteDC(hDCFrameBuffer);
	}

	if (hWnd)
	{
		DestroyWindow(hWnd);
	}
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;

	if (GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer[VK_UP] & 0xF0)
		{
			dwDirection |= DIR_FORWARD;
		}

		if (pKeyBuffer[VK_DOWN] & 0xF0)
		{
			dwDirection |= DIR_BACKWARD;
		}

		if (pKeyBuffer[VK_LEFT] & 0xF0)
		{
			dwDirection |= DIR_LEFT;
		}

		if (pKeyBuffer[VK_RIGHT] & 0xF0)
		{
			dwDirection |= DIR_RIGHT;
		}
		
		if (pKeyBuffer[VK_PRIOR] & 0xF0)
		{
			dwDirection |= DIR_UP;
		}

		if (pKeyBuffer[VK_NEXT] & 0xF0)
		{
			dwDirection |= DIR_DOWN;
		}
	}

	FLOAT cxDelta{ 0.0f }, cyDelta{ 0.0f };
	POINT ptCursorPos;

	if (GetCapture() == hWnd)
	{
		SetCursor(nullptr);
		GetCursorPos(&ptCursorPos);

		cxDelta = (FLOAT)(ptCursorPos.x - ptOldCursorPos.x) / 3.0f;
		cyDelta = (FLOAT)(ptCursorPos.y - ptOldCursorPos.y) / 3.0f;

		SetCursorPos(ptOldCursorPos.x, ptOldCursorPos.y);
	}

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
			{
				pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			}
			else
			{
				pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
		}

		if (dwDirection)
		{
			pPlayer->Move(dwDirection, 0.15f);
		}
	}

	pPlayer->Update(GameTimer.GetTimeElapsed());
}

void CGameFramework::FrameAdvance()
{
	if (!bActive)
		return;

	GameTimer.Tick(0.0f);

	ProcessInput();

	FLOAT fTimeElapsed{ GameTimer.GetTimeElapsed() };

	pPlayer->Animate(fTimeElapsed);
	pScene->Animate(fTimeElapsed);

	ClearFrameBuffer(RGB(255, 255, 255));

	pScene->Render(hDCFrameBuffer, pPlayer->m_pCamera);
	pPlayer->Render(hDCFrameBuffer, pPlayer->m_pCamera);

	PresentFrameBuffer();

	GameTimer.GetFrameRate(pszFrameRate + 12, 37);
	::SetWindowTextW(hWnd, pszFrameRate);
}


