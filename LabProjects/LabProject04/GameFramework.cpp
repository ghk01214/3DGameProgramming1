#include "stdafx.h"
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
	m_nFenceValue					 = 0;
	m_nWndClientWidth				 = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight				 = FRAME_BUFFER_HEIGHT;

	_tcscpy_s(m_pszFrameRate, _T("LabProject04 ("));
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

	//for (INT i = 0; i < m_nSwapChainBuffers; ++i)
	//{
	//	if (m_ppd3dRenderTargetBuffers[i])
	//		m_ppd3dRenderTargetBuffers[i]->Release();
	//}

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
#if defined(_DEBUG)
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
#if defined(_DEBUG)
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
	m_nFenceValue							 = 0;
	
	// Fence�� ����ȭ�� ���� �̺�Ʈ ��ü�� ����(�̺�Ʈ ��ü�� �ʱⰪ = FALSE)
	// �̺�Ʈ�� ����Ǹ�(Signal) �̺�Ʈ�� ���� �ڵ������� FALSE�� �ǵ��� ����
	m_hFenceEvent							 = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
	
	// Viewport�� �� �������� Ŭ���̾�Ʈ ���� ��ü�� ����
	m_d3dViewport.TopLeftX					 = 0;
	m_d3dViewport.TopLeftY					 = 0;
	m_d3dViewport.Width						 = static_cast<float>(m_nWndClientWidth);
	m_d3dViewport.Height					 = static_cast<float>(m_nWndClientHeight);
	m_d3dViewport.MinDepth					 = 0.0f;
	m_d3dViewport.MaxDepth					 = 1.0f;
	
	// Scissor �簢���� �� �������� Ŭ���̾�Ʈ ���� ��ü�� ����
	m_d3dScissorRect						 = { 0, 0, m_nWndClientWidth, m_nWndClientHeight };
	
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

	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

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

void CGameFramework::BuildObjects()
{
}

void CGameFramework::ReleaseObjects()
{
}

void CGameFramework::ProcessInput()
{
}

void CGameFramework::Animate()
{
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

	// Viewport�� Scissor �簢�� ����
	m_pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	m_pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
	
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle{ m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	// ������ Render Target�� �ش��ϴ� �������� CPU �ּ�(�ڵ�) ���
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex *	m_nRtvDescriptorIncrementSize);
	
	FLOAT pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	// ���ϴ� �������� Render Target(��)�� �����.
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle,	pfClearColor/*Colors::Azure*/, 0, nullptr);

	// Depth-Stencil �������� CPU �ּҸ� ����Ѵ�.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =	m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	
	// ���ϴ� ������ Depth-Stencil(��)�� �����.
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,	D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	
	// Render Target View(������)�� Depth-Stencil View(������)�� ���-���� �ܰ�(OM)�� ����
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);
	
	// Rendering �ڵ�� ���⿡ �߰��� ���̴�.

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
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	
	// GPU�� ��� ���� List�� ������ �� ���� ��ٸ���.
	WaitForGpuComplete();
	
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = nullptr;
	dxgiPresentParameters.pScrollRect = nullptr;
	dxgiPresentParameters.pScrollOffset = nullptr;
	
	// Swap Chain�� ������Ʈ�Ѵ�. ������Ʈ�� �ϸ� ���� Render Target(�ĸ����)�� ������ ������۷� �Ű����� Render Target �ε����� �ٲ� ���̴�.
	//m_pdxgiSwapChain->Present(1, 0, &dxgiPresentParameters);
	m_pdxgiSwapChain->Present(0, 0);

	/*������ ������ ����Ʈ�� ���ڿ��� �����ͼ� �� �������� Ÿ��Ʋ�� ����Ѵ�. m_pszBuffer ���ڿ���
	"LapProject ("���� �ʱ�ȭ�Ǿ����Ƿ� (m_pszFrameRate+12)�������� ������ ����Ʈ�� ���ڿ��� ���
	�Ͽ� �� FPS)�� ���ڿ��� ��ģ��.
	::_itow_s(m_nCurrentFrameRate, (m_pszFrameRate+12), 37, 10);
	::wcscat_s((m_pszFrameRate+12), 37, _T(" FPS)"));*/
	
	m_GameTimer.GetFrameRate(m_pszFrameRate + 14, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

void CGameFramework::WaitForGpuComplete()
{
	// CPU Fence�� �� ����
	++m_nFenceValue;

	// GPU�� Fence�� ���� �����ϴ� ������ ���� Queue�� �߰��Ѵ�.
	const UINT64	 nFence{ m_nFenceValue };
	HRESULT			 hResult{ m_pd3dCommandQueue->Signal(m_pd3dFence, nFence) };

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
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
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