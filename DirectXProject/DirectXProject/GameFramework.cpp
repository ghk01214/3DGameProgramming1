#include "framework.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	m_pdxgiFactory					 = nullptr;
	m_pdxgiSwapChain				 = nullptr;
	m_pd3dDevice					 = nullptr;
	m_pd3dCommandAllocator			 = nullptr;
	m_pd3dCommandQueue				 = nullptr;
	m_pd3dPipelineState				 = nullptr;
	m_pd3dCommandList				 = nullptr;

	m_vpd3dSwapChainBackBuffers.reserve(m_nSwapChainBuffers);

	for (INT i = 0; i < m_nSwapChainBuffers; ++i)
	{
		m_vpd3dSwapChainBackBuffers.push_back(nullptr);
	}

	m_pd3dRtvDescriptorHeap			 = nullptr;
	m_nRtvDescriptorIncrementSize	 = 0;
	m_pd3dDepthStencilBuffer		 = nullptr;
	m_pd3dDsvDescriptorHeap			 = nullptr;
	m_nDsvDescriptorIncrementSize	 = 0;
	m_nSwapChainBufferIndex			 = 0;
	m_hFenceEvent					 = nullptr;
	m_pd3dFence						 = nullptr;
	
	m_nFenceValue.reserve(m_nSwapChainBuffers);

	for (INT i = 0; i < m_nSwapChainBuffers; ++i)
	{
		m_nFenceValue.push_back(0);
	}
	
	m_pScene						 = nullptr;

	m_nWndClientWidth				 = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight				 = FRAME_BUFFER_HEIGHT;

	//m_pszFrameRate = L"DirectXProject (";
	_tcscpy_s(m_pszFrameRate, _T("DirectXProject ("));
}

CGameFramework::~CGameFramework()
{
}

BOOL CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance		 = hInstance;
	m_hWnd			 = hMainWnd;

	// Direct3D Device, 명령 Queue와 명령 List, Swap Chain 등을 생성하는 함수 호출
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateDepthStencilView();

	// Rendering할 게임 객체를 생성한다.
	BuildObjects();
	
	return TRUE;
}

void CGameFramework::OnDestroy()
{
	WaitForGpuComplete();				// GPU가 모든 명령 List를 실행할 때까지 대기
	ReleaseObjects();					// 게임 객체(게임 월드 객체) 소멸
	::CloseHandle(m_hFenceEvent);

	for (auto buffer = m_vpd3dSwapChainBackBuffers.cbegin(); buffer != m_vpd3dSwapChainBackBuffers.cend(); ++buffer)
	{
		if (*buffer)
			(*buffer)->Release();
	}

	if (m_pd3dRtvDescriptorHeap)
		m_pd3dRtvDescriptorHeap->Release();
	
	if (m_pd3dDepthStencilBuffer)
		m_pd3dDepthStencilBuffer->Release();
	
	if (m_pd3dDsvDescriptorHeap)
		m_pd3dDsvDescriptorHeap->Release();
	
	if (m_pd3dCommandAllocator)
		m_pd3dCommandAllocator->Release();
	
	if (m_pd3dCommandQueue)
		m_pd3dCommandQueue->Release();
	
	if (m_pd3dPipelineState)
		m_pd3dPipelineState->Release();
	
	if (m_pd3dCommandList)
		m_pd3dCommandList->Release();
	
	if (m_pd3dFence)
		m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, nullptr);

	if (m_pdxgiSwapChain)
		m_pdxgiSwapChain->Release();
	
	if (m_pd3dDevice)
		m_pd3dDevice->Release();
	
	if (m_pdxgiFactory)
		m_pdxgiFactory->Release();

#ifdef _DEBUG
	IDXGIDebug1* pdxgiDebug{ nullptr };

	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), reinterpret_cast<void**>(&pdxgiDebug));

	HRESULT hResult{ pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL) };

	pdxgiDebug->Release();
