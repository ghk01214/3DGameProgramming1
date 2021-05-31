#pragma once
#include "GameObject.h"
#include "Camera.h"

#define DIR_FORWARD		 0x01
#define DIR_BACKWARD	 0x02
#define DIR_LEFT		 0x04
#define DIR_RIGHT		 0x08
#define DIR_UP			 0x10
#define DIR_DOWN		 0x20

class CPlayer : public CGameObject
{
public:
	CPlayer(INT nMeshes);
	virtual ~CPlayer();
public:
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
public:
	void SetFriction(FLOAT fFriction) { m_fFriction = fFriction; }
	void SetGravity(XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(FLOAT fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(FLOAT fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	/*플레이어의 위치를 xmf3Position 위치로 설정. xmf3Position 벡터에서 현재 플레이어의 위치 벡터를 빼면
	현재 플레이어의 위치에서 xmf3Position 방향으로의 벡터가 된다. 현재 플레이어의 위치에서 이 벡터 만큼 이동.*/
	void SetPosition(XMFLOAT3& xmf3Position) { Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), FALSE); }
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }
public:
	XMFLOAT3 GetPosition() { return m_xmf3Position; }
	XMFLOAT3 GetLookVector() { return m_xmf3Look; }
	XMFLOAT3 GetUpVector() { return m_xmf3Up; }
	XMFLOAT3 GetRightVector() { return m_xmf3Right; }
	XMFLOAT3& GetVelocity() { return m_xmf3Velocity; }
	FLOAT GetYaw() { return m_fYaw; }
	FLOAT GetPitch() { return m_fPitch; }
	FLOAT GetRoll() { return m_fRoll; }
	CCamera* GetCamera() { return m_pCamera; }
public:
	// 플레이어를 이동
	void Move(DWORD dwDirection, FLOAT fDistance, BOOL bVelocity = FALSE);
	void Move(const XMFLOAT3& xmf3Shift, BOOL bVelocity = FALSE);
	void Move(FLOAT fxOffset = 0.0f, FLOAT fyOffset = 0.0f, FLOAT fzOffset = 0.0f);
public:																								// 플레이어를 회전
	void Rotate(FLOAT x, FLOAT y, FLOAT z);
public:
	void Update(FLOAT fTimeElapsed);														   		// 플레이어의 위치와 회전 정보를 경과 시간에 따라 갱신
public:
	virtual void OnPlayerUpdateCallback(FLOAT fTimeElapsed) {}										// 플레이어의 위치가 바뀔 때마다 호출되는 함수와 그 함수에서 사용하는 정보를 설정하는 함수
	virtual void OnCameraUpdateCallback(FLOAT fTimeElapsed) {}								   		// 카메라의 위치가 바뀔 때마다 호출되는 함수와 그 함수에서 사용하는 정보를 설정하는 함수
public:
	CCamera* OnChangeCamera(DWORD dwNewCameraMode, DWORD dwCurrentCameraMode);						// 카메라를 변경하기 위하여 호출하는 함수
	virtual CCamera* ChangeCamera(DWORD dwNewCameraMode, FLOAT fTimeElapsed) { return nullptr; }
public:
	virtual void PrepareRender();																	// 플레이어의 위치와 회전축으로부터 월드 변환 행렬을 생성하는 함수
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = nullptr);	// 플레이어의 카메라가 3인칭 카메라일 때 플레이어(Mesh)를 Rendering

protected:
	XMFLOAT3	 m_xmf3Position;																	// 플레이어의 위치 벡터
	XMFLOAT3	 m_xmf3Right;																		// 플레이어의 x축(Right) 벡터
	XMFLOAT3	 m_xmf3Up;																			// 플레이어의 y축(Up) 벡터
	XMFLOAT3	 m_xmf3Look;																		// 플레이어의 z축(Look) 벡터
protected:
	FLOAT		 m_fPitch;																			// 플레이어의 x축(Right) 회전량
	FLOAT		 m_fYaw;																			// 플레이어의 y축(Up) 회전량
	FLOAT		 m_fRoll;																			// 플레이어의 z축(Look) 회전량
protected:
	XMFLOAT3	 m_xmf3Velocity;																	// 플레이어의 이동 속도 벡터
	XMFLOAT3	 m_xmf3Gravity;																		// 플레이어에 작용하는 중력 벡터
	FLOAT		 m_fMaxVelocityXZ;																	// xz평면에서 (한 Frame 동안) 플레이어의 이동 속력의 최대값
	FLOAT		 m_fMaxVelocityY;																	// y축 방향으로 (한 Frame동안) 플레이어의 이동 속력의 최대값
	FLOAT		 m_fFriction;																		// 플레이어에 작용하는 마찰력
protected:
	LPVOID		 m_pPlayerUpdatedContext;															// 플레이어의 위치가 바뀔 때마다 호출되는 OnPlayerUpdateCallback() 함수에서 사용하는 데이터
	LPVOID		 m_pCameraUpdatedContext;															// 카메라의 위치가 바뀔 때마다 호출되는 OnCameraUpdateCallback() 함수에서 사용하는 데이터
protected:
	CCamera* m_pCamera{ nullptr };																// 플레이어에 현재 설정된 카메라
};

class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, INT nMeshes = 1);
	virtual ~CAirplanePlayer();
public:
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, FLOAT fTimeElapsed);
	virtual void PrepareRender();
};

class CTerrainPlayer : public CPlayer
{
public:
	CTerrainPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, INT nMeshes = 1);
	virtual ~CTerrainPlayer();
public:
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, FLOAT fTimeElapsed);
public:
	virtual void OnPlayerUpdateCallback(FLOAT fTimeElapsed);
	virtual void OnCameraUpdateCallback(FLOAT fTimeElapsed);
};