#pragma once
#include "Timer.h"

class CScene
{
public:
	CScene();
	~CScene();
public:
	void CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice);
public:
	void BuildObjects(ID3D12Device* pd3dDevice);
	void ReleaseObjects();
public:
	BOOL ProcessInput();
	void Animate(FLOAT fTimeElapsed);
public:
	void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
public:
	BOOL OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam);
	BOOL OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature{ nullptr };								// Root Signature를 나타내는 Interface 포인터
	ID3D12PipelineState* m_pd3dPipelineState{ nullptr };										// Graphics Pipeline 상태를 나타내는 Interface 포인터
};