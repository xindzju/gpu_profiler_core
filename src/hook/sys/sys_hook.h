#pragma once
#include "gpc_hook_utils.h"

namespace gpc {
#pragma region HookCreateProcessA
    typedef BOOL(WINAPI* PFN_CreateProcessA)(
        _In_opt_ LPCSTR lpApplicationName,
        _Inout_opt_ LPSTR lpCommandLine,
        _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
        _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
        _In_ BOOL bInheritHandles,
        _In_ DWORD dwCreationFlags,
        _In_opt_ LPVOID lpEnvironment,
        _In_opt_ LPCSTR lpCurrentDirectory,
        _In_ LPSTARTUPINFOA lpStartupInfo,
        _Out_ LPPROCESS_INFORMATION lpProcessInformation
        );
    extern PFN_CreateProcessA pReal_CreateProcessA;
    BOOL WINAPI Real_CreateProcessA(
        _In_opt_ LPCSTR lpApplicationName,
        _Inout_opt_ LPSTR lpCommandLine,
        _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
        _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
        _In_ BOOL bInheritHandles,
        _In_ DWORD dwCreationFlags,
        _In_opt_ LPVOID lpEnvironment,
        _In_opt_ LPCSTR lpCurrentDirectory,
        _In_ LPSTARTUPINFOA lpStartupInfo,
        _Out_ LPPROCESS_INFORMATION lpProcessInformation
    );
    BOOL WINAPI My_CreateProcessA(
            _In_opt_ LPCSTR lpApplicationName,
            _Inout_opt_ LPSTR lpCommandLine,
            _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
            _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
            _In_ BOOL bInheritHandles,
            _In_ DWORD dwCreationFlags,
            _In_opt_ LPVOID lpEnvironment,
            _In_opt_ LPCSTR lpCurrentDirectory,
            _In_ LPSTARTUPINFOA lpStartupInfo,
            _Out_ LPPROCESS_INFORMATION lpProcessInformation
        );
    void WINAPI Pre_CreateProcessA(
        _In_opt_ LPCSTR lpApplicationName,
        _Inout_opt_ LPSTR lpCommandLine,
        _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
        _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
        _In_ BOOL bInheritHandles,
        _In_ DWORD dwCreationFlags,
        _In_opt_ LPVOID lpEnvironment,
        _In_opt_ LPCSTR lpCurrentDirectory,
        _In_ LPSTARTUPINFOA lpStartupInfo,
        _Out_ LPPROCESS_INFORMATION lpProcessInformation
    );
    void WINAPI Post_CreateProcessA(
        _In_opt_ LPCSTR lpApplicationName,
        _Inout_opt_ LPSTR lpCommandLine,
        _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
        _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
        _In_ BOOL bInheritHandles,
        _In_ DWORD dwCreationFlags,
        _In_opt_ LPVOID lpEnvironment,
        _In_opt_ LPCSTR lpCurrentDirectory,
        _In_ LPSTARTUPINFOA lpStartupInfo,
        _Out_ LPPROCESS_INFORMATION lpProcessInformation
    );
#pragma endregion

#pragma region HookCreateProcessW
    BOOL WINAPI CreateProcessW(
            _In_opt_ LPCWSTR lpApplicationName,
            _Inout_opt_ LPWSTR lpCommandLine,
            _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
            _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
            _In_ BOOL bInheritHandles,
            _In_ DWORD dwCreationFlags,
            _In_opt_ LPVOID lpEnvironment,
            _In_opt_ LPCWSTR lpCurrentDirectory,
            _In_ LPSTARTUPINFOW lpStartupInfo,
            _Out_ LPPROCESS_INFORMATION lpProcessInformation
        );
#pragma endregion

#pragma region HookMouse
#pragma endregion

#pragma region HookKeyboard
#pragma endregion
}