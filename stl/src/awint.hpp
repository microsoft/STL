// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Internal definitions for A&W Win32 wrapper routines.
#pragma once

#include <yvals.h>

#include <sdkddkver.h>

#include <Windows.h>

_CRT_BEGIN_C_HEADER

#if defined(_CRT_WINDOWS) || defined(UNDOCKED_WINDOWS_UCRT)
#if defined _M_IX86 || defined _M_X64 || defined _M_ARM64
void __cdecl __crtTerminateProcess(_In_ UINT uExitCode);
#endif
#else // defined(_CRT_WINDOWS) || defined(UNDOCKED_WINDOWS_UCRT)
_CRTIMP2 BOOL __cdecl __crtIsPackagedApp(void);
#endif // defined(_CRT_WINDOWS) || defined(UNDOCKED_WINDOWS_UCRT)

#if _STL_WIN32_WINNT >= _WIN32_WINNT_WS03

#define __crtFlsAlloc(lpCallback) FlsAlloc(lpCallback)

#define __crtFlsFree(dwFlsIndex) FlsFree(dwFlsIndex)

#define __crtFlsGetValue(dwFlsIndex) FlsGetValue(dwFlsIndex)

#define __crtFlsSetValue(dwFlsIndex, lpFlsData) FlsSetValue(dwFlsIndex, lpFlsData)

#else // _STL_WIN32_WINNT >= _WIN32_WINNT_WS03

DWORD __cdecl __crtFlsAlloc(__in PFLS_CALLBACK_FUNCTION lpCallback);

BOOL __cdecl __crtFlsFree(__in DWORD dwFlsIndex);

PVOID __cdecl __crtFlsGetValue(__in DWORD dwFlsIndex);

BOOL __cdecl __crtFlsSetValue(__in DWORD dwFlsIndex, __in_opt PVOID lpFlsData);

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
    CompareStringEx(lpLocaleName, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2, NULL, NULL, 0)

#define __crtLCMapStringEx(lpLocaleName, dwMapFlags, lpSrcStr, cchStr, lpDestStr, cchDest) \
    LCMapStringEx(lpLocaleName, dwMapFlags, lpSrcStr, cchStr, lpDestStr, cchDest, NULL, NULL, 0)

#define __crtGetLocaleInfoEx(lpLocaleName, LCType, lpLCData, cchData) \
    GetLocaleInfoEx(lpLocaleName, LCType, lpLCData, cchData)

#else // _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA

_CRTIMP2 BOOL __cdecl __crtInitializeCriticalSectionEx(
    __out LPCRITICAL_SECTION lpCriticalSection, __in DWORD dwSpinCount, __in DWORD Flags);

_CRTIMP2 BOOL __cdecl __crtInitOnceExecuteOnce(
    _Inout_ PINIT_ONCE InitOnce, _In_ PINIT_ONCE_FN InitFn, _Inout_opt_ PVOID Parameter, _Out_opt_ LPVOID* Context);

_CRTIMP2 HANDLE __cdecl __crtCreateEventExW(__in_opt LPSECURITY_ATTRIBUTES lpEventAttributes, __in_opt LPCWSTR lpName,
    __reserved DWORD dwFlags, __in DWORD dwDesiredAccess);

_CRTIMP2 HANDLE __cdecl __crtCreateSemaphoreExW(__in_opt LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
    __in LONG lInitialCount, __in LONG lMaximumCount, __in_opt LPCWSTR lpName, __reserved DWORD dwFlags,
    __in DWORD dwDesiredAccess);

_CRTIMP2 PTP_TIMER __cdecl __crtCreateThreadpoolTimer(
    __in PTP_TIMER_CALLBACK pfnti, __inout_opt PVOID pv, __in_opt PTP_CALLBACK_ENVIRON pcbe);

_CRTIMP2 VOID __cdecl __crtSetThreadpoolTimer(
    __inout PTP_TIMER pti, __in_opt PFILETIME pftDueTime, __in DWORD msPeriod, __in_opt DWORD msWindowLength);

_CRTIMP2 VOID __cdecl __crtWaitForThreadpoolTimerCallbacks(__inout PTP_TIMER pti, __in BOOL fCancelPendingCallbacks);

