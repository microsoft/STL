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

#if _STL_WIN32_WINNT >= _WIN32_WINNT_WS03

#define __crtFlsAlloc(lpCallback) FlsAlloc(lpCallback)

#define __crtFlsFree(dwFlsIndex) FlsFree(dwFlsIndex)

#define __crtFlsGetValue(dwFlsIndex) FlsGetValue(dwFlsIndex)

#define __crtFlsSetValue(dwFlsIndex, lpFlsData) FlsSetValue(dwFlsIndex, lpFlsData)

#else // _STL_WIN32_WINNT >= _WIN32_WINNT_WS03

DWORD __cdecl __crtFlsAlloc(_In_opt_ PFLS_CALLBACK_FUNCTION lpCallback);

BOOL __cdecl __crtFlsFree(_In_ DWORD dwFlsIndex);

PVOID __cdecl __crtFlsGetValue(_In_ DWORD dwFlsIndex);

BOOL __cdecl __crtFlsSetValue(_In_ DWORD dwFlsIndex, _In_opt_ PVOID lpFlsData);

#endif // _STL_WIN32_WINNT >= _WIN32_WINNT_WS03


#if _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA

#define __crtInitializeCriticalSectionEx(lpCriticalSection, dwSpinCount, Flags) \
    InitializeCriticalSectionEx(lpCriticalSection, dwSpinCount, Flags)

#define __crtInitOnceExecuteOnce(InitOnce, InitFn, Parameter, Context) \
    InitOnceExecuteOnce(InitOnce, InitFn, Parameter, Context)

#define __crtCreateEventExW(lpEventAttributes, lpName, dwFlags, dwDesiredAccess) \
    CreateEventExW(lpEventAttributes, lpName, dwFlags, dwDesiredAccess)

#define __crtCreateSemaphoreExW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, dwFlags, dwDesiredAccess) \
    CreateSemaphoreExW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, dwFlags, dwDesiredAccess)

#define __crtCreateThreadpoolTimer(pfnti, pv, pcbe) CreateThreadpoolTimer(pfnti, pv, pcbe)

#define __crtSetThreadpoolTimer(pti, pftDueTime, msPeriod, msWindowLength) \
    SetThreadpoolTimer(pti, pftDueTime, msPeriod, msWindowLength)

#define __crtWaitForThreadpoolTimerCallbacks(pti, fCancelPendingCallbacks) \
    WaitForThreadpoolTimerCallbacks(pti, fCancelPendingCallbacks)

#define __crtCloseThreadpoolTimer(pti) CloseThreadpoolTimer(pti)

#define __crtCreateThreadpoolWait(pfnwa, pv, pcbe) CreateThreadpoolWait(pfnwa, pv, pcbe)

#define __crtSetThreadpoolWait(pwa, h, pftTimeout) SetThreadpoolWait(pwa, h, pftTimeout)

#define __crtCloseThreadpoolWait(pwa) CloseThreadpoolWait(pwa)

#define __crtFlushProcessWriteBuffers() FlushProcessWriteBuffers()

#define __crtFreeLibraryWhenCallbackReturns(pci, mod) FreeLibraryWhenCallbackReturns(pci, mod)

#define __crtGetCurrentProcessorNumber() GetCurrentProcessorNumber()

#define __crtCreateSymbolicLinkW(lpSymlinkFileName, lpTargetFileName, dwFlags) \
    CreateSymbolicLinkW(lpSymlinkFileName, lpTargetFileName, dwFlags)

#define __crtGetFileInformationByHandleEx(hFile, FileInformationClass, lpFileInformation, dwBufferSize) \
    GetFileInformationByHandleEx(hFile, FileInformationClass, lpFileInformation, dwBufferSize)

#define __crtSetFileInformationByHandle(hFile, FileInformationClass, lpFileInformation, dwBufferSize) \
    SetFileInformationByHandle(hFile, FileInformationClass, lpFileInformation, dwBufferSize)

#define __crtGetTickCount64() GetTickCount64()

#define __crtInitializeConditionVariable(pCond) InitializeConditionVariable(pCond)

#define __crtWakeConditionVariable(pCond) WakeConditionVariable(pCond)

#define __crtWakeAllConditionVariable(pCond) WakeAllConditionVariable(pCond)

#define __crtSleepConditionVariableCS(pCond, pLock, dwMs) SleepConditionVariableCS(pCond, pLock, dwMs)

#define __crtInitializeSRWLock(pLock) InitializeSRWLock(pLock)

