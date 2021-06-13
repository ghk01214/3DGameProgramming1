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
	BOOL OnCreate(HINSTANCE hInstance, HWND hMainWnd);														// Framework �ʱ�ȭ �Լ�(�� �����찡 ������ ȣ��)
	void OnDestroy();
public:
	void CreateSwapChain();																					// Swap Chain ���� �Լ�
	void CreateRtvAndDsvDescriptorHeaps();																	// ������ Heap ���� �Լ�
	void CreateDirect3DDevice();																			// Direct3D Device ���� �Լ�
	void CreateCommandQueueAndList();																		// ��� Queue/�Ҵ���/List ���� �Լ�
public:
	void CreateRenderTargetViews();																			// Render Target View ���� �Լ�
	void CreateDepthStencilView();																			// Depth-Stencil View ���� �Լ�
public:
	void ChangeSwapChainState();
public:
	void MoveToNextFrame();
public:
	void BuildObjects();																					// Rendering�� Mesh�� ���� ��ü ���� �Լ�
	void ReleaseObjects();																					// ���� ��ü �Ҹ� �Լ�
public:
	void ProcessInput();																					// ����� �Է� �Լ�
	void Animate();																							// �ִϸ��̼� �Լ�
	void FrameAdvance();																					// Rendering �Լ�
public:
	void WaitForGpuComplete();																				// CPU�� GPU ����ȭ �Լ�
public:
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);				// ���콺 �޽��� ó�� �Լ�
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);				// Ű���� �޽��� ó�� �Լ�
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);	// ������ �޽��� ó�� �Լ�
	
private:
	HINSTANCE							 m_hInstance;
	HWND								 m_hWnd;
private:
	INT									 m_nWndClientWidth;
	INT									 m_nWndClientHeight;
private:
	IDXGIFactory4*						 m_pdxgiFactory;													// DXGI Factory Interface�� ���� ������
	IDXGISwapChain3*					 m_pdxgiSwapChain;													// Swap Chain Interface�� ���� ������(���÷��� ����ň ���� �ʿ�)
	ID3D12Device*						 m_pd3dDevice;														// Direct3D Device Interface�� ���� ������(���ҽ� ������ ���� �ʿ�)
private:
	BOOL								 m_bMsaa4xEnable{ FALSE };											// MSAA ���� ���ø� Ȱ��ȭ
	UINT								 m_nMsaa4xQualityLevels{ 0 };										// MSAA ���� ���ø� ���� ����
private:
	static const UINT					 m_nSwapChainBuffers{ 2 };											// Swap Chain �ĸ� ���� ����
	UINT								 m_nSwapChainBufferIndex;											// ���� Swap Chain�� �ĸ� ���� �ε���
private:
	std::vector<ID3D12Resource*>		 m_ppd3dRenderTargetBuffers;										// Render Target ����
	ID3D12DescriptorHeap*				 m_pd3dRtvDescriptorHeap;											// ������ Heap Interface ������
	UINT								 m_nRtvDescriptorIncrementSize;										// Render Target ������ ���� ũ��
private:
	ID3D12Resource*						 m_pd3dDepthStencilBuffer;											// Depth-Stencil ����
	ID3D12DescriptorHeap*				 m_pd3dDsvDescriptorHeap;											// ������ Heap Interface ������
	UINT								 m_nDsvDescriptorIncrementSize;										// Depth-Stencil ������ ���� ũ��
private:
	ID3D12CommandQueue*					 m_pd3dCommandQueue;												// ��� Queue
	ID3D12CommandAllocator*				 m_pd3dCommandAllocator;											// ��� �Ҵ���
	ID3D12GraphicsCommandList*			 m_pd3dCommandList;													// ��� List Interface ������
private:
	ID3D12PipelineState*				 m_pd3dPipelineState;												// Graphics Pipeline ���� ��ü�� ���� Interface �������̴�.
private:
	ID3D12Fence*						 m_pd3dFence;														// Fence Interface ������
	std::vector<UINT64>					 m_nFenceValue;														// Fence ��
	HANDLE								 m_hFenceEvent;														// Fence �̺�Ʈ �ڵ�
public:
	CCamera*							 m_pCamera{ nullptr };
private:
	CScene*								 m_pScene{ nullptr };
public:
	CPlayer*							 m_pPlayer{ nullptr };
	POINT								 m_ptOldCursorPos;
private:
	CGameTimer							 m_GameTimer;														// Ÿ�̸�
	_TCHAR								 m_pszFrameRate[50];												// Frame Rate�� �� �������� ĸ�ǿ� ����ϴ� ���ڿ�
};

