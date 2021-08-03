#include "hook/dx/hook_dxgi.h"
#include "hook/gpc_hook_utils.h"
#include "core/tracker/gpc_swapchain_tracker.h"
#include "core/gpc_inspector.h"

namespace gpc {
    bool HookDXGIEntry(const char* dllName) {
        PRINT_FUNC_NAME
        bool res = true;
        HMODULE module = LoadLibrary(dllName);
        if (module) {
            void* pFuncAddress = GetProcAddress(module, "CreateDXGIFactory");
            if (pFuncAddress) {
                auto apiHookInfo = GetAPIHookInfo("CreateDXGIFactory");
                res &= HookFunc(pFuncAddress, My_CreateDXGIFactory, (void**)&pReal_CreateDXGIFactory, apiHookInfo);
                if (res)
                    std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
                else
                    std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
            }
            else {
                std::cout << "Get CreateDXGIFactory address failed" << std::endl;
                res = false;
            }

            pFuncAddress = GetProcAddress(module, "CreateDXGIFactory1");
            if (pFuncAddress) {
                auto apiHookInfo = GetAPIHookInfo("CreateDXGIFactory1");
                res = HookFunc(pFuncAddress, My_CreateDXGIFactory1, (void**)&pReal_CreateDXGIFactory1, apiHookInfo);
                if (res)
                    std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
                else
                    std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
            }
            else {
                std::cout << "Get CreateDXGIFactory1 address failed" << std::endl;
                res = false;
            }
            
            pFuncAddress = GetProcAddress(module, "CreateDXGIFactory2");
            if (pFuncAddress) {
                auto apiHookInfo = GetAPIHookInfo("CreateDXGIFactory2");
                res &= HookFunc(pFuncAddress, My_CreateDXGIFactory2, (void**)&pReal_CreateDXGIFactory2, apiHookInfo);
                if (res)
                    std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
                else
                    std::cout << "Hook api succeed: " << apiHookInfo->apiName << std::endl;
            }
            else {
                std::cout << "Get CreateDXGIFactory2 address failed" << std::endl;
                res = false;
            }
        }
        else {
            std::cout << "Load dxgi.lib failed" << std::endl;
            res = false;
        }
        //FreeLibrary(module);

        return res;
    }

    bool HookDXGIFactory(IDXGIFactory* pIDXGIFactory) {
        PRINT_FUNC_NAME
        if (!pIDXGIFactory) {
            return false;
        }
        HookDXGIFactoryInterface(pIDXGIFactory); //Hook IDXGIFactory
        ComPtr<IDXGIFactory2> pIDXGIFactory2 = nullptr;
        pIDXGIFactory->QueryInterface(pIDXGIFactory2.GetAddressOf());
        if (pIDXGIFactory2) {
            HookDXGIFactory2Interface(pIDXGIFactory2.Get());//Hook IDXGIFactory2
        }
        return true;
    }

    bool HookDXGIFactoryInterface(IDXGIFactory* pIDXGIFactory) {
        PRINT_FUNC_NAME
        bool res = true;
        //hook IDXGIFactory
        res &= HookInterfaceFunc((void*)pIDXGIFactory, My_IDXGIFactory_CreateSwapChain, (void**)&pReal_IDXGIFactory_CreateSwapChain, "CreateSwapChain");
		return res;
    }

    bool HookDXGIFactory2Interface(IDXGIFactory2* pIDXGIFactory2) {
        PRINT_FUNC_NAME
        bool res = true;
        //hook IDXGIFactory2
        res &= HookInterfaceFunc(pIDXGIFactory2, My_IDXGIFactory2_CreateSwapChainForHwnd, (void**)&pReal_IDXGIFactory2_CreateSwapChainForHwnd, "CreateSwapChainForHwnd");
        res &= HookInterfaceFunc(pIDXGIFactory2, My_IDXGIFactory2_CreateSwapChainForCoreWindow, (void**)&pReal_IDXGIFactory2_CreateSwapChainForCoreWindow, "CreateSwapChainForCoreWindow");
        res &= HookInterfaceFunc(pIDXGIFactory2, My_IDXGIFactory2_CreateSwapChainForComposition, (void**)&pReal_IDXGIFactory2_CreateSwapChainForComposition, "CreateSwapChainForComposition");
        
        //hook the base interface(IDXGIFactory)
        IDXGIFactory* pIDXGIFactory = nullptr;
        pIDXGIFactory2->QueryInterface(&pIDXGIFactory);
        if (pIDXGIFactory) {
            HookDXGIFactoryInterface(pIDXGIFactory);
            pIDXGIFactory->Release();
        }
        return res;
    }

