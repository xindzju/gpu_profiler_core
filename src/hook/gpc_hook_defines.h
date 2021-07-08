#pragma once
#include <stdint.h>
#include <string>

//#define USE_MINHOOK
#define USE_DETOUR
#define HOOK_DX
//#define HOOK_VK
//#define HOOK_OGL
//#define HOOK_CUDA

namespace gpc {
	enum GPC_HOOK_POS : uint8_t {
		GPC_HOOK_POS_NONE = 0, //0
		GPC_HOOK_POS_PRE = 1 << 0, //1
		GPC_HOOK_POS_REAL = 1 << 1, //2
		GPC_HOOK_POS_POST = 1 << 2, // 4
		GPC_HOOK_POS_ALL = GPC_HOOK_POS_PRE | GPC_HOOK_POS_REAL | GPC_HOOK_POS_POST //7
	};

	enum class HOOK_API_TYPE {
		HOOK_API_TYPE_NONE = 0,
		HOOK_API_TYPE_DX = 1 << 0, //1
		HOOK_API_TYPE_VK = 1 << 1, // 2 
		HOOK_API_TYPE_OGL = 1 << 2, // 4
		HOOK_API_TYPE_CUDA = 1 << 3, // 8
		HOOK_API_TYPE_ALL = HOOK_API_TYPE_DX | HOOK_API_TYPE_VK | HOOK_API_TYPE_OGL | HOOK_API_TYPE_CUDA,
	};

	struct GPCAPIHookInfo {
		GPCAPIHookInfo() {
			memset(this, 0, sizeof(*this));
		}

		GPCAPIHookInfo(const char* apiName_, GPC_HOOK_POS hookPos_ = GPC_HOOK_POS_ALL) {
			interfaceName = "";
			apiName = apiName_;
			vTableOffset = -1;//-1 means not a member method
			hookPos = hookPos_;
		}

		GPCAPIHookInfo(const char* interfaceName_, const char* apiName_, int vTableOffset_, GPC_HOOK_POS hookPos_ = GPC_HOOK_POS_ALL) {
			interfaceName = interfaceName_;
			apiName = apiName_;
			vTableOffset = vTableOffset_;
			hookPos = hookPos_;
		}
		std::string interfaceName;
		std::string apiName;
		int vTableOffset; 
		GPC_HOOK_POS hookPos;
	};
}