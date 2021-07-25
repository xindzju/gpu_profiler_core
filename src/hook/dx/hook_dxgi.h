#pragma once
/*
* header file: dxgi.h
* hook entry: CreateDXGIFactory
* dxgi interface: IDXGIFactory
* dxgi method: IDXGIFactory::CreateSwapChain
* swapchain interface: IDXGISwapChain
* swapchain method: IDXGISwapChain::Present
*/
#include <dxgi.h>  
/*
* header file: dxgi1_2.h
* dxgi interface: IDXGIFactory2
* dxgi method: IDXGIFactory2::CreateSwapChainForHwnd(https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_2/nf-dxgi1_2-idxgifactory2-createswapchainforhwnd)
* dxgi method: IDXGIFactory2::CreateSwapChainForCoreWindow(https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_2/nf-dxgi1_2-idxgifactory2-createswapchainforcorewindow)
* dxgi method: IDXGIFactory2::CreateSwapChainForComposition(https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_2/nf-dxgi1_2-idxgifactory2-createswapchainforcomposition)
* swapchain interface: IDXGISwapChain1
* swapchain method: IDXGISwapChain1::Present1
*/
#include <dxgi1_2.h>
/* 
* header file: dxgi1_3.h
* hook entry: CreateDXGIFactory2	
*/
#include <dxgi1_3.h>
#include "utils/gpc_utils.h"

/*
* Note: we need hook CreateDXGIFactory API first, now we have 3 variants of CreateDXGIFactory
* CreateDXGIFactory, CreateDXGIFactory1, CreateDXGIFactory2
* API calling sequence: app create DXGIFactory -> DXGIFactory create SwapChain -> SwapChain present front buffer to output(monitor)
*/
namespace gpc {
	bool HookDXGIEntry(const char* dllName);
	//dxgi 1~7
	bool HookDXGIFactory(IDXGIFactory* pIDXGIFactory); //all dxgifactory interfaces hook entries
	bool HookDXGIFactoryInterface(IDXGIFactory* pIDXGIFactory);
	bool HookDXGIFactory2Interface(IDXGIFactory2* pIDXGIFactory2);
	//swap chain 1~4
	bool HookIDXGISwapChain(IDXGISwapChain* pSwapChain = nullptr, IUnknown* pD3D12Device = nullptr); //all swapchain interfaces hook entries
	bool HookDXGISwapChainInterface(IDXGISwapChain* pIDXGISwapChain);
	bool HookDXGISwapChain1Interface(IDXGISwapChain1* pIDXGISwapChain1);
	
	//dxgi1~7
#pragma region HookCreateDXGIFactory
	typedef HRESULT(STDMETHODCALLTYPE* PFN_CREATEDXGIFACTORY)(REFIID riid, _COM_Outptr_ void** ppFactory); //not found in dxgi.h, manually added
	extern PFN_CREATEDXGIFACTORY pReal_CreateDXGIFactory;
	HRESULT WINAPI Real_CreateDXGIFactory(REFIID riid, _COM_Outptr_ void** ppFactory);
	HRESULT WINAPI My_CreateDXGIFactory(REFIID riid, _COM_Outptr_ void** ppFactory);
	void Pre_CreateDXGIFactory(REFIID riid, _COM_Outptr_ void** ppFactory);
	void Post_CreateDXGIFactory(REFIID riid, _COM_Outptr_ void** ppFactory);
#pragma endregion

#pragma region HookCreateDXGIFactory1
	typedef HRESULT(STDMETHODCALLTYPE* PFN_CREATEDXGIFACTORY1)(REFIID riid, _COM_Outptr_ void** ppFactory); //not found in dxgi.h, manually added
	extern PFN_CREATEDXGIFACTORY1 pReal_CreateDXGIFactory1;
	HRESULT WINAPI Real_CreateDXGIFactory1(REFIID riid, _COM_Outptr_ void** ppFactory);
	HRESULT WINAPI My_CreateDXGIFactory1(REFIID riid, _COM_Outptr_ void** ppFactory);
	void Pre_CreateDXGIFactory1(REFIID riid, _COM_Outptr_ void** ppFactory);
	void Post_CreateDXGIFactory1(REFIID riid, _COM_Outptr_ void** ppFactory);
#pragma endregion

#pragma region HookCreateDXGIFactory2
	typedef HRESULT(STDMETHODCALLTYPE* PFN_CREATEDXGIFACTORY2)(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory); //not found in dxgi1_3.h, manually added
	extern PFN_CREATEDXGIFACTORY2 pReal_CreateDXGIFactory2;
	HRESULT WINAPI Real_CreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory);
	HRESULT WINAPI My_CreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory);
	void WINAPI Pre_CreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory);
	void WINAPI Post_CreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory); //hook dxgi and it's methods(CreateSwapChain)