_CRTIMP2 VOID __cdecl __crtCloseThreadpoolTimer(__inout PTP_TIMER pti);

_CRTIMP2 PTP_WAIT __cdecl __crtCreateThreadpoolWait(
    __in PTP_WAIT_CALLBACK pfnwa, __inout_opt PVOID pv, __in_opt PTP_CALLBACK_ENVIRON pcbe);

_CRTIMP2 VOID __cdecl __crtSetThreadpoolWait(__inout PTP_WAIT pwa, __in_opt HANDLE h, __in_opt PFILETIME pftTimeout);

_CRTIMP2 VOID __cdecl __crtCloseThreadpoolWait(__inout PTP_WAIT pwa);

_CRTIMP2 VOID __cdecl __crtFlushProcessWriteBuffers(void);

_CRTIMP2 VOID __cdecl __crtFreeLibraryWhenCallbackReturns(__inout PTP_CALLBACK_INSTANCE pci, __in HMODULE mod);

_CRTIMP2 DWORD __cdecl __crtGetCurrentProcessorNumber(void);

_CRTIMP2 BOOLEAN __cdecl __crtCreateSymbolicLinkW(
    __in LPCWSTR lpSymlinkFileName, __in LPCWSTR lpTargetFileName, __in DWORD dwFlags);

_CRTIMP2 BOOL __cdecl __crtGetFileInformationByHandleEx(_In_ HANDLE hFile,
    _In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass, _Out_ LPVOID lpFileInformation, _In_ DWORD dwBufferSize);

_CRTIMP2 BOOL __cdecl __crtSetFileInformationByHandle(_In_ HANDLE hFile,
    _In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass, _In_ LPVOID lpFileInformation, _In_ DWORD dwBufferSize);

_CRTIMP2 ULONGLONG __cdecl __crtGetTickCount64();

VOID __cdecl __crtInitializeConditionVariable(__out PCONDITION_VARIABLE);

VOID __cdecl __crtWakeConditionVariable(__inout PCONDITION_VARIABLE);

VOID __cdecl __crtWakeAllConditionVariable(__inout PCONDITION_VARIABLE);

BOOL __cdecl __crtSleepConditionVariableCS(__inout PCONDITION_VARIABLE, __inout PCRITICAL_SECTION, __in DWORD);

VOID __cdecl __crtInitializeSRWLock(__out PSRWLOCK);

VOID __cdecl __crtAcquireSRWLockExclusive(__inout PSRWLOCK);

VOID __cdecl __crtReleaseSRWLockExclusive(__inout PSRWLOCK);

BOOL __cdecl __crtSleepConditionVariableSRW(__inout PCONDITION_VARIABLE, __inout PSRWLOCK, __in DWORD, __in ULONG);

PTP_WORK __cdecl __crtCreateThreadpoolWork(
    __in PTP_WORK_CALLBACK pfnwk, __inout_opt PVOID pv, __in_opt PTP_CALLBACK_ENVIRON pcbe);

VOID __cdecl __crtSubmitThreadpoolWork(__inout PTP_WORK pwk);

VOID __cdecl __crtCloseThreadpoolWork(__inout PTP_WORK pwk);

_CRTIMP2 int __cdecl __crtCompareStringEx(_In_opt_ LPCWSTR lpLocaleName, _In_ DWORD dwCmpFlags,
    _In_NLS_string_(cchCount1) LPCWCH lpString1, _In_ int cchCount1, _In_NLS_string_(cchCount2) LPCWCH lpString2,
    _In_ int cchCount2);

_CRTIMP2 int __cdecl __crtLCMapStringEx(_In_opt_ LPCWSTR lpLocaleName, _In_ DWORD dwMapFlags,
    _In_reads_(cchSrc) LPCWSTR lpSrcStr, _In_ int cchSrc, _Out_writes_opt_(cchDest) LPWSTR lpDestStr, _In_ int cchDest);

_CRTIMP2 int __cdecl __crtGetLocaleInfoEx(
    _In_opt_ LPCWSTR lpLocaleName, _In_ LCTYPE LCType, _Out_opt_ LPWSTR lpLCData, _In_ int cchData);

#endif // _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA

#if _STL_WIN32_WINNT >= _WIN32_WINNT_WIN7

