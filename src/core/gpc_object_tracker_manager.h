#pragma once
#include "core/gpc_object_tracker.h"
#include "utils/gpc_utils.h"

namespace gpc {
	class GPCObjectTrackerManager {
	public:
		GPCObjectTrackerManager() {};
		~GPCObjectTrackerManager() {};
	};

	class GPCSwapChainTrackerManager : public GPCObjectTrackerManager, public GPCSingleton<GPCSwapChainTrackerManager>{
	public:
		GPCSwapChainTrackerManager() :
			m_pSwapChainTracker(nullptr) {
		}
		~GPCSwapChainTrackerManager() {}

		GPCSwapChainTracker* GetSwapChainTracker() {
			if (m_pSwapChainTracker == nullptr)
				m_pSwapChainTracker = new GPCSwapChainTracker();
			return m_pSwapChainTracker;
		}

	private:
		GPCSwapChainTracker* m_pSwapChainTracker;
	};

    class GPCDeviceTrackerManager : public GPCObjectTrackerManager {
	public:
		GPCDeviceTrackerManager();
		~GPCDeviceTrackerManager();
    };

	class GPCCommandQueueTrackerManager : public GPCObjectTrackerManager {
	public:
		GPCCommandQueueTrackerManager();
		~GPCCommandQueueTrackerManager();
	};

	class GPCGraphicsCommandListTrackerManager : public GPCObjectTrackerManager {
	public: 
		GPCGraphicsCommandListTrackerManager();
		~GPCGraphicsCommandListTrackerManager();
	};
}