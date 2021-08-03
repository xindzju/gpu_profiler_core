#pragma once
#include "core/tracker/gpc_object_tracker.h"

namespace gpc {
#pragma region GPCDXGISwapChainTracker
	//there are only one swapchain in most of cases
	class GPCDXGISwapChainTracker : public GPCObjTrakcer, public GPCSingleton<GPCDXGISwapChainTracker> {
	public:
		GPCDXGISwapChainTracker() {}
		~GPCDXGISwapChainTracker() {}
		
		bool Init(IDXGISwapChain* pSwapChain = nullptr, IUnknown* pD3D12Device = nullptr) {
			PRINT_FUNC_NAME
			m_pSwapChain = pSwapChain;

			if (pD3D12Device) {
				pD3D12Device->QueryInterface(&m_pD3D12CommandQueue);
				if (m_pD3D12CommandQueue) {
					m_pD3D12CommandQueue->GetDevice(IID_PPV_ARGS(&m_pD3D12Device));
					m_pD3D12CommandQueue->Release();
				}

				if (m_pD3D12Device) {
					m_pD3D12Device->Release();
					ComPtr<IDXGIFactory4> pDXGIFactory;
					auto adapterLUID = m_pD3D12Device->GetAdapterLuid();
					if (SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&pDXGIFactory))))
					{
						pDXGIFactory->EnumAdapterByLuid(adapterLUID, IID_PPV_ARGS(&m_pAdapter));
					}
				}

				if (m_pAdapter)
				{
					//check adapter info
					DXGI_ADAPTER_DESC desc;
					m_pAdapter->GetDesc(&desc);
				}
			}
			return true;
		}

		IDXGIAdapter3* GetDXGIAdapter() { return m_pAdapter; }
		IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
		ID3D12Device* GetD3D12Device() { return m_pD3D12Device; }
		ID3D12CommandQueue* GetD3D12CommandQueue() { return m_pD3D12CommandQueue; }

		void OnFrameStart();
		void OnFrameEnd(); 

	private:
		//IDXGIAdaptor: represents a display subsystem(including one or more GPUs, DACs and video memory)
		IDXGIAdapter3*		m_pAdapter = nullptr;	//QueryVideoMemoryInfo
		IDXGISwapChain*		m_pSwapChain = nullptr;
		ID3D12Device*		m_pD3D12Device = nullptr;
		ID3D12CommandQueue* m_pD3D12CommandQueue = nullptr;
	};
#pragma endregion
}