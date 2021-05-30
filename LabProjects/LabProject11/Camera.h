#pragma once

// ī�޶��� ����(���: Mode)�� ��Ÿ���� ���
#define FIRST_PERSON_CAMERA	 0x01
#define SPACESHIP_CAMERA	 0x02
#define THIRD_PERSON_CAMERA	 0x03

// Frame ������ ũ��� Aspect Ratio(��Ⱦ��)�� ��Ÿ���� ���
#define ASPECT_RATIO (FLOAT)FRAME_BUFFER_WIDTH / (FLOAT)FRAME_BUFFER_HEIGHT

class CPlayer;

// ī�޶� ��� ���� ����ü
struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
};

class CCamera
{
public:
	CCamera();
	CCamera(CCamera* pCamera);
	virtual ~CCamera();
public:
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
public:
	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void RegenerateViewMatrix();
public:
	void GenerateProjectionMatrix(FLOAT fNearPlaneDistance, FLOAT fFarPlaneDistance, FLOAT fAspectRatio, FLOAT fFOVAngle);
public:
	void SetViewport(INT xTopLeft, INT yTopLeft, INT nWidth, INT nHeight, FLOAT fMinZ = 0.0f, FLOAT fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
public:
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);
public:
	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	void SetMode(DWORD dwMode) { m_dwMode = dwMode; }
	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }
	void SetOffset(XMFLOAT3 xmf3Offset) { m_xmf3Offset = xmf3Offset; }
	void SetTimeLag(FLOAT fTimeLag) { m_fTimeLag = fTimeLag; }
public:
	CPlayer* GetPlayer() { return m_pPlayer; }
	DWORD GetMode() { return m_dwMode; }
	XMFLOAT3& GetPosition() { return m_xmf3Position; }
	XMFLOAT3& GetLookAtPosition() { return m_xmf3LookAtWorld; }
	XMFLOAT3& GetRightVector() { return m_xmf3Right; }
	XMFLOAT3& GetUpVector() { return m_xmf3Up; }
	XMFLOAT3& GetLookVector() { return m_xmf3Look; }
	FLOAT& GetPitch() { return m_fPitch; }
	FLOAT& GetRoll() { return m_fRoll; }
	FLOAT& GetYaw() { return m_fYaw; }
	XMFLOAT3& GetOffset() { return m_xmf3Offset; }
	FLOAT GetTimeLag() { return m_fTimeLag; }
	XMFLOAT4X4 GetViewMatrix() { return m_xmf4x4View; }
	XMFLOAT4X4 GetProjectionMatrix() { return m_xmf4x4Projection; }
	D3D12_VIEWPORT GetViewport() { return m_d3dViewport; }
	D3D12_RECT GetScissorRect() { return m_d3dScissorRect; }
public:
	virtual void Move(XMFLOAT3& xmf3Shift);												   // ī�޶� xmf3Shift ��ŭ �̵�
	virtual void Rotate(FLOAT fPitch = 0.0f, FLOAT fYaw = 0.0f, FLOAT fRoll = 0.0f) {}	   // ī�޶� x��, y��, z������ ȸ���ϴ� �����Լ�
	virtual void Update(XMFLOAT3& xmf3LookAt, FLOAT fTimeElapsed) {}					   // ī�޶��� �̵�, ȸ���� ���� ī�޶��� ������ �����ϴ� �����Լ�
	virtual void SetLookAt(XMFLOAT3& xmf3LookAt) {}										   // 3��Ī ī�޶󿡼� ī�޶� �ٶ󺸴� ���� ����(�Ϲ������� �÷��̾ �ٶ󺸵��� ����)

protected:
	XMFLOAT3		 m_xmf3Position;													   // ī�޶��� ��ġ(���� ��ǥ��) ����
	XMFLOAT3		 m_xmf3Right;														   // ī�޶��� ���� x��(Right) ����
	XMFLOAT3		 m_xmf3Up;															   // ī�޶��� ���� y��(Up) ����
	XMFLOAT3		 m_xmf3Look;														   // ī�޶��� ���� z��(Look) ����
protected:
	FLOAT			 m_fPitch;															   // ī�޶� x�� ȸ����
	FLOAT			 m_fRoll;															   // ī�޶� y�� ȸ����
	FLOAT			 m_fYaw;															   // ī�޶� z�� ȸ����
protected:
	DWORD			 m_dwMode;															   // ī�޶��� ����(1��Ī ī�޶�, Spaceship ī�޶�, 3��Ī ī�޶�)
protected:
	XMFLOAT3		 m_xmf3LookAtWorld;													   // �÷��̾ �ٶ� ��ġ ����(�ַ� 3��Ī ī�޶󿡼� ���)
	XMFLOAT3		 m_xmf3Offset;														   // �÷��̾�� ī�޶��� Offset ����(�ַ� 3��Ī ī�޶󿡼� ���)
	FLOAT			 m_fTimeLag;														   // �÷��̾ ȸ���� �� �󸶸�ŭ�� �ð��� ������Ų �� ī�޶� ȸ����ų ���ΰ��� ��Ÿ����.
protected:
	XMFLOAT4X4		 m_xmf4x4View;														   // ī�޶� ��ȯ ���
	XMFLOAT4X4		 m_xmf4x4Projection;												   // ���� ��ȯ ���
protected:
	D3D12_VIEWPORT	 m_d3dViewport;														   // Viewport
	D3D12_RECT		 m_d3dScissorRect;													   // Scissor �簢��
protected:
	CPlayer*		 m_pPlayer{ nullptr };												   // ī�޶� ������ �ִ� �÷��̾� ������
};

class CFirstPersonCamera : public CCamera
{
public:
	CFirstPersonCamera(CCamera* pCamera);
	virtual ~CFirstPersonCamera() { }
public:
	virtual void Rotate(FLOAT fPitch = 0.0f, FLOAT fYaw = 0.0f, FLOAT fRoll = 0.0f);
};

class CThirdPersonCamera : public CCamera
{
public:
	CThirdPersonCamera(CCamera* pCamera);
	virtual ~CThirdPersonCamera() {}
public:
	virtual void Update(XMFLOAT3& xmf3LookAt, FLOAT fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3 & xmf3LookAt);
};

class CSpaceShipCamera : public CCamera
{
public:
	CSpaceShipCamera(CCamera* pCamera);
	virtual ~CSpaceShipCamera() {}
public:
	virtual void Rotate(FLOAT fPitch = 0.0f, FLOAT fYaw = 0.0f, FLOAT fRoll = 0.0f);
};
