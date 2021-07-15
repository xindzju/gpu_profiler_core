#pragma once
#include "gpc_utils.h"

namespace gpc {
	class GPCPerfCounter {
		/*
		* Chip Level
		* Unit Level
		* Instance level
		*/
	};



	class GPCPerfMarker : public GPCPerfCounter {

	};

	class GPCPerfCounterManager {
	public:
		GPCPerfCounterManager() {
		}
		~GPCPerfCounterManager() {}

	};
}