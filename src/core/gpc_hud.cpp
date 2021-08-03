#include "core/gpc_hud.h"
#include "core/tracker/gpc_swapchain_tracker.h"
#include "core/gpc_inspector.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace gpc {
	struct FrameContext
	{
		ID3D12CommandAllocator* CommandAllocator;
		UINT64                  FenceValue;
	};

	// Data
	static int const                    NUM_FRAMES_IN_FLIGHT = 3;
	static FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
	static UINT                         g_frameIndex = 0;

	static int const                    NUM_BACK_BUFFERS = 3;
	//static ID3D12Device* g_pd3dDevice = NULL;
	//static ID3D12DescriptorHeap* g_pd3dRtvDescHeap = NULL;
	//static ID3D12DescriptorHeap* g_pd3dSrvDescHeap = NULL;
	//static ID3D12CommandQueue* g_pd3dCommandQueue = NULL;
	//static ID3D12GraphicsCommandList* g_pd3dCommandList = NULL;
	static ID3D12Fence* g_fence = NULL;
	static HANDLE                       g_fenceEvent = NULL;
	static UINT64                       g_fenceLastSignaledValue = 0;
	//static IDXGISwapChain3* g_pSwapChain = NULL;
	//static HANDLE                       g_hSwapChainWaitableObject = NULL;
	static ID3D12Resource* g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
	static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

	static bool bShowOverlay = true;
	static WNDPROC pRealMsgProc = nullptr;
	static LRESULT WINAPI GPC_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool GPCD3D12HUD::OnInit() {
		if (m_pHUDBackend == nullptr)
			m_pHUDBackend = new GPCD3D12HUDBackend();
		m_pHUDBackend->Init();
		return true;
	}

	void GPCD3D12HUD::OnUpdate() {
		//update memory info
		auto pMemInspector = GPCInspectorManager::GetSingleton()->GetMemoryInspector();
		m_vidMemInfo = pMemInspector->GetVidMemInfo();
		return;
	}

	void GPCD3D12HUD::OnRender() {
		//Init
		if (!m_initialized) { 
			if (!OnInit())
				return;
			m_initialized = true;
		}

		//Update
		OnUpdate();

		//Render

		// Start the Dear ImGui frame
		m_pHUDBackend->NewFrame();

		//Cutomize Overlay layout
		{
			ImGui::Begin("GPU Profiler Overlay");                        
			ImGui::Text("Hello World");
			auto pFrameInspector = GPCInspectorManager::GetSingleton()->GetFrameInspector();
			ImGui::End();
		}

		m_pHUDBackend->RenderDrawData();
		return;
	}

	void GPCD3D12HUD::OnDestory() {
		m_pHUDBackend->ShutDown();
		return;
	}

