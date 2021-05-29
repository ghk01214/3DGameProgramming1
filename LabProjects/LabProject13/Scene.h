#pragma once
#include "Timer.h"
#include "Camera.h"
#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene();
public:
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);						// Graphic Root Signature 생성
	void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice);
public:
	ID3D12RootSignature* GetGraphicsRootSignature();
public:
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();
public:
	void ReleaseUploadBuffers();
public:
	BOOL ProcessInput(UCHAR* pKeysBuffer);
	void Animate(FLOAT fTimeElapsed);
public:
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
public:
	BOOL OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam);
	BOOL OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	ID3D12PipelineState*	 m_pd3dPipelineState{ nullptr };										// Graphics Pipeline 상태를 나타내는 Interface 포인터
protected:
	CInstancingShader*		 m_pShaders{ nullptr };
	INT						 m_nShaders{ 0 };
protected:
	ID3D12RootSignature*	 m_pd3dGraphicsRootSignature{ nullptr };								// Root Signature를 나타내는 Interface 포인터
};