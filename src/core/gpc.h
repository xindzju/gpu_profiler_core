/*
* The GPU Profiler Core is a library which used to profile GPU performance, memory and shaders of D3D12/CUDA applications
* Framework: https://app.diagrams.net/#G1mf4-nKd5mJUAEmCJaKEXi7mqRiZrUhvY
* Methodology: app => gpc inspector => gpc analyzer => gpc optimizer
*/
#pragma once
#include "core/gpc_internal.h"

/*
* Note: when gpc used as a static library, file name with prefix "gpc_" should be input into the include folder
*/

namespace gpc {
    extern HMODULE                 g_injectorDllModule;
    extern std::string             g_injectorDllName;
    extern std::filesystem::path   g_injectorDllPath;

    class GPUProfilerCore { 
    public:
        GPUProfilerCore();
        ~GPUProfilerCore();

        //gpc main entries
        bool SetUpGPC(HMODULE hModule);
        void TearDownGPC();

        std::unique_ptr<GPCProcessManager> m_processManager; 
        std::unique_ptr<GPCHookManager> m_hookManager; //set hook by each application for low overhead(deffer hook)
        std::unique_ptr<GPCInpectorManager> m_inspectorManager;
    private:
        void InitSharedMemory() {
            std::cout << "Init dll shared memory" << std::endl;
            memset(&g_GPCSharedMemory, 0, sizeof(GPCSharedMemory));
            fs::path vtablePath = fs::current_path() / "vtable.csv";
            memcpy(g_GPCSharedMemory.vtablePath, vtablePath.string().c_str(), MAX_PATH_LEN);
            fs::path blacklistPath = fs::current_path() / "blacklist.txt";
            memcpy(g_GPCSharedMemory.blacklistPath, blacklistPath.string().c_str(), MAX_PATH_LEN);
        }
    };
}

extern std::unique_ptr<gpc::GPUProfilerCore> g_GPUProfilerCore; //each injected process has it's own instance