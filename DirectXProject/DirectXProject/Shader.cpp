#include "framework.h"
#include "Shader.h"
#include "Player.h"

CShader::CShader()
{
}

CShader::~CShader()
{
	if (m_pd3dPipelineStates)
	{
		//for (INT i = 0; i < m_nPipelineStates; ++i)
		//{
		//	if (m_ppd3dPipelineStates[i])
		//		m_ppd3dPipelineStates[i]->Release();
		//}

		m_pd3dPipelineStates->Release();
		m_pd3dPipelineStates = nullptr;
		//delete[] m_pd3dPipelineStates;
	}

	ReleaseShaderVariables();
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
#ifdef _DEBUG
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

	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineStates);

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
	if (m_pd3dPipelineStates)
		pd3dCommandList->SetPipelineState(m_pd3dPipelineStates);
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
	//m_pd3dPipelineStates	 = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

//=============================================================================================================

CObjectsShader::CObjectsShader()
{
}

CObjectsShader::~CObjectsShader()
{
}

void CObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	CHeightMapTerrain*	 pTerrain{ static_cast<CHeightMapTerrain*>(pContext) };
	FLOAT				 fTerrainWidth{ pTerrain->GetWidth() };
	FLOAT				 fTerrainLength{ pTerrain->GetLength() };

	FLOAT				 fxPitch{ 12.0f * 3.5f};
	FLOAT				 fyPitch{ 12.0f * 3.5f};
	FLOAT				 fzPitch{ 12.0f * 3.5f};

	INT					 xObjects{ static_cast<INT>(fTerrainWidth / fxPitch) };
	INT					 yObjects{ 2 };
	INT					 zObjects{ static_cast<INT>(fTerrainLength / fzPitch) };

	m_nObjects			 = xObjects * yObjects * zObjects;
	m_vpObjects.reserve(m_nObjects);

	CCubeMeshDiffused*	 pCubeMesh{ new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f) };

	XMFLOAT3			 xmf3RotateAxis;
	XMFLOAT3			 xmf3SurfaceNormal;

	CRotatingObject*	 pRotatingObject{ nullptr };

	for (INT i = 0, x = 0; x < xObjects; ++x)
	{
		for (INT z = 0; z < zObjects; ++z)
		{
			for (INT y = 0; y < yObjects; ++y)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh);

				FLOAT xPosition{ x * fxPitch };
				FLOAT zPosition{ z * fzPitch };
				FLOAT fHeight{ pTerrain->GetHeight(xPosition, zPosition) };

				pRotatingObject->SetPosition(xPosition, fHeight + (y * 10.0f * fyPitch) + 6.0f, zPosition);

				if (y == 0)
				{
					/*지형의 표면에 위치하는 직육면체는 지형의 기울기에 따라 방향이 다르게 배치한다.
					직육면체가 위치할 지형의 법선 벡터 방향과 직육면체의 y축이 일치하도록 한다.*/
					xmf3SurfaceNormal	 = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis		 = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);

					if (Vector3::IsZero(xmf3RotateAxis))
						xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);

					FLOAT fAngle{ acos(Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal)) };
					
					pRotatingObject->Rotate(&xmf3RotateAxis, XMConvertToDegrees(fAngle));
				}

				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(36.0f * (i++ % 10) + 36.0f);

				m_vpObjects.push_back(pRotatingObject);
			}
		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
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
	//m_ppd3dPipelineStates	 = new ID3D12PipelineState * [m_nPipelineStates];

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

CPseudoLightingShader::CPseudoLightingShader()
{
}

CPseudoLightingShader::~CPseudoLightingShader()
{
}

#define _WITH_BINARY_MODEL_FILE

void CPseudoLightingShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	m_nObjects = 5;
	m_vpObjects.reserve(m_nObjects);

#ifdef _WITH_TEXT_MODEL_FILE
	CMesh* pUfoMesh{ new CMesh(pd3dDevice, pd3dCommandList, "Models/UFO.txt", true) };
	CMesh* pFlyerMesh{ new CMesh(pd3dDevice, pd3dCommandList, "Models/FlyerPlayership.txt", true) };
#endif
#ifdef _WITH_BINARY_MODEL_FILE
	CMesh* pUfoMesh{ new CMesh(pd3dDevice, pd3dCommandList, "Models/UFO.bin", false) };
	CMesh* pFlyerMesh{ new CMesh(pd3dDevice, pd3dCommandList, "Models/FlyerPlayership.bin", false) };
#endif

	CUFOObject* pUFOObject{ nullptr };
	
	pUFOObject = new CUFOObject();
	pUFOObject->SetMesh(pUfoMesh);
	pUFOObject->SetPosition(6.0f, 0.0f, 13.0f);
	pUFOObject->SetColor(XMFLOAT3(0.7f, 0.0f, 0.0f));
	//pUFOObject->SetBoundingBox(pUfoMesh->GetBoundingBox());
	m_vpObjects.push_back(pUFOObject);

	pUFOObject = new CUFOObject();
	pUFOObject->SetMesh(pUfoMesh);
	pUFOObject->SetPosition(10.0f, 2.0f, 8.0f);
	pUFOObject->SetColor(XMFLOAT3(0.0f, 0.7f, 0.0f));
	m_vpObjects.push_back(pUFOObject);

	pUFOObject = new CUFOObject();
	pUFOObject->SetMesh(pUfoMesh);
	pUFOObject->SetPosition(5.0f, 4.0f, 11.0f);
	pUFOObject->SetColor(XMFLOAT3(0.0f, 0.7f, 0.0f));
	m_vpObjects.push_back(pUFOObject);

	pUFOObject = new CUFOObject();
	pUFOObject->SetMesh(pUfoMesh);
	pUFOObject->SetPosition(10.0f, 2.0f, 8.0f);
	pUFOObject->SetColor(XMFLOAT3(0.0f, 0.0f, 0.7f));
	m_vpObjects.push_back(pUFOObject);

	pUFOObject = new CUFOObject();
	pUFOObject->SetMesh(pUfoMesh);
	pUFOObject->SetPosition(0.0f, 4.0f, 20.0f);
	pUFOObject->Rotate(0.0f, 180.0f, 0.0f);
	pUFOObject->SetColor(XMFLOAT3(0.25f, 0.75f, 0.65f));
	m_vpObjects.push_back(pUFOObject);
}

void CPseudoLightingShader::ReleaseObjects()
{
}

D3D12_INPUT_LAYOUT_DESC CPseudoLightingShader::CreateInputLayout()
{
	UINT						 nInputElementDescs{ 3 };
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs] };

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXTURECOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;

	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return d3dInputLayoutDesc;
}

D3D12_SHADER_BYTECODE CPseudoLightingShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return CShader::CompileShaderFromFile(L"VertexShader.hlsl", "PseudoLighting", "vs_5_1", ppd3dShaderBlob);
}

D3D12_SHADER_BYTECODE CPseudoLightingShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return CShader::CompileShaderFromFile(L"PixelShader.hlsl", "PseudoLighting", "ps_5_1", ppd3dShaderBlob);
}

//=============================================================================================================

CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return CShader::CompileShaderFromFile(L"VertexShader.hlsl", "Diffused", "vs_5_1", ppd3dShaderBlob);
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return CShader::CompileShaderFromFile(L"PixelShader.hlsl", "Diffused", "ps_5_1", ppd3dShaderBlob);
}

void CTerrainShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates		 = 1;
	//m_ppd3dPipelineStates	 = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}
