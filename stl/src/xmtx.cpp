// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// mutex support

#include "xmtx.h"

#include "awint.h"

_EXTERN_C_UNLESS_PURE

// Win32 critical sections are recursive

void __CLRCALL_PURE_OR_CDECL _Mtxinit(_Rmtx* _Mtx) noexcept { // initialize mutex
    __crtInitializeCriticalSectionEx(_Mtx, 4000, 0);
}

void __CLRCALL_PURE_OR_CDECL _Mtxdst(_Rmtx* _Mtx) noexcept { // delete mutex
    DeleteCriticalSection(_Mtx);
}

_RELIABILITY_CONTRACT
void __CLRCALL_PURE_OR_CDECL _Mtxlock(_Rmtx* _Mtx) noexcept { // lock mutex
#ifdef _M_CEE
    System::Threading::Thread::BeginThreadAffinity();
#endif // _M_CEE
    EnterCriticalSection(_Mtx);
}

_RELIABILITY_CONTRACT
void __CLRCALL_PURE_OR_CDECL _Mtxunlock(_Rmtx* _Mtx) noexcept { // unlock mutex
    LeaveCriticalSection(_Mtx);
#ifdef _M_CEE
    System::Threading::Thread::EndThreadAffinity();
#endif // _M_CEE
}

_END_EXTERN_C_UNLESS_PURE