#pragma endregion

#pragma region HookIDXGIFactory_CreateSwapChain
	typedef HRESULT(STDMETHODCALLTYPE* PFN_IDXGIFACTORY_CREATE_SWAPCHAIN)(
		IDXGIFactory* This,
		/* [annotation][in] */
		_In_  IUnknown* pDevice,
		/* [annotation][in] */
		_In_  DXGI_SWAP_CHAIN_DESC* pDesc,
		/* [annotation][out] */
		_COM_Outptr_  IDXGISwapChain** ppSwapChain);
	extern PFN_IDXGIFACTORY_CREATE_SWAPCHAIN pReal_IDXGIFactory_CreateSwapChain;
	HRESULT STDMETHODCALLTYPE Real_IDXGIFactory_CreateSwapChain(
		IDXGIFactory* This,
		/* [annotation][in] */
		_In_  IUnknown* pDevice,
		/* [annotation][in] */
		_In_  DXGI_SWAP_CHAIN_DESC* pDesc,
		/* [annotation][out] */
		_COM_Outptr_  IDXGISwapChain** ppSwapChain);
	HRESULT STDMETHODCALLTYPE My_IDXGIFactory_CreateSwapChain(
		IDXGIFactory* This,
		/* [annotation][in] */
		_In_  IUnknown* pDevice,
		/* [annotation][in] */
		_In_  DXGI_SWAP_CHAIN_DESC* pDesc,
		/* [annotation][out] */
		_COM_Outptr_  IDXGISwapChain** ppSwapChain);
	void STDMETHODCALLTYPE Pre_IDXGIFactory_CreateSwapChain(
		IDXGIFactory* This,
		/* [annotation][in] */
		_In_  IUnknown* pDevice,
		/* [annotation][in] */
		_In_  DXGI_SWAP_CHAIN_DESC* pDesc,
		/* [annotation][out] */
		_COM_Outptr_  IDXGISwapChain** ppSwapChain);
	void STDMETHODCALLTYPE Post_IDXGIFactory_CreateSwapChain(
		IDXGIFactory* This,
		/* [annotation][in] */
		_In_  IUnknown* pDevice,
		/* [annotation][in] */
		_In_  DXGI_SWAP_CHAIN_DESC* pDesc,
		/* [annotation][out] */
		_COM_Outptr_  IDXGISwapChain** ppSwapChain);
#pragma endregion

