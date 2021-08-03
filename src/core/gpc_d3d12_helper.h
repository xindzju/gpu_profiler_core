#pragma once
#include <iostream>
#include <list>
#include <vector>
#include <memory>
#include <wrl.h>

//dxgi
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <dxgi1_5.h>
#include <dxgi1_6.h>
//d3d12
#include <d3d12.h>
#include "utils/d3dx12.h"

#include "utils/gpc_utils.h"

namespace gpc {
    struct GPCCommandList
    {
        GPCCommandList(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type);

        void Reset();

        void SetCompleteFence(ID3D12Fence* fence, UINT64 val);

        bool IsCompeted() const;

        ID3D12GraphicsCommandList*  m_pCL = nullptr;
        ID3D12CommandAllocator*     m_pCA = nullptr;
        ID3D12Fence* m_pFence = nullptr;
        UINT64  m_completeValue = 0;
    };

    class GPCCommandListPool
    {
    public:
        GPCCommandListPool(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type) :
            m_pDevice(pDevice), m_CLType(type) {
        }

        GPCCommandList* GetCL();

        void FreeCL(GPCCommandList* wCL);

    protected:
        ID3D12Device*                m_pDevice;
        D3D12_COMMAND_LIST_TYPE      m_CLType;
        std::list<GPCCommandList*>   m_CLs;
    };


    struct GPCD3D12CommandQueueHelper
    {
        GPCD3D12CommandQueueHelper(ID3D12CommandQueue* pCommandQueue);

        ID3D12GraphicsCommandList* GetCL();

        UINT64 ExecuteCL();

        UINT64 Signal();

        void SignalValue(UINT64 val);

        void WaitForGPUIdle();

        ID3D12CommandQueue*                         m_pCommandQueue = nullptr;
        ID3D12Device*                               m_pD3D12Device = nullptr;
        GPCCommandListPool*                         m_pCLPool = nullptr;
        GPCCommandList*                             m_pCurrentCL = nullptr;
        ID3D12Fence*                                m_pGPUFence = nullptr;
        UINT64                                      m_iFenceVal = 0;
        HANDLE                                      m_hGPUEvent = nullptr;
        bool                                        m_bCLOpen = false;
    };

    struct GPCD3D12CommandQueueHelper1 : public GPCD3D12CommandQueueHelper
    {
        GPCD3D12CommandQueueHelper1(ID3D12CommandQueue* pQ) : GPCD3D12CommandQueueHelper(pQ) {}

        void Close();

        UINT64 Execute();

        void WaitForValue(UINT64 fenceVal);
    };
}