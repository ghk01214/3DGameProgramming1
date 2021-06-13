#pragma once
#include "Timer.h"
#include "Scene.h"
#include "Camera.h"
#include "Player.h"

//#define _WITH_PLAYER_TOP

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();
public:
	BOOL OnCreate(HINSTANCE hInstance, HWND hMainWnd);														// Framework 초기화 함수(주 윈도우가 생성시 호출)
	void OnDestroy();
public:
	void CreateSwapChain();																					// Swap Chain 생성 함수
	void CreateRtvAndDsvDescriptorHeaps();																	// 서술자 Heap 생성 함수
	void CreateDirect3DDevice();																			// Direct3D Device 생성 함수
	void CreateCommandQueueAndList();																		// 명령 Queue/할당자/List 생성 함수
public:
	void CreateRenderTargetViews();																			// Render Target View 생성 함수
	void CreateDepthStencilView();																			// Depth-Stencil View 생성 함수
public:
	void ChangeSwapChainState();
public:
	void MoveToNextFrame();
public:
	void BuildObjects();																					// Rendering할 Mesh와 게임 객체 생성 함수
	void ReleaseObjects();																					// 게임 객체 소멸 함수
public:
	void ProcessInput();																					// 사용자 입력 함수
	void Animate();																							// 애니메이션 함수
	void FrameAdvance();																					// Rendering 함수
public:
	void WaitForGpuComplete();																				// CPU와 GPU 동기화 함수
public:
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);				// 마우스 메시지 처리 함수
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);				// 키보드 메시지 처리 함수
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);	// 윈도우 메시지 처리 함수
	
private:
	HINSTANCE							 m_hInstance;
	HWND								 m_hWnd;
private:
	INT									 m_nWndClientWidth;
	INT									 m_nWndClientHeight;
private:
	IDXGIFactory4*						 m_pdxgiFactory;													// DXGI Factory Interface에 대한 포인터
	IDXGISwapChain3*					 m_pdxgiSwapChain;													// Swap Chain Interface에 대한 포인터(디스플레이 제어흫 위해 필요)
	ID3D12Device*						 m_pd3dDevice;														// Direct3D Device Interface에 대한 포인터(리소스 생성을 위해 필요)
private:
	BOOL								 m_bMsaa4xEnable{ FALSE };											// MSAA 다중 샘플링 활성화
	UINT								 m_nMsaa4xQualityLevels{ 0 };										// MSAA 다중 샘플링 레벨 설정
private:
	static const UINT					 m_nSwapChainBuffers{ 2 };											// Swap Chain 후면 버퍼 개수
	UINT								 m_nSwapChainBufferIndex;											// 현재 Swap Chain의 후면 버퍼 인덱스
private:
	std::vector<ID3D12Resource*>		 m_ppd3dRenderTargetBuffers;										// Render Target 버퍼
	ID3D12DescriptorHeap*				 m_pd3dRtvDescriptorHeap;											// 서술자 Heap Interface 포인터
	UINT								 m_nRtvDescriptorIncrementSize;										// Render Target 서술자 원소 크기
private:
	ID3D12Resource*						 m_pd3dDepthStencilBuffer;											// Depth-Stencil 버퍼
	ID3D12DescriptorHeap*				 m_pd3dDsvDescriptorHeap;											// 서술자 Heap Interface 포인터
	UINT								 m_nDsvDescriptorIncrementSize;										// Depth-Stencil 서술자 원소 크기
private:
	ID3D12CommandQueue*					 m_pd3dCommandQueue;												// 명령 Queue
	ID3D12CommandAllocator*				 m_pd3dCommandAllocator;											// 명령 할당자
	ID3D12GraphicsCommandList*			 m_pd3dCommandList;													// 명령 List Interface 포인터
private:
	ID3D12PipelineState*				 m_pd3dPipelineState;												// Graphics Pipeline 상태 객체에 대한 Interface 포인터이다.
private:
	ID3D12Fence*						 m_pd3dFence;														// Fence Interface 포인터
	std::vector<UINT64>					 m_nFenceValue;														// Fence 값
	HANDLE								 m_hFenceEvent;														// Fence 이벤트 핸들
public:
	CCamera*							 m_pCamera{ nullptr };
private:
	CScene*								 m_pScene{ nullptr };
public:
	CPlayer*							 m_pPlayer{ nullptr };
	POINT								 m_ptOldCursorPos;
private:
	CGameTimer							 m_GameTimer;														// 타이머
	_TCHAR								 m_pszFrameRate[50];												// Frame Rate를 주 윈도우의 캡션에 출력하는 문자열
};

