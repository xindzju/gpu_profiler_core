#pragma once
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include "gpc_hook_defines.h"

#ifdef USE_DETOUR
#ifdef _WIN32
#include <Windows.h>
//https://github.com/microsoft/Detours
#include "detours/detours.h" //must after including Windows.h
#endif
#endif

#ifdef USE_MINHOOK
#include "minhook/MinHook.h"
#endif
//https://github.com/danielaparker/jsoncons
#include "jsoncons/json.hpp"
#include "jsoncons_ext/csv/csv.hpp"

namespace gpc {
	extern std::map<std::string, std::shared_ptr<GPCAPIHookInfo>> g_apiHookInfos; //load from vtable.csv
	extern std::map<void*, void*> g_allHookFuncs; //real function address: detoured function address

#ifdef USE_DETOUR
	//More API details check this doc: https://github.com/microsoft/Detours/wiki/Reference
	inline PBYTE MyDetourAttach(PBYTE targetFunc, PBYTE detourFunc) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)targetFunc, detourFunc);
		DetourTransactionCommit();
		return targetFunc;
	}

	inline void MyDetourDetach(PBYTE targetFunc, PBYTE detourFunc) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)targetFunc, detourFunc);
		DetourTransactionCommit();
	}
#endif

	inline void HookFuncImpl(void* pRealFunc, void* pHookFunc, void** pNextFunc) {
#ifdef USE_DETOUR
#endif
#ifdef USE_MINHOOK
#endif
	}

	inline void UnHookFuncImpl(void* pRealFunc, void* pHookFunc, void** pNextFunc) {
#ifdef USE_DETOUR
#endif
#ifdef USE_MINHOOK
#endif
	}

	inline bool HookFunc(void* pRealFunc, void* pHookFunc, void** pNextFunc, GPCAPIHookInfo* apiHookInfo) {
		bool res = true;
		if (g_allHookFuncs.find(pRealFunc) != g_allHookFuncs.end()) {
			std::cout << "API already get detoured: " << apiHookInfo->apiName << std::endl;
			return true;
		}
		else {
#ifdef USE_DETOUR
			*pNextFunc = (void*)MyDetourAttach((PBYTE)pRealFunc, (PBYTE)pHookFunc);
#endif
#ifdef USE_MINHOOK
			//initial hook => create hook => enable hook => disable hook => uninitialize hook
			// Create a hook for MessageBoxW, in disabled state.
			if (MH_CreateHook(pRealFunc, pHookFunc, pNextFunc) != MH_OK)
			{
				std::cout << "[minhook]: Create hook failed: " << apiHookInfo->apiName << std::endl;
				return false;
			}
			// Enable the hook for MessageBoxW.
			if (MH_EnableHook(pRealFunc) != MH_OK)
			{
				std::cout << "[minhook]: Enable hook failed: " << apiHookInfo->apiName << std::endl;
				return false;
			}
#endif
			if (!pNextFunc)
				res = false;
			g_allHookFuncs[pRealFunc] = pHookFunc;
		}
		return res;
	}

	inline GPCAPIHookInfo* GetAPIHookInfo(const char* apiName) {
		return g_apiHookInfos[apiName].get();
	}

	inline bool HookInterfaceFunc(void* pInterface, void* pHookFunc, void** pRealFunc, const char* apiName) {
		bool res = true;
		uint64_t* pVTable = (uint64_t*)*(uint64_t*)pInterface;
		auto apiHookInfo = GetAPIHookInfo(apiName);
		res = HookFunc((void*)pVTable[apiHookInfo->vTableOffset], pHookFunc, (void**)&pRealFunc, apiHookInfo);
		if (res)
			std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
		else
			std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
		return res;
	};

	const char* GetHookAPITypeStr(HOOK_API_TYPE hookType);
	const char* GetHookPosStr(GPC_HOOK_POS hookPos);
}