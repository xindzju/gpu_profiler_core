#include "gpc_utils.h"

namespace gpc {
	namespace utils {
        std::string GetCurrentProcessName() {
            char modulePath[1024];
            auto res = GetModuleFileName(nullptr, modulePath, 1024);
            if (res == 0) {
                std::cout << "GetProcess Name failed, error code: " << GetLastError() << std::endl;
                return "System process";
            }
            std::string processName = fs::path(modulePath).filename().string();
            return processName;
        }

        bool LoadLib(const char* lib) {
            bool res = true;
#ifdef _WIN32
            HMODULE module = LoadLibrary(lib);
            if (!module)
                res = false;
#else
			void* handle = dlopen(lib, RTLD_LAZY);
			if (!handle)
				res = false;
#endif
            return res;
        }

        void FreeLib(void* lib) {
#ifdef _WIN32
            FreeLibrary((HMODULE)lib);
#else
            dlclose(module)
#endif
        }

        fs::path GetDllPath(const char* dllName) {
#ifdef _WIN32
            HMODULE module = LoadLibrary(dllName);
            char modulePath[1024];
            GetModuleFileName(module, modulePath, 1024);
            return modulePath;
#else
#endif
        }
	}
}