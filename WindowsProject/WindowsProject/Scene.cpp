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
	D3D12_ROOT_PARAMETER	 pd3dRootParameters[2];

	pd3dRootParameters[0].ParameterType					 = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues		 = 16;
	pd3dRootParameters[0].Constants.ShaderRegister		 = 0;
	pd3dRootParameters[0].Constants.RegisterSpace		 = 0;
	pd3dRootParameters[0].ShaderVisibility				 = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType					 = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues		 = 32;
	pd3dRootParameters[1].Constants.ShaderRegister		 = 1;
	pd3dRootParameters[1].Constants.RegisterSpace		 = 0;
	pd3dRootParameters[1].ShaderVisibility				 = D3D12_SHADER_VISIBILITY_VERTEX;

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

	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
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

	CObjectsShader* pShaders;

	m_nShaders = 1 + 1;
	m_vShaders.reserve(m_nShaders);

	pShaders = new CWallShader();
	pShaders->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShaders->BuildObjects(pd3dDevice, pd3dCommandList);
	m_vShaders.push_back(pShaders);

	pShaders = new CApproachingShader();
	pShaders->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShaders->BuildObjects(pd3dDevice, pd3dCommandList);
	m_vShaders.push_back(pShaders);

	CCarMeshDiffused* pCarMesh{ new CCarMeshDiffused(pd3dDevice, pd3dCommandList, 10.0f, 10.0f, 10.0f, XMFLOAT4(0.0f, 255.0f, 0.0f, 0.0f)) };
	m_vPlayerMesh.push_back(pCarMesh);

	pCarMesh = new CCarMeshDiffused(pd3dDevice, pd3dCommandList, 10.0f, 10.0f, 10.0f, XMFLOAT4(255.0f, 0.0, 0.0f, 0.0f));
	m_vPlayerMesh.push_back(pCarMesh);
}

void CScene::RebuildObject()
{
	std::vector<CGameObject*> vpObject;
	vpObject.reserve(m_vShaders[ObjectType::Approaching]->GetObjectsNum());

	for (INT i = 0; i < m_vShaders[ObjectType::Approaching]->GetObjectsNum(); ++i)
	{
		vpObject.push_back(m_vShaders[ObjectType::Approaching]->GetObjects(i));
	}

	for (auto iter = vpObject.begin(); iter != vpObject.end(); ++iter)
	{
		if (IsBehindCamera(*iter))
		{
			CApproachingShader* pApproachingShader{ static_cast<CApproachingShader*>(m_vShaders[ObjectType::Approaching]) };

			pApproachingShader->ReleaseApproachingObject(*iter);
			pApproachingShader->RespawnObjects(m_pPlayer);
		}
	}
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature->Release();

	for (INT i = 0; i < m_nShaders; ++i)
	{
		m_vShaders[i]->ReleaseShaderVariables();
		m_vShaders[i]->ReleaseObjects();
	}

	if (!m_vShaders.empty())
	{
		m_vShaders.clear();
		m_vShaders.shrink_to_fit();
	}
}

void CScene::ReleaseUploadBuffers()
{
	for (auto iter = m_vShaders.begin(); iter != m_vShaders.end(); ++iter)
	{
		(*iter)->ReleaseUploadBuffers();
	}
}

BOOL CScene::IsBehindCamera(CGameObject* pObject)
{
	BOOL	 bIsVisible{ FALSE };
	CCamera* pCamera{ m_pPlayer->GetCamera() };

	if (pObject->GetPosition().z < pCamera->GetPosition().z)
		bIsVisible = TRUE;

	return bIsVisible;
}

void CScene::CheckObjectByObjectCollisions()
{
	// 다가오는 육면체들을 별도의 자료구조에 저장
	std::vector<CGameObject*> pObject;
	pObject.reserve(m_vShaders[ObjectType::Approaching]->GetObjectsNum());

	for (INT i = 0; i < m_vShaders[ObjectType::Approaching]->GetObjectsNum(); ++i)
	{
		pObject.push_back(m_vShaders[ObjectType::Approaching]->GetObjects(i));
	}
	
	for (auto iter = pObject.begin(); iter != pObject.end(); ++iter)
	{
		(*iter)->SetCollidedObject(nullptr);
	}

	for (auto iter1 = pObject.begin(); iter1 != pObject.end(); ++iter1)
	{
		for (auto iter2 = iter1 + 1; iter2 != pObject.end(); ++iter2)
		{
			if ((*iter1)->GetBoundingBox().Intersects((*iter2)->GetBoundingBox()))
			{
				(*iter1)->SetCollidedObject(*iter2);
				(*iter2)->SetCollidedObject(*iter1);
			}
		}
	}

	for (auto iter = pObject.begin(); iter != pObject.end(); ++iter)
	{
		if ((*iter)->GetCollidedObject())
		{
			auto childIter{ static_cast<CApproachingObject*>(*iter) };
			auto childIterCollidedObject{ static_cast<CApproachingObject*>(childIter->GetCollidedObject()) };
			FLOAT fApproachingSpeed{ childIter->GetApproachingSpeed() };

			childIter->SetApproachingSpeed(childIterCollidedObject->GetApproachingSpeed());
			childIterCollidedObject->SetApproachingSpeed(fApproachingSpeed);
			childIterCollidedObject->GetCollidedObject()->SetCollidedObject(nullptr);
			childIter->SetCollidedObject(nullptr);
		}
	}
}

