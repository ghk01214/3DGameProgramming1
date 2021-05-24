#pragma once
#include "Timer.h"

class CShader;

class CScene
{
public:
	CScene();
	~CScene();
public:
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);						// Graphic Root Signature ����
	void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice);
public:
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();
	void ReleaseUploadBuffers();
public:
	ID3D12RootSignature* GetGraphicsRootSignature();
public:
	BOOL ProcessInput(UCHAR* pKeysBuffer);
	void Animate(FLOAT fTimeElapsed);
public:
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
public:
	BOOL OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam);
	BOOL OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	ID3D12PipelineState*	 m_pd3dPipelineState{ nullptr };										// Graphics Pipeline ���¸� ��Ÿ���� Interface ������
protected:
	// Scene�� Shder���� ����, Shder���� Game Object���� ����
	CShader**				 m_ppShaders{ nullptr };
	INT						 m_nShaders{ 0 };
protected:
	ID3D12RootSignature*	 m_pd3dGraphicsRootSignature{ nullptr };								// Root Signature�� ��Ÿ���� Interface ������
};