#pragma once
#include "core/tracker/gpc_object_tracker.h"

namespace gpc {
    class GPCCommandQueueTracker : public GPCObjTrakcer {
	public:
		GPCCommandQueueTracker() {}
		~GPCCommandQueueTracker() {}

		bool Init(ID3D12CommandQueue* pCommandQueue) {
			m_pCommandQueue = pCommandQueue;
			return true;
		}

		virtual void OnFrameStart() {};
		virtual void OnFrameEnd() {};

	private:
		ID3D12CommandQueue* m_pCommandQueue = nullptr;
    };

	class GPCD3D12CommandQueueTrackerManager : public GPCSingleton<GPCD3D12CommandQueueTrackerManager> {
	public:
		GPCD3D12CommandQueueTrackerManager();
		~GPCD3D12CommandQueueTrackerManager();
	};
}