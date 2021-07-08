/*
* The GPU Profiler Core is a library which used to profile GPU performance, memory and shaders of D3D12/CUDA applications
* Framework: https://app.diagrams.net/#G1mf4-nKd5mJUAEmCJaKEXi7mqRiZrUhvY
*/
#pragma once
#include <iostream>
#include <filesystem>
#include <string>
#include <set>
#include <map>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include "Windows.h"
#else
#endif

#include "core/gpc_defines.h"
#include "core/gpc_process_manager.h"
#include "core/gpc_shared.h" 
#include "utils/gpc_utils.h"
#include "utils/gpc_vtable_parser.h"
#include "hook/gpc_hook_manager.h"

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
    };
}

extern std::unique_ptr<gpc::GPUProfilerCore> g_GPUProfilerCore; //each injected process has it's own instance