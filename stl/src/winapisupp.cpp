// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// clang-format off
// Prevent clang-format from reordering <AppModel.h> before <Windows.h>
#include <Windows.h>
#include <AppModel.h>
#include "awint.h"
#include <internal_shared.h>
#include <stdlib.h>
// clang-format on

#pragma warning(push)
#pragma warning(disable : 4265) // non-virtual destructor in base class
#include <wrl/wrappers/corewrappers.h>
#pragma warning(pop)
#include <intrin.h>
#include <stdint.h>

#if defined(_CRT_WINDOWS) || defined(UNDOCKED_WINDOWS_UCRT)
// __crtTerminateProcess() - Terminates the current process.
//
// Purpose:
//        Terminates the current process.
//        This function is not needed for ARM and Windows 8, which call __fastfail() instead.
// Entry:
//        The exit code to be used by the process and threads terminated
#if !defined _CRT_APP && (defined _M_IX86 || defined _M_X64)

extern "C" void __cdecl __crtTerminateProcess(UINT const uExitCode) {
    // Terminate the current process - the return code is currently unusable in
    // the CRT, so we ignore it.
    TerminateProcess(GetCurrentProcess(), uExitCode);
}

#endif // !defined _CRT_APP && (defined _M_IX86 || defined _M_X64)

#else // defined(_CRT_WINDOWS) || defined(UNDOCKED_WINDOWS_UCRT)

// GetCurrentPackageId retrieves the current package id, if the app is deployed via a package.
typedef BOOL(WINAPI* PFNGETCURRENTPACKAGEID)(UINT32*, BYTE*);

#if !defined _CRT_APP
#if defined _ONECORE

namespace {
    struct HMODULETraits {
        typedef HMODULE Type;

        static bool Close(Type const h) noexcept {
            return ::FreeLibrary(h) != FALSE;
        }

        static Type GetInvalidValue() noexcept {
            return nullptr;
        }
    };

    typedef Microsoft::WRL::Wrappers::HandleT<HMODULETraits> HMODULEHandle;
}

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

#endif // defined(_CRT_WINDOWS) || defined(UNDOCKED_WINDOWS_UCRT)


#if _STL_WIN32_WINNT < _WIN32_WINNT_WS03

extern "C" DWORD __cdecl __crtFlsAlloc(PFLS_CALLBACK_FUNCTION const lpCallback) {
    // use FlsAlloc if it is available (only on Windows Server 2003+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNFLSALLOC, FlsAlloc, pfFlsAlloc) {
        return pfFlsAlloc(lpCallback);
    }

    // ...otherwise fall back to using TlsAlloc.
    return TlsAlloc();
}

extern "C" BOOL __cdecl __crtFlsFree(DWORD const dwFlsIndex) {
    // use FlsFree if it is available (only on Windows Server 2003+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNFLSFREE, FlsFree, pfFlsFree) {
        return pfFlsFree(dwFlsIndex);
    }

    // ...otherwise fall back to using TlsFree.
    return TlsFree(dwFlsIndex);
}

extern "C" PVOID __cdecl __crtFlsGetValue(DWORD const dwFlsIndex) {
    // use FlsGetValue if it is available (only on Windows Server 2003+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNFLSGETVALUE, FlsGetValue, pfFlsGetValue) {
        return pfFlsGetValue(dwFlsIndex);
    }

    // ...otherwise fall back to using TlsGetValue.
    return TlsGetValue(dwFlsIndex);
}

extern "C" BOOL __cdecl __crtFlsSetValue(DWORD const dwFlsIndex, PVOID const lpFlsData) {
    // use FlsSetValue if it is available (only on Windows Server 2003+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNFLSSETVALUE, FlsSetValue, pfFlsSetValue) {
        return pfFlsSetValue(dwFlsIndex, lpFlsData);
    }

    // ...otherwise fall back to using TlsSetValue.
    return TlsSetValue(dwFlsIndex, lpFlsData);
}

#endif // _STL_WIN32_WINNT < _WIN32_WINNT_WS03


#if _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

