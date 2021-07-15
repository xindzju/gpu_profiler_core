#include "d3d12_hook.h"
#include "gpc_hook_utils.h"
#include "gpc_shared.h"

namespace gpc {
    bool HookD3D12Entry(const char* dllName) {
        bool res = true;
        HMODULE module = LoadLibrary(dllName);
        if (module) {
            void* pFuncAddress = GetProcAddress(module, "D3D12CreateDevice");
            if (pFuncAddress) {
                auto apiHookInfo = GetAPIHookInfo("D3D12CreateDevice");
                res &= HookFunc(pFuncAddress, My_D3D12CreateDevice, (void**)&pReal_D3D12CreateDevice, apiHookInfo);
                if (res)
                    std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
                else
                    std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
            }
            else {
                std::cout << "Get D3D12CreateDevice address failed" << std::endl;
                res = false;
            }
        }
        else {
            std::cout << "Load d3d12.lib failed" << std::endl;
            res = false;
        }
        //FreeLibrary(module);

        return res;
    }

    bool HookD3D12DeviceInterface(ID3D12Device* pDevice) {
        std::cout << "Start to hook ID3D12Device interface" << std::endl;
        //ID3DDevice1 ~ 9
        bool res = true;
        if (!pDevice)
            return false;
        //res = HookInterfaceFunc((void*)pDevice, My_ID3D12Device_CreateCommandQueue, (void**)&pReal_ID3D12Device_CreateCommandQueue, "CreateCommandQueue");
        //app crash when enabling CreateCommandList 
        //res = HookInterfaceFunc((void*)pDevice, My_ID3D12Device_CreateCommandList, (void**)&pReal_ID3D12Device_CreateCommandList, "CreateCommandList"); 
        return res;
    }

#pragma region HookD3D12CreateDevice
    PFN_D3D12CREATEDEVICE pReal_D3D12CreateDevice = Real_D3D12CreateDevice;

    HRESULT WINAPI Real_D3D12CreateDevice(
        _In_opt_ IUnknown* pAdapter,
        D3D_FEATURE_LEVEL MinimumFeatureLevel,
        _In_ REFIID riid, // Expected: ID3D12Device
        _COM_Outptr_opt_ void** ppDevice) {
        return D3D12CreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice);
    }

    HRESULT WINAPI My_D3D12CreateDevice(
        _In_opt_ IUnknown* pAdapter,
        D3D_FEATURE_LEVEL MinimumFeatureLevel,
        _In_ REFIID riid, // Expected: ID3D12Device
        _COM_Outptr_opt_ void** ppDevice) {
        HRESULT res;
        //std::cout << "Enter My_D3D12CreateDevice" << std::endl;
        if (!ppDevice) //debug mode war
            return S_OK;
        Pre_D3D12CreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice);
        //res = D3D12CreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice); //false: recursive hook
        res = pReal_D3D12CreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice); //right
        Post_D3D12CreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice);
        return res;
    }

    void WINAPI Pre_D3D12CreateDevice(
        _In_opt_ IUnknown* pAdapter,
        D3D_FEATURE_LEVEL MinimumFeatureLevel,
        _In_ REFIID riid, // Expected: ID3D12Device
        _COM_Outptr_opt_ void** ppDevice) {
        //std::cout << "Enter Pre_D3D12CreateDevice" << std::endl;
        return;
    }

    void WINAPI Post_D3D12CreateDevice(
        _In_opt_ IUnknown* pAdapter,
        D3D_FEATURE_LEVEL MinimumFeatureLevel,
        _In_ REFIID riid, // Expected: ID3D12Device
        _COM_Outptr_opt_ void** ppDevice) {
        //std::cout << "Enter Post_D3D12CreateDevice" << std::endl;
        static bool hookOnce = false;
        if (!hookOnce) {
            hookOnce = HookD3D12DeviceInterface((ID3D12Device*)*ppDevice);
            if (hookOnce)
                std::cout << "Hook ID3D12Device interface succeed" << std::endl;
            else
                std::cout << "Hook ID3D12Device interface failed" << std::endl;
        }
        return;
    }