void CScene::CheckPlayerByObjectCollision()
{
	std::vector<CGameObject*> vpObject;
	vpObject.reserve(m_vShaders[ObjectType::Approaching]->GetObjectsNum());

	m_pPlayer->SetCollidedObject(nullptr);

	for (INT i = 0; i < m_vShaders[ObjectType::Approaching]->GetObjectsNum(); ++i)
	{
		vpObject.push_back(m_vShaders[ObjectType::Approaching]->GetObjects(i));
	}

	for (auto iter = vpObject.begin(); iter != vpObject.end(); ++iter)
	{
		if (m_pPlayer->GetBoundingBox().Intersects((*iter)->GetBoundingBox()))
		{
			(*iter)->SetCollidedObject(m_pPlayer);
			m_pPlayer->SetCollidedObject((*iter));

			break;
		}
	}

	if (m_pPlayer->GetCollidedObject())
	{
		static std::chrono::high_resolution_clock::time_point startTime;
		CCarPlayer* pPlayer{ static_cast<CCarPlayer*>(m_pPlayer) };

		if (pPlayer->GetFeverMode())
		{
			CApproachingShader* pApproachingShader{ static_cast<CApproachingShader*>(m_vShaders[ObjectType::Approaching]) };

			pApproachingShader->ReleaseApproachingObject(pPlayer->GetCollidedObject());
			pApproachingShader->RespawnObjects(pPlayer);

			if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - startTime).count() > 10.0f)
			{
				pPlayer->SetFeverMode(FALSE);
				pPlayer->SetFeverStack(0);
				pPlayer->SetMesh(m_vPlayerMesh[PlayerMesh::Normal]);
			}
		}
		else
		{
			if (CFeverObject* temp = dynamic_cast<CFeverObject*>(pPlayer->GetCollidedObject()))
			{
				pPlayer->IncreaseFeverStack();

				CApproachingShader* pApproachingShader{ static_cast<CApproachingShader*>(m_vShaders[ObjectType::Approaching]) };

				pApproachingShader->ReleaseApproachingObject(pPlayer->GetCollidedObject());
				pApproachingShader->RespawnObjects(pPlayer);

				if (pPlayer->GetFeverStack() == 10)
				{
					startTime = std::chrono::high_resolution_clock::now();

					pPlayer->SetFeverMode(TRUE);
					pPlayer->SetMesh(m_vPlayerMesh[PlayerMesh::Fever]);
				}
			}
			else
				m_bGameOver = TRUE;
		}
	}
}

void CScene::CheckPlayerByWallCollision()
{
	std::vector<CGameObject*>		 vpObject;
	std::vector<BoundingOrientedBox> xmBoundingBox(m_vShaders[ObjectType::Wall]->GetObjectsNum());

	vpObject.reserve(m_vShaders[ObjectType::Wall]->GetObjectsNum());

	for (INT i = 0; i < m_vShaders[ObjectType::Wall]->GetObjectsNum(); ++i)
	{
		vpObject.push_back(m_vShaders[ObjectType::Wall]->GetObjects(i));
		vpObject[i]->GetBoundingBox().Transform(xmBoundingBox[i], XMLoadFloat4x4(&vpObject[i]->GetWorldMatrix()));
		XMStoreFloat4(&xmBoundingBox[i].Orientation, XMQuaternionNormalize(XMLoadFloat4(&xmBoundingBox[i].Orientation)));
	}

	for (auto iter = xmBoundingBox.begin(); iter != xmBoundingBox.end(); ++iter)
	{
		if (!(*iter).Intersects(m_pPlayer->GetBoundingBox()))
		{
			FLOAT xPos;
			XMFLOAT3 xmf3PlayerPos{ m_pPlayer->GetPosition() };

			xPos = xmf3PlayerPos.x;

			if (m_pPlayer->GetPosition().x > 80.0f - 5.0f)
				xPos = 80.0f - 5.0f;
			else if (m_pPlayer->GetPosition().x < -80.0f + 5.0f)
				xPos = -80.0f + 5.0f;

			m_pPlayer->SetPosition(XMFLOAT3(xPos, xmf3PlayerPos.y, xmf3PlayerPos.z));
		}
	}
}

BOOL CScene::ProcessInput(UCHAR* pKeysBuffer)
{
	return 0;
}

void CScene::Animate(FLOAT fTimeElapsed, CCamera* pCamera)
{
	for (auto iter = m_vShaders.begin(); iter != m_vShaders.end(); ++iter)
	{
		(*iter)->Animate(fTimeElapsed, pCamera);
	}

	CheckPlayerByObjectCollision();
	CheckObjectByObjectCollisions();
	CheckPlayerByWallCollision();
	RebuildObject();
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (pCamera)
		pCamera->UpdateShaderVariables(pd3dCommandList);

	// Scene을 Rendering하는 것은 Scene을 구성하는 Game Object(Shader가 포함하는 객체)들을 Rendering하는 것
	for (auto i = m_vShaders.begin(); i != m_vShaders.end(); ++i)
	{
		(*i)->Render(pd3dCommandList, pCamera);
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