extern "C" ULONGLONG __cdecl __crtGetTickCount64() {
    // use GetTickCount64 if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNGETTICKCOUNT64, GetTickCount64, pfGetTickCount64) {
        return pfGetTickCount64();
    }

    // ...otherwise fall back to using GetTickCount.
    return GetTickCount();
}

extern "C" BOOL __cdecl __crtInitializeCriticalSectionEx(
    LPCRITICAL_SECTION const lpCriticalSection, DWORD const dwSpinCount, DWORD const Flags) {
    // use InitializeCriticalSectionEx if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(
        PFNINITIALIZECRITICALSECTIONEX, InitializeCriticalSectionEx, pfInitializeCriticalSectionEx) {
        return pfInitializeCriticalSectionEx(lpCriticalSection, dwSpinCount, Flags);
    }

    // ...otherwise fall back to using InitializeCriticalSectionAndSpinCount.
    InitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount);
    return TRUE;
}

extern "C" BOOL __cdecl __crtInitOnceExecuteOnce(
    PINIT_ONCE const InitOnce, PINIT_ONCE_FN const InitFn, PVOID const Parameter, LPVOID* const Context) {
    // use InitOnceExecuteOnce if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNINITONCEEXECUTEONCE, InitOnceExecuteOnce, pfInitOnceExecuteOnce) {
        return pfInitOnceExecuteOnce(InitOnce, InitFn, Parameter, Context);
    }

    // ...otherwise fall back to atomic operations.
    void* const PV_INITIAL = reinterpret_cast<void*>(static_cast<uintptr_t>(0));
    void* const PV_WORKING = reinterpret_cast<void*>(static_cast<uintptr_t>(1));
    void* const PV_SUCCESS = reinterpret_cast<void*>(static_cast<uintptr_t>(2));

    void** const ppv = reinterpret_cast<void**>(InitOnce);

    for (;;) {
        void* const previous = _InterlockedCompareExchangePointer(ppv, PV_WORKING, PV_INITIAL);

        if (previous == PV_SUCCESS) {
            return TRUE;
        } else if (previous == PV_INITIAL) {
            void* next = PV_SUCCESS;
            BOOL ret   = TRUE;

            if (InitFn(InitOnce, Parameter, Context) == 0) {
                next = PV_INITIAL;
                ret  = FALSE;
            }

            if (_InterlockedExchangePointer(ppv, next) == PV_WORKING) {
                return ret;
            } else {
                SetLastError(ERROR_INVALID_DATA);
                return FALSE;
            }
        } else if (previous == PV_WORKING) {
            SwitchToThread();
        } else {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }
    }
}

extern "C" HANDLE __cdecl __crtCreateEventExW(LPSECURITY_ATTRIBUTES const lpEventAttributes, LPCWSTR const lpName,
    DWORD const dwFlags, DWORD const dwDesiredAccess) {
    // use CreateEventEx if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNCREATEEVENTEXW, CreateEventExW, pfCreateEventExW) {
        return pfCreateEventExW(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
    }

    // ...otherwise fall back to using CreateEvent.
    return CreateEventW(
        lpEventAttributes, dwFlags & CREATE_EVENT_MANUAL_RESET, dwFlags & CREATE_EVENT_INITIAL_SET, lpName);
}

extern "C" HANDLE __cdecl __crtCreateSemaphoreExW(LPSECURITY_ATTRIBUTES const lpSemaphoreAttributes,
    LONG const lInitialCount, LONG const lMaximumCount, LPCWSTR const lpName, DWORD const dwFlags,
    DWORD const dwDesiredAccess) {
    // use CreateSemaphoreEx if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNCREATESEMAPHOREEXW, CreateSemaphoreExW, pfCreateSemaphoreExW) {
        return pfCreateSemaphoreExW(
            lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, dwFlags, dwDesiredAccess);
    }

    // ...otherwise fall back to using CreateSemaphore on Windows XP (API is not present on OneCore)...
    IFDYNAMICGETCACHEDFUNCTION(PFNCREATESEMAPHOREW, CreateSemaphoreW, pfCreateSemaphoreW) {
        return pfCreateSemaphoreW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName);
    }

    // ... otherwise return failure because there is no fall back.
    return nullptr;
}

