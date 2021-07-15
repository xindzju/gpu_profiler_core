/*
* Integrated Dear ImGui: platform backend(Win32, SDL, glfw), render backend(dx12, vulkan, ogl)
*/
#pragma once
#include "imgui.h"
//using default Platform + Renderer backends
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx12.h"

#include "core/gpc_internal.h"

namespace gpc {
	class GPCHUDBackend {
	public:
		virtual bool Init() = 0;
		virtual void ShutDown() = 0;
		virtual void NewFrame() = 0;
		virtual void RenderDrawData() = 0;
	};

	class GPCHUDBackendDX12 : public GPCHUDBackend {
	public:
		GPCHUDBackendDX12() {};
		~GPCHUDBackendDX12() {};

		//platform(win32), render(dx12)
		virtual bool Init();
		virtual void ShutDown();
		virtual void NewFrame();
		virtual void RenderDrawData();

		//TODO:
		void GPC_ImplWin32_Init() {}
		void GPC_ImplDX12_Init() {}
		void GPC_ImplWin32_NewFrame() {}
		void GPC_ImplDX12_NewFrame() {}
		void GPC_ImplDX12_RenderDrawData() {}
		void GPC_ImplWin32_Shutdown() {}
		void GPC_ImplDX12_Shutdown() {}
	};

	/*
	class GPCHUDBackendVK {
		//platform(win32), render(dx12)
		virtual bool Init();
		virtual void ShutDown();
		virtual void NewFrame();
		virtual void RenderDrawData();
	};
	*/

	struct FrameResource {

	};

	//frame inspector, memory inspector, process inspector(?)
	/*referenece:
	* https://github.com/dfeneyrou/palanteer
	*/
	class GPCHUD {
	public:
		GPCHUD() {
			m_backend = std::make_unique<GPCHUDBackendDX12>();
			OnInit();
		};
		~GPCHUD() {
			OnDestory();
		};

		virtual void OnInit();
		virtual void OnRender();
		virtual void OnUpdate();
		virtual void OnDestory();

	private:
		//bool m_bInitialized = false;
		HWND m_window;
		std::unique_ptr<FrameResource> m_frameResource;
		std::unique_ptr<GPCHUDBackendDX12> m_backend = nullptr;
	};
}