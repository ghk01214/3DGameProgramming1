#pragma once
#include "GameObject.h"
#include "Camera.h"

#define DIR_FORWARD		 0x01
#define DIR_BACKWARD	 0x02
#define DIR_LEFT		 0x04
#define DIR_RIGHT		 0x08
#define DIR_UP			 0x10
#define DIR_DOWN		 0x20

//class CGameObject;
class CCamera;

class CPlayer : public CGameObject
{
public:
	CPlayer();
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
	/*�÷��̾��� ��ġ�� xmf3Position ��ġ�� ����. xmf3Position ���Ϳ��� ���� �÷��̾��� ��ġ ���͸� ����
	���� �÷��̾��� ��ġ���� xmf3Position ���������� ���Ͱ� �ȴ�. ���� �÷��̾��� ��ġ���� �� ���� ��ŭ �̵��Ѵ�.*/
	void SetPosition(XMFLOAT3& xmf3Position) { Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), FALSE); }
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
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
	void Move(DWORD dwDirection, FLOAT fDistance, BOOL bVelocity = FALSE);							// �÷��̾� �̵� �Լ�
	void Move(XMFLOAT3& xmf3Shift, BOOL bVelocity = FALSE);											// �÷��̾� �̵� �Լ�
	void Move(FLOAT fxOffset = 0.0f, FLOAT fyOffset = 0.0f, FLOAT fzOffset = 0.0f);					// �÷��̾� �̵� �Լ�
	void Rotate(FLOAT x, FLOAT y, FLOAT z);															// �÷��̾� ȸ�� �Լ�
	void Update(FLOAT fTimeElapsed);																// �÷��̾��� ��ġ�� ȸ�� ������ ��� �ð��� ���� �����ϴ� �Լ�
public:
	virtual void OnPlayerUpdateCallback(FLOAT fTimeElapsed) {}										// �÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ�
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }
	virtual void OnCameraUpdateCallback(FLOAT fTimeElapsed) {}										// ī�޶��� ��ġ�� �ٲ� ������ ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ�
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }
public:
	CCamera* ChangeCamera(DWORD dwNewCameraMode, DWORD dwCurrentCameraMode);						// ī�޶� �����ϱ� ���Ͽ� ȣ���ϴ� �Լ�
	virtual CCamera* ChangeCamera(DWORD dwNewCameraMode, FLOAT fTimeElapsed) { return nullptr; }
public:
	virtual void PrepareRender();																	// �÷��̾��� ��ġ�� ȸ�������κ��� ���� ��ȯ ��� ����
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = nullptr);	// �÷��̾��� ī�޶� 3��Ī ī�޶��� �� �÷��̾�(Mesh) Rendering

protected:
	XMFLOAT3	 m_xmf3Position;																	// �÷��̾��� ��ġ ����
	XMFLOAT3	 m_xmf3Right;																		// �÷��̾��� x��(Right) ����
	XMFLOAT3	 m_xmf3Up;																			// �÷��̾��� y��(Up) ����
	XMFLOAT3	 m_xmf3Look;																		// �÷��̾��� z��(Look) ����
protected:
	FLOAT		 m_fPitch;																			// �÷��̾��� ���� x��(Right) ȸ����
	FLOAT		 m_fYaw;																			// �÷��̾��� ���� y��(Up) ȸ����
	FLOAT		 m_fRoll;																			// �÷��̾��� ���� z��(Look) ȸ����
protected:
	XMFLOAT3	 m_xmf3Velocity;																	// �÷��̾��� �̵� �ӵ��� ��Ÿ���� ����
	XMFLOAT3	 m_xmf3Gravity;																		// �÷��̾ �ۿ��ϴ� �߷��� ��Ÿ���� ����
	FLOAT		 m_fMaxVelocityXZ;																	// xz��鿡�� (�� ������ ����) �÷��̾��� �̵� �ӷ��� �ִ밪�� ��Ÿ����.
	FLOAT		 m_fMaxVelocityY;																	// y�� �������� (�� ������ ����) �÷��̾��� �̵� �ӷ��� �ִ밪�� ��Ÿ����.																								//�÷��̾ �ۿ��ϴ� �������� ��Ÿ����.
	FLOAT		 m_fFriction;
protected:
	LPVOID		 m_pPlayerUpdatedContext;															// �÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnPlayerUpdateCallback() �Լ����� ����ϴ� ������
	LPVOID		 m_pCameraUpdatedContext;															// ī�޶��� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnCameraUpdateCallback() �Լ����� ����ϴ� ������
protected:
	CCamera*	 m_pCamera{ nullptr };																// �÷��̾ ���� ������ ī�޶�
};

class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CAirplanePlayer();
public:
	virtual CCamera* ChangeCamera(DWORD dwNewCameraMode, FLOAT fTimeElapsed);
	virtual void PrepareRender();
};