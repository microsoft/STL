// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Internal definitions for A&W Win32 wrapper routines.
#pragma once

#include <yvals.h>

#include <sdkddkver.h>

#include <Windows.h>

_CRT_BEGIN_C_HEADER

#if !defined(_CRT_WINDOWS) && !defined(UNDOCKED_WINDOWS_UCRT)
_CRTIMP2 BOOL __cdecl __crtIsPackagedApp();
#endif // !defined(_CRT_WINDOWS) && !defined(UNDOCKED_WINDOWS_UCRT)

#if _STL_WIN32_WINNT >= _WIN32_WINNT_WIN7

#define __crtTryAcquireSRWLockExclusive(pLock) TryAcquireSRWLockExclusive(pLock)

#else // _STL_WIN32_WINNT >= _WIN32_WINNT_WIN7

BOOLEAN __cdecl __crtTryAcquireSRWLockExclusive(_Inout_ PSRWLOCK);

#endif // _STL_WIN32_WINNT >= _WIN32_WINNT_WIN7

#if _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8

#define __crtGetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime) \
    GetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime)

#else // _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8

_CRTIMP2 void __cdecl __crtGetSystemTimePreciseAsFileTime(_Out_ LPFILETIME lpSystemTimeAsFileTime);

#endif // _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8

// This enum should not change, even though some functions are no longer imported dynamically
enum wrapKERNEL32Functions {
    eFlsAlloc,
    eFlsFree,
    eFlsGetValue,
    eFlsSetValue,
    eInitializeCriticalSectionEx,
    eInitOnceExecuteOnce,
    eCreateEventExW,
    eCreateSemaphoreW,
    eCreateSemaphoreExW,
    eCreateThreadpoolTimer,
    eSetThreadpoolTimer,
    eWaitForThreadpoolTimerCallbacks,
    eCloseThreadpoolTimer,
    eCreateThreadpoolWait,
    eSetThreadpoolWait,
    eCloseThreadpoolWait,
    eFlushProcessWriteBuffers,
    eFreeLibraryWhenCallbackReturns,
    eGetCurrentProcessorNumber,
    eCreateSymbolicLinkW,
#if defined(_CRT_WINDOWS) || defined(UNDOCKED_WINDOWS_UCRT)
    eSetDefaultDllDirectories,
    eCompareStringEx,
    eEnumSystemLocalesEx,
    eGetLocaleInfoEx,
    eGetUserDefaultLocaleName,
    eIsValidLocaleName,
    eLCMapStringEx,
#else // defined(_CRT_WINDOWS) || defined(UNDOCKED_WINDOWS_UCRT)
    eGetCurrentPackageId,
#endif // defined(_CRT_WINDOWS) || defined(UNDOCKED_WINDOWS_UCRT)
    eGetTickCount64,
    eGetFileInformationByHandleEx,
    eSetFileInformationByHandle,
    eGetSystemTimePreciseAsFileTime,
    eInitializeConditionVariable,
    eWakeConditionVariable,
    eWakeAllConditionVariable,
    eSleepConditionVariableCS,
    eInitializeSRWLock,
    eAcquireSRWLockExclusive,
    eTryAcquireSRWLockExclusive,
    eReleaseSRWLockExclusive,
    eSleepConditionVariableSRW,
    eCreateThreadpoolWork,
    eSubmitThreadpoolWork,
    eCloseThreadpoolWork,
#if !defined(_CRT_WINDOWS) && !defined(UNDOCKED_WINDOWS_UCRT)
    eCompareStringEx,
    eGetLocaleInfoEx,
    eLCMapStringEx,
#endif // !defined(_CRT_WINDOWS) && !defined(UNDOCKED_WINDOWS_UCRT)
    eMaxKernel32Function
};

extern PVOID __KERNEL32Functions[eMaxKernel32Function];

using PFNGETSYSTEMTIMEPRECISEASFILETIME = VOID(WINAPI*)(LPFILETIME);
using PFNTRYACQUIRESRWLOCKEXCLUSIVE     = BOOLEAN(WINAPI*)(PSRWLOCK);

// Use this macro for caching a function pointer from a DLL
#define STOREFUNCTIONPOINTER(instance, function_name) \
    __KERNEL32Functions[e##function_name] = reinterpret_cast<PVOID>(GetProcAddress(instance, #function_name));

// Use this macro as a cached function pointer from a DLL
#define DYNAMICGETCACHEDFUNCTION(function_pointer_type, function_name, variable_name) \
    const auto variable_name = reinterpret_cast<function_pointer_type>(__KERNEL32Functions[e##function_name])

#define IFDYNAMICGETCACHEDFUNCTION(function_pointer_type, function_name, variable_name) \
    DYNAMICGETCACHEDFUNCTION(function_pointer_type, function_name, variable_name);      \
    if (variable_name != nullptr)

_CRTIMP2 int __cdecl __crtCompareStringA(_In_z_ LPCWSTR _LocaleName, _In_ DWORD _DwCmpFlags,
    _In_reads_(_CchCount1) LPCSTR _LpString1, _In_ int _CchCount1, _In_reads_(_CchCount2) LPCSTR _LpString2,
    _In_ int _CchCount2, _In_ int _CodePage);

_CRTIMP2 int __cdecl __crtCompareStringW(_In_z_ LPCWSTR _LocaleName, _In_ DWORD _DwCmpFlags,
    _In_reads_(_CchCount1) LPCWSTR _LpString1, _In_ int _CchCount1, _In_reads_(_CchCount2) LPCWSTR _LpString2,
    _In_ int _CchCount2);

_CRTIMP2 int __cdecl __crtLCMapStringA(_In_opt_z_ LPCWSTR _LocaleName, _In_ DWORD _DwMapFlag,
    _In_reads_(_CchSrc) LPCSTR _LpSrcStr, _In_ int _CchSrc, _Out_writes_opt_(_CchDest) char* _LpDestStr,
    _In_ int _CchDest, _In_ int _CodePage, _In_ BOOL _BError);

_CRTIMP2 int __cdecl __crtLCMapStringW(_In_opt_z_ LPCWSTR _LocaleName, _In_ DWORD _DWMapFlag,
    _In_reads_(_CchSrc) LPCWSTR _LpSrcStr, _In_ int _CchSrc, _Out_writes_opt_(_CchDest) wchar_t* _LpDestStr,
    _In_ int _CchDest);

_CRT_END_C_HEADER
