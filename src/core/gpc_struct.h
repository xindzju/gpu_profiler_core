/*
* Internal data struct used by GPC
*/
#pragma once
#include <stdint.h>

namespace gpc {
	enum GPC_RET {
		GPC_RET_OK = 0,
		GPC_RET_ERR_HOOK_FAILED = 1,
		GPC_RET_ERR_DETOUR_FAILED
	};

	enum GPC_SHADER_STAGE {
		GPC_SHADER_STAGE_VS = 0,
		GPC_SHADER_STAGE_PS
	};

	//How to define bucket: render target change? pso change? cmdlist change?
	enum GPC_BUCKET_TYPE {
		GPC_BUCKET_TYPE_GRAPHICS = 0,
		GPC_BUCKET_TYPE_COMPUTE
	};

	enum GPC_COMMAND_LIST_TYPE {
		GPC_COMMAND_LIST_TYPE_DIRECT = 0,
		GPC_COMMAND_LIST_TYPE_COMPUTE,
		GPC_COMMAND_LIST_TYPE_COPY
	};

	enum GPC_PROFILE_LEVEL {
		GPC_PROFILE_LEVEL_CHIP = 0,
		GPC_PROFILE_LEVEL_UNIT
		//GPC_PROFILE_LEVEL_INSTANCE
	};

	enum GPC_OPTIMIZATION_METHOD {
		GPC_OPTIMIZATION_METHOD_A = 0,
		GPC_OPTIMIZATION_METHOD_B,
	};
}