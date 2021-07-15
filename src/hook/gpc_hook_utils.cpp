#include "gpc_hook_utils.h"

namespace gpc {
	std::map<std::string, std::shared_ptr<GPCAPIHookInfo>> g_apiHookInfos = {}; //load from vtable.csv
	std::map<void*, void*> g_allHookFuncs = {}; //real function address: detoured function address

	const char* GetHookAPITypeStr(HOOK_API_TYPE hookType) {
		switch (hookType) {
		case HOOK_API_TYPE::HOOK_API_TYPE_DX:
			return "DX";
			break;
		case HOOK_API_TYPE::HOOK_API_TYPE_VK:
			return "VK";
			break;
		case HOOK_API_TYPE::HOOK_API_TYPE_OGL:
			return "OGL";
			break;
		case HOOK_API_TYPE::HOOK_API_TYPE_CUDA:
			return "CUDA";
			break;
		case HOOK_API_TYPE::HOOK_API_TYPE_ALL:
			return "DX,VK,OGL,CUDA";
			break;
		}
		return "";
	}

	const char* g_HookPosStr[8] = {"None","Pre","Real","","Post","","","All"};

	const char* GetHookPosStr(GPC_HOOK_POS hookPos) {
		return g_HookPosStr[hookPos];
	}
}