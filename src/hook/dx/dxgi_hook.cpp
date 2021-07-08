#include "dxgi_hook.h"
#include "d3d12_hook.h"
#include "hook/gpc_hook_utils.h"

namespace gpc {
    bool HookDXGIFactoryInterface(IDXGIFactory* pIDXGIFactory) {
        std::cout << "\nStart to hook IDXGIFactory interface" << std::endl;
        bool res = true;
        //hook IDXGIFactory
        res = HookInterfaceFunc((void*)pIDXGIFactory, My_IDXGIFactory_CreateSwapChain, (void**)&pReal_IDXGIFactory_CreateSwapChain, "CreateSwapChain");

        //hook IDXGIFactory2
        IDXGIFactory2* pIDXGI2Factory = nullptr;
        pIDXGIFactory->QueryInterface(&pIDXGI2Factory);
        if (!pIDXGI2Factory)
        {
            res = HookInterfaceFunc(pIDXGI2Factory, My_IDXGIFactory2_CreateSwapChainForHwnd, (void**)&pReal_IDXGIFactory2_CreateSwapChainForHwnd, "CreateSwapChainForHwnd");
            res = HookInterfaceFunc(pIDXGI2Factory, My_IDXGIFactory2_CreateSwapChainForCoreWindow, (void**)&pReal_IDXGIFactory2_CreateSwapChainForCoreWindow, "CreateSwapChainForCoreWindow");
            res = HookInterfaceFunc(pIDXGI2Factory, My_IDXGIFactory2_CreateSwapChainForComposition, (void**)&pReal_IDXGIFactory2_CreateSwapChainForComposition, "CreateSwapChainForComposition");

            pIDXGI2Factory->Release();
        }
		return res;
    }

    bool HookDXGISwapChainInterface(IDXGISwapChain* pIDXGISwapChain) {
        std::cout << "\nStart to hook IDXGISwapChain interface" << std::endl;
        bool res = true;
        //hook IDXGISwapChain
        res = HookInterfaceFunc((void*)pIDXGISwapChain, My_IDXGISwapChain_Present, (void**)&pReal_IDXGISwapChain_Present, "Present");

        //hook IDXGISwapChain1
        IDXGISwapChain1* pIDXGISwapChain1 = nullptr;
        pIDXGISwapChain->QueryInterface(&pIDXGISwapChain1);
        if (!pIDXGISwapChain1)
        {
            res = HookInterfaceFunc((void*)pIDXGISwapChain1, My_IDXGISwapChain1_Present1, (void**)&pReal_IDXGISwapChain1_Present1, "Present1");
            pIDXGISwapChain1->Release();
        }
        return res;
    }

#pragma region HookCreateDXGIFactory
    PFN_CREATEDXGIFACTORY pReal_CreateDXGIFactory = Real_CreateDXGIFactory;

    //if detour failed, default will protect app from crashing
    HRESULT WINAPI Real_CreateDXGIFactory(REFIID riid, _COM_Outptr_ void** ppFactory) {
        //std::cout << "Enter Real_CreateDXGIFactory" << std::endl;
        return CreateDXGIFactory(riid, ppFactory);
    }

    HRESULT WINAPI My_CreateDXGIFactory(REFIID riid, _COM_Outptr_ void** ppFactory) {
        std::cout << "Enter My_CreateDXGIFactory" << std::endl;
        HRESULT res;
        Pre_CreateDXGIFactory(riid, ppFactory);
        res = pReal_CreateDXGIFactory(riid, ppFactory);
        Post_CreateDXGIFactory(riid, ppFactory);
        return res;
    }

    void WINAPI Pre_CreateDXGIFactory(REFIID riid, _COM_Outptr_ void** ppFactory) {
        //std::cout << "Enter Pre_CreateDXGIFactory" << std::endl;
        return;
    }