#define __crtTryAcquireSRWLockExclusive(pLock) TryAcquireSRWLockExclusive(pLock)

#else // _STL_WIN32_WINNT >= _WIN32_WINNT_WIN7

BOOLEAN __cdecl __crtTryAcquireSRWLockExclusive(__inout PSRWLOCK);

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

BOOL __cdecl __crtQueueUserWorkItem(__in LPTHREAD_START_ROUTINE function, __in_opt PVOID context, __in ULONG flags);

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

typedef DWORD(WINAPI* PFNFLSALLOC)(PFLS_CALLBACK_FUNCTION);
typedef BOOL(WINAPI* PFNFLSFREE)(DWORD);
typedef PVOID(WINAPI* PFNFLSGETVALUE)(DWORD);
typedef BOOL(WINAPI* PFNFLSSETVALUE)(DWORD, PVOID);
typedef BOOL(WINAPI* PFNINITIALIZECRITICALSECTIONEX)(LPCRITICAL_SECTION, DWORD, DWORD);
typedef BOOL(WINAPI* PFNINITONCEEXECUTEONCE)(PINIT_ONCE, PINIT_ONCE_FN, PVOID, LPVOID*);
typedef HANDLE(WINAPI* PFNCREATEEVENTEXW)(LPSECURITY_ATTRIBUTES, LPCWSTR, DWORD, DWORD);
typedef HANDLE(WINAPI* PFNCREATESEMAPHOREW)(LPSECURITY_ATTRIBUTES, LONG, LONG, LPCWSTR);
typedef HANDLE(WINAPI* PFNCREATESEMAPHOREEXW)(LPSECURITY_ATTRIBUTES, LONG, LONG, LPCWSTR, DWORD, DWORD);
typedef PTP_TIMER(WINAPI* PFNCREATETHREADPOOLTIMER)(PTP_TIMER_CALLBACK, PVOID, PTP_CALLBACK_ENVIRON);
typedef VOID(WINAPI* PFNSETTHREADPOOLTIMER)(PTP_TIMER, PFILETIME, DWORD, DWORD);
typedef VOID(WINAPI* PFNWAITFORTHREADPOOLTIMERCALLBACKS)(PTP_TIMER, BOOL);
typedef VOID(WINAPI* PFNCLOSETHREADPOOLTIMER)(PTP_TIMER);
typedef PTP_WAIT(WINAPI* PFNCREATETHREADPOOLWAIT)(PTP_WAIT_CALLBACK, PVOID, PTP_CALLBACK_ENVIRON);
typedef VOID(WINAPI* PFNSETTHREADPOOLWAIT)(PTP_WAIT, HANDLE, PFILETIME);
typedef VOID(WINAPI* PFNCLOSETHREADPOOLWAIT)(PTP_WAIT);
typedef VOID(WINAPI* PFNFLUSHPROCESSWRITEBUFFERS)(void);
typedef VOID(WINAPI* PFNFREELIBRARYWHENCALLBACKRETURNS)(PTP_CALLBACK_INSTANCE, HMODULE);
typedef DWORD(WINAPI* PFNGETCURRENTPROCESSORNUMBER)(void);
typedef BOOLEAN(WINAPI* PFNCREATESYMBOLICLINKW)(LPCWSTR, LPCWSTR, DWORD);
typedef ULONGLONG(WINAPI* PFNGETTICKCOUNT64)(void);
typedef BOOL(WINAPI* PFNGETFILEINFORMATIONBYHANDLEEX)(HANDLE, FILE_INFO_BY_HANDLE_CLASS, LPVOID, DWORD);
typedef BOOL(WINAPI* PFNSETFILEINFORMATIONBYHANDLE)(HANDLE, FILE_INFO_BY_HANDLE_CLASS, LPVOID, DWORD);
typedef VOID(WINAPI* PFNGETSYSTEMTIMEPRECISEASFILETIME)(LPFILETIME);
typedef VOID(WINAPI* PFNINITIALIZECONDITIONVARIABLE)(PCONDITION_VARIABLE);
typedef VOID(WINAPI* PFNWAKECONDITIONVARIABLE)(PCONDITION_VARIABLE);
typedef VOID(WINAPI* PFNWAKEALLCONDITIONVARIABLE)(PCONDITION_VARIABLE);
typedef BOOL(WINAPI* PFNSLEEPCONDITIONVARIABLECS)(PCONDITION_VARIABLE, PCRITICAL_SECTION, DWORD);
typedef VOID(WINAPI* PFNINITIALIZESRWLOCK)(PSRWLOCK);
typedef VOID(WINAPI* PFNACQUIRESRWLOCKEXCLUSIVE)(PSRWLOCK);
typedef BOOLEAN(WINAPI* PFNTRYACQUIRESRWLOCKEXCLUSIVE)(PSRWLOCK);
typedef VOID(WINAPI* PFNRELEASESRWLOCKEXCLUSIVE)(PSRWLOCK);
typedef BOOL(WINAPI* PFNSLEEPCONDITIONVARIABLESRW)(PCONDITION_VARIABLE, PSRWLOCK, DWORD, ULONG);
typedef PTP_WORK(WINAPI* PFNCREATETHREADPOOLWORK)(PTP_WORK_CALLBACK, PVOID, PTP_CALLBACK_ENVIRON);
typedef VOID(WINAPI* PFNSUBMITTHREADPOOLWORK)(PTP_WORK);
typedef VOID(WINAPI* PFNCLOSETHREADPOOLWORK)(PTP_WORK);
typedef int(WINAPI* PFNCOMPARESTRINGEX)(LPCWSTR, DWORD, LPCWSTR, int, LPCWSTR, int, LPNLSVERSIONINFO, LPVOID, LPARAM);
typedef int(WINAPI* PFNGETLOCALEINFOEX)(LPCWSTR, LCTYPE, LPWSTR, int);
typedef int(WINAPI* PFNLCMAPSTRINGEX)(LPCWSTR, DWORD, LPCWSTR, int, LPWSTR, int, LPNLSVERSIONINFO, LPVOID, LPARAM);

