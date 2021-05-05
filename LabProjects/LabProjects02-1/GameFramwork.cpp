#include "stdafx.h"
#include "GameFramwork.h"

// �� �����찡 �����Ǹ� ȣ��
void CGameFramwork::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	BuildFrameBuffer();				//������ ȭ���� ����
	BuildObjects();					// �÷��̾�� ���� ����(��)�� ����
}

// ���� ���α׷��� ����� �� ȣ��
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

// ȭ�� �������� ���̱� ���Ͽ� �������� Ŭ���̾�Ʈ ������
// ����̽� ���ؽ�Ʈ�� ����Ͽ� ���� Ŭ���̾�Ʈ ǥ�鿡 �׸��� �ʴ´�

// Ŭ���̾�Ʈ ǥ���� ����� ��Ʈ���� �����ϰ� �� ��Ʈ���� �޸� ����̽� ���ؽ�Ʈ�� ����
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

// �������� �����ϱ� ���� ��Ʈ�� ǥ���� ���ϴ� �������� ����
// ���� ȭ��(Ŭ���̾�Ʈ ����)���� �������ϱ� ���Ͽ� ���� ���� ���� ��ü���� ��Ʈ�� ǥ������ ������
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

// ��Ʈ�� ǥ������ �������� ��Ʈ�� Ŭ���̾�Ʈ �������� �ű��.
void CGameFramwork::PresentFrameBuffer()
{
	HDC hDC{ ::GetDC(m_hWnd) };

	::BitBlt(hDC, m_rcClient.left, m_rcClient.top, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top, m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top, SRCCOPY);
	::ReleaseDC(m_hWnd, hDC);
}

// ī�޶� ���� �� ����, �÷��̾� ��ü ����, ��(���� ����) ����
void CGameFramwork::BuildObjects()
{
	// ī�޶� �����ϰ� ����Ʈ�� �þ߰�(FOV)�� ����
	CCamera* pCamera{ new CCamera() };
	pCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	pCamera->SetFOVAngle(60.0f);

	// �÷��̾� ���� ��ü�� �����ϰ� ī�޶�� ��ġ�� ����
	m_pPlayer = new CPlayer();
	m_pPlayer->SetCamera(pCamera);
	m_pPlayer->SetPosition(0.0f, 3.0f, -40.0f);

	// �� ��ü�� �����ϰ� ���� ��ü���� ����
	m_pScene = new CScene();
	m_pScene->BuildObjects();
}

void CGameFramwork::ReleaseObjects()
{
	// �� ��ü�� ���� ��ü���� �Ҹ��Ű��, �� ��ü�� �÷��̾� ��ü�� �Ҹ�
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

// Ű���� �Է��� ó���Ͽ� �÷��̾� ��ü�� �̵�
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

// ���� ���� ��ü���� �ִϸ��̼�
void CGameFramwork::AnimateObjects()
{
	if (m_pScene)
	{
		m_pScene->Animate(1.0f / 60.0f);
	}
}

// ������ �޽��� �������� �ݺ������� ȣ�� = ���� ���α׷��� ����Ǵ� ���� �� �Լ��� �ݺ������� ��� ����
// ����� �Է��� �޾� �÷��̾� �Ǵ� ���� ������ ���� ��ü���� �����̰� �� ����� ���� ���� ���踦 ȭ������ ������
void CGameFramwork::FrameAdvance()
{
	ProcessInput();					// ������� �Է��� ó��
	AnimateObjects();				// ���� ���踦 �ִϸ��̼�(������)

	// �������� �� ��� ȭ��(��Ʈ��)�� ����
	ClearFrameBuffer(RGB(90, 103, 224));

	// ���� ������
	CCamera* pCamera{ m_pPlayer->GetCamera() };

	if (m_pScene)
	{
		m_pScene->Render(m_hDCFrameBuffer, pCamera);
	}

	// �������� �� ȭ��(��Ʈ��)�� Ŭ���̾�Ʈ �������� ����
	PresentFrameBuffer();
}