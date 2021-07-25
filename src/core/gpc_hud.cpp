#include "core/gpc_hud.h"
#include "core/tracker/gpc_swapchain_tracker.h"
#include "core/gpc_inspector.h"

namespace gpc {

	bool GPCD3D12HUD::OnInit() {
		if (m_pHUDBackend == nullptr)
			m_pHUDBackend = new GPCD3D12HUDBackend();
		m_pHUDBackend->Init();
		return true;
	}

	void GPCD3D12HUD::OnUpdate() {
		//update memory info
		auto pMemInspector = GPCInpectorManager::GetSingleton()->GetMemoryInspector();
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
		ImGui::Text("Hello world");

		m_pHUDBackend->RenderDrawData();
		return;
	}

	void GPCD3D12HUD::OnDestory() {
		m_pHUDBackend->ShutDown();
		return;
	}

	bool GPCD3D12HUDBackend::Init() {
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
		auto pSwapChainTracker = GPCDXGISwapChainTrackerManager::GetSingleton()->GetSwapChainTracker();
		m_pSwapChain = pSwapChainTracker->GetSwapChain();
		m_window = GetSwapChainOutputWindow(m_pSwapChain);
		ImGui_ImplWin32_Init(m_window);
		auto pD3D12Device = pSwapChainTracker->GetD3D12Device();
		ImGui_ImplDX12_Init(pD3D12Device, 3, DXGI_FORMAT_R8G8B8A8_UNORM, m_pSrvDescHeap, m_pSrvDescHeap->GetCPUDescriptorHandleForHeapStart(), m_pSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
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
}