// Use this macro for caching a function pointer from a DLL
#define STOREFUNCTIONPOINTER(instance, functionname) \
    __KERNEL32##Functions[e##functionname] = (PVOID)(GetProcAddress(instance, #functionname));

// Use this macro as a cached function pointer from a DLL
#define DYNAMICGETCACHEDFUNCTION(functiontypedef, functionname, functionpointer) \
    functiontypedef functionpointer = (functiontypedef)(__KERNEL32##Functions[e##functionname])

#define IFDYNAMICGETCACHEDFUNCTION(functiontypedef, functionname, functionpointer) \
    DYNAMICGETCACHEDFUNCTION(functiontypedef, functionname, functionpointer);      \
    if (functionpointer != NULL)

_CRTIMP2 int __cdecl __crtCompareStringA(_In_ LPCWSTR _LocaleName, _In_ DWORD _DwCmpFlags,
    _In_reads_(_CchCount1) LPCSTR _LpString1, _In_ int _CchCount1, _In_reads_(_CchCount2) LPCSTR _LpString2,
    _In_ int _CchCount2, _In_ int _CodePage);

_CRTIMP2 int __cdecl __crtCompareStringW(_In_ LPCWSTR _LocaleName, _In_ DWORD _DwCmpFlags,
    _In_reads_(_CchCount1) LPCWSTR _LpString1, _In_ int _CchCount1, _In_reads_(_CchCount2) LPCWSTR _LpString2,
    _In_ int _CchCount2);

_CRTIMP2 int __cdecl __crtLCMapStringA(_In_ LPCWSTR _LocaleName, _In_ DWORD _DwMapFlag,
    _In_reads_(_CchSrc) LPCSTR _LpSrcStr, _In_ int _CchSrc, _Out_writes_opt_(_CchDest) LPSTR _LpDestStr,
    _In_ int _CchDest, _In_ int _CodePage, _In_ BOOL _BError);

_CRTIMP2 int __cdecl __crtLCMapStringW(_In_ LPCWSTR _LocaleName, _In_ DWORD _DWMapFlag,
    _In_reads_(_CchSrc) LPCWSTR _LpSrcStr, _In_ int _CchSrc, _Out_writes_opt_(_CchDest) LPWSTR _LpDestStr,
    _In_ int _CchDest);

_CRT_END_C_HEADER
