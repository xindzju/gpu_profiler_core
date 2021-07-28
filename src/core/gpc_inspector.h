#pragma once
#include "utils/gpc_utils.h"
#include "core/tracker/gpc_swapchain_tracker.h" 
#include "core/gpc.h" 

extern std::unique_ptr<gpc::GPUProfilerCore> g_GPUProfilerCore;

namespace gpc {
	//forward declarations
	class GPCInpectorManager; //manage GPCFrameInspector and GPCMemoryInspector
	class GPCInspector; //interfaces
	class GPCFrameInspector;
	class GPCMemoryInspector;
	class GPCFPSInspector; //member of GPCFrameInspector

	class GPCInspector {
	public:
		virtual void StartFrame() = 0;
		virtual void EndFrame() = 0;
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

		virtual void StartFrame() {
			LARGE_INTEGER time;
			QueryPerformanceCounter(&time);
			m_frameStartTime = time.QuadPart;
		}

		virtual void EndFrame() {
			LARGE_INTEGER stop;
			QueryPerformanceCounter(&stop);
			auto frametimeMS = (stop.QuadPart - m_frameStartTime) * m_invFreq / 1e6;
			auto fps = int(1 / frametimeMS);
			m_fpsLog << frametimeMS << "," << fps << "\n";
		}
	private:
		std::ofstream           m_fpsLog;
		uint64_t                m_frameStartTime = 0;
		double                  m_invFreq = 1.0;
	};

	struct GPCFrameState {
		int m_iFrameIndex;
	};

	struct GPCProfileState {
		bool m_bProfile;
	};

	struct GPCGlobalData {

	};

	//frame source inspector
	class GPCFrameInspector : public GPCInspector {
	public:
		GPCFrameInspector() {};
		~GPCFrameInspector() {};

		//after present: frame start entry
		virtual void StartFrame() {
			if (m_fpsInspector == nullptr)
				m_fpsInspector = new GPCFPSInspector();
			m_fpsInspector->StartFrame();

			GPCDXGISwapChainTracker::GetSingleton()->OnFrameStart();
			g_GPUProfilerCore->OnProfileStart();
		}

		//before present: frame end entry
		virtual void EndFrame() {
			if (m_fpsInspector == nullptr)
				m_fpsInspector = new GPCFPSInspector();
			m_fpsInspector->EndFrame();

			GPCDXGISwapChainTracker::GetSingleton()->OnFrameEnd();
			g_GPUProfilerCore->OnProfileEnd();
			//frame state controller, communicate with client
			//shared memory or using ocat methodology
		}

		GPCFPSInspector* m_fpsInspector = nullptr;
	};

	//memory inspector
	class GPCMemoryInspector : public GPCInspector {
	public:
		GPCMemoryInspector() {}
		~GPCMemoryInspector() {}

		virtual void StartFrame() {

		}
		virtual void EndFrame() {

		}

		DXGI_QUERY_VIDEO_MEMORY_INFO GetVidMemInfo() {
			auto pAdaptor = GPCDXGISwapChainTracker::GetSingleton()->GetDXGIAdaptor();
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