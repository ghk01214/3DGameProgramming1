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

	m_ppd3dRenderTargetBuffers.reserve(m_nSwapChainBuffers);

	for (INT i = 0; i < m_nSwapChainBuffers; ++i)
	{
		m_ppd3dRenderTargetBuffers.push_back(nullptr);
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
	
	m_pScene = nullptr;

	m_nWndClientWidth				 = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight				 = FRAME_BUFFER_HEIGHT;

	_tcscpy_s(m_pszFrameRate, _T("LabProject14 ("));
}

CGameFramework::~CGameFramework()
{
}

BOOL CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance		 = hInstance;
	m_hWnd			 = hMainWnd;

	// Direct3D Device, ���� Queue�� ���� List, Swap Chain ���� �����ϴ� �Լ� ȣ��
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateDepthStencilView();

	// Rendering�� ���� ��ü�� �����Ѵ�.
	BuildObjects();
	
	return TRUE;
}

void CGameFramework::OnDestroy()
{
	WaitForGpuComplete();				// GPU�� ��� ���� List�� ������ ������ ���
	ReleaseObjects();					// ���� ��ü(���� ���� ��ü) �Ҹ�
	::CloseHandle(m_hFenceEvent);

	for (auto buffer = m_ppd3dRenderTargetBuffers.cbegin(); buffer != m_ppd3dRenderTargetBuffers.cend(); ++buffer)
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

	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);

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
	// ��üȭ�� ��忡�� ����ȭ���� �ػ󵵸� Swap Chain(�ĸ� ����)�� ũ�⿡ �°� ����
	dxgiSwapChainDesc.Flags								 = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hResult{ m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain) };
	
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

	// Render Target ������ Heap(�������� ������ Swap Chain ������ ����) ����
	HRESULT hResult{ m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap) };

	// Render Target ������ Heap�� ������ ũ�⸦ �����Ѵ�.
	m_nRtvDescriptorIncrementSize				 =	m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	
	// Depth-Stencil ������ Heap(�������� ������ 1) ����
	d3dDescriptorHeapDesc.NumDescriptors		 = 1;
	d3dDescriptorHeapDesc.Type					 = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult										 = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);

	// Depth-Stencil ������ Heap�� ������ ũ�� ����
	m_nDsvDescriptorIncrementSize =	m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CGameFramework::CreateDirect3DDevice()
{
	HRESULT		 hResult;
	UINT		 nDXGIFactoryFlags{ 0 };
#ifdef _DEBUG
	ID3D12Debug* pd3dDebugController{ nullptr };
	
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&pd3dDebugController);

	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}

	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)&m_pdxgiFactory);

	IDXGIAdapter1* pd3dAdapter{ nullptr };

	// ��� �ϵ���� ����� ���Ͽ� Ư�� ���� 12.0�� �����ϴ� �ϵ���� Device ����
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i,&pd3dAdapter); ++i)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;

		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);

		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0,_uuidof(ID3D12Device), (void**)&m_pd3dDevice)))
			break;
	}

	// Ư�� ���� 12.0�� �����ϴ� �ϵ���� Device�� ������ �� ������ WARP Device ����
	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void**)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}

	// Device�� �����ϴ� ���� ������ ǰ�� ���� Ȯ��
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;

	d3dMsaaQualityLevels.Format				 = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount		 = 4; // Msaa4x ���� ���ø�
	d3dMsaaQualityLevels.Flags				 = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels	 = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,	&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels					 = d3dMsaaQualityLevels.NumQualityLevels;
	
	// ���� ������ ǰ�� ������ 1���� ũ�� ���� ���ø� Ȱ��ȭ
	m_bMsaa4xEnable							 = (m_nMsaa4xQualityLevels > 1) ? TRUE : FALSE;
	
	// Fence�� �����ϰ� Fence ���� 0���� ����
	hResult									 = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
	
	// Fence�� ����ȭ�� ���� �̺�Ʈ ��ü�� ����(�̺�Ʈ ��ü�� �ʱⰪ = FALSE)
	// �̺�Ʈ�� ����Ǹ�(Signal) �̺�Ʈ�� ���� �ڵ������� FALSE�� �ǵ��� ����
	m_hFenceEvent							 = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
	
	if (pd3dAdapter)
		pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));

	d3dCommandQueueDesc.Flags		 = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type		 = D3D12_COMMAND_LIST_TYPE_DIRECT;
	
	// ����(Direct) ���� Queue ����
	HRESULT hResult{ m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc,	_uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue) };
	
	// ����(Direct) ���� �Ҵ��� ����
	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);
	
	// ����(Direct) ���� List ����
	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, nullptr, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);
	
	// ���� List�� �����Ǹ� ����(Open) �����̹Ƿ� ����(Closed) ���·� ����
	hResult = m_pd3dCommandList->Close();
}

// Swap Chain�� �� �ĸ� ���ۿ� ���� Render Target View ����
void CGameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle{ m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	for (UINT i = 0; i < m_nSwapChainBuffers; ++i)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_ppd3dRenderTargetBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], nullptr,d3dRtvCPUDescriptorHandle);

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

	// Depth-Stencil ���� ����
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,	&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,	__uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);
	
	// Depth-Stencil ���� �� ����
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle{ m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, nullptr,	d3dDsvCPUDescriptorHandle);
}

void CGameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL bFullScreenState{ FALSE };

	//m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, nullptr);
	//m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, nullptr);

	DXGI_MODE_DESC dxgiTargetParameters;

	dxgiTargetParameters.Format						 = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width						 = m_nWndClientWidth;
	dxgiTargetParameters.Height						 = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator		 = 60;
	dxgiTargetParameters.RefreshRate.Denominator	 = 1;
	dxgiTargetParameters.Scaling					 = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering			 = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (auto buffer = m_ppd3dRenderTargetBuffers.cbegin(); buffer != m_ppd3dRenderTargetBuffers.cend(); ++buffer)
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

	// Scene ��ü�� �����ϰ� Scene�� ���Ե� Game Object ����
	m_pScene = new CScene();

	if (m_pScene)
		m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	m_pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->GetTerrain(), 1);
	m_pCamera = m_pPlayer->GetCamera();

	// Scene ��ü�� �����ϱ� ���Ͽ� �ʿ��� Graphic ���� List���� ���� Queue�� �߰�
	m_pd3dCommandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	// Graphic ���� List���� ��� ����� ������ ���
	WaitForGpuComplete();
	
	// Graphic ���ҽ����� �����ϴ� ������ ������ ���ε� ���۵� �Ҹ�
	if (m_pScene)
		m_pScene->ReleaseUploadBuffers();

	m_GameTimer.Reset();
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene)
		(*m_pScene).ReleaseObjects();
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	DWORD		 dwDirection{ 0 };

	/*Ű������ ���� ������ ��ȯ�Ѵ�. ȭ��ǥ Ű�� ������ �÷��̾ ������/����(���� x��), ��/��(���� z��)�� �̵�.
	'Page Up'�� 'Page Down' Ű�� ������ �÷��̾ ��/�Ʒ�(���� y��)�� �̵�.*/
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

	/*���콺�� ĸ�������� ���콺�� �󸶸�ŭ �̵��Ͽ��� ���� ���. ���콺 ���� �Ǵ� ������ ��ư�� ������ ����
	�޽���(WM_LBUTTONDOWN, WM_RBUTTONDOWN)�� ó���� �� ���콺�� ĸ��.
	�׷��Ƿ� ���콺�� ĸ�ĵ� ���� ���콺 ��ư�� ������ ���¸� �ǹ�.
	���콺 ��ư�� ������ ���¿��� ���콺�� �¿� �Ǵ� ���Ϸ� �����̸� �÷��̾ x�� �Ǵ� y������ ȸ��.*/
	if (::GetCapture() == m_hWnd)
	{
		// ���콺 Ŀ���� ȭ�鿡�� ���ش�(������ �ʰ� �Ѵ�)
		::SetCursor(nullptr);

		// ���� ���콺 Ŀ���� ��ġ�� �����´�
		::GetCursorPos(&ptCursorPos);

		// ���콺 ��ư�� ���� ���¿��� ���콺�� ������ ���� ���Ѵ�.
		cxDelta = (FLOAT)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (FLOAT)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;

		// ���콺 Ŀ���� ��ġ�� ���콺�� �������� ��ġ�� �����Ѵ�.
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}

	// ���콺 �Ǵ� Ű �Է��� ������ �÷��̾ �̵��ϰų�(dwDirection) ȸ��(cxDelta �Ǵ� cyDelta).
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			/*cxDelta�� y���� ȸ���� ��Ÿ���� cyDelta�� x���� ȸ���� ��Ÿ����.
			������ ���콺 ��ư�� ������ ��� cxDelta�� z���� ȸ���� ��Ÿ����.*/
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}

		/*�÷��̾ dwDirection �������� �̵�(�����δ� �ӵ� ���� ����).
		�̵� �Ÿ��� �ð��� ����ϵ��� �Ѵ�. �÷��̾��� �̵� �ӷ��� (50/��)�� ����.*/
		if (dwDirection)
			m_pPlayer->Move(dwDirection, 50.0f * m_GameTimer.GetTimeElapsed(), TRUE);
	}

	// �÷��̾ ������ �̵��ϰ� ī�޶� ����. �߷°� �������� ������ �ӵ� ���Ϳ� ����.
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::Animate()
{
	if (m_pScene)
		(*m_pScene).Animate(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::FrameAdvance()
{
	// Ÿ�̸��� �ð��� ���ŵǵ��� �ϰ� Frame Rate�� ���
	m_GameTimer.Tick(0.0f);

	ProcessInput();
	Animate();

	// ���� �Ҵ��ڿ� ���� List ����
	HRESULT hResult{ m_pd3dCommandAllocator->Reset() };
	hResult											 = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, nullptr);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));

	d3dResourceBarrier.Type							 = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags						 = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource			 = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore		 = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter		 = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource		 = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	/*���� Render Target�� ���� ������Ʈ�� �����⸦ ��ٸ���. ������Ʈ�� ������ Render Target ������ ���´� ������Ʈ ����
	(D3D12_RESOURCE_STATE_PRESENT)���� Render Target ����(D3D12_RESOURCE_STATE_RENDER_TARGET)�� �ٲ� ���̴�.*/
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle{ m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	// ������ Render Target�� �ش��ϴ� �������� CPU �ּ�(�ڵ�) ���
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex *	m_nRtvDescriptorIncrementSize);

	// Depth-Stencil �������� CPU �ּҸ� ����Ѵ�.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle{ m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	
	// Render Target View(������)�� Depth-Stencil View(������)�� ���-���� �ܰ�(OM)�� ����
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);
	
	FLOAT pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	// ���ϴ� �������� Render Target(��)�� �����.
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor/*Colors::Azure*/, 0, nullptr);

	// ���ϴ� ������ Depth-Stencil(��)�� �����.
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Rendering �ڵ�� ���⿡ �߰��� ���̴�.
	if (m_pScene)
		m_pScene->Render(m_pd3dCommandList, m_pCamera);

	//3��Ī ī�޶��� �� �÷��̾ �׻� ���̵��� �������Ѵ�.
