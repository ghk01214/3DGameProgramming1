#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"

class CShader;

class CGameObject
{
public:
	CGameObject(INT nMeshes = 1);
	virtual ~CGameObject();
public:
	void AddReference() { ++m_nReferences; }
	void Release();
public:
	void ReleaseUploadBuffers();
public:
	virtual void SetMesh(INT nIndex, CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
public:
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);		// 상수 버퍼 생성
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);									// 상수 버퍼의 내용 갱신
	virtual void ReleaseShaderVariables();
public:
	// Game Object의 위치 설정
	void SetPosition(FLOAT x, FLOAT y, FLOAT z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetCollidedObject(CGameObject* pObjectCollided);
	void SetBoundingBox(BoundingOrientedBox xmBoundingBox) { m_xmBoundingBox = xmBoundingBox; }
public:
	// Game Object의 월드 변환 행렬에서 위치 벡터와 방향(x축, y축, z축)벡터 반환
	XMFLOAT3 GetPosition() { return XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43); }
	XMFLOAT3 GetLook() { return XMFLOAT3(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33))); }
	XMFLOAT3 GetUp() { return XMFLOAT3(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23))); }
	XMFLOAT3 GetRight() { return XMFLOAT3(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13))); }
	XMFLOAT4X4 GetWorldMatrix() { return m_xmf4x4World; }
	CGameObject* GetCollidedObject() { return m_pObjectCollided; }
	virtual BoundingOrientedBox GetBoundingBox() { return m_xmBoundingBox; }
	virtual BoundingOrientedBox CGameObject::GetPlayerMoveCheckBoundingBox() { return BoundingOrientedBox(); }
public:
	void UpdateBoundingBox();
public:
	void MoveStrate(FLOAT fDistance = 1.0f);
	void MoveUp(FLOAT fDistance = 1.0f);
	void MoveForward(FLOAT fDistance = 1.0f);
public:
	void Rotate(FLOAT fPitch = 10.0f, FLOAT fYaw = 10.0f, FLOAT fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, FLOAT fAngle);
public:
	virtual void Animate(FLOAT fTimeElapsed, CCamera* pCamera);
public:
	virtual void PrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

private:
	INT						m_nReferences{ 0 };
protected:
	XMFLOAT4X4				m_xmf4x4World;
protected:
	std::vector<CMesh*>		m_vpMeshes{ nullptr };
	INT						m_nMeshes{ 0 };
	CShader*				m_pShader{ nullptr };
protected:
	CGameObject*			m_pObjectCollided{ nullptr };
protected:
	BoundingOrientedBox		m_xmBoundingBox;
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(INT nMeshes = 1);
	virtual ~CRotatingObject();
public:
	void SetRotationSpeed(FLOAT fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
public:
	virtual void Animate(FLOAT fTimeElapsed, CCamera* pCamera);

private:
	XMFLOAT3			 m_xmf3RotationAxis;
	FLOAT				 m_fRotationSpeed;
};

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName,
		INT nWidth, INT nLength, INT nBlockWidth, INT nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CHeightMapTerrain();
public:
	// 지형의 높이를 계산하는 함수(월드 좌표계). Height Map의 높이에 Scale의 y를 곱한 값.
	FLOAT GetHeight(FLOAT x, FLOAT z) { return(m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y); }
	// 지형의 법선 벡터를 계산하는 함수(월드 좌표계). Height Map의 법선 벡터 사용.
	XMFLOAT3 GetNormal(FLOAT x, FLOAT z) { return(m_pHeightMapImage->GetHeightMapNormal(INT(x / m_xmf3Scale.x), INT(z / m_xmf3Scale.z))); }
	INT GetHeightMapWidth() { return m_pHeightMapImage->GetHeightMapWidth(); }
	INT GetHeightMapLength() { return m_pHeightMapImage->GetHeightMapLength(); }
	XMFLOAT3 GetScale() { return m_xmf3Scale; }
	// 지형의 크기(가로/세로) 반환. Height Map의 크기에 Scale을 곱한 값.
	FLOAT GetWidth() { return m_nWidth * m_xmf3Scale.x; }
	FLOAT GetLength() { return m_nLength * m_xmf3Scale.z; }

private:
	CHeightMapImage*	 m_pHeightMapImage;								  // 지형의 Height Map으로 사용할 이미지.
private:
	INT					 m_nWidth;										  // Height Map의 가로 크기
	INT					 m_nLength;										  // Height Map의 세로 크기
private:
	XMFLOAT3			 m_xmf3Scale;									  // 지형을 실제로 몇 배 확대할 것인가를 나타내는 Scale 벡터
};