#pragma region HookIDXGIFactory2_CreateSwapChainForHwnd
	typedef HRESULT (STDMETHODCALLTYPE* PFN_IDXGIFactory2_CreateSwapChainForHwnd)(
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
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
	extern PFN_IDXGIFactory2_CreateSwapChainForHwnd pReal_IDXGIFactory2_CreateSwapChainForHwnd;
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
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
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
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
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
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
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
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
#pragma endregion

#pragma region HookIDXGIFactory2_CreateSwapChainForCoreWindow
	typedef HRESULT(STDMETHODCALLTYPE* PFN_IDXGIFactory2_CreateSwapChainForCoreWindow)(
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
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
	extern PFN_IDXGIFactory2_CreateSwapChainForCoreWindow pReal_IDXGIFactory2_CreateSwapChainForCoreWindow;
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
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
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
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
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
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
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
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
#pragma endregion

#pragma region HookIDXGIFactory2_CreateSwapChainForComposition
	typedef HRESULT (STDMETHODCALLTYPE* PFN_IDXGIFactory2_CreateSwapChainForComposition)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  IUnknown* pDevice,
		/* [annotation][in] */
		_In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
		/* [annotation][in] */
		_In_opt_  IDXGIOutput* pRestrictToOutput,
		/* [annotation][out] */
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
	extern PFN_IDXGIFactory2_CreateSwapChainForComposition pReal_IDXGIFactory2_CreateSwapChainForComposition;
	HRESULT STDMETHODCALLTYPE Real_IDXGIFactory2_CreateSwapChainForComposition(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  IUnknown* pDevice,
		/* [annotation][in] */
		_In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
		/* [annotation][in] */
		_In_opt_  IDXGIOutput* pRestrictToOutput,
		/* [annotation][out] */
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
	HRESULT STDMETHODCALLTYPE My_IDXGIFactory2_CreateSwapChainForComposition(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  IUnknown* pDevice,
		/* [annotation][in] */
		_In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
		/* [annotation][in] */
		_In_opt_  IDXGIOutput* pRestrictToOutput,
		/* [annotation][out] */
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
	void STDMETHODCALLTYPE Pre_IDXGIFactory2_CreateSwapChainForComposition(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  IUnknown* pDevice,
		/* [annotation][in] */
		_In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
		/* [annotation][in] */
		_In_opt_  IDXGIOutput* pRestrictToOutput,
		/* [annotation][out] */
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
	void STDMETHODCALLTYPE Post_IDXGIFactory2_CreateSwapChainForComposition(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  IUnknown* pDevice,
		/* [annotation][in] */
		_In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
		/* [annotation][in] */
		_In_opt_  IDXGIOutput* pRestrictToOutput,
		/* [annotation][out] */
		_COM_Outptr_  IDXGISwapChain1** ppSwapChain);
#pragma endregion

#pragma region HookPresent
	typedef HRESULT(STDMETHODCALLTYPE* PFN_IDXGISWAPCHAIN_PRESENT)(
		IDXGISwapChain* This,
		/* [in] */ UINT SyncInterval,
		/* [in] */ UINT Flags);
	extern PFN_IDXGISWAPCHAIN_PRESENT pReal_IDXGISwapChain_Present;
	HRESULT STDMETHODCALLTYPE Real_IDXGISwapChain_Present(
		IDXGISwapChain* This,
		/* [in] */ UINT SyncInterval,
		/* [in] */ UINT Flags);
	HRESULT STDMETHODCALLTYPE My_IDXGISwapChain_Present(
		IDXGISwapChain* This,
		/* [in] */ UINT SyncInterval,
		/* [in] */ UINT Flags);
	void STDMETHODCALLTYPE Pre_IDXGISwapChain_Present(
		IDXGISwapChain* This,
		/* [in] */ UINT SyncInterval,
		/* [in] */ UINT Flags);
	void STDMETHODCALLTYPE Post_IDXGISwapChain_Present(
		IDXGISwapChain* This,
		/* [in] */ UINT SyncInterval,
		/* [in] */ UINT Flags);
#pragma endregion

#pragma region HookIDXGISwapChain1_Present1
	typedef HRESULT (STDMETHODCALLTYPE* PFN_IDXGISwapChain1_Present1)(
		IDXGISwapChain1* This,
		/* [in] */ UINT SyncInterval,
		/* [in] */ UINT PresentFlags,
		/* [annotation][in] */
		_In_  const DXGI_PRESENT_PARAMETERS* pPresentParameters);
	extern PFN_IDXGISwapChain1_Present1 pReal_IDXGISwapChain1_Present1;
	HRESULT STDMETHODCALLTYPE Real_IDXGISwapChain1_Present1(
		IDXGISwapChain1* This,
		/* [in] */ UINT SyncInterval,
		/* [in] */ UINT PresentFlags,
		/* [annotation][in] */
		_In_  const DXGI_PRESENT_PARAMETERS* pPresentParameters);
	HRESULT STDMETHODCALLTYPE My_IDXGISwapChain1_Present1(
		IDXGISwapChain1* This,
		/* [in] */ UINT SyncInterval,
		/* [in] */ UINT PresentFlags,
		/* [annotation][in] */
		_In_  const DXGI_PRESENT_PARAMETERS* pPresentParameters);
	void STDMETHODCALLTYPE Pre_IDXGISwapChain1_Present1(
		IDXGISwapChain1* This,
		/* [in] */ UINT SyncInterval,
		/* [in] */ UINT PresentFlags,
		/* [annotation][in] */
		_In_  const DXGI_PRESENT_PARAMETERS* pPresentParameters);
	void STDMETHODCALLTYPE Post_IDXGISwapChain1_Present1(
		IDXGISwapChain1* This,
		/* [in] */ UINT SyncInterval,
		/* [in] */ UINT PresentFlags,
		/* [annotation][in] */
		_In_  const DXGI_PRESENT_PARAMETERS* pPresentParameters);
#pragma endregion
}