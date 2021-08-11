#pragma once
#include "gpc_utils.h"
#ifdef _WIN32
#include <Windows.h>
#include <tchar.h>
#include <Pdh.h>
#include <Psapi.h>
#else
#endif

//reference: https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

namespace gpc {
	class GPCMetadata {
	public:
		GPCMetadata();
		~GPCMetadata();

		void QueryHardwareInfo();
		void QuerySoftwareInfo();
		void QueryProcessUsageInfo();

		void QueryCPUInfo();
		void QueryGPUInfo();
		void QuerySysMemInfo();
		void QueryVidMemInfo();
		void QueryProcessCPUUsage();
		void QueryProcessGPUUsage();
		void QueryProcessSysMemUsage();
		void QueryProcessVidMemUsage();

	private:
		//hardware info
		std::string m_OS;
		std::string m_CPUName;
		std::string m_GPUName;
		std::string m_totalSysMem;
		std::string m_totalVidMem;

		//process usage info
		std::string m_processUsedCPU;
		std::string m_processUsedGPU;
		std::string m_processUsedSysMem;
		std::string m_processUsedVidMem;
		
		//software
		std::string m_driverVersion;
		std::string m_appName;
		std::string m_resolution;
	};
}