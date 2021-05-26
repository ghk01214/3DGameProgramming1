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
	// Game Object�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x��, y��, z��)���� ��ȯ
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
public:
	// Game Object�� ��ġ ����
	void SetPosition(FLOAT x, FLOAT y, FLOAT z);
	void SetPosition(XMFLOAT3 xmf3Position);
public:
	// Game Object�� Local x��, y��, z�� �������� �̵�
	void MoveStrate(FLOAT fDistance = 1.0f);
	void MoveUp(FLOAT fDistance = 1.0f);
	void MoveForward(FLOAT fDistance = 1.0f);
public:
	// Game Object�� ȸ��(x��, y��, z��)
	void Rotate(FLOAT fPitch = 10.0f, FLOAT fYaw = 10.0f, FLOAT fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, FLOAT fAngle);
public:
	virtual void Animate(FLOAT fTimeElapsed);
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

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();
public:
	void SetRotationSpeed(FLOAT fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
public:
	virtual void Animate(FLOAT fTimeElapsed);

private:
	XMFLOAT3			 m_xmf3RotationAxis;
	FLOAT				 m_fRotationSpeed;
};