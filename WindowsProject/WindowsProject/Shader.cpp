#include "framework.h"
#include "Shader.h"
#include "Player.h"

CShader::CShader()
{
}

CShader::~CShader()
{
	if (m_ppd3dPipelineStates)
	{
		for (INT i = 0; i < m_nPipelineStates; ++i)
		{
			if (m_ppd3dPipelineStates[i])
				m_ppd3dPipelineStates[i]->Release();
		}

		delete[] m_ppd3dPipelineStates;
	}
}

void CShader::Release()
{
	if (--m_nReferences <= 0)
		delete this;
}

// 입력 조립기에게 Vertex 버퍼의 구조를 알려주기 위한 구조체 반환
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	
	d3dInputLayoutDesc.pInputElementDescs	 = nullptr;
	d3dInputLayoutDesc.NumElements			 = 0;
	
	return d3dInputLayoutDesc;
}

// Rasterizer 상태를 설정하기 위한 구조체 반환
D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	d3dRasterizerDesc.FillMode					 = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode					 = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.FrontCounterClockwise		 = FALSE;
	d3dRasterizerDesc.DepthBias					 = 0;
	d3dRasterizerDesc.DepthBiasClamp			 = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias		 = 0.0f;
	d3dRasterizerDesc.DepthClipEnable			 = TRUE;
	d3dRasterizerDesc.MultisampleEnable			 = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable		 = FALSE;
	d3dRasterizerDesc.ForcedSampleCount			 = 0;
	d3dRasterizerDesc.ConservativeRaster		 = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return d3dRasterizerDesc;
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));

	d3dBlendDesc.AlphaToCoverageEnable					 = FALSE;
	d3dBlendDesc.IndependentBlendEnable					 = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable			 = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable			 = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend				 = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend				 = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp				 = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha			 = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha			 = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha			 = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp				 = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask	 = D3D12_COLOR_WRITE_ENABLE_ALL;

	return d3dBlendDesc;
}

// Depth-Stencil 검사를 위한 상태를 설정하기 위한 구조체 반환
D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));

	d3dDepthStencilDesc.DepthEnable						 = TRUE;
	d3dDepthStencilDesc.DepthWriteMask					 = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc						 = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable					 = FALSE;
	d3dDepthStencilDesc.StencilReadMask					 = 0x00;
	d3dDepthStencilDesc.StencilWriteMask				 = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp			 = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp	 = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp			 = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc			 = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp			 = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp		 = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp			 = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc			 = D3D12_COMPARISON_FUNC_NEVER;

	return d3dDepthStencilDesc;
}

// Vertex Shader 바이트 코드 생성(컴파일)
D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;

	d3dShaderByteCode.BytecodeLength	 = 0;
	d3dShaderByteCode.pShaderBytecode	 = nullptr;

	return d3dShaderByteCode;
}

// Pixel Shader 바이트 코드 생성(컴파일)
D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;

	d3dShaderByteCode.BytecodeLength	 = 0;
	d3dShaderByteCode.pShaderBytecode	 = nullptr;

	return d3dShaderByteCode;
}

// Shader 소스 코드를 컴파일하여 바이트 코드 구조체 반환
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	UINT nCompileFlags{ 0 };
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	::D3DCompileFromFile(pszFileName, nullptr, nullptr, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, nullptr);

	D3D12_SHADER_BYTECODE d3dShaderByteCode;

	d3dShaderByteCode.BytecodeLength	 = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode	 = (*ppd3dShaderBlob)->GetBufferPointer();

	return d3dShaderByteCode;
}

// Graphics Pipeline 상태 객체 생성
void CShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
	ID3DBlob* pd3dVertexShaderBlob{ nullptr };
	ID3DBlob* pd3dPixelShaderBlob{ nullptr };

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	d3dPipelineStateDesc.pRootSignature				 = pd3dRootSignature;
	d3dPipelineStateDesc.VS							 = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS							 = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState			 = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState					 = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState			 = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout				 = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask					 = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType		 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets			 = 1;
	d3dPipelineStateDesc.RTVFormats[0]				 = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat					 = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count			 = 1;
	d3dPipelineStateDesc.Flags						 = D3D12_PIPELINE_STATE_FLAG_NONE;

	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);

	if (pd3dVertexShaderBlob)
		pd3dVertexShaderBlob->Release();

	if (pd3dPixelShaderBlob)
		pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;

	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CShader::ReleaseShaderVariables()
{
}

