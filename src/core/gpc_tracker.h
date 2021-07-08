#pragma once

namespace gpc {
	class GPCTracker {

	};

	class GPCSwapChainTracker : public GPCTracker {

	};

	//TODO: how to design
	class GPCSwapChainTrackerManager {
	public:
		void StartFrame();
		void EndFrame();
	};
}