    void WINAPI Post_CreateDXGIFactory(REFIID riid, _COM_Outptr_ void** ppFactory) {
        //std::cout << "Enter Post_CreateDXGIFactory" << std::endl;
        static bool hookOnce = false;
        if (!hookOnce) {
            hookOnce = HookDXGIFactoryInterface((IDXGIFactory*)*ppFactory);
            if (hookOnce)
                std::cout << "Hook IDXGIFactory interface succeed" << std::endl;
            else
                std::cout << "Hook IDXGIFactory interface failed" << std::endl;
        }
        return;
    }
#pragma endregion

/*
#pragma region HookCreateDXGIFactory1
    PFN_CREATEDXGIFACTORY1 pReal_CreateDXGIFactory1 = Real_CreateDXGIFactory1;

    //if detour failed, default will protect app from crashing
    HRESULT WINAPI Real_CreateDXGIFactory1(REFIID riid, _COM_Outptr_ void** ppFactory) {
        std::cout << "Enter Real_CreateDXGIFactory1" << std::endl;
        return CreateDXGIFactory1(riid, ppFactory);
    }

    HRESULT WINAPI My_CreateDXGIFactory1(REFIID riid, _COM_Outptr_ void** ppFactory) {
        std::cout << "Enter My_CreateDXGIFactory1" << std::endl;
        HRESULT res;
        Pre_CreateDXGIFactory1(riid, ppFactory);
        res = pReal_CreateDXGIFactory1(riid, ppFactory);
        Post_CreateDXGIFactory1(riid, ppFactory);
        return res;
    }

    void WINAPI Pre_CreateDXGIFactory1(REFIID riid, _COM_Outptr_ void** ppFactory) {
        std::cout << "Enter Pre_CreateDXGIFactory1" << std::endl;
        return;
    }

    void WINAPI Post_CreateDXGIFactory1(REFIID riid, _COM_Outptr_ void** ppFactory) {
        std::cout << "Enter Post_CreateDXGIFactory1" << std::endl;
        HookDXGIFactoryInterface((IDXGIFactory*)*ppFactory); //hook after the dxgi factory get real created
        return;
    }
#pragma endregion
*/

#pragma region HookCreateDXGIFactory2
        PFN_CREATEDXGIFACTORY2 pReal_CreateDXGIFactory2 = Real_CreateDXGIFactory2;
        
        //if detour failed, default will protect app from crashing
        HRESULT WINAPI Real_CreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory) {
            //std::cout << "Enter Real_CreateDXGIFactory2" << std::endl;
            return CreateDXGIFactory2(Flags, riid, ppFactory);
        }

        HRESULT WINAPI My_CreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory) {
            std::cout << "Enter My_CreateDXGIFactory2" << std::endl;
            HRESULT res;
            Pre_CreateDXGIFactory2(Flags, riid, ppFactory);
            res = pReal_CreateDXGIFactory2(Flags, riid, ppFactory);
            Post_CreateDXGIFactory2(Flags, riid, ppFactory);
            return res;
        }

        void WINAPI Pre_CreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory) {
            //std::cout << "Enter Pre_CreateDXGIFactory2" << std::endl;
            return;
        }

        void WINAPI Post_CreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory) {
            //std::cout << "Enter Post_CreateDXGIFactory2" << std::endl;
            //TODO:Cast IDXGIFactory2 to IDXGIFactory
            //static bool hookOnce = false;
            //if (!hookOnce) {
            //    hookOnce = HookDXGIFactoryInterface((IDXGIFactory*)*ppFactory);
            //    if (hookOnce)
            //        std::cout << "Hook IDXGIFactory interface succeed" << std::endl;
            //    else
            //        std::cout << "Hook IDXGIFactory interface failed" << std::endl;
            //}
            return;
        }
#pragma endregion

