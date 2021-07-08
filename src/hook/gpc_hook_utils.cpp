#include "gpc_hook_utils.h"

namespace gpc {
	std::map<std::string, std::shared_ptr<GPCAPIHookInfo>> g_apiHookInfos = {}; //load from vtable.csv
	std::map<void*, void*> g_allHookFuncs = {}; //real function address: detoured function address

	///*
	//* before detour: pRealFunc -> ICD(Installable Client Driver)
	//* after detour: pRealFunc -> pHookFunc-> pNextFunc -> ICD(Installable Client Driver)
	//*/
	//bool HookFunc(void* pRealFunc, void* pHookFunc, void** pNextFunc, GPCAPIHookInfo* apiHookInfo) {
	//	bool res = true;
	//	if (g_allHookFuncs.find(pRealFunc) != g_allHookFuncs.end()) {
	//		std::cout << "API already get detoured: " << apiHookInfo->apiName << std::endl;
	//		return true;
	//	}
	//	else {
	//		*pNextFunc = (void*)MyDetourAttach((PBYTE)pRealFunc, (PBYTE)pHookFunc);
	//		if (!pNextFunc)
	//			res = false;
	//		g_allHookFuncs[pRealFunc] = pHookFunc;
	//	}
	//	return res;
	//}

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