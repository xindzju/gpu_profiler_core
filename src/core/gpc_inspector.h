#pragma once
#include "gpc_utils.h"

namespace gpc {
	class GPCInspector {

	};

	//fps inspector
	class GPCFPSInspector : public GPCInspector {
	public:
		GPCFPSInspector() {
			fs::path fpsLogPath = fs::current_path() / "fps.csv";
			m_fpsLog.open(fpsLogPath);
			m_fpsLog << "frametime,fps\n";
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			m_invFreq = 1e6 / freq.QuadPart;
			LARGE_INTEGER time;
			QueryPerformanceCounter(&time);
			m_frameStartTime = time.QuadPart;
		}
		~GPCFPSInspector() {
			m_fpsLog.close();
		}

		void OnFrameEnd() {
			LARGE_INTEGER stop;
			QueryPerformanceCounter(&stop);
			auto frametimeMS = (stop.QuadPart - m_frameStartTime) * m_invFreq / 1e6;
			auto fps = int(1 / frametimeMS);
			m_fpsLog << frametimeMS << "," << fps << "\n";
		}

		void OnFrameStart() {
			LARGE_INTEGER time;
			QueryPerformanceCounter(&time);
			m_frameStartTime = time.QuadPart;
		}

	private:
		std::ofstream           m_fpsLog;
		uint64_t                m_frameStartTime = 0;
		double                  m_invFreq = 1.0;
	};

	//resource inspector
	class GPCResourceInspector : public GPCInspector {

	};

    //memory inspector
	class GPCMemoryInspector : public GPCInspector {
		//both sysmem and vidmem
	};

	//frame source inspector
	class GPCFrameInspector : public GPCInspector {
	public:
		GPCFrameInspector() {
			m_fpsInspector = std::make_unique<GPCFPSInspector>();
			m_resourceInspector = std::make_unique<GPCResourceInspector>();
			m_memoryInspector = std::make_unique<GPCMemoryInspector>();
		};
		~GPCFrameInspector() {};

		//after present
		void OnFrameStart() {
			m_fpsInspector->OnFrameStart();
		}

		//before present
		void OnFrameEnd() {
			m_fpsInspector->OnFrameEnd();
		}

		std::unique_ptr<GPCFPSInspector> m_fpsInspector;
		std::unique_ptr<GPCResourceInspector> m_resourceInspector;
		std::unique_ptr<GPCMemoryInspector> m_memoryInspector;
	};

	class GPCInpectorManager {
	public:
		GPCInpectorManager() {
			m_frameInspector = std::make_unique<GPCFrameInspector>();
		}
		~GPCInpectorManager() {}

		std::unique_ptr<GPCFrameInspector> m_frameInspector;
	};
}