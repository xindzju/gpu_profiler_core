#pragma once
#include "utils/gpc_utils.h"

namespace gpc {
	class GPCProcessManager {
    public:
        GPCProcessManager();
        ~GPCProcessManager();

        bool IsValidProcess(std::string processName = "");
        std::string GetProcessNameFromHandle(HANDLE handle);
        std::string GetProcessName();

        int GetProcessHookCount(const char* processName = nullptr) {
            if (!processName) 
                return 0;
            return m_hookedProcesses[processName];
        }
        void AddProcessHookCount(const char* processName = nullptr) {
            if (processName)
                m_hookedProcesses[processName]++;
        }

        //add/delete process
        void AddProcess(const char* processName = nullptr) {
            if (!processName)
                return;
            m_hookedProcesses[processName] = 1;
            std::cout << "Hooked process number: " << m_hookedProcesses.size() << std::endl;
        }
        void DeleteProcess(const char* processName = nullptr) {
            if (!processName)
                return;
            m_hookedProcesses.erase(processName);
            std::cout << "Hooked process number: " << m_hookedProcesses.size() << std::endl;
        }

        void LoadBlackList(fs::path blacklistFile = "");
        void DumpBlackList();
        void AddToBlackList(const char* processName = nullptr) {
            if (!processName)
                return;
            m_blackListSet.insert(processName);
        }
        void DeleteFromBlackList(const char* processName = nullptr) {
            if (!processName)
                return;
            m_blackListSet.erase(processName);
        }

    private:
        std::map<std::string, int> m_hookedProcesses = {}; //process_name: hook_count
        std::set<std::string> m_blackListSet;
        fs::path m_blackListFile;
	};
}