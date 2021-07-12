/*
* Integrated Dear ImGui: platform backend(Win32, SDL, glfw), render backend(dx12, vulkan, ogl)
*/
#pragma once
#include "imgui.h"
#include "gpc_utils.h"

namespace gpc {
	struct FrameResource {

	};

	//frame inspector, memory inspector, process inspector(?)
	class GPCHUD {
	public:
		GPCHUD() {
			m_backend = std::make_unique<GPCHUDBackendDX12>();
		};
		~GPCHUD() {};

		virtual void OnInit();
		virtual void OnRender();
		virtual void OnUpdate();
		virtual void OnDestory();

	private:
		std::unique_ptr<FrameResource> m_frameResource;
		std::unique_ptr<GPCHUDBackend> m_backend = nullptr;
	};

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
}