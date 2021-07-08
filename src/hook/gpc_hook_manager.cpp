#include "gpc_hook_manager.h"

namespace gpc {

	GPCHookManager::~GPCHookManager() {
	}

	bool GPCHookManager::Hook(HOOK_API_TYPE hookType) {
		bool res = true;
		switch (hookType) {
		case HOOK_API_TYPE::HOOK_API_TYPE_DX:
			res = HookDXGI("dxgi.dll") && HookD3D12("d3d12.dll");
			break;
		case HOOK_API_TYPE::HOOK_API_TYPE_VK:
			res = HookVK();
			break;
		case HOOK_API_TYPE::HOOK_API_TYPE_OGL:
			res = HookOGL();
			break;
		case HOOK_API_TYPE::HOOK_API_TYPE_CUDA:
			res = HookCUDA();
			break;
		case HOOK_API_TYPE::HOOK_API_TYPE_ALL:
			res = HookDXGI() && HookD3D12() && HookVK() && HookOGL() && HookCUDA();
			break;
		default:
			std::cout << "No API hooked" << std::endl;
			return false;
		}
		return res;
	}

	void GPCHookManager::UnHook() {
		std::cout << "Start to undetour all functions: " << g_allHookFuncs.size() << std::endl;
#ifdef USE_DETOUR
		for (auto hookFunc : g_allHookFuncs)
			MyDetourDetach((PBYTE)hookFunc.first, (PBYTE)hookFunc.second);
#endif
#ifdef USE_MINHOOK

#endif
	}

	bool GPCHookManager::HookDXGI(const char* dllName) {
		bool res = true;
		HMODULE module = LoadLibrary(dllName);
		if (module) {
			void* pFuncAddress = GetProcAddress(module, "CreateDXGIFactory");
			if (pFuncAddress) {
				auto apiHookInfo = GetAPIHookInfo("CreateDXGIFactory");
				res = HookFunc(pFuncAddress, My_CreateDXGIFactory, (void**)&pReal_CreateDXGIFactory, apiHookInfo);
				if (res)
					std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
				else 
					std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
			}
			else {
				std::cout << "Get CreateDXGIFactory address failed" << std::endl;
				res = false;
			}

			/*
			pFuncAddress = GetProcAddress(module, "CreateDXGIFactory1");
			if (pFuncAddress) {
				auto apiHookInfo = GetAPIHookInfo("CreateDXGIFactory1");
				res = HookFunc(pFuncAddress, My_CreateDXGIFactory1, (void**)&pReal_CreateDXGIFactory1, apiHookInfo);
				if (res)
					std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
				else
					std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
			}
			else {
				std::cout << "Get CreateDXGIFactory1 address failed" << std::endl;
				res = false;
			}
			*/

			pFuncAddress = GetProcAddress(module, "CreateDXGIFactory2");
			if (pFuncAddress) {
				auto apiHookInfo = GetAPIHookInfo("CreateDXGIFactory2");
				res = HookFunc(pFuncAddress, My_CreateDXGIFactory2, (void**)&pReal_CreateDXGIFactory2, apiHookInfo);
				if (res)
					std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
				else
					std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
			}
			else {
				std::cout << "Get CreateDXGIFactory2 address failed" << std::endl;
				res = false;
			}
		}
		else {
			std::cout << "Load dxgi.lib failed" << std::endl;
			res = false;
		}
		//FreeLibrary(module);

		return res;
	}

	bool GPCHookManager::HookD3D12(const char* pModule) {
		bool res = true;
		HMODULE module = LoadLibrary(pModule);
		if (module) {
			void* pFuncAddress = GetProcAddress(module, "D3D12CreateDevice");
			if (pFuncAddress) {
				auto apiHookInfo = GetAPIHookInfo("D3D12CreateDevice");
				res = HookFunc(pFuncAddress, My_D3D12CreateDevice, (void**)&pReal_D3D12CreateDevice, apiHookInfo);
				if (res)
					std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
				else
					std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
			}
			else {
				std::cout << "Get D3D12CreateDevice address failed" << std::endl;
				res = false;
			}
		}
		else {
			std::cout << "Load d3d12.lib failed" << std::endl;
			res = false;
		}
		//FreeLibrary(module);

		return res;
	}