    bool HookIDXGISwapChain(IDXGISwapChain* pSwapChain, IUnknown* pD3D12Device) {
        PRINT_FUNC_NAME
        if (!pSwapChain || !pD3D12Device) {
            return false;
        }

        HookDXGISwapChainInterface(pSwapChain); //Hook IDXGISwapChain
        ComPtr<IDXGISwapChain1> pIDXGISwapChain1 = nullptr;
        pSwapChain->QueryInterface(pIDXGISwapChain1.GetAddressOf());
        if (pIDXGISwapChain1) {
            HookDXGISwapChain1Interface(pIDXGISwapChain1.Get());//Hook IDXGISwapChain1
        }

        //create GPCSwapChainTracker
        auto pSwapChainTracker = GPCDXGISwapChainTracker::GetSingleton();
        pSwapChainTracker->Init(pSwapChain, pD3D12Device);
        return true;
    }

    bool HookDXGISwapChainInterface(IDXGISwapChain* pIDXGISwapChain) {
        PRINT_FUNC_NAME
        bool res = true;
        //hook IDXGISwapChain
        res &= HookInterfaceFunc((void*)pIDXGISwapChain, My_IDXGISwapChain_Present, (void**)&pReal_IDXGISwapChain_Present, "Present");
        return res;
    }

    bool HookDXGISwapChain1Interface(IDXGISwapChain1* pIDXGISwapChain1) {
        PRINT_FUNC_NAME
        bool res = true;
        //hook IDXGISwapChain1
        res &= HookInterfaceFunc((void*)pIDXGISwapChain1, My_IDXGISwapChain1_Present1, (void**)&pReal_IDXGISwapChain1_Present1, "Present1");

        //hook the base interface(IDXGISwapChain)
        IDXGISwapChain* pIDXGISwapChain = nullptr;
        pIDXGISwapChain1->QueryInterface(&pIDXGISwapChain);
        if (pIDXGISwapChain) {
            HookDXGISwapChainInterface(pIDXGISwapChain);
            pIDXGISwapChain->Release();
        }
        return res;
    }

#pragma region HookCreateDXGIFactory
    PFN_CREATEDXGIFACTORY pReal_CreateDXGIFactory = Real_CreateDXGIFactory;

    //if detour failed, default will protect app from crashing
    HRESULT WINAPI Real_CreateDXGIFactory(REFIID riid, _COM_Outptr_ void** ppFactory) {
        PRINT_FUNC_NAME
        return CreateDXGIFactory(riid, ppFactory);
    }

    HRESULT WINAPI My_CreateDXGIFactory(REFIID riid, _COM_Outptr_ void** ppFactory) {
        PRINT_FUNC_NAME
        HRESULT res;
        Pre_CreateDXGIFactory(riid, ppFactory);
        res = pReal_CreateDXGIFactory(riid, ppFactory);
        Post_CreateDXGIFactory(riid, ppFactory);
        return res;
    }

    void WINAPI Pre_CreateDXGIFactory(REFIID riid, _COM_Outptr_ void** ppFactory) {
        PRINT_FUNC_NAME
        return;
    }

