#pragma once
#include "Player.h"
#include "Scene.h"

class CGameFramwork
{
private:
	// 윈도우 응용프로그램의 인스턴스 핸들과 주 윈도우 핸들
	HINSTANCE	m_hInstance{ nullptr };
	HWND		m_hWnd{ nullptr };
private:
	// 주 윈도우의 클라이언트 사각형
	RECT		m_rcClient;
private:
	// 렌더링의 대상이 되는 화면에 해당하는 비트맵과 비트맵 디바이스 컨텍스트(Device Context)
	HDC			m_hDCFrameBuffer{ nullptr };
	HBITMAP		m_hBitmapFrameBuffer{ nullptr };
	HBITMAP		m_hBitmapSelect{ nullptr };
private:
	// 플레이어 객체
	CPlayer*	m_pPlayer{ nullptr };
	//게임 객체들을 포함하는 씬(게임 세계) 클래스
	CScene*		m_pScene{ nullptr };
public:
	CGameFramwork() {}
	~CGameFramwork() {}
public:
	// Framework를 생성하는 함수(주 윈도우가 생성되면 호출)
	void OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	// Framework를 소멸하는 함수(응용 프로그램이 종료되면 호출)
	void OnDestroy();
public:
	// 게임 세계를 렌더링할 비트맵 표면을 생성, 소멸, 클라이언트 영역으로 복사
	void BuildFrameBuffer();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer();
public:
	// 렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수
	void BuildObjects();
	void ReleaseObjects();
public:
	// 프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
};