#ifdef _WITH_PLAYER_TOP
	//���� Ÿ���� �״�� �ΰ� ���� ���۸� 1.0���� ����� �÷��̾ �������ϸ� �÷��̾�� ������ �׷��� ���̴�.
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
#endif
	//3��Ī ī�޶��� �� �÷��̾ �������Ѵ�.
	if (m_pPlayer)
		m_pPlayer->Render(m_pd3dCommandList, m_pCamera);

	/*���� Render Target�� ���� Rendering�� �����⸦ ��ٸ���. GPU�� Render Target(����)�� �� �̻� ������� ������ Render Target��
	���´� ������Ʈ ����(D3D12_RESOURCE_STATE_PRESENT)�� �ٲ� ���̴�.*/
	d3dResourceBarrier.Transition.StateBefore			 = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter			 = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource			 = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	// ���� List�� ���� ���·� ����
	hResult = m_pd3dCommandList->Close();

	// ���� List�� ���� Queue�� �߰��Ͽ� ����
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);
	
	// GPU�� ��� ���� List�� ������ �� ���� ��ٸ���.
	WaitForGpuComplete();
	
	// Swap Chain�� ������Ʈ�Ѵ�. ������Ʈ�� �ϸ� ���� Render Target(�ĸ����)�� ������ ������۷� �Ű����� Render Target �ε����� �ٲ� ���̴�.
	//m_pdxgiSwapChain->Present(1, 0, &dxgiPresentParameters);
	m_pdxgiSwapChain->Present(0, 0);

	/*������ ������ ����Ʈ�� ���ڿ��� �����ͼ� �� �������� Ÿ��Ʋ�� ����Ѵ�. m_pszBuffer ���ڿ���
	"LapProject ("���� �ʱ�ȭ�Ǿ����Ƿ� (m_pszFrameRate+12)�������� ������ ����Ʈ�� ���ڿ��� ���
	�Ͽ� �� FPS)�� ���ڿ��� ��ģ��.
	::_itow_s(m_nCurrentFrameRate, (m_pszFrameRate+12), 37, 10);
	::wcscat_s((m_pszFrameRate+12), 37, _T(" FPS)"));*/

	MoveToNextFrame();
	
	m_GameTimer.GetFrameRate(m_pszFrameRate + 14, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void CGameFramework::WaitForGpuComplete()
{
	// GPU�� Fence�� ���� �����ϴ� ������ ���� Queue�� �߰��Ѵ�.
	UINT64		nFence{ ++m_nFenceValue[m_nSwapChainBufferIndex] };
	HRESULT		hResult{ m_pd3dCommandQueue->Signal(m_pd3dFence, nFence) };

	if (m_pd3dFence->GetCompletedValue() < nFence)
	{
		// Fence�� ���� ���� ������ ������ ������ Fence�� ���� ���� ������ ���� �� ������ ��ٸ���.
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
	{
		// ���콺 ĸ�ĸ� �ϰ� ���� ���콺 ��ġ�� �����´�.
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);

		break;
	}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	{
		// ���콺 ĸ�� ����
		::ReleaseCapture();
		break;
	}
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
		case VK_ESCAPE:
		{
			::PostQuitMessage(0);
			break;
		}
		case VK_RETURN:
			break;
			// 'F1' Ű�� ������ 1��Ī ī�޶�, 'F2' Ű�� ������ Spaceship ī�޶�, 'F3' Ű�� ������ 3��Ī ī�޶�� ����
		case VK_F1:
		case VK_F2:
		case VK_F3:
		{
			if (m_pPlayer)
				m_pCamera = m_pPlayer->ChangeCamera((wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());

			break;
		}
		case VK_F8:
			break;
		case VK_F9:						// F9Ű�� �������� ������ ���� ��üȭ�� ����� ��ȯ�� ó��
		{
			ChangeSwapChainState();
			break;
		}
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
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);

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