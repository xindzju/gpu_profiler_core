#pragma once
/*
* d3d12device interface: ID3D12Device
* d3d12device method: ID3D12Device:CreateCommandList
* d3d12device interface: ID3D12Device4
* d3d12device method: ID3D12Device4:CreateCommandList1
*/
#include <d3d12.h>

namespace gpc {
    bool HookD3D12Entry(const char* dllName);
    bool HookD3D12DeviceInterface(ID3D12Device* pDevice); //many different interfaces(9)
    
    //hook device
#pragma region HookD3D12CreateDevice
    typedef HRESULT(WINAPI* PFN_D3D12CREATEDEVICE)(
        _In_opt_ IUnknown* pAdapter,
        D3D_FEATURE_LEVEL MinimumFeatureLevel,
        _In_ REFIID riid, // Expected: ID3D12Device
        _COM_Outptr_opt_ void** ppDevice);
    extern PFN_D3D12CREATEDEVICE pReal_D3D12CreateDevice;
    HRESULT WINAPI Real_D3D12CreateDevice(
        _In_opt_ IUnknown* pAdapter,
        D3D_FEATURE_LEVEL MinimumFeatureLevel,
        _In_ REFIID riid, // Expected: ID3D12Device
        _COM_Outptr_opt_ void** ppDevice);
    HRESULT WINAPI My_D3D12CreateDevice(
        _In_opt_ IUnknown* pAdapter,
        D3D_FEATURE_LEVEL MinimumFeatureLevel,
        _In_ REFIID riid, // Expected: ID3D12Device
        _COM_Outptr_opt_ void** ppDevice);
    void WINAPI Pre_D3D12CreateDevice(
        _In_opt_ IUnknown* pAdapter,
        D3D_FEATURE_LEVEL MinimumFeatureLevel,
        _In_ REFIID riid, // Expected: ID3D12Device
        _COM_Outptr_opt_ void** ppDevice);
    void WINAPI Post_D3D12CreateDevice(
        _In_opt_ IUnknown* pAdapter,
        D3D_FEATURE_LEVEL MinimumFeatureLevel,
        _In_ REFIID riid, // Expected: ID3D12Device
        _COM_Outptr_opt_ void** ppDevice);
#pragma endregion

#pragma region HookCreateCommandQueue
    typedef HRESULT(STDMETHODCALLTYPE* PFN_CREATE_COMMANDQUEUE)(
        _In_  const D3D12_COMMAND_QUEUE_DESC* pDesc,
        REFIID riid,
        _COM_Outptr_  void** ppCommandQueue);
    extern PFN_CREATE_COMMANDQUEUE pReal_CreateCommandQueue;
    HRESULT STDMETHODCALLTYPE Real_CreateCommandQueue(
        ID3D12Device* This,
        _In_  const D3D12_COMMAND_QUEUE_DESC* pDesc,
        REFIID riid,
        _COM_Outptr_  void** ppCommandQueue);
    HRESULT STDMETHODCALLTYPE My_CreateCommandQueue(
        ID3D12Device* This,
        _In_  const D3D12_COMMAND_QUEUE_DESC* pDesc,
        REFIID riid,
        _COM_Outptr_  void** ppCommandQueue);
    void STDMETHODCALLTYPE Pre_CreateCommandQueue(
        ID3D12Device* This,
        _In_  const D3D12_COMMAND_QUEUE_DESC* pDesc,
        REFIID riid,
        _COM_Outptr_  void** ppCommandQueue);
    void STDMETHODCALLTYPE Post_CreateCommandQueue(
        ID3D12Device* This,
        _In_  const D3D12_COMMAND_QUEUE_DESC* pDesc,
        REFIID riid,
        _COM_Outptr_  void** ppCommandQueue);
#pragma endregion

#pragma region HookID3D12Device_CreateCommandList
    typedef HRESULT(STDMETHODCALLTYPE* PFN_ID3D12Device_CreateCommandList)(
        ID3D12Device* This,
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  ID3D12CommandAllocator* pCommandAllocator,
        _In_opt_  ID3D12PipelineState* pInitialState,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList);
    extern PFN_ID3D12Device_CreateCommandList pReal_ID3D12Device_CreateCommandList;
    HRESULT STDMETHODCALLTYPE Real_ID3D12Device_CreateCommandList(
        ID3D12Device* This,
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  ID3D12CommandAllocator* pCommandAllocator,
        _In_opt_  ID3D12PipelineState* pInitialState,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList);
    HRESULT STDMETHODCALLTYPE My_ID3D12Device_CreateCommandList(
        ID3D12Device* This,
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  ID3D12CommandAllocator* pCommandAllocator,
        _In_opt_  ID3D12PipelineState* pInitialState,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList);
    void STDMETHODCALLTYPE Pre_ID3D12Device_CreateCommandList(
        ID3D12Device* This,
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  ID3D12CommandAllocator* pCommandAllocator,
        _In_opt_  ID3D12PipelineState* pInitialState,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList);
    void STDMETHODCALLTYPE Post_ID3D12Device_CreateCommandList(
        ID3D12Device* This,
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  ID3D12CommandAllocator* pCommandAllocator,
        _In_opt_  ID3D12PipelineState* pInitialState,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList);
#pragma endregion

#pragma region HookID3D12Device4_CreateCommandList1
    typedef HRESULT(STDMETHODCALLTYPE* PFN_ID3D12Device4_CreateCommandList1)(
        ID3D12Device4* This, 
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  D3D12_COMMAND_LIST_FLAGS flags,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList);
    extern PFN_ID3D12Device4_CreateCommandList1 pReal_ID3D12Device4_CreateCommandList1;
    HRESULT STDMETHODCALLTYPE Real_ID3D12Device4_CreateCommandList1(
        ID3D12Device4* This, 
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  D3D12_COMMAND_LIST_FLAGS flags,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList);
    HRESULT STDMETHODCALLTYPE My_ID3D12Device4_CreateCommandList1(
        ID3D12Device4* This, 
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  D3D12_COMMAND_LIST_FLAGS flags,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList);
    void STDMETHODCALLTYPE Pre_ID3D12Device4_CreateCommandList1(
        ID3D12Device4* This, 
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  D3D12_COMMAND_LIST_FLAGS flags,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList);
    void STDMETHODCALLTYPE Post_ID3D12Device4_CreateCommandList1(
        ID3D12Device4* This, 
        _In_  UINT nodeMask,
        _In_  D3D12_COMMAND_LIST_TYPE type,
        _In_  D3D12_COMMAND_LIST_FLAGS flags,
        REFIID riid,
        _COM_Outptr_  void** ppCommandList);
#pragma endregion
}