#endif
}

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;

	::GetClientRect(m_hWnd, &rcClient);

	m_nWndClientWidth									 = rcClient.right - rcClient.left;
	m_nWndClientHeight									 = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));

	dxgiSwapChainDesc.BufferDesc.Width					 = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height					 = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format					 = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator	 = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.SampleDesc.Count					 = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality				 = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage						 = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount						 = m_nSwapChainBuffers;
	dxgiSwapChainDesc.OutputWindow						 = m_hWnd;
	dxgiSwapChainDesc.SwapEffect						 = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.Windowed							 = TRUE;
	// 전체화면 모드에서 바탕화면의 해상도를 Swap Chain(후면 버퍼)의 크기에 맞게 변경
	dxgiSwapChainDesc.Flags								 = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hResult{ m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, reinterpret_cast<IDXGISwapChain**>(&m_pdxgiSwapChain)) };
	
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	
#ifndef  _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetViews();
#endif // ! _WITH_SWAPCHAIN_FULLSCREEN_STATE
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	
	d3dDescriptorHeapDesc.NumDescriptors		 = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type					 = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags					 = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask				 = 0;

	// Render Target 서술자 Heap(서술자의 개수는 Swap Chain 버퍼의 개수) 생성
	HRESULT hResult{ m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&m_pd3dRtvDescriptorHeap)) };

	// Render Target 서술자 Heap의 원소의 크기를 저장한다.
	m_nRtvDescriptorIncrementSize				 =	m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	
	// Depth-Stencil 서술자 Heap(서술자의 개수는 1) 생성
	d3dDescriptorHeapDesc.NumDescriptors		 = 1;
	d3dDescriptorHeapDesc.Type					 = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult										 = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&m_pd3dDsvDescriptorHeap));

	// Depth-Stencil 서술자 Heap의 원소의 크기 저장
	m_nDsvDescriptorIncrementSize =	m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CGameFramework::CreateDirect3DDevice()
{
	HRESULT		 hResult;
	UINT		 nDXGIFactoryFlags{ 0 };
#ifdef _DEBUG
	ID3D12Debug* pd3dDebugController{ nullptr };
	
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), reinterpret_cast<void**>(&pd3dDebugController));

	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}

	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), reinterpret_cast<void**>(&m_pdxgiFactory));

	IDXGIAdapter1* pd3dAdapter{ nullptr };

	// 모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 Device 생성
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); ++i)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;

		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);

		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0,_uuidof(ID3D12Device), reinterpret_cast<void**>(&m_pd3dDevice))))
			break;
	}

	// 특성 레벨 12.0을 지원하는 하드웨어 Device를 생성할 수 없으면 WARP Device 생성
	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), reinterpret_cast<void**>(&pd3dAdapter));
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), reinterpret_cast<void**>(&m_pd3dDevice));
	}

	// Device가 지원하는 다중 샘플의 품질 수준 확인
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;

	d3dMsaaQualityLevels.Format				 = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount		 = 4; // Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags				 = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels	 = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,	&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels					 = d3dMsaaQualityLevels.NumQualityLevels;
	
	// 다중 샘플의 품질 수준이 1보다 크면 다중 샘플링 활성화
	m_bMsaa4xEnable							 = (m_nMsaa4xQualityLevels > 1) ? TRUE : FALSE;
	
	// Fence를 생성하고 Fence 값을 0으로 설정
	hResult									 = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), reinterpret_cast<void**>(&m_pd3dFence));
	
	// Fence와 동기화를 위한 이벤트 객체를 생성(이벤트 객체의 초기값 = FALSE)
	// 이벤트가 실행되면(Signal) 이벤트의 값을 자동적으로 FALSE가 되도록 생성
	m_hFenceEvent							 = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
	
	if (pd3dAdapter)
		pd3dAdapter->Release();

	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));

	d3dCommandQueueDesc.Flags		 = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type		 = D3D12_COMMAND_LIST_TYPE_DIRECT;
	
	// 직접(Direct) 명령 Queue 생성
	HRESULT hResult{ m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc,	_uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(&m_pd3dCommandQueue)) };
	
	// 직접(Direct) 명령 할당자 생성
	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(&m_pd3dCommandAllocator));
	
	// 직접(Direct) 명령 List 생성
	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, nullptr, __uuidof(ID3D12GraphicsCommandList), reinterpret_cast<void**>(&m_pd3dCommandList));
	
	// 명령 List는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 변경
	hResult = m_pd3dCommandList->Close();
}

