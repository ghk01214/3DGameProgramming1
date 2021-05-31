#include "framework.h"
#include "Shader.h"

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

// �Է� �����⿡�� Vertex ������ ������ �˷��ֱ� ���� ����ü ��ȯ
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	
	d3dInputLayoutDesc.pInputElementDescs	 = nullptr;
	d3dInputLayoutDesc.NumElements			 = 0;
	
	return d3dInputLayoutDesc;
}

// Rasterizer ���¸� �����ϱ� ���� ����ü ��ȯ
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

// Depth-Stencil �˻縦 ���� ���¸� �����ϱ� ���� ����ü ��ȯ
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

// Vertex Shader ����Ʈ �ڵ� ����(������)
D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;

	d3dShaderByteCode.BytecodeLength	 = 0;
	d3dShaderByteCode.pShaderBytecode	 = nullptr;

	return d3dShaderByteCode;
}

// Pixel Shader ����Ʈ �ڵ� ����(������)
D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;

	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = nullptr;

	return d3dShaderByteCode;
}

// Shader �ҽ� �ڵ带 �������Ͽ� ����Ʈ �ڵ� ����ü ��ȯ
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

// Graphics Pipeline ���� ��ü ����
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
	// Pipeline�� Graphics ���� ��ü ����
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
	if (m_ppObjects)
	{
		for (INT i = 0; i < m_nObjects; ++i)
		{
			if (m_ppObjects[i])
				delete m_ppObjects[i];
		}

		delete[] m_ppObjects;
	}
}

void CObjectsShader::Animate(FLOAT fTimeElapsed)
{
	for (INT i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i]->Animate(fTimeElapsed);
	}
}

D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout()
{
	UINT						 nInputElementDescs{ 2 };
	D3D12_INPUT_ELEMENT_DESC*	 pd3dInputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs] };

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;

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
	if (m_ppObjects)
	{
		for (INT i = 0; i < m_nObjects; ++i)
		{
			m_ppObjects[i]->ReleaseUploadBuffers();
		}
	}
}

void CObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	for (INT i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
			m_ppObjects[i]->Render(pd3dCommandList, pCamera);
	}
}

//=============================================================================================================

CInstancingShader::CInstancingShader()
{
}

CInstancingShader::~CInstancingShader()
{
}

D3D12_INPUT_LAYOUT_DESC CInstancingShader::CreateInputLayout()
{
	UINT						 nInputElementDescs{ 7 };
	D3D12_INPUT_ELEMENT_DESC*	 pd3dInputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs] };
	
	// Vertex ������ ���� �Է� ����
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	// Instancing ������ ���� �Է� ����
	pd3dInputElementDescs[2] = { "WORLDMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	pd3dInputElementDescs[3] = { "WORLDMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	pd3dInputElementDescs[4] = { "WORLDMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	pd3dInputElementDescs[5] = { "WORLDMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	pd3dInputElementDescs[6] = { "INSTANCECOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;

	d3dInputLayoutDesc.pInputElementDescs	 = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements			 = nInputElementDescs;

	return d3dInputLayoutDesc;
}

D3D12_SHADER_BYTECODE CInstancingShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return CShader::CompileShaderFromFile(L"VertexShader.hlsl", "Instancing", "vs_5_1", ppd3dShaderBlob);
}

D3D12_SHADER_BYTECODE CInstancingShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return CShader::CompileShaderFromFile(L"PixelShader.hlsl", "Instancing", "ps_5_1", ppd3dShaderBlob);
}

void CInstancingShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates		 = 1;
	m_ppd3dPipelineStates	 = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CInstancingShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// Instance ������ ������ Vertex ���۸� ���ε� Heap �������� ����
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, sizeof(VS_VB_INSTANCE) * m_nObjects, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	
	// Vertex ����(���ε� Heap)�� ���� ������ ����
	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);
	
	// Vertex ���ۿ� ���� �� ����
	m_d3dInstancingBufferView.BufferLocation	 = m_pd3dcbGameObjects->GetGPUVirtualAddress();
	m_d3dInstancingBufferView.StrideInBytes		 = sizeof(VS_VB_INSTANCE);
	m_d3dInstancingBufferView.SizeInBytes		 = sizeof(VS_VB_INSTANCE) * m_nObjects;
}

// Instancing ����(��ü�� ���� ��ȯ ��İ� ����)�� Vertex ���ۿ� ����
void CInstancingShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (INT i = 0; i < m_nObjects; i++)
	{
		m_pcbMappedGameObjects[i].m_xmcColor = (i % 2) ? XMFLOAT4(0.5f, 0.0f, 0.0f, 0.0f) : XMFLOAT4(0.0f, 0.0f, 0.5f, 0.0f);

		XMStoreFloat4x4(&m_pcbMappedGameObjects[i].m_xmf4x4Transform, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CInstancingShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
		m_pd3dcbGameObjects->Unmap(0, NULL);

	if (m_pd3dcbGameObjects)
		m_pd3dcbGameObjects->Release();
}

void CInstancingShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// x��, y��, z�� ���� ������ ��ü ����.
	// �� ���� 1�� �ø��ų� ���̸鼭 ������ �� ������ ����Ʈ�� ��� ���ϴ� ���� ���캸�� �ٶ���.
	INT xObjects = 10;
	INT yObjects = 10;
	INT zObjects = 10;
	INT i = 0;

	// x��, y��, z������ 21���� �� 21 x 21 x 21 = 9261���� ������ü�� ���� �� ��ġ
	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);
	m_ppObjects = new CGameObject * [m_nObjects];

	FLOAT fxPitch = 12.0f * 2.5f;
	FLOAT fyPitch = 12.0f * 2.5f;
	FLOAT fzPitch = 12.0f * 2.5f;

	CRotatingObject* pRotatingObject{ nullptr };

	for (INT x = -xObjects; x <= xObjects; ++x)
	{
		for (INT y = -yObjects; y <= yObjects; ++y)
		{
			for (INT z = -zObjects; z <= zObjects; ++z)
			{
				pRotatingObject = new CRotatingObject();

				pRotatingObject->SetPosition(fxPitch * x, fyPitch * y, fzPitch * z);		//�� ������ü ��ü�� ��ġ�� �����Ѵ�.
				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f * (i % 10) + 3.0f);

				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}

	// Instancing�� ����Ͽ� Rendering�ϱ� ���Ͽ� �ϳ��� Game Object�� Mesh�� ������.
	CCubeMeshDiffused* pCubeMesh{ new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f) };
	m_ppObjects[0]->SetMesh(pCubeMesh);

	// Instancing�� ���� Vertex ���ۿ� �� ����
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CInstancingShader::ReleaseObjects()
{
}

void CInstancingShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	// ��� Game Object�� Instancing �����͸� ���ۿ� ����
	UpdateShaderVariables(pd3dCommandList);

	// �ϳ��� Vertex �����͸� ����Ͽ� ��� Game Object(Instance)�� Rendering
	m_ppObjects[0]->Render(pd3dCommandList, pCamera, m_nObjects, m_d3dInstancingBufferView);
}