    void WINAPI Post_CreateDXGIFactory(REFIID riid, _COM_Outptr_ void** ppFactory) {
        PRINT_FUNC_NAME
        static bool hookOnce = false;
        if (!hookOnce) {
            hookOnce = HookDXGIFactory((IDXGIFactory*)*ppFactory);
            if (hookOnce)
                std::cout << "Hook IDXGIFactory interfaces succeed" << std::endl;
            else
                std::cout << "Hook IDXGIFactory interfaces failed" << std::endl;
        }
        return;
    }
#pragma endregion

#pragma region HookCreateDXGIFactory1
    PFN_CREATEDXGIFACTORY1 pReal_CreateDXGIFactory1 = Real_CreateDXGIFactory1;
    HRESULT WINAPI Real_CreateDXGIFactory1(REFIID riid, _COM_Outptr_ void** ppFactory) {
        PRINT_FUNC_NAME
        return CreateDXGIFactory1(riid, ppFactory);
    }

    HRESULT WINAPI My_CreateDXGIFactory1(REFIID riid, _COM_Outptr_ void** ppFactory) {
        PRINT_FUNC_NAME
        HRESULT res;
        Pre_CreateDXGIFactory1(riid, ppFactory);
        res = pReal_CreateDXGIFactory1(riid, ppFactory);
        Post_CreateDXGIFactory1(riid, ppFactory);
        return res;
    }

    void WINAPI Pre_CreateDXGIFactory1(REFIID riid, _COM_Outptr_ void** ppFactory) {
        PRINT_FUNC_NAME
        return;
    }

    void WINAPI Post_CreateDXGIFactory1(REFIID riid, _COM_Outptr_ void** ppFactory) {
        PRINT_FUNC_NAME
        static bool hookOnce = false;
            if (!hookOnce) {
                hookOnce = HookDXGIFactory((IDXGIFactory*)*ppFactory);
                if (hookOnce)
                    std::cout << "Hook IDXGIFactory interfaces succeed" << std::endl;
                else
                    std::cout << "Hook IDXGIFactory interfaces failed" << std::endl;
            }
        return;
    }
#pragma endregion

#pragma region HookCreateDXGIFactory2
        PFN_CREATEDXGIFACTORY2 pReal_CreateDXGIFactory2 = Real_CreateDXGIFactory2;
        