extern "C" PTP_TIMER __cdecl __crtCreateThreadpoolTimer(
    PTP_TIMER_CALLBACK const pfnti, PVOID const pv, PTP_CALLBACK_ENVIRON const pcbe) {
    // use CreateThreadpoolTimer if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNCREATETHREADPOOLTIMER, CreateThreadpoolTimer, pfCreateThreadpoolTimer) {
        return pfCreateThreadpoolTimer(pfnti, pv, pcbe);
    }

    // ...otherwise return failure because there is no fall back.
    return nullptr;
}

extern "C" VOID __cdecl __crtSetThreadpoolTimer(
    PTP_TIMER const pti, PFILETIME const pftDueTime, DWORD const msPeriod, DWORD const msWindowLength) {
    // use SetThreadpoolTimer if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNSETTHREADPOOLTIMER, SetThreadpoolTimer, pfSetThreadpoolTimer) {
        pfSetThreadpoolTimer(pti, pftDueTime, msPeriod, msWindowLength);
    }

    // ...otherwise there is no fall back.
    return;
}

extern "C" VOID __cdecl __crtWaitForThreadpoolTimerCallbacks(PTP_TIMER const pti, BOOL const fCancelPendingCallbacks) {
    // use WaitForThreadpoolTimerCallbacks if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(
        PFNWAITFORTHREADPOOLTIMERCALLBACKS, WaitForThreadpoolTimerCallbacks, pfWaitForThreadpoolTimerCallbacks) {
        pfWaitForThreadpoolTimerCallbacks(pti, fCancelPendingCallbacks);
    }

    // ...otherwise there is no fall back.
    return;
}

extern "C" VOID __cdecl __crtCloseThreadpoolTimer(PTP_TIMER const pti) {
    // use CloseThreadpoolTimer if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNCLOSETHREADPOOLTIMER, CloseThreadpoolTimer, pfCloseThreadpoolTimer) {
        pfCloseThreadpoolTimer(pti);
    }

    // ...otherwise there is no fall back.
    return;
}

extern "C" PTP_WAIT __cdecl __crtCreateThreadpoolWait(
    PTP_WAIT_CALLBACK const pfnwa, PVOID const pv, PTP_CALLBACK_ENVIRON const pcbe) {
    // use CreateThreadpoolWait if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNCREATETHREADPOOLWAIT, CreateThreadpoolWait, pfCreateThreadpoolWait) {
        return pfCreateThreadpoolWait(pfnwa, pv, pcbe);
    }

    // ...otherwise return failure because there is no fall back.
    return nullptr;
}

extern "C" VOID __cdecl __crtSetThreadpoolWait(PTP_WAIT const pwa, HANDLE const h, PFILETIME const pftTimeout) {
    // use SetThreadpoolWait if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNSETTHREADPOOLWAIT, SetThreadpoolWait, pfSetThreadpoolWait) {
        pfSetThreadpoolWait(pwa, h, pftTimeout);
    }

    // ...otherwise there is no fall back.
}

extern "C" VOID __cdecl __crtCloseThreadpoolWait(PTP_WAIT const pwa) {
    // use CloseThreadpoolWait if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNCLOSETHREADPOOLWAIT, CloseThreadpoolWait, pfCloseThreadpoolWait) {
        pfCloseThreadpoolWait(pwa);
    }

    // ...otherwise there is no fall back.
}

extern "C" VOID __cdecl __crtFlushProcessWriteBuffers() {
    // use FlushProcessWriteBuffers if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNFLUSHPROCESSWRITEBUFFERS, FlushProcessWriteBuffers, pfFlushProcessWriteBuffers) {
        pfFlushProcessWriteBuffers();
    }

    // ...otherwise there is no fall back.
}

extern "C" VOID __cdecl __crtFreeLibraryWhenCallbackReturns(PTP_CALLBACK_INSTANCE const pci, HMODULE const mod) {
    // use FreeLibraryWhenCallbackReturns if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(
        PFNFREELIBRARYWHENCALLBACKRETURNS, FreeLibraryWhenCallbackReturns, pfFreeLibraryWhenCallbackReturns) {
        pfFreeLibraryWhenCallbackReturns(pci, mod);
    }

    // ...otherwise there is no fall back.
}

