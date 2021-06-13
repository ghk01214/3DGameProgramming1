#pragma once
#include "GameObject.h"
#include "Camera.h"

class CGameObject;
class CWallObject;

class CShader
{
public:
	CShader();
	virtual ~CShader();
public:
	void AddReference() { ++m_nReferences; }
	void Release();
public:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
public:
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
public:
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
public:
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
public:
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
public:
	virtual void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

private:
	INT									 m_nReferences{ 0 };
protected:
	ID3D12PipelineState**				 m_ppd3dPipelineStates{ nullptr };
	INT									 m_nPipelineStates{ 0 };
};

class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();
public:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
public:
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
};

// CObjectsShader Class는 Game Object들을 포함하는 Shader 객체
class CObjectsShader : public CShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();
public:
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects();
public:
	virtual void Animate(FLOAT fTimeElapsed, CCamera* pCamera);
public:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
public:
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
public:
	virtual void ReleaseUploadBuffers();
public:
	INT GetObjectsNum() { return m_nObjects; }
	virtual CGameObject* GetObjects(INT i) { return m_vpObjects[i]; }
	//virtual CGameObject* GetCollidedObject(INT i) { return m_vpObjects[i]->GetCollidedObject(); }
public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

protected:
	std::vector<CGameObject*>	 m_vpObjects;
	INT							 m_nObjects{ 0 };
};

class CApproachingShader : public CObjectsShader
{
public:
	CApproachingShader();
	~CApproachingShader();
public:
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects();
	void RespawnObjects(CPlayer* pPlayer);
	void ReleaseApproachingObject(CGameObject* pObject);
public:
	virtual CGameObject* GetObjects(INT i);
public:
	virtual void Animate(FLOAT fTimeElapsed, CCamera* pCamera);
public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

protected:
	std::list<CGameObject*>			 m_lpObjects;
	std::vector<CCubeMeshDiffused*>	 m_pObjectMesh{ nullptr, nullptr };
	std::set<FLOAT>					 m_sPositionRepitition;
};

class CWallShader : public CObjectsShader
{
public:
	CWallShader();
	virtual ~CWallShader();
public:
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};