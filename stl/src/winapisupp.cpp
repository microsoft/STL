// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// clang-format off
// Prevent clang-format from reordering <AppModel.h> before <Windows.h>
#include <Windows.h>
#include <AppModel.h>
#include "awint.hpp"
#include <internal_shared.h>
#include <stdlib.h>
// clang-format on

#pragma warning(push)
#pragma warning(disable : 4265) // non-virtual destructor in base class
#include <wrl/wrappers/corewrappers.h>
#pragma warning(pop)
#include <intrin.h>
#include <stdint.h>

#if !defined(_CRT_WINDOWS) && !defined(UNDOCKED_WINDOWS_UCRT)
// GetCurrentPackageId retrieves the current package id, if the app is deployed via a package.
using PFNGETCURRENTPACKAGEID = BOOL(WINAPI*)(UINT32*, BYTE*);

#if !defined _CRT_APP
#if defined _ONECORE

namespace {
    struct HMODULETraits {
        using Type = HMODULE;

        static bool Close(Type const h) noexcept {
            return ::FreeLibrary(h) != FALSE;
        }

        static Type GetInvalidValue() noexcept {
            return nullptr;
        }
    };

    using HMODULEHandle = Microsoft::WRL::Wrappers::HandleT<HMODULETraits>;
} // unnamed namespace

extern "C" int __crt_IsPackagedAppHelper() {
    static wchar_t const* const possible_apisets[] = {
        L"api-ms-win-appmodel-runtime-l1-1-1.dll", // Windows 8.1+ APISet
        L"ext-ms-win-kernel32-package-current-l1-1-0.dll", // Legacy APISet
        L"appmodel.dll" // LNM implementation DLL
    };

    wchar_t const* const* const first_possible_apiset = possible_apisets;
    wchar_t const* const* const last_possible_apiset  = possible_apisets + _countof(possible_apisets);
    for (wchar_t const* const* it = first_possible_apiset; it != last_possible_apiset; ++it) {
        HMODULEHandle const apiset(LoadLibraryExW(*it, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32));
        if (!apiset.IsValid()) {
            continue;
        }

        PFNGETCURRENTPACKAGEID const get_current_package_id =
            reinterpret_cast<PFNGETCURRENTPACKAGEID>(GetProcAddress(apiset.Get(), "GetCurrentPackageId"));

        if (!get_current_package_id) {
            continue;
        }

        UINT32 buffer_length = 0;
        if (get_current_package_id(&buffer_length, nullptr) == ERROR_INSUFFICIENT_BUFFER) {
            return 1;
        } else {
            return 0;
        }
    }

    // Either the app is not packaged or we cannot determine if the app is packaged:
    return 0;
}

#else // defined _ONECORE

extern "C" int __crt_IsPackagedAppHelper() {
    LONG retValue       = APPMODEL_ERROR_NO_PACKAGE;
    UINT32 bufferLength = 0;

    IFDYNAMICGETCACHEDFUNCTION(PFNGETCURRENTPACKAGEID, GetCurrentPackageId, pfn) {
        retValue = pfn(&bufferLength, nullptr);
    }

    if (retValue == ERROR_INSUFFICIENT_BUFFER) {
        return 1;
    }

    // If GetCurrentPackageId was not found, or it returned a different error,
    // then this is NOT a Packaged app
    return 0;
}

#endif // defined _ONECORE
#endif // defined _CRT_APP


// __crtIsPackagedApp() - Check if the current app is a Packaged app
//
// Purpose:
//        Check if the current application was started through a package.
//        This determines if the app is a Packaged app or not.
//
//        This function uses a new Windows 8 API, GetCurrentPackageId, to detect
//        if the application is deployed via a package.
//
// Entry:
//        None
//
// Exit:
//        TRUE if Packaged app, FALSE if not.
extern "C" BOOL __cdecl __crtIsPackagedApp() {
#ifdef _CRT_APP
    return TRUE;
#else
    static int isPackaged = -1; // Initialize to undefined state

    // If we've already made this check, just return the prev result
    if (isPackaged < 0) {
        isPackaged = __crt_IsPackagedAppHelper();
    }

    return (isPackaged > 0) ? TRUE : FALSE;
#endif
}