extern "C" DWORD __cdecl __crtGetCurrentProcessorNumber() {
    // use GetCurrentProcessorNumber if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNGETCURRENTPROCESSORNUMBER, GetCurrentProcessorNumber, pfGetCurrentProcessorNumber) {
        return pfGetCurrentProcessorNumber();
    }

    // ...otherwise return 0 because there is no fall back.
    return 0;
}

extern "C" BOOLEAN __cdecl __crtCreateSymbolicLinkW(
    LPCWSTR const lpSymlinkFileName, LPCWSTR const lpTargetFileName, DWORD const dwFlags) {
    // use CreateSymbolicLink if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNCREATESYMBOLICLINKW, CreateSymbolicLinkW, pfCreateSymbolicLink) {
        return pfCreateSymbolicLink(lpSymlinkFileName, lpTargetFileName, dwFlags);
    }

    // ...otherwise return 0 and set error code because there is no fall back.
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}

extern "C" BOOL __cdecl __crtGetFileInformationByHandleEx(HANDLE const hFile,
    FILE_INFO_BY_HANDLE_CLASS const FileInformationClass, LPVOID const lpFileInformation, DWORD const dwBufferSize) {
    // use GetFileInformationByHandleEx if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(
        PFNGETFILEINFORMATIONBYHANDLEEX, GetFileInformationByHandleEx, pfGetFileInformationByHandleEx) {
        return pfGetFileInformationByHandleEx(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
    }

    // ...otherwise return 0 and set error code because there is no fall back.
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}

extern "C" BOOL __cdecl __crtSetFileInformationByHandle(HANDLE const hFile,
    FILE_INFO_BY_HANDLE_CLASS const FileInformationClass, LPVOID const lpFileInformation, DWORD const dwBufferSize) {
    // use SetFileInformationByHandle if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(
        PFNSETFILEINFORMATIONBYHANDLE, SetFileInformationByHandle, pfSetFileInformationByHandle) {
        return pfSetFileInformationByHandle(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
    }

    // ...otherwise return 0 and set error code because there is no fall back.
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}

extern "C" VOID __cdecl __crtInitializeConditionVariable(PCONDITION_VARIABLE const pCond) {
    DYNAMICGETCACHEDFUNCTION(
        PFNINITIALIZECONDITIONVARIABLE, InitializeConditionVariable, pfInitializeConditionVariable);
    pfInitializeConditionVariable(pCond);
    // Don't have fallbacks because the only caller (in primitives.h) will check the existence before calling
}

extern "C" VOID __cdecl __crtWakeConditionVariable(PCONDITION_VARIABLE const pCond) {
    DYNAMICGETCACHEDFUNCTION(PFNWAKECONDITIONVARIABLE, WakeConditionVariable, pfWakeConditionVariable);
    pfWakeConditionVariable(pCond);
    // Don't have fallbacks because the only caller (in primitives.h) will check the existence before calling
}

extern "C" VOID __cdecl __crtWakeAllConditionVariable(PCONDITION_VARIABLE const pCond) {
    DYNAMICGETCACHEDFUNCTION(PFNWAKEALLCONDITIONVARIABLE, WakeAllConditionVariable, pfWakeAllConditionVariable);
    pfWakeAllConditionVariable(pCond);
    // Don't have fallbacks because the only caller (in primitives.h) will check the existence before calling
}

extern "C" BOOL __cdecl __crtSleepConditionVariableCS(
    PCONDITION_VARIABLE const pCond, PCRITICAL_SECTION const pLock, DWORD const dwMs) {
    DYNAMICGETCACHEDFUNCTION(PFNSLEEPCONDITIONVARIABLECS, SleepConditionVariableCS, pfSleepConditionVariableCS);
    return pfSleepConditionVariableCS(pCond, pLock, dwMs);
    // Don't have fallbacks because the only caller (in primitives.h) will check the existence before calling
}

