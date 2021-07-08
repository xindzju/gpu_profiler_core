#pragma once
#include "gpc_defines.h"
#include "gpc_process_manager.h"

#ifdef _WIN32
#    ifdef LIBRARY_EXPORTS
#        define LIBRARY_API __declspec(dllexport)
#    else
#        define LIBRARY_API __declspec(dllimport)
#    endif
#elif
#    define LIBRARY_API
#endif

//1KB(1024 bytes)
typedef struct GPCSharedMemory {
	GPCSharedMemory() {
		sizeof(this, 0, sizeof(*this));
	}
	char vtablePath[MAX_PATH_LEN] = "";
	char lastHookedProcessName[MIN_PATH_LEN] = "";
	uint16_t dllLoadCount = 0;
	bool hooked = false;
}GPCSharedMemory;

extern "C" {
	extern LIBRARY_API GPCSharedMemory g_GPCSharedMemory;
	LIBRARY_API GPCSharedMemory GetGPCSharedMemory();
	LIBRARY_API void SetGPCSharedMemory(GPCSharedMemory sharedMemory);
}