#pragma region HookCreateSwapChain
        PFN_IDXGIFACTORY_CREATE_SWAPCHAIN pReal_IDXGIFactory_CreateSwapChain = Real_IDXGIFactory_CreateSwapChain;

        HRESULT STDMETHODCALLTYPE Real_IDXGIFactory_CreateSwapChain(
            IDXGIFactory * This,
            /* [annotation][in] */
            _In_  IUnknown * pDevice,
            /* [annotation][in] */
            _In_  DXGI_SWAP_CHAIN_DESC * pDesc,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain * *ppSwapChain) {
            return This->CreateSwapChain(pDevice, pDesc, ppSwapChain);
        }

        HRESULT STDMETHODCALLTYPE My_IDXGIFactory_CreateSwapChain(
            IDXGIFactory * This,
            /* [annotation][in] */
            _In_  IUnknown * pDevice,
            /* [annotation][in] */
            _In_  DXGI_SWAP_CHAIN_DESC * pDesc,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain * *ppSwapChain) {
            std::cout << "Enter My_IDXGIFactory_CreateSwapChain" << std::endl;
            HRESULT res;
            Pre_IDXGIFactory_CreateSwapChain(This, pDevice, pDesc, ppSwapChain);
            res = pReal_IDXGIFactory_CreateSwapChain(This, pDevice, pDesc, ppSwapChain);
            Post_IDXGIFactory_CreateSwapChain(This, pDevice, pDesc, ppSwapChain);
            return res;
        }

        void STDMETHODCALLTYPE Pre_IDXGIFactory_CreateSwapChain(
            IDXGIFactory * This,
            /* [annotation][in] */
            _In_  IUnknown * pDevice,
            /* [annotation][in] */
            _In_  DXGI_SWAP_CHAIN_DESC * pDesc,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain * *ppSwapChain) {
            //std::cout << "Enter Pre_IDXGIFactory_CreateSwapChain" << std::endl;
            return;
        }

        void STDMETHODCALLTYPE Post_IDXGIFactory_CreateSwapChain(
            IDXGIFactory * This,
            /* [annotation][in] */
            _In_  IUnknown * pDevice,
            /* [annotation][in] */
            _In_  DXGI_SWAP_CHAIN_DESC * pDesc,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain * *ppSwapChain) {
            //std::cout << "Enter Post_IDXGIFactory_CreateSwapChain" << std::endl;
            static bool hookOnce = false;
            if (!hookOnce) {
                hookOnce = HookDXGISwapChainInterface(*ppSwapChain);
                if (hookOnce)
                    std::cout << "Hook IDXGISwapChain interface succeed" << std::endl;
                else
                    std::cout << "Hook IDXGISwapChain interface failed" << std::endl;
            }
            return;
        }
#pragma endregion

