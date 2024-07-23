// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// clang-format off
// Prevent clang-format from reordering <AppModel.h> before <Windows.h>
#include <Windows.h>
#include <AppModel.h> // for APPMODEL_ERROR_NO_PACKAGE
#include <internal_shared.h>
#include <cstdlib>
#include <yvals.h>
// clang-format on

#pragma warning(push)
#pragma warning(disable : 4265) // non-virtual destructor in base class
#include <wrl/wrappers/corewrappers.h>
#pragma warning(pop)

#if !defined(_ONECORE)
namespace {

// Use this macro for defining the following function pointers
#define DEFINEFUNCTIONPOINTER(fn_name) decltype(&fn_name) __KERNEL32Function_##fn_name = nullptr

#if !defined(_CRT_WINDOWS) && !defined(UNDOCKED_WINDOWS_UCRT)
    DEFINEFUNCTIONPOINTER(GetCurrentPackageId);
#endif // !defined(_CRT_WINDOWS) && !defined(UNDOCKED_WINDOWS_UCRT)

#if _STL_WIN32_WINNT < _WIN32_WINNT_WIN8
    DEFINEFUNCTIONPOINTER(GetSystemTimePreciseAsFileTime);
#endif // _STL_WIN32_WINNT < _WIN32_WINNT_WIN8

    DEFINEFUNCTIONPOINTER(GetTempPath2W);

// Use this macro for caching a function pointer from a DLL
#define STOREFUNCTIONPOINTER(instance, fn_name) \
    __KERNEL32Function_##fn_name = reinterpret_cast<decltype(&fn_name)>(GetProcAddress(instance, #fn_name))

// Use this macro for retrieving a cached function pointer from a DLL
#define IFDYNAMICGETCACHEDFUNCTION(fn_name) if (const auto pf##fn_name = __KERNEL32Function_##fn_name)

} // unnamed namespace
#endif // ^^^ !defined(_ONECORE) ^^^

#if !defined(_CRT_WINDOWS) && !defined(UNDOCKED_WINDOWS_UCRT)

#if !defined(_CRT_APP)
#if defined(_ONECORE)

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