#endif // !defined(_CRT_WINDOWS) && !defined(UNDOCKED_WINDOWS_UCRT)

#if _STL_WIN32_WINNT <= _WIN32_WINNT_VISTA

extern "C" BOOL __cdecl __crtInitializeCriticalSectionEx(
    __out LPCRITICAL_SECTION lpCriticalSection, __in DWORD dwSpinCount, __in DWORD Flags) {
    return InitializeCriticalSectionEx(lpCriticalSection, dwSpinCount, Flags);
}

extern "C" BOOL __cdecl __crtInitOnceExecuteOnce(
    _Inout_ PINIT_ONCE InitOnce, _In_ PINIT_ONCE_FN InitFn, _Inout_opt_ PVOID Parameter, _Out_opt_ LPVOID* Context) {
    return InitOnceExecuteOnce(InitOnce, InitFn, Parameter, Context);
}

extern "C" HANDLE __cdecl __crtCreateEventExW(__in_opt LPSECURITY_ATTRIBUTES lpEventAttributes, __in_opt LPCWSTR lpName,
    __reserved DWORD dwFlags, __in DWORD dwDesiredAccess) {
    return CreateEventExW(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
}

extern "C" HANDLE __cdecl __crtCreateSemaphoreExW(__in_opt LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
    __in LONG lInitialCount, __in LONG lMaximumCount, __in_opt LPCWSTR lpName, __reserved DWORD dwFlags,
    __in DWORD dwDesiredAccess) {
    return CreateSemaphoreExW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, dwFlags, dwDesiredAccess);
}

extern "C" PTP_TIMER __cdecl __crtCreateThreadpoolTimer(
    __in PTP_TIMER_CALLBACK pfnti, __inout_opt PVOID pv, __in_opt PTP_CALLBACK_ENVIRON pcbe) {
    return CreateThreadpoolTimer(pfnti, pv, pcbe);
}

extern "C" VOID __cdecl __crtSetThreadpoolTimer(
    __inout PTP_TIMER pti, __in_opt PFILETIME pftDueTime, __in DWORD msPeriod, __in_opt DWORD msWindowLength) {
    SetThreadpoolTimer(pti, pftDueTime, msPeriod, msWindowLength);
}

extern "C" VOID __cdecl __crtWaitForThreadpoolTimerCallbacks(__inout PTP_TIMER pti, __in BOOL fCancelPendingCallbacks) {
    WaitForThreadpoolTimerCallbacks(pti, fCancelPendingCallbacks);
}

extern "C" VOID __cdecl __crtCloseThreadpoolTimer(__inout PTP_TIMER pti) {
    CloseThreadpoolTimer(pti);
}

extern "C" PTP_WAIT __cdecl __crtCreateThreadpoolWait(
    __in PTP_WAIT_CALLBACK pfnwa, __inout_opt PVOID pv, __in_opt PTP_CALLBACK_ENVIRON pcbe) {
    return CreateThreadpoolWait(pfnwa, pv, pcbe);
}

extern "C" VOID __cdecl __crtSetThreadpoolWait(__inout PTP_WAIT pwa, __in_opt HANDLE h, __in_opt PFILETIME pftTimeout) {
    SetThreadpoolWait(pwa, h, pftTimeout);
}

extern "C" VOID __cdecl __crtCloseThreadpoolWait(__inout PTP_WAIT pwa) {
    CloseThreadpoolWait(pwa);
}

extern "C" VOID __cdecl __crtFlushProcessWriteBuffers() {
    FlushProcessWriteBuffers();
}

extern "C" VOID __cdecl __crtFreeLibraryWhenCallbackReturns(__inout PTP_CALLBACK_INSTANCE pci, __in HMODULE mod) {
    FreeLibraryWhenCallbackReturns(pci, mod);
}

extern "C" DWORD __cdecl __crtGetCurrentProcessorNumber() {
    return GetCurrentProcessorNumber();
}

extern "C" BOOLEAN __cdecl __crtCreateSymbolicLinkW(
    __in LPCWSTR lpSymlinkFileName, __in LPCWSTR lpTargetFileName, __in DWORD dwFlags) {
    return CreateSymbolicLinkW(lpSymlinkFileName, lpTargetFileName, dwFlags);
}

