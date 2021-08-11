#pragma once
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <regex>
#include <shared_mutex>

#ifdef _WIN32
#include "Windows.h"
#include <Psapi.h> //process related api
#include <wrl.h> //COM Ptr
#else
#endif

#include "cpp_utils.h"
using namespace cutils;

namespace fs = std::filesystem;
template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace gpc {
	//singleton interface
    template<class T>
    class GPCSingleton
    {
    public:
        static T* GetSingleton()
        {
			static T* s_pSingleton = nullptr; 
            if (!s_pSingleton)
            {
                s_pSingleton = new T();
            }
            return s_pSingleton;
        }
    };

    struct GPCReadWriteLock {
        //multi reader, single writer
        //shared_mutex, shared_lock, unique_lock
    };

	namespace utils {
		std::string GetCurrentProcessName();
		fs::path GetDllPath(const char* dllName);

		bool LoadLib(const char* lib);
		void FreeLib(void* lib);
	}

	namespace profile {
		//func profiling
	}

    template<typename T>
    inline T GetAlignedInteger(T size, T alignment)
    {
        return  alignment * ((size - 1) / alignment + 1); // (size + (alignment - 1)) & ~(alignment - 1);
    }
}