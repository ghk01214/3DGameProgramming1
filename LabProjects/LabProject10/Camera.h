#pragma once

#define ASPECT_RATIO (FLOAT)FRAME_BUFFER_WIDTH / (FLOAT)FRAME_BUFFER_HEIGHT

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
	virtual ~CCamera();
public:
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
public:
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void GenerateProjectionMatrix(FLOAT fNearPlaneDistance, FLOAT fFarPlaneDistance, FLOAT fAspectRatio, FLOAT fFOVAngle);
public:
	void SetViewport(INT xTopLeft, INT yTopLeft, INT nWidth, INT nHeight, FLOAT fMinZ = 0.0f, FLOAT fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
public:
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);

protected:
	XMFLOAT4X4 m_xmf4x4View;								  // 카메라 변환 행렬
	XMFLOAT4X4 m_xmf4x4Projection;							  // 투영 변환 행렬
protected:
	D3D12_VIEWPORT m_d3dViewport;							  // Viewport
	D3D12_RECT m_d3dScissorRect;							  // Scissor 사각형
};