extern "C" BOOL __cdecl __crtGetFileInformationByHandleEx(_In_ HANDLE hFile,
    _In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass, _Out_ LPVOID lpFileInformation, _In_ DWORD dwBufferSize) {
    return GetFileInformationByHandleEx(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
}

extern "C" BOOL __cdecl __crtSetFileInformationByHandle(_In_ HANDLE hFile,
    _In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass, _In_ LPVOID lpFileInformation, _In_ DWORD dwBufferSize) {
    return SetFileInformationByHandle(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
}

extern "C" ULONGLONG __cdecl __crtGetTickCount64() {
    return GetTickCount64();
}

extern "C" int __cdecl __crtCompareStringEx(_In_opt_ LPCWSTR lpLocaleName, _In_ DWORD dwCmpFlags,
    _In_NLS_string_(cchCount1) LPCWCH lpString1, _In_ int cchCount1, _In_NLS_string_(cchCount2) LPCWCH lpString2,
    _In_ int cchCount2) {
    return CompareStringEx(lpLocaleName, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2, nullptr, nullptr, 0);
}

extern "C" int __cdecl __crtLCMapStringEx(_In_opt_ LPCWSTR lpLocaleName, _In_ DWORD dwMapFlags,
    _In_reads_(cchSrc) LPCWSTR lpSrcStr, _In_ int cchSrc, _Out_writes_opt_(cchDest) LPWSTR lpDestStr,
    _In_ int cchDest) {
    return LCMapStringEx(lpLocaleName, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest, nullptr, nullptr, 0);
}

extern "C" int __cdecl __crtGetLocaleInfoEx(
    _In_opt_ LPCWSTR lpLocaleName, _In_ LCTYPE LCType, _Out_opt_ LPWSTR lpLCData, _In_ int cchData) {
    return GetLocaleInfoEx(lpLocaleName, LCType, lpLCData, cchData);
}

#endif // _STL_WIN32_WINNT <= _WIN32_WINNT_VISTA

#if _STL_WIN32_WINNT < _WIN32_WINNT_WIN7

extern "C" BOOLEAN __cdecl __crtTryAcquireSRWLockExclusive(PSRWLOCK const pLock) {
    DYNAMICGETCACHEDFUNCTION(PFNTRYACQUIRESRWLOCKEXCLUSIVE, TryAcquireSRWLockExclusive, pfTryAcquireSRWLockExclusive);
    return pfTryAcquireSRWLockExclusive(pLock);
    // Don't have fallbacks because the only caller (in primitives.hpp) will check the existence before calling
}

#endif // _STL_WIN32_WINNT < _WIN32_WINNT_WIN7


#if _STL_WIN32_WINNT < _WIN32_WINNT_WIN8

extern "C" void __cdecl __crtGetSystemTimePreciseAsFileTime(_Out_ LPFILETIME lpSystemTimeAsFileTime) {
    // use GetSystemTimePreciseAsFileTime if it is available (only on Windows 8+)...
    IFDYNAMICGETCACHEDFUNCTION(
        PFNGETSYSTEMTIMEPRECISEASFILETIME, GetSystemTimePreciseAsFileTime, pfGetSystemTimePreciseAsFileTime) {
        pfGetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime);
        return;
    }

    // ...otherwise use GetSystemTimeAsFileTime.
    GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
}

#endif // _STL_WIN32_WINNT < _WIN32_WINNT_WIN8


// Helper to load all necessary Win32 API function pointers

#if defined _ONECORE

// All APIs are statically available, and we can't call GetModuleHandleW().

#else // defined _ONECORE

extern "C" PVOID __KERNEL32Functions[eMaxKernel32Function] = {0};

static int __cdecl initialize_pointers() {
    HINSTANCE hKernel32 = GetModuleHandleW(L"kernel32.dll");

    STOREFUNCTIONPOINTER(hKernel32, FlsAlloc);
    STOREFUNCTIONPOINTER(hKernel32, FlsFree);
    STOREFUNCTIONPOINTER(hKernel32, FlsGetValue);
    STOREFUNCTIONPOINTER(hKernel32, FlsSetValue);
    STOREFUNCTIONPOINTER(hKernel32, InitializeCriticalSectionEx);
    STOREFUNCTIONPOINTER(hKernel32, InitOnceExecuteOnce);
    STOREFUNCTIONPOINTER(hKernel32, CreateEventExW);
    STOREFUNCTIONPOINTER(hKernel32, CreateSemaphoreW);
    STOREFUNCTIONPOINTER(hKernel32, CreateSemaphoreExW);
    STOREFUNCTIONPOINTER(hKernel32, CreateThreadpoolTimer);
    STOREFUNCTIONPOINTER(hKernel32, SetThreadpoolTimer);
    STOREFUNCTIONPOINTER(hKernel32, WaitForThreadpoolTimerCallbacks);
    STOREFUNCTIONPOINTER(hKernel32, CloseThreadpoolTimer);
    STOREFUNCTIONPOINTER(hKernel32, CreateThreadpoolWait);
    STOREFUNCTIONPOINTER(hKernel32, SetThreadpoolWait);
    STOREFUNCTIONPOINTER(hKernel32, CloseThreadpoolWait);
    STOREFUNCTIONPOINTER(hKernel32, FlushProcessWriteBuffers);
    STOREFUNCTIONPOINTER(hKernel32, FreeLibraryWhenCallbackReturns);
    STOREFUNCTIONPOINTER(hKernel32, GetCurrentProcessorNumber);
    STOREFUNCTIONPOINTER(hKernel32, CreateSymbolicLinkW);
#if defined(_CRT_WINDOWS) || defined(UNDOCKED_WINDOWS_UCRT)
    STOREFUNCTIONPOINTER(hKernel32, SetDefaultDllDirectories);
    STOREFUNCTIONPOINTER(hKernel32, EnumSystemLocalesEx);
    STOREFUNCTIONPOINTER(hKernel32, CompareStringEx);
    STOREFUNCTIONPOINTER(hKernel32, GetLocaleInfoEx);
    STOREFUNCTIONPOINTER(hKernel32, GetUserDefaultLocaleName);
    STOREFUNCTIONPOINTER(hKernel32, IsValidLocaleName);
    STOREFUNCTIONPOINTER(hKernel32, LCMapStringEx);
#else
    STOREFUNCTIONPOINTER(hKernel32, GetCurrentPackageId);
#endif
    STOREFUNCTIONPOINTER(hKernel32, GetTickCount64);
    STOREFUNCTIONPOINTER(hKernel32, GetFileInformationByHandleEx);
    STOREFUNCTIONPOINTER(hKernel32, SetFileInformationByHandle);
    STOREFUNCTIONPOINTER(hKernel32, GetSystemTimePreciseAsFileTime);
    STOREFUNCTIONPOINTER(hKernel32, InitializeConditionVariable);
    STOREFUNCTIONPOINTER(hKernel32, WakeConditionVariable);
    STOREFUNCTIONPOINTER(hKernel32, WakeAllConditionVariable);
    STOREFUNCTIONPOINTER(hKernel32, SleepConditionVariableCS);
    STOREFUNCTIONPOINTER(hKernel32, InitializeSRWLock);
    STOREFUNCTIONPOINTER(hKernel32, AcquireSRWLockExclusive);
    STOREFUNCTIONPOINTER(hKernel32, TryAcquireSRWLockExclusive);
    STOREFUNCTIONPOINTER(hKernel32, ReleaseSRWLockExclusive);
    STOREFUNCTIONPOINTER(hKernel32, SleepConditionVariableSRW);
    STOREFUNCTIONPOINTER(hKernel32, CreateThreadpoolWork);
    STOREFUNCTIONPOINTER(hKernel32, SubmitThreadpoolWork);
    STOREFUNCTIONPOINTER(hKernel32, CloseThreadpoolWork);
#if !defined(_CRT_WINDOWS) && !defined(UNDOCKED_WINDOWS_UCRT)
    STOREFUNCTIONPOINTER(hKernel32, CompareStringEx);
    STOREFUNCTIONPOINTER(hKernel32, GetLocaleInfoEx);
    STOREFUNCTIONPOINTER(hKernel32, LCMapStringEx);
#endif

    return 0;
}

_CRTALLOC(".CRT$XIC") static _PIFV pinit = initialize_pointers;

#endif // defined _ONECORE
