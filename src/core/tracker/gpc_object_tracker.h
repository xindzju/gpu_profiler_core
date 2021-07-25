#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include "core/gpc_common.h"

namespace gpc {
	class GPCObjTrakcer {
	public:
		GPCObjTrakcer() {};
		~GPCObjTrakcer() {};
	};

	//e.g. multiple objects scenario
	template <class Key, class Value>
	class GPCObjTrackerManager {

	};
}