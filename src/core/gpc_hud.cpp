#include "core/gpc_hud.h"
#include "core/tracker/gpc_swapchain_tracker.h"
#include "core/gpc_inspector.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void TextFocused(const char* label, const char* value)
{
	ImGui::TextColored(ImVec4{ 1, 0.5, 0.5, 1 }, label);
	ImGui::SameLine();
	ImGui::TextUnformatted(value);
}

namespace gpc {
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
	static HANDLE                       g_hSwapChainWaitableObject = NULL;
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

	void ShowDemo_RealtimePlots() {
		ImGui::BulletText("Move your mouse to change the data!");
		ImGui::BulletText("This example assumes 60 FPS. Higher FPS requires larger buffer size.");
		static ScrollingBuffer sdata1, sdata2;
		ImVec2 mouse = ImGui::GetMousePos();
		static float t = 0;
		t += ImGui::GetIO().DeltaTime;
		sdata1.AddPoint(t, mouse.x * 0.0005f);
		sdata2.AddPoint(t, mouse.y * 0.0005f);

		static float history = 10.0f;
		ImGui::SliderFloat("History", &history, 1, 30, "%.1f s");

		static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;
		//ImPlot::SetNextPlotLimitsX(t - history, t, ImGuiCond_Always);
		//ImPlot::SetNextPlotLimitsY(0, 1);
		if (ImPlot::BeginPlot("##Scrolling", NULL, NULL, ImVec2(-1, 150), 0, flags, flags)) {
			ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
			ImPlot::PlotShaded("Mouse X", &sdata1.Data[0].x, &sdata1.Data[0].y, sdata1.Data.size(), -INFINITY, sdata1.Offset, 2 * sizeof(float));
			ImPlot::PlotLine("Mouse Y", &sdata2.Data[0].x, &sdata2.Data[0].y, sdata2.Data.size(), sdata2.Offset, 2 * sizeof(float));
			ImPlot::EndPlot();
		}
	}

	//update overlay layout and related real-time data
	void GPCD3D12HUD::OnUpdate() {
		//update overlay data
		auto pMemInspector = GPCInspectorManager::GetSingleton()->GetMemoryInspector();
		m_vidMemInfo = pMemInspector->GetVidMemInfo();
		
		auto pFrameInspector = GPCInspectorManager::GetSingleton()->GetFrameInspector();
		float totalInjectedTime = pFrameInspector->m_fpsInspector->GetTotalInjectedTime();
		float lastFPS = pFrameInspector->m_fpsInspector->GetLastFPS();
		float fpsLimit = 200;
		if (lastFPS > fpsLimit)
			lastFPS = fpsLimit;
		m_fpsScrollingBuffer.AddPoint(totalInjectedTime, lastFPS);
		std::cout << "fps data: " << totalInjectedTime << "\t" << lastFPS << std::endl;
		//update overlay layout
		ImGui::Begin("GPU Profiler Overlay"/*, nullptr, ImGuiWindowFlags_AlwaysAutoResize*/);
		if (ImGui::TreeNodeEx("Hardware Info", ImGuiTreeNodeFlags_DefaultOpen)) {
			TextFocused("CPU Name: ", "");
			TextFocused("GPU Name: ", "");
			TextFocused("System Memory Size: ", "");
			TextFocused("Video Memory Size: ", "");
		}

		if (ImGui::TreeNodeEx("Process Usage Info", ImGuiTreeNodeFlags_DefaultOpen)) {
			TextFocused("Process CPU Usage: ", "");
			TextFocused("Process GPU Usage: ", "");
			TextFocused("Process System Memory Usage: ", "");
			TextFocused("Process Video Memory Usage: ", "");
		}

		ImPlot::SetNextPlotLimits(totalInjectedTime - 10.0, totalInjectedTime, 0, fpsLimit, ImGuiCond_Always);
		//ImPlot::BeginPlot("FPS Chart##GIFT", "time", "FPS", ImVec2(-1, 0), ImPlotFlags_None, ImPlotAxisFlags_NoDecorations, ImPlotFlags_None)
		if (ImPlot::BeginPlot("Process Realtime Performance Metrics"))
		{
			ImPlot::PlotLine("FPS" , &m_fpsScrollingBuffer.Data[0].x, &m_fpsScrollingBuffer.Data[0].y, m_fpsScrollingBuffer.Data.size(), m_fpsScrollingBuffer.Offset, 2 * sizeof(float));
			ImPlot::EndPlot();
		}
		//static bool bShowCheckbox = true;
		//ImGui::Checkbox("Show CheckBox", &bShowCheckbox);
		ImGui::End();
		return;
	}

