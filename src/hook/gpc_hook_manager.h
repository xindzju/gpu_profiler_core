#pragma once
#include "utils/gpc_utils.h" 
#include "gpc_hook_utils.h"
#include "gpc_hook_defines.h"

//dx hook
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_3.h>
#include "dx/dxgi_hook.h"
#include "dx/d3d12_hook.h"
//vk hook
#include "vk/vk_hook.h"
//ogl hook
#include "ogl/ogl_hook.h"
//cuda hook
#include "cuda/cuda_hook.h"
//sys hook
#include "sys/sys_hook.h"

namespace gpc {
	class GPCHookManager : public GPCSingleton<GPCHookManager>{
	public:
		GPCHookManager() {
			m_vtablePath = fs::current_path() / "vtable.csv";
			LoadAPIHookInfos(m_vtablePath);
		}
		~GPCHookManager();

		//hook entry
		bool Hook(HOOK_API_TYPE hookType = HOOK_API_TYPE::HOOK_API_TYPE_NONE);
		//unhook entry
		void UnHook(); 

	protected:
		//API hook entries
		bool HookDXGI(const char* dllName = "dxgi.dll"); //CreateDXGIFactory
		bool HookD3D12(const char* dllName = "d3d12.dll"); //D3D12CreateDevice
		bool HookCUDA(const char* dllName = "");
		bool HookVK(const char* dllName = "");
		bool HookOGL(const char* dllName = "");
		bool HookSys();

	private:
		void LoadAPIHookInfos(fs::path csvPath);
		fs::path m_vtablePath;
	};
}