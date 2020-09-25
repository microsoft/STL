// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// clang-format off
// Prevent clang-format from reordering <AppModel.h> before <Windows.h>
#include <Windows.h>
#include <AppModel.h> // for APPMODEL_ERROR_NO_PACKAGE
#include "awint.hpp"
#include <internal_shared.h>
#include <cstdlib>
// clang-format on

#pragma warning(push)
#pragma warning(disable : 4265) // non-virtual destructor in base class
#include <wrl/wrappers/corewrappers.h>
#pragma warning(pop)
#include <cstdint>

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


#if _STL_WIN32_WINNT < _WIN32_WINNT_WS03

// TRANSITION, ABI: preserved for binary compatibility
extern "C" DWORD __cdecl __crtFlsAlloc(_In_opt_ PFLS_CALLBACK_FUNCTION const lpCallback) {
    return FlsAlloc(lpCallback);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" BOOL __cdecl __crtFlsFree(_In_ DWORD const dwFlsIndex) {
    return FlsFree(dwFlsIndex);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" PVOID __cdecl __crtFlsGetValue(_In_ DWORD const dwFlsIndex) {
    return FlsGetValue(dwFlsIndex);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" BOOL __cdecl __crtFlsSetValue(_In_ DWORD const dwFlsIndex, _In_opt_ PVOID const lpFlsData) {
    return FlsSetValue(dwFlsIndex, lpFlsData);
}

#endif // _STL_WIN32_WINNT < _WIN32_WINNT_WS03


