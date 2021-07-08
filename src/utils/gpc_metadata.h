#pragma once
#include "gpc_utils.h"

namespace gpc {
	class GPCMetadata {
	public:
		GPCMetadata();
		~GPCMetadata();

	private:
		//hardware
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
	};
}