#define __crtAcquireSRWLockExclusive(pLock) AcquireSRWLockExclusive(pLock)

#define __crtReleaseSRWLockExclusive(pLock) ReleaseSRWLockExclusive(pLock)

#define __crtSleepConditionVariableSRW(pCond, pLock, dwMs, flags) SleepConditionVariableSRW(pCond, pLock, dwMs, flags)

#define __crtCreateThreadpoolWork(pfnwk, pv, pcbe) CreateThreadpoolWork(pfnwk, pv, pcbe)

#define __crtSubmitThreadpoolWork(pwk) SubmitThreadpoolWork(pwk)

#define __crtCloseThreadpoolWork(pwk) CloseThreadpoolWork(pwk)

#define __crtCompareStringEx(lpLocaleName, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2) \
    CompareStringEx(lpLocaleName, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2, nullptr, nullptr, 0)

#define __crtLCMapStringEx(lpLocaleName, dwMapFlags, lpSrcStr, cchStr, lpDestStr, cchDest) \
    LCMapStringEx(lpLocaleName, dwMapFlags, lpSrcStr, cchStr, lpDestStr, cchDest, nullptr, nullptr, 0)

#define __crtGetLocaleInfoEx(lpLocaleName, LCType, lpLCData, cchData) \
    GetLocaleInfoEx(lpLocaleName, LCType, lpLCData, cchData)

#else // _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA

_CRTIMP2 BOOL __cdecl __crtInitializeCriticalSectionEx(
    _Out_ LPCRITICAL_SECTION lpCriticalSection, _In_ DWORD dwSpinCount, _In_ DWORD Flags);

// N.B. Context is not used
_CRTIMP2 BOOL __cdecl __crtInitOnceExecuteOnce(
    _Inout_ PINIT_ONCE InitOnce, _In_ PINIT_ONCE_FN InitFn, _Inout_opt_ PVOID Parameter, LPVOID* Context);

_CRTIMP2 HANDLE __cdecl __crtCreateEventExW(_In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes, _In_opt_ LPCWSTR lpName,
    _In_ DWORD dwFlags, _In_ DWORD dwDesiredAccess);

_CRTIMP2 HANDLE __cdecl __crtCreateSemaphoreExW(_In_opt_ LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
    _In_ LONG lInitialCount, _In_ LONG lMaximumCount, _In_opt_ LPCWSTR lpName, _Reserved_ DWORD dwFlags,
    _In_ DWORD dwDesiredAccess);

_CRTIMP2 PTP_TIMER __cdecl __crtCreateThreadpoolTimer(
    _In_ PTP_TIMER_CALLBACK pfnti, _Inout_opt_ PVOID pv, _In_opt_ PTP_CALLBACK_ENVIRON pcbe);

_CRTIMP2 VOID __cdecl __crtSetThreadpoolTimer(
    _Inout_ PTP_TIMER pti, _In_opt_ PFILETIME pftDueTime, _In_ DWORD msPeriod, _In_opt_ DWORD msWindowLength);

_CRTIMP2 VOID __cdecl __crtWaitForThreadpoolTimerCallbacks(_Inout_ PTP_TIMER pti, _In_ BOOL fCancelPendingCallbacks);

_CRTIMP2 VOID __cdecl __crtCloseThreadpoolTimer(_Inout_ PTP_TIMER pti);

_CRTIMP2 PTP_WAIT __cdecl __crtCreateThreadpoolWait(
    _In_ PTP_WAIT_CALLBACK pfnwa, _Inout_opt_ PVOID pv, _In_opt_ PTP_CALLBACK_ENVIRON pcbe);

_CRTIMP2 VOID __cdecl __crtSetThreadpoolWait(_Inout_ PTP_WAIT pwa, _In_opt_ HANDLE h, _In_opt_ PFILETIME pftTimeout);

_CRTIMP2 VOID __cdecl __crtCloseThreadpoolWait(_Inout_ PTP_WAIT pwa);

_CRTIMP2 VOID __cdecl __crtFlushProcessWriteBuffers();

_CRTIMP2 VOID __cdecl __crtFreeLibraryWhenCallbackReturns(_Inout_ PTP_CALLBACK_INSTANCE pci, _In_ HMODULE mod);

_CRTIMP2 DWORD __cdecl __crtGetCurrentProcessorNumber();

_CRTIMP2 BOOLEAN __cdecl __crtCreateSymbolicLinkW(
    _In_ LPCWSTR lpSymlinkFileName, _In_ LPCWSTR lpTargetFileName, _In_ DWORD dwFlags);

