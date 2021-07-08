#pragma once
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <filesystem>
#include <regex>
#include <shared_mutex>

namespace fs = std::filesystem;

#ifdef _WIN32
#include <Windows.h>
#include <Psapi.h>
#else
#endif

#include "..\core\gpc_struct.h"

namespace gpc {
	//singleton interface
    template<class T>
    class GPCSingleton
    {
    public:
        static T* GetSingleton()
        {
            if (!s_pSingleton)
            {
                if (!s_pSingleton)
                    s_pSingleton = new T();
            }
            return s_pSingleton;
        }
    protected:
        static inline T* s_pSingleton = nullptr;
    };

    struct GPCReadWriteLock {
        //multi reader, single writer
        //shared_mutex, shared_lock, unique_lock
    };

	namespace utils {
		std::string GetProcessName();
		fs::path GetDllPath(const char* dllName);

		bool LoadLib(const char* lib);
		void FreeLib(void* lib);
	}

	namespace profile {
		//func profiling
	}
}