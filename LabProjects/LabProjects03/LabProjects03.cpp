﻿// LabProjects03.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "LabProjects03.h"
#include "GameFramework.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE           hInst;                                    // 현재 인스턴스입니다.
WCHAR               szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR               szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

CGameFramework       gGameFramework;                          // 기본 게임 프레임워크입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, INT);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

INT APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LABPROJECTS03, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable{ LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABPROJECTS03)) };

	MSG msg;

	// 기본 메시지 루프입니다:
	while (TRUE)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			gGameFramework.FrameAdvance();
		}
	}

	gGameFramework.OnDestroy();

	return (INT)msg.wParam;
}

//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
//  설명:
//
//    Windows 95에서 추가된 'RegisterClassEx' 함수보다 먼저
//    해당 코드가 Win32 시스템과 호환되도록
//    하려는 경우에만 이 함수를 사용합니다. 이 함수를 호출해야
//    해당 응용 프로그램에 연결된
//    '올바른 형식의' 작은 아이콘을 가져올 수 있습니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LABPROJECTS03));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;//MAKEINTRESOURCE(IDC_LABPROJECTS03);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, INT nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	// 주 윈도우의 클라이언트 영역의 크기를 원하는 크기로 설정
	RECT rc = { 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT };

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hWnd{ CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 0, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr) };

	if (!hWnd)
	{
		return FALSE;
	}

	// 프로그램의 주 윈도우가 생성되면 CGameFramework 클래스의 OnCreate() 함수를 호출하여 프레임워크 객체를 초기화하도록 한다
	if (!gGameFramework.OnCreate(hInstance, hWnd))
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
	{
		if (wParam == SIZE_MINIMIZED)
		{
			gGameFramework.SetActive(FALSE);
		}
		else
		{
			gGameFramework.SetActive(TRUE);
		}

		break;
	}
	case WM_LBUTTONDOWN: case WM_LBUTTONUP:
	case WM_RBUTTONDOWN: case WM_RBUTTONUP:
	case WM_KEYDOWN:
	{
		gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		break;
	}
	case WM_COMMAND:
	{
		INT wmId{ LOWORD(wParam) }, wmEvent{ HIWORD(wParam) };
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{
		case IDM_ABOUT:
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		}
		case IDM_EXIT:
		{
			DestroyWindow(hWnd);
			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC         hdc{ BeginPaint(hWnd, &ps) };

		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
		EndPaint(hWnd, &ps);

		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		break;
	}
	}

	return (INT_PTR)FALSE;
}