#pragma endregion

#pragma region HookID3D12Device_CreateCommandList
    PFN_ID3D12Device_CreateCommandList pReal_ID3D12Device_CreateCommandList = Real_ID3D12Device_CreateCommandList;

    HRESULT STDMETHODCALLTYPE Real_ID3D12Device_CreateCommandList(
        ID3D12Device* This,
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  ID3D12CommandAllocator* pCommandAllocator,
        _In_opt_  ID3D12PipelineState* pInitialState,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList) {
        return This->CreateCommandList(nodeMask, type, pCommandAllocator, pInitialState, riid, ppCommandList);
    }

    HRESULT STDMETHODCALLTYPE My_ID3D12Device_CreateCommandList(
        ID3D12Device* This,
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  ID3D12CommandAllocator* pCommandAllocator,
        _In_opt_  ID3D12PipelineState* pInitialState,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList) {
        //std::cout << "Enter My_ID3D12Device_CreateCommandList" << std::endl;
        //debug mode war
        if (!ppCommandList)
            return S_OK;
        HRESULT res;
        Pre_ID3D12Device_CreateCommandList(This, nodeMask, type, pCommandAllocator, pInitialState, riid, ppCommandList);
        res = pReal_ID3D12Device_CreateCommandList(This, nodeMask, type, pCommandAllocator, pInitialState, riid, ppCommandList);
        Post_ID3D12Device_CreateCommandList(This, nodeMask, type, pCommandAllocator, pInitialState, riid, ppCommandList);
        return res;
    }

    void STDMETHODCALLTYPE Pre_ID3D12Device_CreateCommandList(
        ID3D12Device* This,
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  ID3D12CommandAllocator* pCommandAllocator,
        _In_opt_  ID3D12PipelineState* pInitialState,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList) {
        //std::cout << "Enter Pre_CreateCommandList" << std::endl;
        return;
    }

    void STDMETHODCALLTYPE Post_ID3D12Device_CreateCommandList(
        ID3D12Device* This,
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  ID3D12CommandAllocator* pCommandAllocator,
        _In_opt_  ID3D12PipelineState* pInitialState,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList) {
        //std::cout << "Enter Post_CreateCommandList" << std::endl;
        return;
    }
#pragma endregion

#pragma region HookID3D12Device4_CreateCommandList1
    PFN_ID3D12Device4_CreateCommandList1 pReal_ID3D12Device4_CreateCommandList1 = Real_ID3D12Device4_CreateCommandList1;
    HRESULT STDMETHODCALLTYPE Real_ID3D12Device4_CreateCommandList1(
        ID3D12Device4* This,
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  D3D12_COMMAND_LIST_FLAGS flags,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList) {
        return This->CreateCommandList1(nodeMask, type, flags, riid, ppCommandList);
    }
    HRESULT STDMETHODCALLTYPE My_ID3D12Device4_CreateCommandList1(
        ID3D12Device4* This,
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  D3D12_COMMAND_LIST_FLAGS flags,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList) {
        //std::cout << "Enter My_ID3D12Device4_CreateCommandList1" << std::endl;
        HRESULT res;
        Pre_ID3D12Device4_CreateCommandList1(This, nodeMask, type, flags, riid, ppCommandList);
        res = pReal_ID3D12Device4_CreateCommandList1(This, nodeMask, type, flags, riid, ppCommandList);
        Post_ID3D12Device4_CreateCommandList1(This, nodeMask, type, flags, riid, ppCommandList);
        return res;
    }
    void STDMETHODCALLTYPE Pre_ID3D12Device4_CreateCommandList1(
        ID3D12Device4* This,
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  D3D12_COMMAND_LIST_FLAGS flags,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList) {
        return;
    }
    void STDMETHODCALLTYPE Post_ID3D12Device4_CreateCommandList1(
        ID3D12Device4* This,
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  D3D12_COMMAND_LIST_FLAGS flags,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList) {
        return;
    }
#pragma endregion
}