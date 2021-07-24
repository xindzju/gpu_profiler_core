/*
* Integrated Dear ImGui: platform backend(Win32, SDL, glfw), render backend(dx12, vulkan, ogl)
*/
#pragma once
#include "core/gpc_common.h"

#include "imgui.h"
#ifndef USE_CUSTOM_BACKEND
//using default Platform + Renderer backends
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx12.h"
#endif


namespace gpc {
	//GPCHUDBackend, including platform backend + render backend
	class GPCHUDBackend {
	public:
		virtual bool Init() = 0;
		virtual void ShutDown() = 0;
		virtual void NewFrame() = 0;
		virtual void RenderDrawData() = 0;
	};

	class GPCD3D12HUDBackend : public GPCHUDBackend {
	public:
		GPCD3D12HUDBackend() {};
		~GPCD3D12HUDBackend() {};

		//platform(win32), render(dx12)
		virtual bool Init();
		virtual void ShutDown();
		virtual void NewFrame();
		virtual void RenderDrawData();

		//platform backend: win32
		void GPC_ImplWin32_Init() {}
		void GPC_ImplWin32_Shutdown() {}
		void GPC_ImplWin32_NewFrame() {}

		//render backend: dx12
		void GPC_ImplDX12_Init() {}
		void GPC_ImplDX12_NewFrame() {}
		void GPC_ImplDX12_RenderDrawData() {}
		void GPC_ImplDX12_Shutdown() {}

	private:
		HWND m_window;
		ID3D12DescriptorHeap*	m_pSrvDescHeap = nullptr;
		ID3D12DescriptorHeap*	m_pRtvDescHeap = nullptr;
		ID3D12GraphicsCommandList* m_pCommandList = nullptr;
	};

	//frame inspector, memory inspector, process inspector(?)
/*referenece:
* https://github.com/dfeneyrou/palanteer
*/
	class GPCD3D12HUD {
	public:
		GPCD3D12HUD() {};
		~GPCD3D12HUD() {
			OnDestory();
		};

		bool OnInit();
		void OnUpdate();
		void OnRender();
		void OnDestory();

	private:
		bool m_initialized = false;
		GPCD3D12HUDBackend* m_pHUDBackend = nullptr;
		DXGI_QUERY_VIDEO_MEMORY_INFO m_vidMemInfo;
	};

	class GPCHUDManager : public GPCSingleton<GPCHUDManager> {
	public:
		GPCD3D12HUD* GetD3D12HUD() {
			if (m_pD3D12HUD == nullptr)
				m_pD3D12HUD = new GPCD3D12HUD();
			return m_pD3D12HUD;
		}

	private:
		GPCD3D12HUD* m_pD3D12HUD = nullptr;
	};
}