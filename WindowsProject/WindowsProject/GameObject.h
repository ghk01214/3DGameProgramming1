#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"

class CShader;

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();
public:
	void AddReference() { ++m_nReferences; }
	void Release();
public:
	void ReleaseUploadBuffers();
public:
	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
public:
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);		// 상수 버퍼 생성
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);									// 상수 버퍼의 내용 갱신
	virtual void ReleaseShaderVariables();
public:
	// Game Object의 위치 설정
	void SetPosition(FLOAT x, FLOAT y, FLOAT z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	void SetCollidedObject(CGameObject* pObjectCollided) { m_pObjectCollided = pObjectCollided; }
	void SetBoundingBox(BoundingOrientedBox xmOOBB) { m_xmBoundingBox = xmOOBB; }
public:
	// Game Object의 월드 변환 행렬에서 위치 벡터와 방향(x축, y축, z축)벡터 반환
	XMFLOAT3 GetPosition() { return XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43); }
	XMFLOAT3 GetLook() { return XMFLOAT3(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33))); }
	XMFLOAT3 GetUp() { return XMFLOAT3(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23))); }
	XMFLOAT3 GetRight() { return XMFLOAT3(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13))); }
	XMFLOAT4X4 GetWorldMatrix() { return m_xmf4x4World; }
	CPlayer* GetPlayer() { return m_pPlayer; }
	CGameObject* GetCollidedObject() { return m_pObjectCollided; }
	BoundingOrientedBox GetBoundingBox() { return m_xmBoundingBox; }
	virtual BoundingOrientedBox CGameObject::GetPlayerMoveCheckBoundingBox() { return BoundingOrientedBox(); }
public:
	BOOL IsVisible(CCamera* pCamera = nullptr);
	void UpdateBoundingBox();
public:
	// Game Object를 Local x축, y축, z축 방향으로 이동
	void MoveStrate(FLOAT fDistance = 1.0f);
	void MoveUp(FLOAT fDistance = 1.0f);
	void MoveForward(FLOAT fDistance = 1.0f);
public:
	virtual void Animate(FLOAT fTimeElapsed, CCamera* pCamera);
public:
	virtual void PrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

private:
	INT					m_nReferences{ 0 };
protected:
	XMFLOAT4X4			m_xmf4x4World;
	CMesh*				m_pMesh{ nullptr };
	CShader*			m_pShader{ nullptr };
	CPlayer*			m_pPlayer{ nullptr };
protected:
	CGameObject*		m_pObjectCollided{ nullptr };
protected:
	BoundingOrientedBox m_xmBoundingBox;
public:
	std::vector<XMFLOAT4>	 m_pxmf4WallPlanes;
};

class CWallObject : public CGameObject
{
public:
	CWallObject(FLOAT fDepth);
	virtual ~CWallObject();
public:
	virtual void Animate(FLOAT fTimeElapsed, CCamera* pCamera);

private:
	XMFLOAT3			m_xmf3Position;
	FLOAT				m_fDepth;
};

class CApproachingObject : public CGameObject
{
public:
	CApproachingObject(FLOAT fDepth);
	virtual ~CApproachingObject();
public:
	virtual void Animate(FLOAT fTimeElapsed, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

private:
	FLOAT				m_fApproachingSpeed;
	FLOAT				m_fDepth;
};