extern "C" VOID __cdecl __crtInitializeSRWLock(PSRWLOCK const pLock) {
    DYNAMICGETCACHEDFUNCTION(PFNINITIALIZESRWLOCK, InitializeSRWLock, pfInitializeSRWLock);
    pfInitializeSRWLock(pLock);
    // Don't have fallbacks because the only caller (in primitives.h) will check the existence before calling
}

extern "C" VOID __cdecl __crtAcquireSRWLockExclusive(PSRWLOCK const pLock) {
    DYNAMICGETCACHEDFUNCTION(PFNACQUIRESRWLOCKEXCLUSIVE, AcquireSRWLockExclusive, pfAcquireSRWLockExclusive);
    pfAcquireSRWLockExclusive(pLock);
    // Don't have fallbacks because the only caller (in primitives.h) will check the existence before calling
}

extern "C" VOID __cdecl __crtReleaseSRWLockExclusive(PSRWLOCK const pLock) {
    DYNAMICGETCACHEDFUNCTION(PFNRELEASESRWLOCKEXCLUSIVE, ReleaseSRWLockExclusive, pfReleaseSRWLockExclusive);
    pfReleaseSRWLockExclusive(pLock);
    // Don't have fallbacks because the only caller (in primitives.h) will check the existence before calling
}

extern "C" BOOL __cdecl __crtSleepConditionVariableSRW(
    PCONDITION_VARIABLE const pCond, PSRWLOCK const pLock, DWORD const dwMs, ULONG const flags) {
    DYNAMICGETCACHEDFUNCTION(PFNSLEEPCONDITIONVARIABLESRW, SleepConditionVariableSRW, pfSleepConditionVariableSRW);
    return pfSleepConditionVariableSRW(pCond, pLock, dwMs, flags);
    // Don't have fallbacks because the only caller (in primitives.h) will check the existence before calling
}

extern "C" PTP_WORK __cdecl __crtCreateThreadpoolWork(
    PTP_WORK_CALLBACK const pfnwk, PVOID const pv, PTP_CALLBACK_ENVIRON const pcbe) {
    DYNAMICGETCACHEDFUNCTION(PFNCREATETHREADPOOLWORK, CreateThreadpoolWork, pfCreateThreadpoolWork);
    return pfCreateThreadpoolWork(pfnwk, pv, pcbe);
    // Don't have fallbacks because the only caller (in taskscheduler.cpp) will check the existence before calling
}

extern "C" VOID __cdecl __crtSubmitThreadpoolWork(PTP_WORK const pwk) {
    DYNAMICGETCACHEDFUNCTION(PFNSUBMITTHREADPOOLWORK, SubmitThreadpoolWork, pfSubmitThreadpoolWork);
    return pfSubmitThreadpoolWork(pwk);
    // Don't have fallbacks because the only caller (in taskscheduler.cpp) will check the existence before calling
}

extern "C" VOID __cdecl __crtCloseThreadpoolWork(PTP_WORK const pwk) {
    DYNAMICGETCACHEDFUNCTION(PFNCLOSETHREADPOOLWORK, CloseThreadpoolWork, pfCloseThreadpoolWork);
    return pfCloseThreadpoolWork(pwk);
    // Don't have fallbacks because the only caller (in taskscheduler.cpp) will check the existence before calling
}

#else // _STL_WIN32_WINNT < _WIN32_WINNT_VISTA
extern "C" BOOL __cdecl __crtQueueUserWorkItem(LPTHREAD_START_ROUTINE, PVOID, ULONG) {
    // This function doesn't have an implementation as it is only used on Windows XP
    return 0;
}
#endif // _STL_WIN32_WINNT < _WIN32_WINNT_VISTA


#if _STL_WIN32_WINNT < _WIN32_WINNT_WIN7

extern "C" BOOLEAN __cdecl __crtTryAcquireSRWLockExclusive(PSRWLOCK const pLock) {
    DYNAMICGETCACHEDFUNCTION(PFNTRYACQUIRESRWLOCKEXCLUSIVE, TryAcquireSRWLockExclusive, pfTryAcquireSRWLockExclusive);
    return pfTryAcquireSRWLockExclusive(pLock);
    // Don't have fallbacks because the only caller (in primitives.h) will check the existence before calling
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
