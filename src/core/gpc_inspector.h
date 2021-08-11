#pragma once
#include "utils/gpc_utils.h"
#include "core/tracker/gpc_swapchain_tracker.h" 
#include "core/gpc.h" 

extern std::unique_ptr<gpc::GPUProfilerCore> g_GPUProfilerCore;

namespace gpc {
	//forward declarations
	class GPCInspectorManager; //manage GPCFrameInspector and GPCMemoryInspector
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
			m_invFreq = 1.0 / freq.QuadPart;
			LARGE_INTEGER time;
			QueryPerformanceCounter(&time);
			m_startInjectedTime = time.QuadPart;
			m_fpsStartTime = time.QuadPart;
		}
		~GPCFPSInspector() {
			m_fpsLog.close();
		}

		virtual void StartFrame() {
			LARGE_INTEGER frameStartTime;
			QueryPerformanceCounter(&frameStartTime);
			m_frameStartTime = frameStartTime.QuadPart;
		}

		virtual void EndFrame() {
			//update fps every seconds
			LARGE_INTEGER frameEndTime;
			QueryPerformanceCounter(&frameEndTime);
			auto fpsElapsedTime = (frameEndTime.QuadPart - m_fpsStartTime) * m_invFreq;
			m_fpsFrameCount++;
			std::cout << "fps elapsed time: " << fpsElapsedTime << "\tfps framecount: " << m_fpsFrameCount << std::endl;
			if (fpsElapsedTime > 1.0) {
				auto fps = (double)(1.0 / fpsElapsedTime) * m_fpsFrameCount;
				m_fpsLog << fpsElapsedTime / m_fpsFrameCount << "," << fps << "\n";
				m_lastFPS = fps;
				m_fpsStartTime = frameEndTime.QuadPart;
				m_fpsFrameCount = 0;
			}
			m_totalInjectedTimeNS = (frameEndTime.QuadPart - m_startInjectedTime) * m_invFreq;
		}

		double GetLastFPS() {
			return m_lastFPS;
		}

		double GetTotalInjectedTime() {
			return m_totalInjectedTimeNS;
		}

	private:
		std::ofstream           m_fpsLog;
		double					m_frameStartTime = 0;
		double					m_fpsStartTime = 0;
		double					m_fpsFrameCount = 0;
		double                  m_fpsUpdateThreshold = 1.0; //update fps every seconds
		double                  m_invFreq = 1.0;
		double                  m_lastFPS = 0.0;
		double					m_totalInjectedTimeNS = 0;
		double					m_startInjectedTime = 0;
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

			auto pSwapChainTracker = GPCDXGISwapChainTracker::GetSingleton();
			pSwapChainTracker->OnFrameEnd();
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

		//DXGI_QUERY_VIDEO_MEMORY_INFO GetVidMemInfo() {
		//	auto pAdapter = GPCDXGISwapChainTracker::GetSingleton()->GetDXGIAdapter();
		//	pAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &m_vidMemInfo);
		//	return m_vidMemInfo;
		//}

		VidMemInfo GetVidMemInfo() {
			auto pAdapter = GPCDXGISwapChainTracker::GetSingleton()->GetDXGIAdapter();
			DXGI_QUERY_VIDEO_MEMORY_INFO localVidMemInfo = {};
			DXGI_QUERY_VIDEO_MEMORY_INFO nonLocalVidMemInfo = {};
			HRESULT hrLocal = pAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &localVidMemInfo);
			HRESULT hrNonLocal = pAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &nonLocalVidMemInfo);

			if (SUCCEEDED(hrLocal))
			{
				m_vidMemInfo.m_D3D12UsageLocal = localVidMemInfo.CurrentUsage;
				m_vidMemInfo.m_D3D12BudgetLocal = localVidMemInfo.Budget;
			}

			if (SUCCEEDED(hrNonLocal))
			{
				m_vidMemInfo.m_D3D12UsageNonLocal = nonLocalVidMemInfo.CurrentUsage;
				m_vidMemInfo.m_D3D12BudgetNonLocal = nonLocalVidMemInfo.Budget;
			}

			return m_vidMemInfo;
		}

	private:
		//DXGI_QUERY_VIDEO_MEMORY_INFO m_vidMemInfo;
		VidMemInfo m_vidMemInfo;
	};

	class GPCInspectorManager : public GPCSingleton<GPCInspectorManager> {
	public:
		GPCInspectorManager() {}
		~GPCInspectorManager() {}

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