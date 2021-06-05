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
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);		// ��� ���� ����
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);									// ��� ������ ���� ����
	virtual void ReleaseShaderVariables();
public:
	// Game Object�� ��ġ ����
	void SetPosition(FLOAT x, FLOAT y, FLOAT z);
	void SetPosition(XMFLOAT3 xmf3Position);
public:
	// Game Object�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x��, y��, z��)���� ��ȯ
	XMFLOAT3 GetPosition() { return XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43); }
	XMFLOAT3 GetLook() { return XMFLOAT3(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33))); }
	XMFLOAT3 GetUp() { return XMFLOAT3(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23))); }
	XMFLOAT3 GetRight() { return XMFLOAT3(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13))); }
	XMFLOAT4X4 GetWorldMatrix() { return m_xmf4x4World; }
public:
	BOOL IsVisible(CCamera* pCamera = nullptr);
public:
	// Game Object�� Local x��, y��, z�� �������� �̵�
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
};

class CWallObject : public CGameObject
{
public:
	CWallObject(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth);
	virtual ~CWallObject();
public:
	virtual void Animate(FLOAT fTimeElapsed, CCamera* pCamera);

private:
	XMFLOAT3			m_xmf3Position;
	FLOAT				m_fWidth;
	FLOAT				m_fHeight;
	FLOAT				m_fDepth;
};

class CCarObject : public CGameObject
{

};