#pragma region HookIDXGIFactory2_CreateSwapChainForHwnd
		PFN_IDXGIFactory2_CreateSwapChainForHwnd pReal_IDXGIFactory2_CreateSwapChainForHwnd = Real_IDXGIFactory2_CreateSwapChainForHwnd;
        HRESULT STDMETHODCALLTYPE Real_IDXGIFactory2_CreateSwapChainForHwnd(
            IDXGIFactory2* This,
            /* [annotation][in] */
            _In_  IUnknown* pDevice,
            /* [annotation][in] */
            _In_  HWND hWnd,
            /* [annotation][in] */
            _In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
            /* [annotation][in] */
            _In_opt_  const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
            /* [annotation][in] */
            _In_opt_  IDXGIOutput* pRestrictToOutput,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain1** ppSwapChain) {
            return This->CreateSwapChainForHwnd(pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
        }
        HRESULT STDMETHODCALLTYPE My_IDXGIFactory2_CreateSwapChainForHwnd(
            IDXGIFactory2* This,
            /* [annotation][in] */
            _In_  IUnknown* pDevice,
            /* [annotation][in] */
            _In_  HWND hWnd,
            /* [annotation][in] */
            _In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
            /* [annotation][in] */
            _In_opt_  const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
            /* [annotation][in] */
            _In_opt_  IDXGIOutput* pRestrictToOutput,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain1** ppSwapChain) {
            std::cout << "Enter My_IDXGIFactory2_CreateSwapChainForHwnd" << std::endl;
            HRESULT res;
            Pre_IDXGIFactory2_CreateSwapChainForHwnd(This, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
            res = pReal_IDXGIFactory2_CreateSwapChainForHwnd(This, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
            Post_IDXGIFactory2_CreateSwapChainForHwnd(This, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
            return res;
        }
        void STDMETHODCALLTYPE Pre_IDXGIFactory2_CreateSwapChainForHwnd(
            IDXGIFactory2* This,
            /* [annotation][in] */
            _In_  IUnknown* pDevice,
            /* [annotation][in] */
            _In_  HWND hWnd,
            /* [annotation][in] */
            _In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
            /* [annotation][in] */
            _In_opt_  const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
            /* [annotation][in] */
            _In_opt_  IDXGIOutput* pRestrictToOutput,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain1** ppSwapChain) {
            return;
        }
        void STDMETHODCALLTYPE Post_IDXGIFactory2_CreateSwapChainForHwnd(
            IDXGIFactory2* This,
            /* [annotation][in] */
            _In_  IUnknown* pDevice,
            /* [annotation][in] */
            _In_  HWND hWnd,
            /* [annotation][in] */
            _In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
            /* [annotation][in] */
            _In_opt_  const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
            /* [annotation][in] */
            _In_opt_  IDXGIOutput* pRestrictToOutput,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain1** ppSwapChain) {
            //TODO: IDXGISwapChain1 cast to IDXGISwapChain
            //if (HookDXGISwapChainInterface((IDXGISwapChain*)*ppSwapChain))
            //    std::cout << "Hook DXGI SwapChain interface succeed" << std::endl;
            //else
            //    std::cout << "Hook DXGI SwapChain interface failed" << std::endl;
            return;
        }
#pragma endregion

#pragma region HookIDXGIFactory2_CreateSwapChainForCoreWindow
		PFN_IDXGIFactory2_CreateSwapChainForCoreWindow pReal_IDXGIFactory2_CreateSwapChainForCoreWindow = Real_IDXGIFactory2_CreateSwapChainForCoreWindow;
        HRESULT STDMETHODCALLTYPE Real_IDXGIFactory2_CreateSwapChainForCoreWindow(
            IDXGIFactory2* This,
            /* [annotation][in] */
            _In_  IUnknown* pDevice,
            /* [annotation][in] */
            _In_  IUnknown* pWindow,
            /* [annotation][in] */
            _In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
            /* [annotation][in] */
            _In_opt_  IDXGIOutput* pRestrictToOutput,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain1** ppSwapChain) {
            return This->CreateSwapChainForCoreWindow(pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
        }
        HRESULT STDMETHODCALLTYPE My_IDXGIFactory2_CreateSwapChainForCoreWindow(
            IDXGIFactory2* This,
            /* [annotation][in] */
            _In_  IUnknown* pDevice,
            /* [annotation][in] */
            _In_  IUnknown* pWindow,
            /* [annotation][in] */
            _In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
            /* [annotation][in] */
            _In_opt_  IDXGIOutput* pRestrictToOutput,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain1** ppSwapChain) {
            std::cout << "Enter My_IDXGIFactory2_CreateSwapChainForCoreWindow" << std::endl;
            HRESULT res;
            Pre_IDXGIFactory2_CreateSwapChainForCoreWindow(This, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
            res = pReal_IDXGIFactory2_CreateSwapChainForCoreWindow(This, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
            Post_IDXGIFactory2_CreateSwapChainForCoreWindow(This, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
            return res;
        }
        void STDMETHODCALLTYPE Pre_IDXGIFactory2_CreateSwapChainForCoreWindow(
            IDXGIFactory2* This,
            /* [annotation][in] */
            _In_  IUnknown* pDevice,
            /* [annotation][in] */
            _In_  IUnknown* pWindow,
            /* [annotation][in] */
            _In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
            /* [annotation][in] */
            _In_opt_  IDXGIOutput* pRestrictToOutput,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain1** ppSwapChain) {
            return;
        }
        void STDMETHODCALLTYPE Post_IDXGIFactory2_CreateSwapChainForCoreWindow(
            IDXGIFactory2* This,
            /* [annotation][in] */
            _In_  IUnknown* pDevice,
            /* [annotation][in] */
            _In_  IUnknown* pWindow,
            /* [annotation][in] */
            _In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
            /* [annotation][in] */
            _In_opt_  IDXGIOutput* pRestrictToOutput,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain1** ppSwapChain) {
            //TODO: IDXGISwapChain1 cast to IDXGISwapChain
            //if (HookDXGISwapChainInterface((IDXGISwapChain*)*ppSwapChain))
            //    std::cout << "Hook DXGI SwapChain interface succeed" << std::endl;
            //else
            //    std::cout << "Hook DXGI SwapChain interface failed" << std::endl;
            return;
        }
#pragma endregion

#pragma region HookIDXGIFactory2_CreateSwapChainForComposition
		PFN_IDXGIFactory2_CreateSwapChainForComposition pReal_IDXGIFactory2_CreateSwapChainForComposition = Real_IDXGIFactory2_CreateSwapChainForComposition;
        HRESULT STDMETHODCALLTYPE Real_IDXGIFactory2_CreateSwapChainForComposition(
            IDXGIFactory2* This,
            /* [annotation][in] */
            _In_  IUnknown* pDevice,
            /* [annotation][in] */
            _In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
            /* [annotation][in] */
            _In_opt_  IDXGIOutput* pRestrictToOutput,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain1** ppSwapChain) {
            return This->CreateSwapChainForComposition(pDevice, pDesc, pRestrictToOutput, ppSwapChain);
        }
        HRESULT STDMETHODCALLTYPE My_IDXGIFactory2_CreateSwapChainForComposition(
            IDXGIFactory2* This,
            /* [annotation][in] */
            _In_  IUnknown* pDevice,
            /* [annotation][in] */
            _In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
            /* [annotation][in] */
            _In_opt_  IDXGIOutput* pRestrictToOutput,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain1** ppSwapChain) {
            std::cout << "Enter My_IDXGIFactory2_CreateSwapChainForComposition" << std::endl;
            HRESULT res;
            Pre_IDXGIFactory2_CreateSwapChainForComposition(This, pDevice, pDesc, pRestrictToOutput, ppSwapChain);
            res = pReal_IDXGIFactory2_CreateSwapChainForComposition(This, pDevice, pDesc, pRestrictToOutput, ppSwapChain);
            Post_IDXGIFactory2_CreateSwapChainForComposition(This, pDevice, pDesc, pRestrictToOutput, ppSwapChain);
            return res;
        }
        void STDMETHODCALLTYPE Pre_IDXGIFactory2_CreateSwapChainForComposition(
            IDXGIFactory2* This,
            /* [annotation][in] */
            _In_  IUnknown* pDevice,
            /* [annotation][in] */
            _In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
            /* [annotation][in] */
            _In_opt_  IDXGIOutput* pRestrictToOutput,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain1** ppSwapChain) {
            return;
        }
        void STDMETHODCALLTYPE Post_IDXGIFactory2_CreateSwapChainForComposition(
            IDXGIFactory2* This,
            /* [annotation][in] */
            _In_  IUnknown* pDevice,
            /* [annotation][in] */
            _In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
            /* [annotation][in] */
            _In_opt_  IDXGIOutput* pRestrictToOutput,
            /* [annotation][out] */
            _COM_Outptr_  IDXGISwapChain1** ppSwapChain) {
            //TODO: IDXGISwapChain1 cast to IDXGISwapChain
            //if (HookDXGISwapChainInterface((IDXGISwapChain*)*ppSwapChain))
            //    std::cout << "Hook DXGI SwapChain interface succeed" << std::endl;
            //else
            //    std::cout << "Hook DXGI SwapChain interface failed" << std::endl;
            return;
        }
#pragma endregion

#pragma region HookPresent
        PFN_IDXGISWAPCHAIN_PRESENT pReal_IDXGISwapChain_Present = Real_IDXGISwapChain_Present;

        HRESULT STDMETHODCALLTYPE Real_IDXGISwapChain_Present(
            IDXGISwapChain * This,
            /* [in] */ UINT SyncInterval,
            /* [in] */ UINT Flags) {
            return This->Present(SyncInterval, Flags);
        }

        HRESULT STDMETHODCALLTYPE My_IDXGISwapChain_Present(
            IDXGISwapChain * This,
            /* [in] */ UINT SyncInterval,
            /* [in] */ UINT Flags) {
            std::cout << "Enter My_IDXGISwapChain_Present" << std::endl;
            HRESULT res;
            Pre_IDXGISwapChain_Present(This, SyncInterval, Flags);
            res = pReal_IDXGISwapChain_Present(This, SyncInterval, Flags);
            Pre_IDXGISwapChain_Present(This, SyncInterval, Flags);

            //debug
            static int frameIndex = 0;
            std::ofstream ofs("my_present.log", std::ios_base::app);
            ofs << frameIndex++ << "\n";
            ofs.close();
            return res;
        }

        void STDMETHODCALLTYPE Pre_IDXGISwapChain_Present(
            IDXGISwapChain * This,
            /* [in] */ UINT SyncInterval,
            /* [in] */ UINT Flags) {
            //std::cout << "Enter Pre_IDXGISwapChain_Present" << std::endl;
            return;
        }

        void STDMETHODCALLTYPE Post_IDXGISwapChain_Present(
            IDXGISwapChain * This,
            /* [in] */ UINT SyncInterval,
            /* [in] */ UINT Flags) {
            //std::cout << "Enter Post_IDXGISwapChain_Present" << std::endl;
            return;
        }
#pragma endregion

#pragma region HookIDXGISwapChain1_Present1
        extern PFN_IDXGISwapChain1_Present1 pReal_IDXGISwapChain1_Present1 = Real_IDXGISwapChain1_Present1;
        HRESULT STDMETHODCALLTYPE Real_IDXGISwapChain1_Present1(
            IDXGISwapChain1* This,
            /* [in] */ UINT SyncInterval,
            /* [in] */ UINT PresentFlags,
            /* [annotation][in] */
            _In_  const DXGI_PRESENT_PARAMETERS* pPresentParameters) {
            return This->Present1(SyncInterval, PresentFlags, pPresentParameters);
        }
        HRESULT STDMETHODCALLTYPE My_IDXGISwapChain1_Present1(
            IDXGISwapChain1* This,
            /* [in] */ UINT SyncInterval,
            /* [in] */ UINT PresentFlags,
            /* [annotation][in] */
            _In_  const DXGI_PRESENT_PARAMETERS* pPresentParameters) {
            std::cout << "Enter My_IDXGISwapchain1_Present1" << std::endl;
            HRESULT res;
            Pre_IDXGISwapChain1_Present1(This, SyncInterval, PresentFlags, pPresentParameters);
            res = pReal_IDXGISwapChain1_Present1(This, SyncInterval, PresentFlags, pPresentParameters);
            Post_IDXGISwapChain1_Present1(This, SyncInterval, PresentFlags, pPresentParameters);
            return res;
        }
        void STDMETHODCALLTYPE Pre_IDXGISwapChain1_Present1(
            IDXGISwapChain1* This,
            /* [in] */ UINT SyncInterval,
            /* [in] */ UINT PresentFlags,
            /* [annotation][in] */
            _In_  const DXGI_PRESENT_PARAMETERS* pPresentParameters) {
            return;
        }
        void STDMETHODCALLTYPE Post_IDXGISwapChain1_Present1(
            IDXGISwapChain1* This,
            /* [in] */ UINT SyncInterval,
            /* [in] */ UINT PresentFlags,
            /* [annotation][in] */
            _In_  const DXGI_PRESENT_PARAMETERS* pPresentParameters) {
            return;
        }
#pragma endregion
}