	void GPCD3D12HUD::OnRender() {
		//Init
		if (!m_initialized) { 
			if (!OnInit())
				return;
			m_initialized = true;
		}

		// Start the Dear ImGui frame
		m_pHUDBackend->NewFrame();

		//Update
		OnUpdate();


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
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		ImPlot::CreateContext();
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

		//	Load Fonts
		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
		// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		// - Read 'docs/FONTS.md' for more instructions and details.
		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
		//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
		//IM_ASSERT(font != NULL);

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
		ImPlot::DestroyContext();
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
		FrameContext* nextFrameCtx = WaitForNextFrameResources(); //next frame context
		IDXGISwapChain3* pIDXGISwapChain3 = nullptr;
		m_pSwapChain->QueryInterface(&pIDXGISwapChain3);
		UINT backBufferIdx = pIDXGISwapChain3->GetCurrentBackBufferIndex();
		nextFrameCtx->CommandAllocator->Reset(); //indicates to re-use the memory that is associated with the command allocator

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		m_pCommandList->Reset(nextFrameCtx->CommandAllocator, NULL); //resets a command list back to its initial state as if a new command list was just created
		m_pCommandList->ResourceBarrier(1, &barrier); //notifies the driver that it needs to synchronize multiple accesses to resources

		// Render Dear ImGui graphics
		//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		//const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		//m_pCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, NULL);
		m_pCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
		m_pCommandList->SetDescriptorHeaps(1, &m_pSrvDescHeap);
		// Rendering
		ImGui::Render(); //prepare data for rendering so you can call ImGui::GetDrawData

#ifdef USE_CUSTOM_BACKEND
		GPC_ImplDX12_RenderDrawData();
#else
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_pCommandList); //set up render state 
#endif
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		m_pCommandList->ResourceBarrier(1, &barrier);
		m_pCommandList->Close();
		m_pD3D12CommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&m_pCommandList);
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
		return nullptr;
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
		IDXGISwapChain2* pSwapChain2 = nullptr;
		if (SUCCEEDED(m_pSwapChain->QueryInterface(&pSwapChain2))) {
			g_hSwapChainWaitableObject = pSwapChain2->GetFrameLatencyWaitableObject();
		}
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

	FrameContext* GPCD3D12HUDBackend::WaitForNextFrameResources()
	{
		UINT nextFrameIndex = g_frameIndex + 1;
		g_frameIndex = nextFrameIndex;

		HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, NULL }; //g_hSwapChainWaitableObject: Returns a waitable handle that signals when the DXGI adapter has finished presenting a new frame.
		DWORD numWaitableObjects = 1;

		FrameContext* frameCtx = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
		UINT64 fenceValue = frameCtx->FenceValue;
		if (fenceValue != 0) // means no fence was signaled
		{
			frameCtx->FenceValue = 0;
			g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent); //specifies an event that should be fired when the fence reaches a certain value
			waitableObjects[1] = g_fenceEvent;
			numWaitableObjects = 2;
		}

		WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

		return frameCtx;
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
			return CallWindowProc(pRealMsgProc, hWnd, msg, wParam, lParam);
		}
		else {
			// Otherwise call the game's wndProc function
			return CallWindowProc(pRealMsgProc, hWnd, msg, wParam, lParam);
		}
	}
}

