#include "gpc_process_manager.h"

//extern GPCSharedMemory g_GPCSharedMemory;

namespace gpc {
	GPCProcessManager::GPCProcessManager() {
		std::cout << "Create GPC process manager: " << GetCurrentProcessName() << std::endl;
		m_blackListFile = g_GPCSharedMemory.blacklistPath;
		LoadBlackList(m_blackListFile);
	}

	GPCProcessManager::~GPCProcessManager() {
		DumpBlackList();
	}

	bool GPCProcessManager::IsValidProcess(std::string processName) {
		bool res = true;
		if (processName.empty())
			return false;
		// convert string to back to lower case
		std::for_each(processName.begin(), processName.end(), [](char& c) {c = ::tolower(c);});
		if (m_blackListSet.find(processName) != m_blackListSet.end()) {
			res = false;
		}
		else {
			res = true;
		}
		return res;
	}

	std::string GPCProcessManager::GetProcessNameFromHandle(HANDLE handle)
	{
		std::string buffer;
		HMODULE module;
		DWORD moduleArraySize = 0;
		DWORD nameLength = 0;
		DWORD bufferSize = 1024;
		if (EnumProcessModules(handle, &module, sizeof(module), &moduleArraySize))
		{
			do
			{
				buffer.resize(bufferSize);
				nameLength = GetModuleBaseName(handle, module, &buffer[0], static_cast<DWORD>(buffer.size()));
				if (!nameLength)
				{
					std::cout << "GetAbsolutePath - Unable to get name." << GetLastError() << std::endl;
					return "";
				}
				bufferSize *= 2;
			}
			// if same size, name is likely truncated, double buffer size
			while (nameLength == buffer.size());
		}
		else
		{
			std::cout << "GetAbsolutePath - Unable to enumerate process hModules." << GetLastError() << std::endl;
		}
		return buffer.substr(0, nameLength);
	}

	std::string GPCProcessManager::GetCurrentProcessName() {
		char modulePath[1024];
		auto res = GetModuleFileName(nullptr, modulePath, 1024);
		if (res == 0) {
			std::cout << "GetProcess Name failed, error code: " << GetLastError() << std::endl;
			return "System process";
		}
		std::string processName = fs::path(modulePath).filename().string();
		if (processName.find(".exe") == std::string::npos)
			processName += ".exe";
		return processName;
	}

	void GPCProcessManager::LoadBlackList(fs::path blacklistFile) {
		std::cout << "Start to load process blacklist: " << m_blackListFile.string() << std::endl;
		if (fs::exists(blacklistFile))
			m_blackListFile = blacklistFile;
		if (fs::exists(m_blackListFile)) {
			std::ifstream ifs(m_blackListFile);
			if (ifs.good()) {
				std::string processName;
				while (std::getline(ifs, processName)) {
					if (processName.find(".exe") == std::string::npos)
						processName += ".exe";
					std::for_each(processName.begin(), processName.end(), [](char& c) {c = ::tolower(c); });
					m_blackListSet.insert(processName);
				}
			}
		}
		else {
			std::cout << "Can't find the process black list file: " << m_blackListFile.string() << std::endl;
		}
		return;
	}

	void GPCProcessManager::DumpBlackList() {
		std::cout << "Start to dump process blacklist" << std::endl;
		std::ofstream ofs(m_blackListFile);
		if (ofs.good()) {
			for (auto processName : m_blackListSet)
				ofs << processName << "\n";
		}
	}
}