void CShader::PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// Pipeline에 Graphics 상태 객체 설정
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}

void CShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	PrepareRender(pd3dCommandList);
}

//=============================================================================================================

CPlayerShader::CPlayerShader()
{
}

CPlayerShader::~CPlayerShader()
{
}

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{
	UINT						 nInputElementDescs{ 2 };
	D3D12_INPUT_ELEMENT_DESC*	 pd3dInputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs] };

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC		 d3dInputLayoutDesc;

	d3dInputLayoutDesc.pInputElementDescs	 = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements			 = nInputElementDescs;

	return d3dInputLayoutDesc;
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return CShader::CompileShaderFromFile(L"VertexShader.hlsl", "Diffused", "vs_5_1", ppd3dShaderBlob);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return CShader::CompileShaderFromFile(L"PixelShader.hlsl", "Diffused", "ps_5_1", ppd3dShaderBlob);
}

void CPlayerShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates		 = 1;
	m_ppd3dPipelineStates	 = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

//=============================================================================================================

CObjectsShader::CObjectsShader()
{
}

CObjectsShader::~CObjectsShader()
{
}

void CObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CObjectsShader::ReleaseObjects()
{
	if (!m_vpObjects.empty())
	{
		for (auto iter = m_vpObjects.begin(); iter != m_vpObjects.end(); ++iter)
		{
			if (*iter)
				delete* iter;
		}

		m_vpObjects.clear();
		m_vpObjects.shrink_to_fit();
	}
}

void CObjectsShader::Animate(FLOAT fTimeElapsed, CCamera* pCamera)
{
	for (auto iter = m_vpObjects.begin(); iter != m_vpObjects.end(); ++iter)
	{
		(*iter)->Animate(fTimeElapsed, pCamera);
	}
}

D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout()
{
	UINT						 nInputElementDescs{ 2 };
	D3D12_INPUT_ELEMENT_DESC*	 pd3dInputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs] };

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC		 d3dInputLayoutDesc;

	d3dInputLayoutDesc.pInputElementDescs	 = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements			 = nInputElementDescs;

	return d3dInputLayoutDesc;
}

D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return CShader::CompileShaderFromFile(L"VertexShader.hlsl", "Diffused", "vs_5_1", ppd3dShaderBlob);
}

D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return CShader::CompileShaderFromFile(L"PixelShader.hlsl", "Diffused", "ps_5_1", ppd3dShaderBlob);
}

void CObjectsShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates		 = 1;
	m_ppd3dPipelineStates	 = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CObjectsShader::ReleaseUploadBuffers()
{
	if (!m_vpObjects.empty())
	{
		for (auto iter = m_vpObjects.begin(); iter != m_vpObjects.end(); ++iter)
		{
			(*iter)->ReleaseUploadBuffers();
		}
	}
}

void CObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	for (auto iter = m_vpObjects.begin(); iter != m_vpObjects.end(); ++iter)
	{
		if (*iter)
			(*iter)->Render(pd3dCommandList, pCamera);
	}
}

//=============================================================================================================

CApproachingShader::CApproachingShader()
{
}

CApproachingShader::~CApproachingShader()
{
}

void CApproachingShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nObjects = 9 * 2;

	CCubeMeshDiffused*	 pObjectMesh{ new CFeverCubeMeshDiffused(pd3dDevice, pd3dCommandList, 15.0f, 15.0f, 15.0f) };
	m_pObjectMesh[1]	 = pObjectMesh;

	CGameObject*		 pObject{ nullptr };
	INT					 j{ -3 };

	pObjectMesh			 = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 15.0f, 15.0f, 15.0f);
	m_pObjectMesh[0]	 = pObjectMesh;

	for (INT i = 0; i < m_nObjects; ++i)
	{
		std::uniform_int_distribution<>	 randomPositionX(-3, 3);
		std::uniform_real_distribution<> randomPositionZ(200.0f, 800.0f);
		FLOAT							 fPosition{ static_cast<FLOAT>(randomPositionZ(dre)) };
		INT								 nSeed{ randomPositionX(dre) };

		while (TRUE)
		{
			auto pos{ std::find_if(m_lPositionRepitition.begin(), m_lPositionRepitition.end(), [fPosition](const FLOAT& a)
				{
					return (fPosition - 7.5f < a) && (a < fPosition + 7.5f);
				})};

			if (pos == m_lPositionRepitition.end())
				break;

			fPosition = static_cast<FLOAT>(randomPositionZ(dre));
		}

		m_lPositionRepitition.push_back(fPosition);

		pObjectMesh	 = m_pObjectMesh[0];
		pObject		 = new CApproachingObject();

		if (nSeed % 3 == 0 && nSeed > 0)
		{
			pObjectMesh	 = m_pObjectMesh[1];
			pObject		 = new CFeverObject();
		}

		pObject->SetMesh(pObjectMesh);
		pObject->SetPosition(20.0f * j++, 0.0f, fPosition);
		pObject->SetBoundingBox(pObjectMesh->GetBoundingBox());

		m_lpObjects.push_back(pObject);

		if (j > 3)
			j = -3;
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CApproachingShader::ReleaseObjects()
{
	if (!m_lpObjects.empty())
	{
		for (auto iter = m_lpObjects.begin(); iter != m_lpObjects.end(); ++iter)
		{
			if (*iter)
				delete* iter;
		}

		m_lpObjects.clear();
	}
}

void CApproachingShader::RespawnObjects(CPlayer* pPlayer)
{
	++m_nObjects;

	CCubeMeshDiffused*	 pObjectMesh{ m_pObjectMesh[0] };
	CGameObject*		 pObject{ nullptr };

	std::uniform_int_distribution<>	 randomPositionX(-3, 3);
	std::uniform_real_distribution<> randomPositionZ(200.0f, 800.0f);
	FLOAT							 fPositionZ{ static_cast<FLOAT>(randomPositionZ(dre)) };
	INT								 nSeed{ randomPositionX(dre) };

	while (TRUE)
	{
		auto pos{ std::find_if(m_lPositionRepitition.begin(), m_lPositionRepitition.end(), [fPositionZ](const FLOAT& a)
			{
				return (fPositionZ - 7.5f < a) && (a < fPositionZ + 7.5f);
			}) };

		if (pos == m_lPositionRepitition.end())
			break;

		fPositionZ = static_cast<FLOAT>(randomPositionZ(dre));
	}

	m_lPositionRepitition.push_back(fPositionZ);

	pObject = new CApproachingObject();

	if (nSeed % 3 == 0 && nSeed > 0)
	{
		pObjectMesh	 = m_pObjectMesh[1];
		pObject		 = new CFeverObject();
	}

	pObject->SetMesh(pObjectMesh);
	pObject->SetPosition(20.0f * randomPositionX(dre), 0.0f, pPlayer->GetPosition().z + fPositionZ);
	pObject->SetBoundingBox(pObjectMesh->GetBoundingBox());

	m_lpObjects.push_back(pObject);
}

void CApproachingShader::ReleaseApproachingObject(CGameObject* pObject)
{
	--m_nObjects;

	m_lPositionRepitition.remove(pObject->GetPosition().z);
	m_lpObjects.remove(pObject);
}

CGameObject* CApproachingShader::GetObjects(INT i)
{
	auto pos{ m_lpObjects.begin() };

	std::advance(pos, i);

	return *pos;
}

void CApproachingShader::Animate(FLOAT fTimeElapsed, CCamera* pCamera)
{
	for (auto iter = m_lpObjects.begin(); iter != m_lpObjects.end(); ++iter)
	{
		(*iter)->Animate(fTimeElapsed, pCamera);
	}
}

void CApproachingShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	for (auto iter = m_lpObjects.begin(); iter != m_lpObjects.end(); ++iter)
	{
		if (*iter)
			(*iter)->Render(pd3dCommandList, pCamera);
	}
}

//=============================================================================================================

CWallShader::CWallShader()
{
}

CWallShader::~CWallShader()
{
}

void CWallShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nObjects = 2 * 2;
	m_vpObjects.reserve(m_nObjects);

	CWallMeshDiffused*	 pWallMesh{ nullptr };
	CWallObject*		 pWallObject{ nullptr };

	FLOAT				 fWidth{ 160.0f };
	FLOAT				 fHeight{ 160.0f };
	FLOAT				 fDepth{ 160.0f };

	for (INT i = 0; i < m_nObjects; ++i)
	{
		pWallMesh	 = new CWallMeshDiffused(pd3dDevice, pd3dCommandList, fWidth, fHeight, fDepth, 1.0f * ((i % 4) + 1));
		pWallObject	 = new CWallObject(fDepth);

		pWallObject->SetMesh(pWallMesh);
		pWallObject->SetPosition(0.0f, 0.0f, fDepth * i);
		pWallObject->SetBoundingBox(pWallMesh->GetBoundingBox());

		m_vpObjects.push_back(pWallObject);
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}