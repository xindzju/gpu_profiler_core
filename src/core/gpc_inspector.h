#pragma once
#include "utils/gpc_utils.h"
#include "core/tracker/gpc_swapchain_tracker.h"

namespace gpc {
	//forward declarations
	class GPCInpectorManager; //manage GPCFrameInspector and GPCMemoryInspector
	class GPCInspector; //interfaces
	class GPCFrameInspector;
	class GPCMemoryInspector;
	class GPCFPSInspector; //member of GPCFrameInspector

	class GPCInspector {
	public:
		virtual void OnFrameStart() = 0;
		virtual void OnFrameEnd() = 0;
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

		virtual void OnFrameEnd() {
			LARGE_INTEGER stop;
			QueryPerformanceCounter(&stop);
			auto frametimeMS = (stop.QuadPart - m_frameStartTime) * m_invFreq / 1e6;
			auto fps = int(1 / frametimeMS);
			m_fpsLog << frametimeMS << "," << fps << "\n";
		}

		virtual void OnFrameStart() {
			LARGE_INTEGER time;
			QueryPerformanceCounter(&time);
			m_frameStartTime = time.QuadPart;
		}

	private:
		std::ofstream           m_fpsLog;
		uint64_t                m_frameStartTime = 0;
		double                  m_invFreq = 1.0;
	};

	//frame source inspector
	class GPCFrameInspector : public GPCInspector {
	public:
		GPCFrameInspector() {};
		~GPCFrameInspector() {};

		//after present: frame start entry
		virtual void OnFrameStart() {
			if (m_fpsInspector == nullptr)
				m_fpsInspector = new GPCFPSInspector();
			m_fpsInspector->OnFrameStart();

			auto pSwapChainTracker = GPCDXGISwapChainTrackerManager::GetSingleton()->GetSwapChainTracker();
			pSwapChainTracker->OnFrameStart();
		}

		//before present: frame end entry
		virtual void OnFrameEnd() {
			if (m_fpsInspector == nullptr)
				m_fpsInspector = new GPCFPSInspector();
			m_fpsInspector->OnFrameEnd();

			auto pSwapChainTracker = GPCDXGISwapChainTrackerManager::GetSingleton()->GetSwapChainTracker();
			pSwapChainTracker->OnFrameEnd();
		}

		GPCFPSInspector* m_fpsInspector = nullptr;
	};

	//memory inspector
	class GPCMemoryInspector : public GPCInspector {
	public:
		GPCMemoryInspector() {}
		~GPCMemoryInspector() {}

		virtual void OnFrameStart() {

		}
		virtual void OnFrameEnd() {

		}

		DXGI_QUERY_VIDEO_MEMORY_INFO GetVidMemInfo() {
			auto pAdaptor = GPCDXGISwapChainTrackerManager::GetSingleton()->GetSwapChainTracker()->GetDXGIAdaptor();
			pAdaptor->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &m_vidMemInfo);
			return m_vidMemInfo;
		}

	private:
		DXGI_QUERY_VIDEO_MEMORY_INFO m_vidMemInfo;
	};

	class GPCInpectorManager : public GPCSingleton<GPCInpectorManager> {
	public:
		GPCInpectorManager() {}
		~GPCInpectorManager() {}

		GPCFrameInspector* GetFrameInspector() {
			if (m_frameInspector == nullptr)
				m_frameInspector = new GPCFrameInspector();
			return m_frameInspector;
		}

		GPCMemoryInspector* GetMemoryInspector() {
			if (m_memoryInspector == nullptr)
				m_memoryInspector = new GPCMemoryInspector();
			return m_memoryInspector;
		}

	private:
		GPCFrameInspector* m_frameInspector = nullptr;
		GPCMemoryInspector* m_memoryInspector = nullptr;
	};
}