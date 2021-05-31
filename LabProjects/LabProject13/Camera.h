#pragma once

// 카메라의 종류(모드: Mode)를 나타내는 상수
#define FIRST_PERSON_CAMERA	 0x01
#define SPACESHIP_CAMERA	 0x02
#define THIRD_PERSON_CAMERA	 0x03

// Frame 버퍼의 크기와 Aspect Ratio(종횡비)를 나타내는 상수
#define ASPECT_RATIO (FLOAT)FRAME_BUFFER_WIDTH / (FLOAT)FRAME_BUFFER_HEIGHT

class CPlayer;

// 카메라 상수 버퍼 구조체
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
	virtual void Move(XMFLOAT3& xmf3Shift);												   // 카메라를 xmf3Shift 만큼 이동
	virtual void Rotate(FLOAT fPitch = 0.0f, FLOAT fYaw = 0.0f, FLOAT fRoll = 0.0f) {}	   // 카메라를 x축, y축, z축으로 회전하는 가상함수
	virtual void Update(XMFLOAT3& xmf3LookAt, FLOAT fTimeElapsed) {}					   // 카메라의 이동, 회전에 따라 카메라의 정보를 갱신하는 가상함수
	virtual void SetLookAt(XMFLOAT3& xmf3LookAt) {}										   // 3인칭 카메라에서 카메라가 바라보는 지점 설정(일반적으로 플레이어를 바라보도록 설정)

protected:
	XMFLOAT3		 m_xmf3Position;													   // 카메라의 위치(월드 좌표계) 벡터
	XMFLOAT3		 m_xmf3Right;														   // 카메라의 로컬 x축(Right) 벡터
	XMFLOAT3		 m_xmf3Up;															   // 카메라의 로컬 y축(Up) 벡터
	XMFLOAT3		 m_xmf3Look;														   // 카메라의 로컬 z축(Look) 벡터
protected:
	FLOAT			 m_fPitch;															   // 카메라 x축 회전량
	FLOAT			 m_fRoll;															   // 카메라 y축 회전량
	FLOAT			 m_fYaw;															   // 카메라 z축 회전량
protected:
	DWORD			 m_dwMode;															   // 카메라의 종류(1인칭 카메라, Spaceship 카메라, 3인칭 카메라)
protected:
	XMFLOAT3		 m_xmf3LookAtWorld;													   // 플레이어가 바라볼 위치 벡터(주로 3인칭 카메라에서 사용)
	XMFLOAT3		 m_xmf3Offset;														   // 플레이어와 카메라의 Offset 벡터(주로 3인칭 카메라에서 사용)
	FLOAT			 m_fTimeLag;														   // 플레이어가 회전할 때 얼마만큼의 시간을 지연시킨 후 카메라를 회전시킬 것인가를 나타낸다.
protected:
	XMFLOAT4X4		 m_xmf4x4View;														   // 카메라 변환 행렬
	XMFLOAT4X4		 m_xmf4x4Projection;												   // 투영 변환 행렬
protected:
	D3D12_VIEWPORT	 m_d3dViewport;														   // Viewport
	D3D12_RECT		 m_d3dScissorRect;													   // Scissor 사각형
protected:
	CPlayer*		 m_pPlayer{ nullptr };												   // 카메라를 가지고 있는 플레이어 포인터
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
