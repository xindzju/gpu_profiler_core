#include "gpc_swapchain_tracker.h"
#include "core/gpc_hud.h"

namespace gpc {
#pragma region  GPCDXGISwapChainTracker
	GPCDXGISwapChainTracker::GPCDXGISwapChainTracker() {
	}

	GPCDXGISwapChainTracker::~GPCDXGISwapChainTracker() {
	}

	void GPCDXGISwapChainTracker::OnFrameStart() {
		PRINT_MEMBER_FUNC_NAME
	}

	void GPCDXGISwapChainTracker::OnFrameEnd() {
		PRINT_MEMBER_FUNC_NAME
		//game overlay
		auto pD3D12HUD = GPCHUDManager::GetSingleton()->GetD3D12HUD();
		pD3D12HUD->OnRender();
	}
#pragma endregion
}