	bool GPCHookManager::HookCUDA(const char* dllName) {
		bool res = true;
		return res;
	}

	bool GPCHookManager::HookVK(const char* pModule) {
		bool res = true;
		return res;
	}

	bool GPCHookManager::HookOGL(const char* pModule) {
		bool res = true;
		return res;
	}

	void GPCHookManager::LoadAPIHookInfos(fs::path vtablePath) {
		//pre-defined for entry api
		g_apiHookInfos["CreateDXGIFactory"] = std::make_shared<GPCAPIHookInfo>("CreateDXGIFactory");
		g_apiHookInfos["CreateDXGIFactory1"] = std::make_shared<GPCAPIHookInfo>("CreateDXGIFactory1");
		g_apiHookInfos["CreateDXGIFactory2"] = std::make_shared<GPCAPIHookInfo>("CreateDXGIFactory2");
		g_apiHookInfos["D3D12CreateDevice"] = std::make_shared<GPCAPIHookInfo>("D3D12CreateDevice");

		//debug purpose
		g_apiHookInfos["CreateSwapChain"] = std::make_shared<GPCAPIHookInfo>("IDXGIFactory", "CreateSwapChain", 10);
		g_apiHookInfos["CreateSwapChainForHwnd"] = std::make_shared<GPCAPIHookInfo>("IDXGIFactory2", "CreateSwapChainForHwnd", 15);
		g_apiHookInfos["CreateSwapChainForCoreWindow"] = std::make_shared<GPCAPIHookInfo>("IDXGIFactory2", "CreateSwapChainForCoreWindow", 16);
		g_apiHookInfos["CreateSwapChainForComposition"] = std::make_shared<GPCAPIHookInfo>("IDXGIFactory2", "CreateSwapChainForComposition", 24);
		g_apiHookInfos["Present"] = std::make_shared<GPCAPIHookInfo>("IDXGISwapChain", "Present", 8);
		g_apiHookInfos["Present1"] = std::make_shared<GPCAPIHookInfo>("IDXGISwapChain1", "Present1", 22);
		g_apiHookInfos["CreateCommandQueue"] = std::make_shared<GPCAPIHookInfo>("ID3D12Device", "CreateCommandQueue", 8);
		g_apiHookInfos["CreateCommandList"] = std::make_shared<GPCAPIHookInfo>("ID3D12Device", "CreateCommandList", 12);
		g_apiHookInfos["CreateCommandList1"] = std::make_shared<GPCAPIHookInfo>("ID3D12Device4", "CreateCommandList1", 51);

		if (fs::exists(vtablePath)) {
			std::cout << "Start to load vtable" << std::endl;
			//member methods
			std::ifstream ifs(vtablePath);
			jsoncons::csv::csv_options options;
			options.assume_header(true);
			jsoncons::ojson data = jsoncons::csv::decode_csv<jsoncons::ojson>(ifs, options);
			int rowIndex = 0;
			for (jsoncons::ojson& row : data.array_range()) {
				//std::cout << "row index: " << rowIndex++ << std::endl;
				//std::string interfaceName = row["interface"].as_string();
				//std::cout << "interface: " << interfaceName << std::endl;
				//std::string methodName = row["method"].as_string();
				//std::cout << "method: " << methodName << std::endl;
				//int vtableOffset = row["offset"].as_int();
				//GPC_HOOK_POS hookPos = (GPC_HOOK_POS)row["hook"].as_int();
				//std::cout << "interface: " << interfaceName << "\tmethodName: " << methodName << "\tvtable offset: " << vtableOffset << "\thookPos: " << hookPos << std::endl;
				//g_apiHookInfos[methodName] = std::make_shared<GPCAPIHookInfo>(interfaceName.c_str(), methodName.c_str(), vtableOffset, hookPos);
			}
		}
	}
}