        //if detour failed, default will protect app from crashing
        HRESULT WINAPI Real_CreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory) {
            PRINT_FUNC_NAME
            return CreateDXGIFactory2(Flags, riid, ppFactory);
        }

        HRESULT WINAPI My_CreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory) {
            PRINT_FUNC_NAME
            HRESULT res;
            Pre_CreateDXGIFactory2(Flags, riid, ppFactory);
            res = pReal_CreateDXGIFactory2(Flags, riid, ppFactory);
            Post_CreateDXGIFactory2(Flags, riid, ppFactory);
            return res;
        }

        void WINAPI Pre_CreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory) {
            PRINT_FUNC_NAME
            return;
        }

        void WINAPI Post_CreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory) {
            PRINT_FUNC_NAME
            static bool hookOnce = false;
            if (!hookOnce) {
                hookOnce = HookDXGIFactory((IDXGIFactory*)*ppFactory);
                if (hookOnce)
                    std::cout << "Hook IDXGIFactory interfaces succeed" << std::endl;
                else
                    std::cout << "Hook IDXGIFactory interfaces failed" << std::endl;
            }
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
            PRINT_FUNC_NAME
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
            PRINT_FUNC_NAME
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
            PRINT_FUNC_NAME
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
            PRINT_FUNC_NAME
            static bool hookOnce = false;
            if (!hookOnce) {
                hookOnce = HookIDXGISwapChain(*ppSwapChain, pDevice);
                if (hookOnce)
                    std::cout << "Hook IDXGISwapChain interfaces succeed" << std::endl;
                else
                    std::cout << "Hook IDXGISwapChain interfaces failed" << std::endl;
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
            PRINT_FUNC_NAME
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
            PRINT_FUNC_NAME
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
            PRINT_FUNC_NAME
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
            PRINT_FUNC_NAME
            static bool hookOnce = false;
            if (!hookOnce) {
                hookOnce = HookIDXGISwapChain(*ppSwapChain, pDevice);
                if (hookOnce)
                    std::cout << "Hook IDXGISwapChain interfaces succeed" << std::endl;
                else
                    std::cout << "Hook IDXGISwapChain interfaces failed" << std::endl;
            }
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
            PRINT_FUNC_NAME
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
            PRINT_FUNC_NAME
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
            PRINT_FUNC_NAME
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
            PRINT_FUNC_NAME
            static bool hookOnce = false;
            if (!hookOnce) {
                hookOnce = HookIDXGISwapChain(*ppSwapChain, pDevice);
                if (hookOnce)
                    std::cout << "Hook IDXGISwapChain interfaces succeed" << std::endl;
                else
                    std::cout << "Hook IDXGISwapChain interfaces failed" << std::endl;
            }
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
            PRINT_FUNC_NAME
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
            PRINT_FUNC_NAME
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
            PRINT_FUNC_NAME
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
            PRINT_FUNC_NAME
            static bool hookOnce = false;
            if (!hookOnce) {
                hookOnce = HookIDXGISwapChain(*ppSwapChain, pDevice);
                if (hookOnce)
                    std::cout << "Hook IDXGISwapChain interfaces succeed" << std::endl;
                else
                    std::cout << "Hook IDXGISwapChain interfaces failed" << std::endl;
            }
            return;
        }
#pragma endregion

#pragma region HookPresent
        PFN_IDXGISWAPCHAIN_PRESENT pReal_IDXGISwapChain_Present = Real_IDXGISwapChain_Present;

        HRESULT STDMETHODCALLTYPE Real_IDXGISwapChain_Present(
            IDXGISwapChain * This,
            /* [in] */ UINT SyncInterval,
            /* [in] */ UINT Flags) {
            PRINT_FUNC_NAME
            return This->Present(SyncInterval, Flags);
        }

        HRESULT STDMETHODCALLTYPE My_IDXGISwapChain_Present(
            IDXGISwapChain * This,
            /* [in] */ UINT SyncInterval,
            /* [in] */ UINT Flags) {
            PRINT_FUNC_NAME
            HRESULT res;
            Pre_IDXGISwapChain_Present(This, SyncInterval, Flags);
            res = pReal_IDXGISwapChain_Present(This, SyncInterval, Flags);
            Post_IDXGISwapChain_Present(This, SyncInterval, Flags);
            return res;
        }

        void STDMETHODCALLTYPE Pre_IDXGISwapChain_Present(
            IDXGISwapChain * This,
            /* [in] */ UINT SyncInterval,
            /* [in] */ UINT Flags) {
            PRINT_FUNC_NAME
            auto pFrameInspector = GPCInspectorManager::GetSingleton()->GetFrameInspector();
            pFrameInspector->EndFrame();
            return;
        }

        void STDMETHODCALLTYPE Post_IDXGISwapChain_Present(
            IDXGISwapChain * This,
            /* [in] */ UINT SyncInterval,
            /* [in] */ UINT Flags) {
            PRINT_FUNC_NAME
            auto pFrameInspector = GPCInspectorManager::GetSingleton()->GetFrameInspector();
            pFrameInspector->StartFrame();
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
            PRINT_FUNC_NAME
            return This->Present1(SyncInterval, PresentFlags, pPresentParameters);
        }
        HRESULT STDMETHODCALLTYPE My_IDXGISwapChain1_Present1(
            IDXGISwapChain1* This,
            /* [in] */ UINT SyncInterval,
            /* [in] */ UINT PresentFlags,
            /* [annotation][in] */
            _In_  const DXGI_PRESENT_PARAMETERS* pPresentParameters) {
            PRINT_FUNC_NAME
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
            PRINT_FUNC_NAME
            auto pFrameInspector = GPCInspectorManager::GetSingleton()->GetFrameInspector();
            pFrameInspector->EndFrame();
            return;
        }
        void STDMETHODCALLTYPE Post_IDXGISwapChain1_Present1(
            IDXGISwapChain1* This,
            /* [in] */ UINT SyncInterval,
            /* [in] */ UINT PresentFlags,
            /* [annotation][in] */
            _In_  const DXGI_PRESENT_PARAMETERS* pPresentParameters) {
            PRINT_FUNC_NAME
            auto pFrameInspector = GPCInspectorManager::GetSingleton()->GetFrameInspector();
            pFrameInspector->StartFrame();
            return;
        }
#pragma endregion
}