#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>

namespace gpc {
	class GPCTracker {

	};

	class GPCSwapChainTracker : public GPCTracker {
	public:
		GPCSwapChainTracker() {}
		~GPCSwapChainTracker() {}

		//D3D12: CommandQueue D3D11: DeviceContext

		ID3D12Device* GetD3D12Device() {
			return m_pD3D12Device;
		}

		void SetSwapChain(IDXGISwapChain* pSwapChain) {
			m_pSwapChain = pSwapChain;
		}

		IDXGISwapChain* GetSwapChain() {
			return m_pSwapChain;
		}

	private:
		ID3D12Device*		m_pD3D12Device;
		IDXGISwapChain*		m_pSwapChain;
	};

    class GPCDeviceTracker : public GPCTracker {
	public:
		GPCDeviceTracker() {}
		~GPCDeviceTracker() {}
    };

	class GPCCommandQueueTracker : public GPCTracker {

	};

	class GPCGraphicsCommandListTracker : public GPCTracker {

	};
}