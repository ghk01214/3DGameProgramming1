#pragma once
#include "Player.h"
#include "Scene.h"

class CGameFramwork
{
private:
	// ������ �������α׷��� �ν��Ͻ� �ڵ�� �� ������ �ڵ�
	HINSTANCE	m_hInstance{ nullptr };
	HWND		m_hWnd{ nullptr };
private:
	// �� �������� Ŭ���̾�Ʈ �簢��
	RECT		m_rcClient;
private:
	// �������� ����� �Ǵ� ȭ�鿡 �ش��ϴ� ��Ʈ�ʰ� ��Ʈ�� ����̽� ���ؽ�Ʈ(Device Context)
	HDC			m_hDCFrameBuffer{ nullptr };
	HBITMAP		m_hBitmapFrameBuffer{ nullptr };
	HBITMAP		m_hBitmapSelect{ nullptr };
private:
	// �÷��̾� ��ü
	CPlayer*	m_pPlayer{ nullptr };
	//���� ��ü���� �����ϴ� ��(���� ����) Ŭ����
	CScene*		m_pScene{ nullptr };
public:
	CGameFramwork() {}
	~CGameFramwork() {}
public:
	// Framework�� �����ϴ� �Լ�(�� �����찡 �����Ǹ� ȣ��)
	void OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	// Framework�� �Ҹ��ϴ� �Լ�(���� ���α׷��� ����Ǹ� ȣ��)
	void OnDestroy();
public:
	// ���� ���踦 �������� ��Ʈ�� ǥ���� ����, �Ҹ�, Ŭ���̾�Ʈ �������� ����
	void BuildFrameBuffer();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer();
public:
	// �������� �޽��� ���� ��ü�� �����ϰ� �Ҹ��ϴ� �Լ�
	void BuildObjects();
	void ReleaseObjects();
public:
	// �����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ�
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
};