// Swap Chain의 각 후면 버퍼에 대한 Render Target View 생성
void CGameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle{ m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	for (auto iter = m_vpd3dSwapChainBackBuffers.begin(); iter != m_vpd3dSwapChainBackBuffers.end(); ++iter)
	{
		m_pdxgiSwapChain->GetBuffer(iter - m_vpd3dSwapChainBackBuffers.begin(), __uuidof(ID3D12Resource), reinterpret_cast<void**>(&(*iter)));
		m_pd3dDevice->CreateRenderTargetView(*iter, nullptr, d3dRtvCPUDescriptorHandle);

		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;

	d3dResourceDesc.Dimension				 = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment				 = 0;
	d3dResourceDesc.Width					 = m_nWndClientWidth;
	d3dResourceDesc.Height					 = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize		 = 1;
	d3dResourceDesc.MipLevels				 = 1;
	d3dResourceDesc.Format					 = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count		 = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality		 = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1)	: 0;
	d3dResourceDesc.Layout					 = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags					 = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));

	d3dHeapProperties.Type					 = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty		 = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference	 = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask		 = 1;
	d3dHeapProperties.VisibleNodeMask		 = 1;

	D3D12_CLEAR_VALUE d3dClearValue;

	d3dClearValue.Format					 = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth		 = 1.0f;
	d3dClearValue.DepthStencil.Stencil		 = 0;

	// Depth-Stencil 버퍼 생성
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,	&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,	__uuidof(ID3D12Resource), reinterpret_cast<void**>(&m_pd3dDepthStencilBuffer));
	
	// Depth-Stencil 버퍼 뷰 생성
	D3D12_CPU_DESCRIPTOR_HANDLE		 d3dDsvCPUDescriptorHandle{ m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	D3D12_DEPTH_STENCIL_VIEW_DESC	 d3dDepthStencilViewDesc;
	::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));

	d3dDepthStencilViewDesc.Format			 = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilViewDesc.ViewDimension	 = D3D12_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Flags			 = D3D12_DSV_FLAG_NONE;

	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, d3dDsvCPUDescriptorHandle);
}

void CGameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL bFullScreenState{ FALSE };

	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, nullptr);
	m_pdxgiSwapChain->SetFullscreenState(bFullScreenState, nullptr);

	DXGI_MODE_DESC dxgiTargetParameters;

	dxgiTargetParameters.Format						 = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width						 = m_nWndClientWidth;
	dxgiTargetParameters.Height						 = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator		 = 60;
	dxgiTargetParameters.RefreshRate.Denominator	 = 1;
	dxgiTargetParameters.Scaling					 = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering			 = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (auto buffer = m_vpd3dSwapChainBackBuffers.begin(); buffer != m_vpd3dSwapChainBackBuffers.end(); ++buffer)
	{
		if (*buffer)
			(*buffer)->Release();
	}

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;

	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth,	m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_nSwapChainBufferIndex							 = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	UINT64	 nFence{ ++m_nFenceValue[m_nSwapChainBufferIndex] };
	HRESULT	 hResult{ m_pd3dCommandQueue->Signal(m_pd3dFence, nFence) };

	if (m_pd3dFence->GetCompletedValue() < nFence)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFence, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, nullptr);

	// Scene 객체를 생성하고 Scene에 포함될 Game Object 생성
	m_pScene = new CScene();
	m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	CTerrainPlayer* pTerrainPlayer{ new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->GetTerrain(), 1) };
	m_pPlayer = pTerrainPlayer;
	m_pCamera = m_pPlayer->GetCamera();
	m_pScene->SetPlayer(m_pPlayer);

	// Scene 객체를 생성하기 위하여 필요한 Graphic 명령 List들을 명령 Queue에 추가
	m_pd3dCommandList->Close();

	std::vector<ID3D12CommandList*> ppd3dCommandLists{ m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists.data());

	// Graphic 명령 List들이 모두 실행될 때까지 대기
	WaitForGpuComplete();
	
	// Graphic 리소스들을 생성하는 과정에 생성된 업로드 버퍼들 소멸
	if (m_pScene)
		m_pScene->ReleaseUploadBuffers();

	if (m_pPlayer)
		m_pPlayer->ReleaseUploadBuffers();

	m_GameTimer.Reset();
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->ReleaseObjects();
		
		delete m_pScene;
	}

	if (m_pPlayer)
		m_pPlayer->Release();
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	DWORD		 dwDirection{ 0 };

	/*키보드의 상태 정보를 반환한다. 화살표 키를 누르면 플레이어를 오른쪽/왼쪽(로컬 x축), 앞/뒤(로컬 z축)로 이동.
	'Page Up'과 'Page Down' 키를 누르면 플레이어를 위/아래(로컬 y축)로 이동.*/
	if (::GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer[VK_UP] & 0xF0)
			dwDirection |= DIR_FORWARD;

		if (pKeyBuffer[VK_DOWN] & 0xF0)
			dwDirection |= DIR_BACKWARD;

		if (pKeyBuffer[VK_LEFT] & 0xF0)
			dwDirection |= DIR_LEFT;

		if (pKeyBuffer[VK_RIGHT] & 0xF0)
			dwDirection |= DIR_RIGHT;

		if (pKeyBuffer[VK_PRIOR] & 0xF0)
			dwDirection |= DIR_UP;

		if (pKeyBuffer[VK_NEXT] & 0xF0)
			dwDirection |= DIR_DOWN;
	}

	FLOAT cxDelta{ 0.0f }, cyDelta{ 0.0f };
	POINT ptCursorPos;

	if (::GetCapture() == m_hWnd)
	{
		// 마우스 커서를 화면에서 없앤다(보이지 않게 한다)
		::SetCursor(nullptr);

		// 현재 마우스 커서의 위치를 가져온다
		::GetCursorPos(&ptCursorPos);

		// 마우스 버튼이 눌린 상태에서 마우스가 움직인 양을 구한다.
		cxDelta = (FLOAT)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (FLOAT)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;

		// 마우스 커서의 위치를 마우스가 눌려졌던 위치로 설정한다.
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}

	// 마우스 또는 키 입력이 있으면 플레이어를 이동하거나(dwDirection) 회전(cxDelta 또는 cyDelta).
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			/*cxDelta는 y축의 회전을 나타내고 cyDelta는 x축의 회전을 나타낸다.
			오른쪽 마우스 버튼이 눌려진 경우 cxDelta는 z축의 회전을 나타낸다.*/
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}

		/*플레이어를 dwDirection 방향으로 이동(실제로는 속도 벡터 변경).
		이동 거리는 시간에 비례하도록 한다. 플레이어의 이동 속력은 (50/초)로 가정.*/
		if (dwDirection)
			m_pPlayer->Move(dwDirection, 5000.0f * m_GameTimer.GetTimeElapsed(), TRUE);
	}

	// 플레이어를 실제로 이동하고 카메라를 갱신. 중력과 마찰력의 영향을 속도 벡터에 적용.
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::Animate()
{
	if (m_pPlayer)
		m_pPlayer->Animate(m_GameTimer.GetTimeElapsed());

	if (m_pScene)
		m_pScene->Animate(m_GameTimer.GetTimeElapsed(), m_pCamera);
}

