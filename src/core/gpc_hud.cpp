#include "gpc_hud.h"

namespace gpc {

	void GPCHUD::OnInit() {
		ImGui::Render();
		return;
	}

	void GPCHUD::OnUpdate() {
		ImGui::Render();
		return;
	}

	void GPCHUD::OnRender() {
		ImGui::Render();
		return;
	}

	void GPCHUD::OnDestory() {
		ImGui::Render();
		return;
	}

	bool GPCHUDBackendDX12::Init() { return true; }

	void GPCHUDBackendDX12::ShutDown() {}

	void GPCHUDBackendDX12::NewFrame() {}

	void GPCHUDBackendDX12::RenderDrawData() {}
}

