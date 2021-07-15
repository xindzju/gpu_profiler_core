#include "gpc.h"

std::unique_ptr<gpc::GPUProfilerCore> g_GPUProfilerCore = std::make_unique<gpc::GPUProfilerCore>();

namespace gpc {
    HMODULE                             g_injectorDllModule = nullptr;
    std::string                         g_injectorDllName = "";
    std::filesystem::path               g_injectorDllPath = "";

	GPUProfilerCore::GPUProfilerCore() {
		std::cout << "Create GPU Profiler Core: " << utils::GetProcessName() << std::endl;
		if (m_processManager->GetProcessName().compare("gpu_profiler.exe") == 0)
			InitSharedMemory();
		m_processManager = std::make_unique<GPCProcessManager>();
		std::string processName = m_processManager->GetProcessName();
		if (m_processManager->IsValidProcess(processName)) {
			m_hookManager = std::make_unique<GPCHookManager>();
			m_inspectorManager = std::make_unique<GPCInpectorManager>();
		}
	}

	GPUProfilerCore::~GPUProfilerCore() {}

    bool GPUProfilerCore::SetUpGPC(HMODULE hModule) {
		std::cout << "Start to set up gpu profiler core" << std::endl;
        bool res = true;
		char modulePath[1024];
		if (g_injectorDllPath.empty()) {
			GetModuleFileName(hModule, modulePath, 1024);
			g_injectorDllModule = hModule;
			g_injectorDllPath = modulePath;
			g_injectorDllName = g_injectorDllPath.filename().string();
		}

		
		//set up hook
		HOOK_API_TYPE hookAPIType = HOOK_API_TYPE::HOOK_API_TYPE_DX;
		if (m_hookManager->Hook(hookAPIType))
			std::cout << GetHookAPITypeStr(hookAPIType) << " hooked successfully" << std::endl;
		else {
			std::cout << GetHookAPITypeStr(hookAPIType) << " hooked failed" << std::endl;
			res = false;
		}
		

		//set up logger
		return res;
    }

    void GPUProfilerCore::TearDownGPC() {
        std::cout << "Start to tear down gpu profiler core" << std::endl;
        m_hookManager->UnHook();
        return;
    }
}