void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick(0.0f);

	ProcessInput();
	Animate();

	HRESULT hResult{ m_pd3dCommandAllocator->Reset() };
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, nullptr);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));

	d3dResourceBarrier.Type							 = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags						 = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource			 = m_vpd3dSwapChainBackBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore		 = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter		 = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource		 = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle{ m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex *	m_nRtvDescriptorIncrementSize);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle{ m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);
	
	std::vector<FLOAT> vfClearColor{ 0.0f, 0.125f, 0.3f, 1.0f };

	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, &(*(vfClearColor.data()))/*Colors::Azure*/, 0, nullptr);
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	if (m_pScene)
		m_pScene->Render(m_pd3dCommandList, m_pCamera);

#ifdef _WITH_PLAYER_TOP
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
#endif

	if (m_pPlayer)
		m_pPlayer->Render(m_pd3dCommandList, m_pCamera);

	d3dResourceBarrier.Transition.StateBefore			 = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter			 = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource			 = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	hResult = m_pd3dCommandList->Close();

	std::vector<ID3D12CommandList*> ppd3dCommandLists{ m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(ppd3dCommandLists.size(), ppd3dCommandLists.data());
	
	WaitForGpuComplete();

	m_pdxgiSwapChain->Present(0, 0);

	MoveToNextFrame();
	
	m_GameTimer.GetFrameRate(m_pszFrameRate + 16, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void CGameFramework::WaitForGpuComplete()
{
	// GPU가 Fence의 값을 설정하는 명령을 명령 Queue에 추가한다.
	UINT64	nFence{ ++m_nFenceValue[m_nSwapChainBufferIndex] };
	HRESULT	hResult{ m_pd3dCommandQueue->Signal(m_pd3dFence, nFence) };

	if (m_pd3dFence->GetCompletedValue() < nFence)
	{
		// Fence의 현재 값이 설정한 값보다 작으면 Fence의 현재 값이 설정한 값이 될 때까지 기다린다.
		hResult = m_pd3dFence->SetEventOnCompletion(nFence, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_F1:
		case VK_F2:
		case VK_F3:
		{
			if (m_pPlayer)
				m_pCamera = m_pPlayer->ChangeCamera((wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());

			break;
		}
		case VK_ESCAPE:
		{
			::PostQuitMessage(0);
			break;
		}
		case VK_RETURN:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		m_nWndClientWidth	 = LOWORD(lParam);
		m_nWndClientHeight	 = HIWORD(lParam);

		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	{
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	}

	return 0;
}