_CRTIMP2 _Success_(return ) BOOL
    __cdecl __crtGetFileInformationByHandleEx(_In_ HANDLE hFile, _In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
        _Out_writes_bytes_(dwBufferSize) LPVOID lpFileInformation, _In_ DWORD dwBufferSize);

_CRTIMP2 BOOL __cdecl __crtSetFileInformationByHandle(_In_ HANDLE hFile,
    _In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass, _In_reads_bytes_(dwBufferSize) LPVOID lpFileInformation,
    _In_ DWORD dwBufferSize);

_CRTIMP2 ULONGLONG __cdecl __crtGetTickCount64();

VOID __cdecl __crtInitializeConditionVariable(_Out_ PCONDITION_VARIABLE);

VOID __cdecl __crtWakeConditionVariable(_Inout_ PCONDITION_VARIABLE);

VOID __cdecl __crtWakeAllConditionVariable(_Inout_ PCONDITION_VARIABLE);

BOOL __cdecl __crtSleepConditionVariableCS(_Inout_ PCONDITION_VARIABLE, _Inout_ PCRITICAL_SECTION, _In_ DWORD);

VOID __cdecl __crtInitializeSRWLock(_Out_ PSRWLOCK);

VOID __cdecl __crtAcquireSRWLockExclusive(_Inout_ PSRWLOCK);

VOID __cdecl __crtReleaseSRWLockExclusive(_Inout_ PSRWLOCK);

BOOL __cdecl __crtSleepConditionVariableSRW(_Inout_ PCONDITION_VARIABLE, _Inout_ PSRWLOCK, _In_ DWORD, _In_ ULONG);

PTP_WORK __cdecl __crtCreateThreadpoolWork(
    _In_ PTP_WORK_CALLBACK pfnwk, _Inout_opt_ PVOID pv, _In_opt_ PTP_CALLBACK_ENVIRON pcbe);

VOID __cdecl __crtSubmitThreadpoolWork(_Inout_ PTP_WORK pwk);

VOID __cdecl __crtCloseThreadpoolWork(_Inout_ PTP_WORK pwk);

_CRTIMP2 int __cdecl __crtCompareStringEx(_In_opt_ LPCWSTR lpLocaleName, _In_ DWORD dwCmpFlags,
    _In_NLS_string_(cchCount1) LPCWCH lpString1, _In_ int cchCount1, _In_NLS_string_(cchCount2) LPCWCH lpString2,
    _In_ int cchCount2);

_CRTIMP2 int __cdecl __crtLCMapStringEx(_In_opt_ LPCWSTR lpLocaleName, _In_ DWORD dwMapFlags,
    _In_reads_(cchSrc) LPCWSTR lpSrcStr, _In_ int cchSrc, _Out_writes_opt_(cchDest) LPWSTR lpDestStr, _In_ int cchDest);

_CRTIMP2 int __cdecl __crtGetLocaleInfoEx(
    _In_opt_ LPCWSTR lpLocaleName, _In_ LCTYPE LCType, _Out_writes_opt_(cchData) LPWSTR lpLCData, _In_ int cchData);

#endif // _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA

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

#if _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

#define __crtQueueUserWorkItem(function, context, flags) QueueUserWorkItem(function, context, flags)

#else // _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

BOOL __cdecl __crtQueueUserWorkItem(_In_ LPTHREAD_START_ROUTINE function, _In_opt_ PVOID context, _In_ ULONG flags);

#endif // _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

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

