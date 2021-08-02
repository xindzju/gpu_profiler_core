#include "gpc_hook_manager.h"

namespace gpc {

	GPCHookManager::~GPCHookManager() {
	}

	bool GPCHookManager::Hook(HOOK_API_TYPE hookType) {
		bool res = true;
		switch (hookType) {
		case HOOK_API_TYPE::HOOK_API_TYPE_DX:
			res = HookDXGI("dxgi.dll") && HookD3D12("d3d12.dll") && HookSys();
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
		return HookDXGIEntry(dllName);
	}

	bool GPCHookManager::HookD3D12(const char* dllName) {
		return HookD3D12Entry(dllName);
	}

	bool GPCHookManager::HookSys() {
		return HookSysEntry();
	}

	bool GPCHookManager::HookCUDA(const char* dllName) {
		bool res = true;
		return res;
	}

	bool GPCHookManager::HookVK(const char* dllName) {
		bool res = true;
		return res;
	}

	bool GPCHookManager::HookOGL(const char* dllName) {
		return HookOGLEntry(dllName);
	}

	void GPCHookManager::LoadAPIHookInfos(fs::path vtablePath) {
		//pre-defined for entry api
		g_apiHookInfos["CreateDXGIFactory"] = std::make_shared<GPCAPIHookInfo>("CreateDXGIFactory");
		g_apiHookInfos["CreateDXGIFactory1"] = std::make_shared<GPCAPIHookInfo>("CreateDXGIFactory1");
		g_apiHookInfos["CreateDXGIFactory2"] = std::make_shared<GPCAPIHookInfo>("CreateDXGIFactory2");
		g_apiHookInfos["D3D12CreateDevice"] = std::make_shared<GPCAPIHookInfo>("D3D12CreateDevice");
		g_apiHookInfos["CreateProcessA"] = std::make_shared<GPCAPIHookInfo>("CreateProcessA");
		g_apiHookInfos["CreateProcessW"] = std::make_shared<GPCAPIHookInfo>("CreateProcessW");

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
