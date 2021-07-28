#pragma once
#include "core/tracker/gpc_object_tracker.h"

namespace gpc {
    class GPCDeviceTracker : public GPCObjTrakcer {
	public:
		GPCDeviceTracker() {}
		~GPCDeviceTracker() {}

		bool Init(ID3D12Device* pD3D12Device) {
			m_pD3D12Device = pD3D12Device;
			return true;
		}

		virtual void OnFrameStart() {};
		virtual void OnFrameEnd() {};

	private:
		ID3D12Device* m_pD3D12Device = nullptr;
    };

	//multi-adaptor system: https://docs.microsoft.com/en-us/windows/win32/direct3d12/multi-engine
	class GPCD3D12DeviceTrackerManager : public GPCSingleton<GPCD3D12DeviceTrackerManager> {
	public:
		GPCD3D12DeviceTrackerManager();
		~GPCD3D12DeviceTrackerManager();
	};
}