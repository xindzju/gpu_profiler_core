#include "core/gpc_hud.h"
#include "core/gpc_object_tracker_manager.h"

namespace gpc {

	void GPCHUD::OnInit() {
		//Create a Dear ImGui context, setup some options
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

#ifdef USE_CUSTOM_BACKEND
		m_backend->GPC_ImplWin32_Init();
		m_backend->GPC_ImplDX12_Init();
#else
		////Initialize Platform + Renderer backends (imgui_impl_win32.cpp + imgui_impl_dx12.cpp)
		ImGui_ImplWin32_Init(m_window);
		auto pSwapChainTracker = GPCSwapChainTrackerManager::GetSingleton()->GetSwapChainTracker();
		auto pD3D12Device = pSwapChainTracker->GetD3D12Device();
		//TODO
		static ID3D12DescriptorHeap* g_pd3dRtvDescHeap = NULL;
		static ID3D12DescriptorHeap* g_pd3dSrvDescHeap = NULL;
		ImGui_ImplDX12_Init(pD3D12Device, 3, DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap, g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(), g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
#endif
		return;
	}

	void GPCHUD::OnUpdate() {
		//update some render data
		return;
	}


	void GPCHUD::OnRender() {
		// Beginning of frame: update Renderer + Platform backend, start Dear ImGui frame
#ifdef USE_CUSTOM_BACKEND
		m_backend->GPC_ImplDX12_NewFrame();
		m_backend->GPC_ImplWin32_NewFrame();
#else
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
#endif
		ImGui::NewFrame();

		//overlay layout
		ImGui::Text("Hello world");

		// End of frame: render Dear ImGui
		ImGui::Render();
#ifdef USE_CUSTOM_BACKEND
		m_backend->GPC_ImplDX12_RenderDrawData();
#else
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData());
#endif
		////Swap
		//g_pSwapChain->Present(1, 0);
		return;
	}

	void GPCHUD::OnDestory() {
#ifdef USE_CUSTOM_BACKEND
		m_backend->GPC_ImplWin32_Shutdown();
		m_backend->GPC_ImplDX12_Shutdown();
#else
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX12_Shutdown();
#endif
		ImGui::DestroyContext();
		return;
	}

	bool GPCHUDBackendDX12::Init() { return true; }

	void GPCHUDBackendDX12::ShutDown() {}

	void GPCHUDBackendDX12::NewFrame() {}

	void GPCHUDBackendDX12::RenderDrawData() {}
}

