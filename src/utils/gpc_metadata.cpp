#include "utils/gpc_metadata.h"

namespace gpc {
	std::string m_motherBoard;
	std::string m_OS;
	std::string m_CPU;
	std::string m_sysMem;
	std::string m_driverVersion;
	std::string m_GPU;
	std::string m_vidMem;
	std::string m_vidMemSize;

	//software
	std::string m_appName;
	std::string m_resolution;
	GPCMetadata::GPCMetadata() {

	}

	GPCMetadata::~GPCMetadata() {

	}

	void GPCMetadata::QueryHardwareInfo() {
		QueryCPUInfo();
		QueryGPUInfo();
		QuerySysMemInfo();
		QueryVidMemInfo();
	}

	void GPCMetadata::QuerySoftwareInfo() {

	}

	void GPCMetadata::QueryProcessUsageInfo() {
		QueryProcessCPUUsage();
		QueryProcessGPUUsage();
		QueryProcessSysMemUsage();
		QueryProcessVidMemUsage();
	}

	void GPCMetadata::QueryCPUInfo() {
		static PDH_HQUERY cpuQuery;
		static PDH_HCOUNTER cpuTotal;

		auto init = []() {
			PdhOpenQuery(NULL, NULL, &cpuQuery);
			// You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
			PdhAddEnglishCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
			PdhCollectQueryData(cpuQuery);
		};

		auto getCurrentValue = []() {
			PDH_FMT_COUNTERVALUE counterVal;

			PdhCollectQueryData(cpuQuery);
			PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
			return counterVal.doubleValue;
		};
	}

	void GPCMetadata::QueryGPUInfo() {

	}

	void GPCMetadata::QuerySysMemInfo() {
		//total virtual memory
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;

		//virtual memory currently used by current process
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;

		//total physical memory
		DWORDLONG totalPhysMem = memInfo.ullTotalPhys;

		//total physical memory  currently used
		DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;

		//total physical memory currently used by current process
		SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
	}

	void GPCMetadata::QueryVidMemInfo() {
	}

	void GPCMetadata::QueryProcessCPUUsage() {

	}

	void GPCMetadata::QueryProcessGPUUsage() {
	}

	void GPCMetadata::QueryProcessSysMemUsage() {
	}


	void GPCMetadata::QueryProcessVidMemUsage() {

	}
}