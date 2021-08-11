/*
* Integrated Dear ImGui: platform backend(Win32, SDL, glfw), render backend(dx12, vulkan, ogl)
*/
#pragma once
#include "core/gpc_common.h"
#include "core/gpc_d3d12_helper.h"

//imgui
#include "imgui.h"
#include "imgui_internal.h"
#ifndef USE_CUSTOM_BACKEND
//using default Platform + Renderer backends
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx12.h"
#endif

//implot
#include "implot.h"

namespace gpc {
	struct FrameContext
	{
		ID3D12CommandAllocator* CommandAllocator;
		UINT64                  FenceValue;
	};

	// utility structure for realtime plot
	struct ScrollingBuffer {
		int MaxSize;
		int Offset;
		double Sum;
		ImVector<ImVec2> Data;
		ScrollingBuffer(int max_size = 2000) {
#if 1
			MaxSize = max_size;
			Offset = 0;
			Sum = 0;
			Data.reserve(MaxSize);
#else
			MaxSize = max_size;
			Offset = 0;
			Data.reserve(MaxSize);
#endif
		}
		void AddPoint(float x, float y) {
#if 1
			if (Data.size() < MaxSize)
			{
				Data.push_back(ImVec2(x, y));
			}
			else
			{
				Sum -= Data[Offset].y;
				Data[Offset] = ImVec2(x, y);
				Offset = (Offset + 1) % MaxSize;
			}
			Sum += y;
#else
			if (Data.size() < MaxSize)
				Data.push_back(ImVec2(x, y));
			else {
				Data[Offset] = ImVec2(x, y);
				Offset = (Offset + 1) % MaxSize;
			}
#endif
		}
		void Erase() {
			if (Data.size() > 0) {
				Data.shrink(0);
				Offset = 0;
			}
		}
	};

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

	protected:
		HWND GetSwapChainOutputWindow(IDXGISwapChain* pSwapChain);
#if 1 //using Dear ImGui default backend
		// Forward declarations of helper functions
		bool CreateDeviceD3D(HWND hWnd);
		void CleanupDeviceD3D();
		void CreateRenderTarget();
		void CleanupRenderTarget();
		void WaitForLastSubmittedFrame();
		FrameContext* WaitForNextFrameResources();

#endif
	private:
		HWND m_window = nullptr;
		IDXGISwapChain* m_pSwapChain = nullptr;
		ID3D12Device* m_pD3D12Device = nullptr;
		ID3D12CommandQueue* m_pD3D12CommandQueue = nullptr;
		ID3D12DescriptorHeap*	m_pSrvDescHeap = nullptr;
		ID3D12DescriptorHeap*	m_pRtvDescHeap = nullptr;
		ID3D12GraphicsCommandList* m_pCommandList = nullptr;
	};

	//frame inspector, memory inspector, process inspector(?)
	//referenece:https://github.com/dfeneyrou/palanteer
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
		VidMemInfo m_vidMemInfo;
		ScrollingBuffer m_fpsScrollingBuffer;
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