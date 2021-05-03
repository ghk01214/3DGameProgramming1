#include "stdafx.h"
#include "GameFramwork.h"

// 주 윈도우가 생성되면 호출
void CGameFramwork::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	BuildFrameBuffer();				//렌더링 화면을 생성
	BuildObjects();					// 플레이어와 게임 세계(씬)을 생성
}

// 응용 프로그램이 종료될 때 호출
void CGameFramwork::OnDestroy()
{
	ReleaseObjects();

	if (m_hBitmapFrameBuffer)
	{
		::DeleteObject(m_hBitmapFrameBuffer);
	}

	if (m_hDCFrameBuffer)
	{
		::DeleteDC(m_hDCFrameBuffer);
	}
}

// 화면 깜박임을 줄이기 위하여 윈도우의 클라이언트 영역의
// 디바이스 컨텍스트를 사용하여 직접 클라이언트 표면에 그리지 않는다

// 클라이언트 표면을 대신할 비트맵을 생성하고 이 비트맵을 메모리 디바이스 컨텍스트로 생성
void CGameFramwork::BuildFrameBuffer()
{
	::GetClientRect(m_hWnd, &m_rcClient);

	HDC hDC{ ::GetDC(m_hWnd) };
	m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top);

	::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);
	::ReleaseDC(m_hWnd, hDC);
	::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
}

// 렌더링을 시작하기 전에 비트맵 표면을 원하는 색상으로 지움
// 씬을 화면(클라이언트 영역)으로 렌더링하기 위하여 먼저 씬의 게임 객체들을 비트맵 표면으로 렌더링
void CGameFramwork::ClearFrameBuffer(DWORD dwColor)
{
	HPEN	hPen{ ::CreatePen(PS_SOLID, 0, dwColor) };
	HPEN	hOldPen{ (HPEN)::SelectObject(m_hDCFrameBuffer, hPen) };
	HBRUSH	hBrush{ ::CreateSolidBrush(dwColor) };
	HBRUSH	hOldBrush{ (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush) };

	::Rectangle(m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top, m_rcClient.right, m_rcClient.bottom);
	::SelectObject(m_hDCFrameBuffer, hOldBrush);
	::SelectObject(m_hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

// 비트맵 표면으로 렌더링된 비트맵 클라이언트 영역으로 옮긴다.
void CGameFramwork::PresentFrameBuffer()
{
	HDC hDC{ ::GetDC(m_hWnd) };

	::BitBlt(hDC, m_rcClient.left, m_rcClient.top, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top, m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top, SRCCOPY);
	::ReleaseDC(m_hWnd, hDC);
}

// 카메라를 생성 및 설정, 플레이어 객체 생성, 씬(게임 세계) 생성
void CGameFramwork::BuildObjects()
{
	// 카메라를 생성하고 뷰포트와 시야각(FOV)를 설정
	CCamera* pCamera{ new CCamera() };
	pCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	pCamera->SetFOVAngle(60.0f);

	// 플레이어 게임 객체를 생성하고 카메라와 위치를 설정
	m_pPlayer = new CPlayer();
	m_pPlayer->SetCamera(pCamera);
	m_pPlayer->SetPosition(0.0f, 3.0f, -40.0f);

	// 씬 객체를 생성하고 게임 객체들을 생성
	m_pScene = new CScene();
	m_pScene->BuildObjects();
}

void CGameFramwork::ReleaseObjects()
{
	// 씬 객체의 게임 객체들을 소멸시키고, 씬 객체와 플레이어 객체를 소멸
	if (m_pScene)
	{
		m_pScene->ReleaseObjects();
	}

	if (m_pScene)
	{
		delete m_pScene;
	}

	if (m_pPlayer)
	{
		delete m_pPlayer;
	}
}

// 키보드 입력을 처리하여 플레이어 객체를 이동
void CGameFramwork::ProcessInput()
{
	static UCHAR pKeyBuffer[256];

	if (::GetKeyboardState(pKeyBuffer))
	{
		FLOAT cxKeyDelta{ 0.0f }, cyKeyDelta{ 0.0f }, czKeyDelta{ 0.0f };

		if (pKeyBuffer[VK_UP] & 0xF0)
		{
			czKeyDelta = +0.125f;
		}

		if (pKeyBuffer[VK_DOWN] & 0xF0)
		{
			czKeyDelta = -0.125f;
		}

		if (pKeyBuffer[VK_LEFT] & 0xF0)
		{
			cxKeyDelta = -0.125f;
		}

		if (pKeyBuffer[VK_RIGHT] & 0xF0)
		{
			cxKeyDelta = +0.125f;
		}

		if (pKeyBuffer[VK_PRIOR] & 0xF0)
		{
			cyKeyDelta = +0.125f;
		}

		if (pKeyBuffer[VK_NEXT] & 0xF0)
		{
			cyKeyDelta = -0.125;
		}

		m_pPlayer->Move(cxKeyDelta, cyKeyDelta, czKeyDelta);
	}
}

// 씬의 게임 객체들을 애니메이션
void CGameFramwork::AnimateObjects()
{
	if (m_pScene)
	{
		m_pScene->Animate(1.0f / 60.0f);
	}
}

// 윈도우 메시지 루프에서 반복적으로 호출 = 응용 프로그램이 실행되는 동안 이 함수가 반복적으로 계속 실행
// 사용자 입력을 받아 플레이어 또는 게임 세계의 게임 객체들을 움직이고 그 결과에 따라 게임 세계를 화면으로 렌더링
void CGameFramwork::FrameAdvance()
{
	ProcessInput();					// 사용자의 입력을 처리
	AnimateObjects();				// 게임 세계를 애니메이션(움직임)

	// 렌더링을 할 대상 화면(비트맵)을 지움
	ClearFrameBuffer(RGB(90, 103, 224));

	// 씬을 렌더링
	CCamera* pCamera{ m_pPlayer->GetCamera() };

	if (m_pScene)
	{
		m_pScene->Render(m_hDCFrameBuffer, pCamera);
	}

	// 렌더링을 한 화면(비트맵)을 클라이언트 영역으로 복사
	PresentFrameBuffer();
}