#if _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 ULONGLONG __cdecl __crtGetTickCount64() {
    return GetTickCount64();
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 BOOL __cdecl __crtInitializeCriticalSectionEx(
    _Out_ LPCRITICAL_SECTION const lpCriticalSection, _In_ DWORD const dwSpinCount, _In_ DWORD const Flags) {
    return InitializeCriticalSectionEx(lpCriticalSection, dwSpinCount, Flags);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 BOOL __cdecl __crtInitOnceExecuteOnce(_Inout_ PINIT_ONCE const InitOnce,
    _In_ PINIT_ONCE_FN const InitFn, _Inout_opt_ PVOID const Parameter, LPVOID* const Context) {
    return InitOnceExecuteOnce(InitOnce, InitFn, Parameter, Context);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 HANDLE __cdecl __crtCreateEventExW(_In_opt_ LPSECURITY_ATTRIBUTES const lpEventAttributes,
    _In_opt_ LPCWSTR const lpName, _In_ DWORD const dwFlags, _In_ DWORD const dwDesiredAccess) {
    return CreateEventExW(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 HANDLE __cdecl __crtCreateSemaphoreExW(_In_opt_ LPSECURITY_ATTRIBUTES const lpSemaphoreAttributes,
    _In_ LONG const lInitialCount, _In_ LONG const lMaximumCount, _In_opt_ LPCWSTR const lpName,
    _Reserved_ DWORD const dwFlags, _In_ DWORD const dwDesiredAccess) {
    return CreateSemaphoreExW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, dwFlags, dwDesiredAccess);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 PTP_TIMER __cdecl __crtCreateThreadpoolTimer(
    _In_ PTP_TIMER_CALLBACK const pfnti, _Inout_opt_ PVOID const pv, _In_opt_ PTP_CALLBACK_ENVIRON const pcbe) {
    return CreateThreadpoolTimer(pfnti, pv, pcbe);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 VOID __cdecl __crtSetThreadpoolTimer(_Inout_ PTP_TIMER const pti,
    _In_opt_ PFILETIME const pftDueTime, _In_ DWORD const msPeriod, _In_opt_ DWORD const msWindowLength) {
    SetThreadpoolTimer(pti, pftDueTime, msPeriod, msWindowLength);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 VOID __cdecl __crtWaitForThreadpoolTimerCallbacks(
    _Inout_ PTP_TIMER const pti, _In_ BOOL const fCancelPendingCallbacks) {
    WaitForThreadpoolTimerCallbacks(pti, fCancelPendingCallbacks);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 VOID __cdecl __crtCloseThreadpoolTimer(_Inout_ PTP_TIMER const pti) {
    CloseThreadpoolTimer(pti);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 PTP_WAIT __cdecl __crtCreateThreadpoolWait(
    _In_ PTP_WAIT_CALLBACK const pfnwa, _Inout_opt_ PVOID const pv, _In_opt_ PTP_CALLBACK_ENVIRON const pcbe) {
    return CreateThreadpoolWait(pfnwa, pv, pcbe);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 VOID __cdecl __crtSetThreadpoolWait(
    _Inout_ PTP_WAIT const pwa, _In_opt_ HANDLE const h, _In_opt_ PFILETIME const pftTimeout) {
    SetThreadpoolWait(pwa, h, pftTimeout);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 VOID __cdecl __crtCloseThreadpoolWait(_Inout_ PTP_WAIT const pwa) {
    CloseThreadpoolWait(pwa);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 VOID __cdecl __crtFlushProcessWriteBuffers() {
    FlushProcessWriteBuffers();
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 VOID __cdecl __crtFreeLibraryWhenCallbackReturns(
    _Inout_ PTP_CALLBACK_INSTANCE const pci, _In_ HMODULE const mod) {
    FreeLibraryWhenCallbackReturns(pci, mod);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 DWORD __cdecl __crtGetCurrentProcessorNumber() {
    return GetCurrentProcessorNumber();
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 BOOLEAN __cdecl __crtCreateSymbolicLinkW(
    _In_ LPCWSTR const lpSymlinkFileName, _In_ LPCWSTR const lpTargetFileName, _In_ DWORD const dwFlags) {
#ifdef _CRT_APP
    (void) lpSymlinkFileName;
    (void) lpTargetFileName;
    (void) dwFlags;
    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
#else // _CRT_APP
    return CreateSymbolicLinkW(lpSymlinkFileName, lpTargetFileName, dwFlags);
#endif // _CRT_APP
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 _Success_(return ) BOOL __cdecl __crtGetFileInformationByHandleEx(_In_ HANDLE const hFile,
    _In_ FILE_INFO_BY_HANDLE_CLASS const FileInformationClass,
    _Out_writes_bytes_(dwBufferSize) LPVOID const lpFileInformation, _In_ DWORD const dwBufferSize) {
    return GetFileInformationByHandleEx(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 BOOL __cdecl __crtSetFileInformationByHandle(_In_ HANDLE const hFile,
    _In_ FILE_INFO_BY_HANDLE_CLASS const FileInformationClass,
    _In_reads_bytes_(dwBufferSize) LPVOID const lpFileInformation, _In_ DWORD const dwBufferSize) {
    return SetFileInformationByHandle(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" VOID __cdecl __crtInitializeConditionVariable(_Out_ PCONDITION_VARIABLE const pCond) {
    InitializeConditionVariable(pCond);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" VOID __cdecl __crtWakeConditionVariable(_Inout_ PCONDITION_VARIABLE const pCond) {
    WakeConditionVariable(pCond);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" VOID __cdecl __crtWakeAllConditionVariable(_Inout_ PCONDITION_VARIABLE const pCond) {
    WakeAllConditionVariable(pCond);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" BOOL __cdecl __crtSleepConditionVariableCS(
    _Inout_ PCONDITION_VARIABLE const pCond, _Inout_ PCRITICAL_SECTION const pLock, _In_ DWORD const dwMs) {
    return SleepConditionVariableCS(pCond, pLock, dwMs);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" VOID __cdecl __crtInitializeSRWLock(_Out_ PSRWLOCK const pLock) {
    InitializeSRWLock(pLock);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" VOID __cdecl __crtAcquireSRWLockExclusive(_Inout_ PSRWLOCK const pLock) {
    AcquireSRWLockExclusive(pLock);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" VOID __cdecl __crtReleaseSRWLockExclusive(_Inout_ PSRWLOCK const pLock) {
    ReleaseSRWLockExclusive(pLock);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" BOOL __cdecl __crtSleepConditionVariableSRW(_Inout_ PCONDITION_VARIABLE const pCond,
    _Inout_ PSRWLOCK const pLock, _In_ DWORD const dwMs, _In_ ULONG const flags) {
    return SleepConditionVariableSRW(pCond, pLock, dwMs, flags);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" PTP_WORK __cdecl __crtCreateThreadpoolWork(
    _In_ PTP_WORK_CALLBACK const pfnwk, _Inout_opt_ PVOID const pv, _In_opt_ PTP_CALLBACK_ENVIRON const pcbe) {
    return CreateThreadpoolWork(pfnwk, pv, pcbe);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" VOID __cdecl __crtSubmitThreadpoolWork(_Inout_ PTP_WORK const pwk) {
    SubmitThreadpoolWork(pwk);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" VOID __cdecl __crtCloseThreadpoolWork(_Inout_ PTP_WORK const pwk) {
    CloseThreadpoolWork(pwk);
}

#else // _STL_WIN32_WINNT < _WIN32_WINNT_VISTA
// TRANSITION, ABI: preserved for binary compatibility
extern "C" BOOL __cdecl __crtQueueUserWorkItem(_In_ LPTHREAD_START_ROUTINE, _In_opt_ PVOID, _In_ ULONG) {
    // This function doesn't have an implementation as it is only used on Windows XP
    return 0;
}
#endif // _STL_WIN32_WINNT < _WIN32_WINNT_VISTA


#if _STL_WIN32_WINNT < _WIN32_WINNT_WIN7

extern "C" BOOLEAN __cdecl __crtTryAcquireSRWLockExclusive(_Inout_ PSRWLOCK const pLock) {
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

extern "C" PVOID __KERNEL32Functions[eMaxKernel32Function] = {nullptr};

static int __cdecl initialize_pointers() {
    HINSTANCE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    _Analysis_assume_(hKernel32);

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
