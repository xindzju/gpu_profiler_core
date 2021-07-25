#include "gpc_swapchain_tracker.h"
#include "core/gpc_hud.h"

namespace gpc {
#pragma region  GPCDXGISwapChainTracker
	void GPCDXGISwapChainTracker::OnFrameStart() {
		PRINT_FUNC_NAME
		return;
	}

	void GPCDXGISwapChainTracker::OnFrameEnd() {
		PRINT_FUNC_NAME
		//game overlay
		auto pD3D12HUD = GPCHUDManager::GetSingleton()->GetD3D12HUD();
		//pD3D12HUD->OnRender();
	}
#pragma endregion
}

