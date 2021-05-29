#include "framework.h"
#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature*	 pd3dGraphicsRootSignature{ nullptr };
	D3D12_ROOT_PARAMETER	 pd3dRootParameters[3];

	pd3dRootParameters[0].ParameterType					 = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues		 = 16;
	pd3dRootParameters[0].Constants.ShaderRegister		 = 0;	// b0 : Player
	pd3dRootParameters[0].Constants.RegisterSpace		 = 0;
	pd3dRootParameters[0].ShaderVisibility				 = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType					 = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues		 = 32;
	pd3dRootParameters[1].Constants.ShaderRegister		 = 1;	// b1 : Camera
	pd3dRootParameters[1].Constants.RegisterSpace		 = 0;
	pd3dRootParameters[1].ShaderVisibility				 = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[2].ParameterType					 = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[2].Constants.ShaderRegister		 = 0;	// t0
	pd3dRootParameters[2].Constants.RegisterSpace		 = 0;
	pd3dRootParameters[2].ShaderVisibility				 = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags{
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS };
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));

	d3dRootSignatureDesc.NumParameters		 = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters		 = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers	 = 0;
	d3dRootSignatureDesc.pStaticSamplers	 = nullptr;
	d3dRootSignatureDesc.Flags				 = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob{ nullptr };
	ID3DBlob* pd3dErrorBlob{ nullptr };

	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	
	if (pd3dSignatureBlob)
		pd3dSignatureBlob->Release();
	
	if (pd3dErrorBlob)
		pd3dErrorBlob->Release();
	
	return pd3dGraphicsRootSignature;
}

void CScene::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice)
{
	// Vertex Shader와 Pixel Shader 생성
	ID3DBlob*	 pd3dVertexShaderBlob{ nullptr };
	ID3DBlob*	 pd3dPixelShaderBlob{ nullptr };
	UINT		 nCompileFlags{ 0 };
#ifdef _DEBUG
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "main", "vs_5_1", nCompileFlags, 0, &pd3dVertexShaderBlob, nullptr);
	D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_1", nCompileFlags, 0, &pd3dPixelShaderBlob, nullptr);

	// Rasterizer 상태 설정
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	d3dRasterizerDesc.FillMode					 = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode					 = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise		 = FALSE;
	d3dRasterizerDesc.DepthBias					 = 0;
	d3dRasterizerDesc.DepthBiasClamp			 = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias		 = 0.0f;
	d3dRasterizerDesc.DepthClipEnable			 = TRUE;
	d3dRasterizerDesc.MultisampleEnable			 = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable		 = FALSE;
	d3dRasterizerDesc.ForcedSampleCount			 = 0;
	d3dRasterizerDesc.ConservativeRaster		 = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// Blend 상태 설정
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

	// Graphics Pipeline 상태 설정
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	d3dPipelineStateDesc.pRootSignature						 = m_pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS.pShaderBytecode					 = pd3dVertexShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.VS.BytecodeLength					 = pd3dVertexShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.PS.pShaderBytecode					 = pd3dPixelShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.PS.BytecodeLength					 = pd3dPixelShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.RasterizerState					 = d3dRasterizerDesc;
	d3dPipelineStateDesc.BlendState							 = d3dBlendDesc;
	d3dPipelineStateDesc.DepthStencilState.DepthEnable		 = FALSE;
	d3dPipelineStateDesc.DepthStencilState.StencilEnable	 = FALSE;
	d3dPipelineStateDesc.InputLayout.pInputElementDescs		 = nullptr;
	d3dPipelineStateDesc.InputLayout.NumElements			 = 0;
	d3dPipelineStateDesc.SampleMask							 = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType				 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets					 = 1;
	d3dPipelineStateDesc.RTVFormats[0]						 = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat							 = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count					 = 1;
	d3dPipelineStateDesc.SampleDesc.Quality					 = 0;

	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

	if (pd3dVertexShaderBlob)
		pd3dVertexShaderBlob->Release();
	
	if (pd3dPixelShaderBlob)
		pd3dPixelShaderBlob->Release();
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// Graphic Root Signature 생성
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_nShaders = 1;
	m_pShaders = new CInstancingShader[m_nShaders];

	m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders[0].BuildObjects(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature->Release();

	for (INT i = 0; i < m_nShaders; ++i)
	{
		m_pShaders[i].ReleaseShaderVariables();
		m_pShaders[i].ReleaseObjects();
	}

	if (m_pShaders)
		delete[] m_pShaders;
}

void CScene::ReleaseUploadBuffers()
{
	for (INT i = 0; i < m_nShaders; ++i)
		m_pShaders[i].ReleaseUploadBuffers();
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return m_pd3dGraphicsRootSignature;
}

BOOL CScene::ProcessInput(UCHAR* pKeysBuffer)
{
	return 0;
}

void CScene::Animate(FLOAT fTimeElapsed)
{
	for (INT i = 0; i < m_nShaders; ++i)
	{
		m_pShaders[i].Animate(fTimeElapsed);
	}
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (pCamera)
		pCamera->UpdateShaderVariables(pd3dCommandList);

	// Scene을 Rendering하는 것은 Scene을 구성하는 Game Object(Shader가 포함하는 객체)들을 Rendering하는 것
	for (INT i = 0; i < m_nShaders; ++i)
	{
		m_pShaders[i].Render(pd3dCommandList, pCamera);
	}
}

BOOL CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

BOOL CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}
