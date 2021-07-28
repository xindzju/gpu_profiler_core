#pragma once
#include "core/tracker/gpc_object_tracker.h"

namespace gpc {
#pragma region GPCDXGISwapChainTracker
	//there are only one swapchain in most of cases
	class GPCDXGISwapChainTracker : public GPCObjTrakcer, public GPCSingleton<GPCDXGISwapChainTracker> {
	public:
		GPCDXGISwapChainTracker() {}
		~GPCDXGISwapChainTracker() {}
		
		bool SetSwapChain(IDXGISwapChain* pSwapChain = nullptr, IUnknown* pD3D12Device = nullptr) {
			m_pSwapChain = pSwapChain;
			pD3D12Device->QueryInterface(&m_pD3D12Device);
			if (m_pD3D12Device) {
				m_pD3D12Device->Release(); //Why?
			}
			return true;
		}

		IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
		IDXGIAdapter3* GetDXGIAdaptor() { return m_pAdaptor; }
		ID3D12Device* GetD3D12Device() { return m_pD3D12Device; }

		void OnFrameStart();
		void OnFrameEnd(); 

	private:
		IDXGIAdapter3*		m_pAdaptor = nullptr;	//QueryVideoMemoryInfo
		IDXGISwapChain*		m_pSwapChain = nullptr;
		ID3D12Device*		m_pD3D12Device = nullptr;
	};
#pragma endregion
}