/*
* every application has it's own GPCProfilerCore
* hookManager is controlled by gpu_profiler(client), after eject dll(doing unhook), new process won't get injected, but those hooked process will start to use real api
* processManager is controlled by gpu_profiler(inject dll globally, also eject dll globally after exiting)
*/
#include "gpc.h"
#include "gpc_shared.h"

#pragma data_seg ("SHARED")
//note: need use fixed size variable with proper initialization
LIBRARY_API GPCSharedMemory g_GPCSharedMemory = {};
#pragma data_seg ()
#pragma comment(linker, "/section:SHARED,RWS")

//global variable for each process
extern std::unique_ptr<gpc::GPUProfilerCore> g_GPUProfilerCore;

using namespace gpc;

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpReserved)  // reserved
{
	std::string processName = g_GPUProfilerCore->m_processManager->GetCurrentProcessName();
	std::cout << "Dll lock count: " << g_GPCSharedMemory.dllLoadCount++ << "\tcurrent process name: " << processName << std::endl;
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: {
		strcpy(g_GPCSharedMemory.lastHookedProcessName, processName.c_str());
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		std::cout << "\nDLL_PROCESS_ATTACH: " << processName << std::endl;
		g_GPUProfilerCore->m_processManager->AddProcess(processName.c_str());
		if (g_GPUProfilerCore->m_processManager->IsValidProcess(processName))
		{
			//incase setup multi times for one process
			int hookCount = g_GPUProfilerCore->m_processManager->GetProcessHookCount(processName.c_str());
			if (hookCount > 1) {
				std::cout << "Dll already injected into process: " << processName << "\thook count: " << hookCount << std::endl;
				g_GPUProfilerCore->m_processManager->AddProcessHookCount(processName.c_str());
				break;
			}
			else {
				std::cout << "First time to inject dll to process: " << processName << std::endl;
				g_GPUProfilerCore->m_processManager->AddProcessHookCount(processName.c_str());
				if (g_GPUProfilerCore->SetUpGPC(hinstDLL))
					break;
				else
					std::cout << "Set up gpu profiler core failed, detach dll" << std::endl;
			}
		}
		else {
			std::cout << processName << " is in the black list, detach dll" << std::endl;
		}
	}

	case DLL_PROCESS_DETACH: {
		// Perform any necessary cleanup.
		std::cout << "\nDLL_PROCESS_DETACH: " << processName << std::endl;
		if (g_GPUProfilerCore->m_processManager->IsValidProcess(processName)) {
			g_GPUProfilerCore->TearDownGPC();
		}
		break;
	}

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		//TODO: use flags to prevent thread hook?
		std::cout << "\nDLL_THREAD_ATTACH: " << processName << std::endl;
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		std::cout << "\nDLL_THREAD_DETACH: " << processName << std::endl;
		break;

	default:
		std::cout << "Invalid process: " << processName << std::endl;
		g_GPUProfilerCore->m_processManager->AddToBlackList(processName.c_str());
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

const char* CBT_TYPE[10] = {
	"HCBT_MOVESIZE",
	"HCBT_MINMAX",
	"HCBT_QS",
	"HCBT_CREATEWND",
	"HCBT_DESTROYWND",
	"HCBT_ACTIVATE",
	"HCBT_CLICKSKIPPED",
	"HCBT_KEYSKIPPED",
	"HCBT_SYSCOMMAND",
	"HCBT_SETFOCUS"
};

extern "C" {
	LIBRARY_API LRESULT CALLBACK GPCHookEntry(
		_In_ int    nCode,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam
	);
}

/*reference:
https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-callnexthookex
* CallNextHookEx:Pass the hook information to the next procedure in the current hook chain
https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644977(v=vs.85)
* CBTProc callback(Computer Based Training):
*/
LRESULT CALLBACK GPCHookEntry(
	_In_ int    nCode,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
) {
	std::cout << "CBT type is: " << CBT_TYPE[nCode] << std::endl;
	return CallNextHookEx(NULL, nCode, wParam, lParam); //note: the first parameter is ignored
}

LIBRARY_API GPCSharedMemory GetGPCSharedMemory() {
	return g_GPCSharedMemory;
}

LIBRARY_API void SetGPCSharedMemory(GPCSharedMemory sharedMemory) {
	g_GPCSharedMemory = sharedMemory;
}