using PFNFLSALLOC                        = DWORD(WINAPI*)(PFLS_CALLBACK_FUNCTION);
using PFNFLSFREE                         = BOOL(WINAPI*)(DWORD);
using PFNFLSGETVALUE                     = PVOID(WINAPI*)(DWORD);
using PFNFLSSETVALUE                     = BOOL(WINAPI*)(DWORD, PVOID);
using PFNINITIALIZECRITICALSECTIONEX     = BOOL(WINAPI*)(LPCRITICAL_SECTION, DWORD, DWORD);
using PFNINITONCEEXECUTEONCE             = BOOL(WINAPI*)(PINIT_ONCE, PINIT_ONCE_FN, PVOID, LPVOID*);
using PFNCREATEEVENTEXW                  = HANDLE(WINAPI*)(LPSECURITY_ATTRIBUTES, LPCWSTR, DWORD, DWORD);
using PFNCREATESEMAPHOREW                = HANDLE(WINAPI*)(LPSECURITY_ATTRIBUTES, LONG, LONG, LPCWSTR);
using PFNCREATESEMAPHOREEXW              = HANDLE(WINAPI*)(LPSECURITY_ATTRIBUTES, LONG, LONG, LPCWSTR, DWORD, DWORD);
using PFNCREATETHREADPOOLTIMER           = PTP_TIMER(WINAPI*)(PTP_TIMER_CALLBACK, PVOID, PTP_CALLBACK_ENVIRON);
using PFNSETTHREADPOOLTIMER              = VOID(WINAPI*)(PTP_TIMER, PFILETIME, DWORD, DWORD);
using PFNWAITFORTHREADPOOLTIMERCALLBACKS = VOID(WINAPI*)(PTP_TIMER, BOOL);
using PFNCLOSETHREADPOOLTIMER            = VOID(WINAPI*)(PTP_TIMER);
using PFNCREATETHREADPOOLWAIT            = PTP_WAIT(WINAPI*)(PTP_WAIT_CALLBACK, PVOID, PTP_CALLBACK_ENVIRON);
using PFNSETTHREADPOOLWAIT               = VOID(WINAPI*)(PTP_WAIT, HANDLE, PFILETIME);
using PFNCLOSETHREADPOOLWAIT             = VOID(WINAPI*)(PTP_WAIT);
using PFNFLUSHPROCESSWRITEBUFFERS        = VOID(WINAPI*)();
using PFNFREELIBRARYWHENCALLBACKRETURNS  = VOID(WINAPI*)(PTP_CALLBACK_INSTANCE, HMODULE);
using PFNGETCURRENTPROCESSORNUMBER       = DWORD(WINAPI*)();
using PFNCREATESYMBOLICLINKW             = BOOLEAN(WINAPI*)(LPCWSTR, LPCWSTR, DWORD);
using PFNGETTICKCOUNT64                  = ULONGLONG(WINAPI*)();
using PFNGETFILEINFORMATIONBYHANDLEEX    = BOOL(WINAPI*)(HANDLE, FILE_INFO_BY_HANDLE_CLASS, LPVOID, DWORD);
using PFNSETFILEINFORMATIONBYHANDLE      = BOOL(WINAPI*)(HANDLE, FILE_INFO_BY_HANDLE_CLASS, LPVOID, DWORD);
using PFNGETSYSTEMTIMEPRECISEASFILETIME  = VOID(WINAPI*)(LPFILETIME);
using PFNINITIALIZECONDITIONVARIABLE     = VOID(WINAPI*)(PCONDITION_VARIABLE);
using PFNWAKECONDITIONVARIABLE           = VOID(WINAPI*)(PCONDITION_VARIABLE);
using PFNWAKEALLCONDITIONVARIABLE        = VOID(WINAPI*)(PCONDITION_VARIABLE);
using PFNSLEEPCONDITIONVARIABLECS        = BOOL(WINAPI*)(PCONDITION_VARIABLE, PCRITICAL_SECTION, DWORD);
using PFNINITIALIZESRWLOCK               = VOID(WINAPI*)(PSRWLOCK);
using PFNACQUIRESRWLOCKEXCLUSIVE         = VOID(WINAPI*)(PSRWLOCK);
using PFNTRYACQUIRESRWLOCKEXCLUSIVE      = BOOLEAN(WINAPI*)(PSRWLOCK);
using PFNRELEASESRWLOCKEXCLUSIVE         = VOID(WINAPI*)(PSRWLOCK);
using PFNSLEEPCONDITIONVARIABLESRW       = BOOL(WINAPI*)(PCONDITION_VARIABLE, PSRWLOCK, DWORD, ULONG);
using PFNCREATETHREADPOOLWORK            = PTP_WORK(WINAPI*)(PTP_WORK_CALLBACK, PVOID, PTP_CALLBACK_ENVIRON);
using PFNSUBMITTHREADPOOLWORK            = VOID(WINAPI*)(PTP_WORK);
using PFNCLOSETHREADPOOLWORK             = VOID(WINAPI*)(PTP_WORK);
using PFNCOMPARESTRINGEX = int(WINAPI*)(LPCWSTR, DWORD, LPCWSTR, int, LPCWSTR, int, LPNLSVERSIONINFO, LPVOID, LPARAM);
using PFNGETLOCALEINFOEX = int(WINAPI*)(LPCWSTR, LCTYPE, LPWSTR, int);
using PFNLCMAPSTRINGEX   = int(WINAPI*)(LPCWSTR, DWORD, LPCWSTR, int, LPWSTR, int, LPNLSVERSIONINFO, LPVOID, LPARAM);

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
