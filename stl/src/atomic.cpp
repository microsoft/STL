// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement shared_ptr spin lock

#include <yvals.h>
#include <synchapi.h>

namespace {
    // MUTEX FOR shared_ptr ATOMIC OPERATIONS
    SRWLOCK _Shared_ptr_lock = SRWLOCK_INIT;
} // unnamed namespace

_EXTERN_C

_CRTIMP2_PURE void __cdecl _Lock_shared_ptr_spin_lock() { // spin until _Shared_ptr_flag successfully set
    AcquireSRWLockExclusive(&_Shared_ptr_lock);
}

_CRTIMP2_PURE void __cdecl _Unlock_shared_ptr_spin_lock() { // release previously obtained lock
    ReleaseSRWLockExclusive(&_Shared_ptr_lock);
}

_END_EXTERN_C