#pragma region GPCD3D12HUDBackend
	bool GPCD3D12HUDBackend::Init() {
		//get SwapChain, Device, Window
		auto pSwapChainTracker = GPCDXGISwapChainTracker::GetSingleton();
		m_pSwapChain = pSwapChainTracker->GetSwapChain();
		m_pD3D12Device = pSwapChainTracker->GetD3D12Device();
		m_pD3D12CommandQueue = pSwapChainTracker->GetD3D12CommandQueue();
		m_window = GetSwapChainOutputWindow(m_pSwapChain);

#if 1
		CreateDeviceD3D(m_window);
#endif

		pRealMsgProc = (WNDPROC)GetWindowLongPtr(m_window, GWLP_WNDPROC);
		if (pRealMsgProc)
		{
			SetWindowLongPtr(m_window, GWLP_WNDPROC, (LONG_PTR)GPC_WndProc);
		}
		else
		{
			std::cout << "Init platform backend failed" << std::endl;
			return false;
		}

		//Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		//Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

#ifdef USE_CUSTOM_BACKEND
		GPC_ImplWin32_Init();
		GPC_ImplDX12_Init();
#else
		//Setup Platform/Renderer backends(D3D12 + Win32)
		ImGui_ImplWin32_Init(m_window);
		ImGui_ImplDX12_Init(m_pD3D12Device, 3, DXGI_FORMAT_R8G8B8A8_UNORM, m_pSrvDescHeap, m_pSrvDescHeap->GetCPUDescriptorHandleForHeapStart(), m_pSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
#endif

		return true; 
	}

	void GPCD3D12HUDBackend::ShutDown() {
#ifdef USE_CUSTOM_BACKEND
		GPC_ImplWin32_Shutdown();
		GPC_ImplDX12_Shutdown();
#else
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX12_Shutdown();
#endif
		ImGui::DestroyContext();
		return;
	}

	void GPCD3D12HUDBackend::NewFrame() {
#ifdef USE_CUSTOM_BACKEND
		GPC_ImplDX12_NewFrame();
		GPC_ImplWin32_NewFrame();
#else
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame(); //update mousepos, mousecursor etc.
#endif
		ImGui::NewFrame();
		return;
	}

	void GPCD3D12HUDBackend::RenderDrawData() {
		// End of frame: render Dear ImGui
		ImGui::Render();

#ifdef USE_CUSTOM_BACKEND
		GPC_ImplDX12_RenderDrawData();
#else
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_pCommandList);
#endif
		return;
	}

	HWND GPCD3D12HUDBackend::GetSwapChainOutputWindow(IDXGISwapChain* pSwapChain) {
		if (pSwapChain) {
			IDXGISwapChain1* pSwapChain1 = nullptr;
			if (SUCCEEDED(pSwapChain->QueryInterface(&pSwapChain1))) {
				HRESULT res = pSwapChain1->GetHwnd(&m_window);
				return m_window;
			}
		}
	}

	bool GPCD3D12HUDBackend::CreateDeviceD3D(HWND hWnd)
	{
		//// Setup swap chain
		//DXGI_SWAP_CHAIN_DESC1 sd;
		//{
		//	ZeroMemory(&sd, sizeof(sd));
		//	sd.BufferCount = NUM_BACK_BUFFERS;
		//	sd.Width = 0;
		//	sd.Height = 0;
		//	sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//	sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		//	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		//	sd.SampleDesc.Count = 1;
		//	sd.SampleDesc.Quality = 0;
		//	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		//	sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		//	sd.Scaling = DXGI_SCALING_STRETCH;
		//	sd.Stereo = FALSE;
		//}

//		// [DEBUG] Enable debug interface
//#ifdef DX12_ENABLE_DEBUG_LAYER
//		ID3D12Debug* pdx12Debug = NULL;
//		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
//			pdx12Debug->EnableDebugLayer();
//#endif
//
//		// Create device
//		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
//		if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
//			return false;

		// [DEBUG] Setup debug interface to break on any warnings/errors
//#ifdef DX12_ENABLE_DEBUG_LAYER
//		if (pdx12Debug != NULL)
//		{
//			ID3D12InfoQueue* pInfoQueue = NULL;
//			g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
//			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
//			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
//			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
//			pInfoQueue->Release();
//			pdx12Debug->Release();
//		}
//#endif

		/*What we need
		* RTV and SRV DescriptorHeap
		* CommandAllocator and CommandList
		* Fence and FenceEvent
		* RTV, GetRenderTargetResource(BackBuffer) from hooked swapchain
		*/
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.NumDescriptors = NUM_BACK_BUFFERS;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 1;
			if (m_pD3D12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pRtvDescHeap)) != S_OK)
				return false;

			SIZE_T rtvDescriptorSize = m_pD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
			for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
			{
				g_mainRenderTargetDescriptor[i] = rtvHandle;
				rtvHandle.ptr += rtvDescriptorSize;
			}
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = 1;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			if (m_pD3D12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pSrvDescHeap)) != S_OK)
				return false;
		}

		//{
		//	D3D12_COMMAND_QUEUE_DESC desc = {};
		//	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		//	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		//	desc.NodeMask = 1;
		//	if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
		//		return false;
		//}

		for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
			if (m_pD3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
				return false;

		if (m_pD3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&m_pCommandList)) != S_OK ||
			m_pCommandList->Close() != S_OK)
			return false;

		if (m_pD3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
			return false;

		g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (g_fenceEvent == NULL)
			return false;

		//{
		//	IDXGIFactory4* dxgiFactory = NULL;
		//	IDXGISwapChain1* swapChain1 = NULL;
		//	if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
		//		return false;
		//	if (dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK)
		//		return false;
		//	if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
		//		return false;
		//	swapChain1->Release();
		//	dxgiFactory->Release();
		//	g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
		//	g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
		//}

		CreateRenderTarget();
		return true;
	}

	void GPCD3D12HUDBackend::CreateRenderTarget()
	{
		for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		{
			ID3D12Resource* pBackBuffer = NULL;
			m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
			m_pD3D12Device->CreateRenderTargetView(pBackBuffer, NULL, g_mainRenderTargetDescriptor[i]);
			g_mainRenderTargetResource[i] = pBackBuffer;
		}
	}

#pragma endregion
	static LRESULT WINAPI GPC_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		// Toggle the overlay using the delete key
		if (msg == WM_KEYDOWN && wParam == VK_DELETE) {
			bShowOverlay = !bShowOverlay;
			return false;
		}

		// If the overlay is shown, direct input to the overlay only
		if (bShowOverlay) {
			CallWindowProc(::ImGui_ImplWin32_WndProcHandler, hWnd, msg, wParam, lParam);
			return true;
		}

		// Otherwise call the game's wndProc function
		return CallWindowProc(pRealMsgProc, hWnd, msg, wParam, lParam);
	}
}

