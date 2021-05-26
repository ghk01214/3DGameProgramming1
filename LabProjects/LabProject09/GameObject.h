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