extern "C" int __crt_IsPackagedAppHelper() noexcept {
    static wchar_t const* const possible_apisets[] = {
        L"api-ms-win-appmodel-runtime-l1-1-1.dll", // Windows 8.1+ APISet
        L"ext-ms-win-kernel32-package-current-l1-1-0.dll", // Legacy APISet
        L"appmodel.dll" // LNM implementation DLL
    };

    for (auto& dll : possible_apisets) {
        HMODULEHandle const apiset(LoadLibraryExW(dll, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32));
        if (!apiset.IsValid()) {
            continue;
        }

        auto const get_current_package_id =
            reinterpret_cast<decltype(&GetCurrentPackageId)>(GetProcAddress(apiset.Get(), "GetCurrentPackageId"));

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

#else // ^^^ defined(_ONECORE) / !defined(_ONECORE) vvv

extern "C" int __crt_IsPackagedAppHelper() noexcept {
    LONG retValue       = APPMODEL_ERROR_NO_PACKAGE;
    UINT32 bufferLength = 0;

    IFDYNAMICGETCACHEDFUNCTION(GetCurrentPackageId) {
        retValue = pfGetCurrentPackageId(&bufferLength, nullptr);
    }

    if (retValue == ERROR_INSUFFICIENT_BUFFER) {
        return 1;
    }

    // If GetCurrentPackageId was not found, or it returned a different error,
    // then this is NOT a Packaged app
    return 0;
}

#endif // ^^^ !defined(_ONECORE) ^^^
#endif // ^^^ !defined(_CRT_APP) ^^^

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
// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 BOOL __cdecl __crtIsPackagedApp() noexcept {
#ifdef _CRT_APP
    return TRUE;
#else // ^^^ defined(_CRT_APP) / !defined(_CRT_APP) vvv
    static int isPackaged = -1; // Initialize to undefined state

    // If we've already made this check, just return the prev result
    if (isPackaged < 0) {
        isPackaged = __crt_IsPackagedAppHelper();
    }

    return (isPackaged > 0) ? TRUE : FALSE;
#endif // ^^^ !defined(_CRT_APP) ^^^
}

#endif // !defined(_CRT_WINDOWS) && !defined(UNDOCKED_WINDOWS_UCRT)

#if _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 ULONGLONG __cdecl __crtGetTickCount64() noexcept {
    return GetTickCount64();
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 BOOL __cdecl __crtInitializeCriticalSectionEx(
    _Out_ LPCRITICAL_SECTION const lpCriticalSection, _In_ DWORD const dwSpinCount, _In_ DWORD const Flags) noexcept {
    return InitializeCriticalSectionEx(lpCriticalSection, dwSpinCount, Flags);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 BOOL __cdecl __crtInitOnceExecuteOnce(_Inout_ PINIT_ONCE const InitOnce,
    _In_ PINIT_ONCE_FN const InitFn, _Inout_opt_ PVOID const Parameter, LPVOID* const Context) noexcept {
    return InitOnceExecuteOnce(InitOnce, InitFn, Parameter, Context);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 HANDLE __cdecl __crtCreateEventExW(_In_opt_ LPSECURITY_ATTRIBUTES const lpEventAttributes,
    _In_opt_ LPCWSTR const lpName, _In_ DWORD const dwFlags, _In_ DWORD const dwDesiredAccess) noexcept {
    return CreateEventExW(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 HANDLE __cdecl __crtCreateSemaphoreExW(_In_opt_ LPSECURITY_ATTRIBUTES const lpSemaphoreAttributes,
    _In_ LONG const lInitialCount, _In_ LONG const lMaximumCount, _In_opt_ LPCWSTR const lpName,
    _Reserved_ DWORD const dwFlags, _In_ DWORD const dwDesiredAccess) noexcept {
    return CreateSemaphoreExW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, dwFlags, dwDesiredAccess);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 PTP_TIMER __cdecl __crtCreateThreadpoolTimer(_In_ PTP_TIMER_CALLBACK const pfnti,
    _Inout_opt_ PVOID const pv, _In_opt_ PTP_CALLBACK_ENVIRON const pcbe) noexcept {
    return CreateThreadpoolTimer(pfnti, pv, pcbe);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 VOID __cdecl __crtSetThreadpoolTimer(_Inout_ PTP_TIMER const pti,
    _In_opt_ PFILETIME const pftDueTime, _In_ DWORD const msPeriod, _In_ DWORD const msWindowLength) noexcept {
    SetThreadpoolTimer(pti, pftDueTime, msPeriod, msWindowLength);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 VOID __cdecl __crtWaitForThreadpoolTimerCallbacks(
    _Inout_ PTP_TIMER const pti, _In_ BOOL const fCancelPendingCallbacks) noexcept {
    WaitForThreadpoolTimerCallbacks(pti, fCancelPendingCallbacks);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 VOID __cdecl __crtCloseThreadpoolTimer(_Inout_ PTP_TIMER const pti) noexcept {
    CloseThreadpoolTimer(pti);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 PTP_WAIT __cdecl __crtCreateThreadpoolWait(
    _In_ PTP_WAIT_CALLBACK const pfnwa, _Inout_opt_ PVOID const pv, _In_opt_ PTP_CALLBACK_ENVIRON const pcbe) noexcept {
    return CreateThreadpoolWait(pfnwa, pv, pcbe);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 VOID __cdecl __crtSetThreadpoolWait(
    _Inout_ PTP_WAIT const pwa, _In_opt_ HANDLE const h, _In_opt_ PFILETIME const pftTimeout) noexcept {
    SetThreadpoolWait(pwa, h, pftTimeout);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 VOID __cdecl __crtCloseThreadpoolWait(_Inout_ PTP_WAIT const pwa) noexcept {
    CloseThreadpoolWait(pwa);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 VOID __cdecl __crtFlushProcessWriteBuffers() noexcept {
    FlushProcessWriteBuffers();
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 VOID __cdecl __crtFreeLibraryWhenCallbackReturns(
    _Inout_ PTP_CALLBACK_INSTANCE const pci, _In_ HMODULE const mod) noexcept {
    FreeLibraryWhenCallbackReturns(pci, mod);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 DWORD __cdecl __crtGetCurrentProcessorNumber() noexcept {
    return GetCurrentProcessorNumber();
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 BOOLEAN __cdecl __crtCreateSymbolicLinkW(
    _In_ LPCWSTR const lpSymlinkFileName, _In_ LPCWSTR const lpTargetFileName, _In_ DWORD const dwFlags) noexcept {
#ifdef _CRT_APP
    (void) lpSymlinkFileName;
    (void) lpTargetFileName;
    (void) dwFlags;
    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
#else // ^^^ defined(_CRT_APP) / !defined(_CRT_APP) vvv
    return CreateSymbolicLinkW(lpSymlinkFileName, lpTargetFileName, dwFlags);
#endif // ^^^ !defined(_CRT_APP) ^^^
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 _Success_(return) BOOL __cdecl __crtGetFileInformationByHandleEx(_In_ HANDLE const hFile,
    _In_ FILE_INFO_BY_HANDLE_CLASS const FileInformationClass,
    _Out_writes_bytes_(dwBufferSize) LPVOID const lpFileInformation, _In_ DWORD const dwBufferSize) noexcept {
    return GetFileInformationByHandleEx(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
}

// TRANSITION, ABI: preserved for binary compatibility
extern "C" _CRTIMP2 BOOL __cdecl __crtSetFileInformationByHandle(_In_ HANDLE const hFile,
    _In_ FILE_INFO_BY_HANDLE_CLASS const FileInformationClass,
    _In_reads_bytes_(dwBufferSize) LPVOID const lpFileInformation, _In_ DWORD const dwBufferSize) noexcept {
    return SetFileInformationByHandle(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
}
#endif // _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

#if _STL_WIN32_WINNT < _WIN32_WINNT_WIN8

extern "C" _CRTIMP2 void __cdecl __crtGetSystemTimePreciseAsFileTime(_Out_ LPFILETIME lpSystemTimeAsFileTime) noexcept {
    // use GetSystemTimePreciseAsFileTime if it is available (only on Windows 8+)...
    IFDYNAMICGETCACHEDFUNCTION(GetSystemTimePreciseAsFileTime) {
        pfGetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime);
        return;
    }

    // ...otherwise use GetSystemTimeAsFileTime.
    GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
}

#endif // _STL_WIN32_WINNT < _WIN32_WINNT_WIN8

extern "C" _Success_(return > 0 && return < BufferLength) DWORD __stdcall __crtGetTempPath2W(
    _In_ DWORD BufferLength, _Out_writes_to_opt_(BufferLength, return +1) LPWSTR Buffer) noexcept {
#if !defined(_ONECORE)
    // use GetTempPath2W if it is available (only on Windows 11+)...
    IFDYNAMICGETCACHEDFUNCTION(GetTempPath2W) {
        return pfGetTempPath2W(BufferLength, Buffer);
    }
#endif // ^^^ !defined(_ONECORE) ^^^

    // ...otherwise use GetTempPathW.
    return GetTempPathW(BufferLength, Buffer);
}

// Helper to load all necessary Win32 API function pointers

#if defined(_ONECORE)

// All APIs are statically available, and we can't call GetModuleHandleW().

#else // ^^^ defined(_ONECORE) / !defined(_ONECORE) vvv

static int __cdecl initialize_pointers() noexcept {
    HINSTANCE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    _Analysis_assume_(hKernel32);

#if !defined(_CRT_WINDOWS) && !defined(UNDOCKED_WINDOWS_UCRT)
    STOREFUNCTIONPOINTER(hKernel32, GetCurrentPackageId);
#endif // !defined(_CRT_WINDOWS) && !defined(UNDOCKED_WINDOWS_UCRT)

#if _STL_WIN32_WINNT < _WIN32_WINNT_WIN8
    STOREFUNCTIONPOINTER(hKernel32, GetSystemTimePreciseAsFileTime);
#endif // _STL_WIN32_WINNT < _WIN32_WINNT_WIN8

    // Note that GetTempPath2W is defined as of Windows 10 Build 20348 (a server release) or Windows 11,
    // but there is no "_WIN32_WINNT_WIN11" constant, so we will always dynamically load it
    STOREFUNCTIONPOINTER(hKernel32, GetTempPath2W);

    return 0;
}

_CRTALLOC(".CRT$XIC") static _PIFV pinit = initialize_pointers;

#endif // ^^^ !defined(_ONECORE) ^^^
