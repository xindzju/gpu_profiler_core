#pragma once
#include "core/tracker/gpc_object_tracker.h"

namespace gpc {
#pragma region GPCDXGISwapChainTracker
	class GPCDXGISwapChainTracker : public GPCObjTrakcer {
	public:
		GPCDXGISwapChainTracker();
		~GPCDXGISwapChainTracker();

		bool Init(IDXGISwapChain* pSwapChain) {
			m_pSwapChain = pSwapChain;
			return true;
		}

		void OnFrameStart();
		void OnFrameEnd(); 

		IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
		IDXGIAdapter3* GetDXGIAdaptor() { return m_pAdaptor; }
		ID3D12Device* GetD3D12Device() { return m_pD3D12Device; }

	private:
		IDXGIAdapter3*		m_pAdaptor = nullptr;	//QueryVideoMemoryInfo
		IDXGISwapChain*		m_pSwapChain = nullptr;
		ID3D12Device*		m_pD3D12Device = nullptr;
	};
#pragma endregion

#pragma region GPCD3D12DeviceTrackerManager
	class GPCDXGISwapChainTrackerManager : public GPCSingleton<GPCDXGISwapChainTrackerManager> {
	public:
		GPCDXGISwapChainTrackerManager() :
			m_pSwapChainTracker(nullptr) {
		}
		~GPCDXGISwapChainTrackerManager() {}

		GPCDXGISwapChainTracker* GetSwapChainTracker() {
			if (m_pSwapChainTracker == nullptr)
				m_pSwapChainTracker = new GPCDXGISwapChainTracker();
			return m_pSwapChainTracker;
		}

	private:
		GPCDXGISwapChainTracker* m_pSwapChainTracker;
	};
#pragma endregion
}