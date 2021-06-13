#pragma once
#include "Timer.h"
#include "Camera.h"
#include "Shader.h"
#include "Player.h"

namespace ObjectType
{
	enum { Wall, Approaching };
}

namespace PlayerMesh
{
	enum { Normal, Fever };
}

class CScene
{
public:
	CScene();
	~CScene();
public:
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);						// Graphic Root Signature 생성
	void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice);
public:
	ID3D12RootSignature* GetGraphicsRootSignature() { return m_pd3dGraphicsRootSignature; }
public:
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void RebuildObject();
	void ReleaseObjects();
public:
	void ReleaseUploadBuffers();
public:
	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	void SetPlayerMesh(CCarMeshDiffused* pCarMesh, INT i) { m_vPlayerMesh[i] = pCarMesh; }
public:
	BOOL IsBehindCamera(CGameObject* pObject);
	BOOL IsGameOver() { return m_bGameOver; }
public:
	void CheckPlayerByObjectCollision();
	void CheckObjectByObjectCollisions();
	void CheckPlayerByWallCollision();
public:
	BOOL ProcessInput(UCHAR* pKeysBuffer);
	void Animate(FLOAT fTimeElapsed, CCamera* pCamera);
public:
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
public:
	BOOL OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam);
	BOOL OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	ID3D12PipelineState*			 m_pd3dPipelineState{ nullptr };										// Graphics Pipeline 상태를 나타내는 Interface 포인터
protected:
	std::vector<CObjectsShader*>	 m_vShaders;
	INT								 m_nShaders{ 0 };
protected:
	CPlayer*						 m_pPlayer{ nullptr };
	std::vector<CCarMeshDiffused*>	 m_vPlayerMesh;
protected:
	BOOL							 m_bGameOver{ FALSE };
protected:
	ID3D12RootSignature*			 m_pd3dGraphicsRootSignature{ nullptr };								// Root Signature를 나타내는 Interface 포인터
};