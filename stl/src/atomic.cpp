// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement shared_ptr spin lock

#include <yvals.h>

#include <synchapi.h>

namespace {
    SRWLOCK _Shared_ptr_lock = SRWLOCK_INIT;
} // unnamed namespace

extern "C" {

_CRTIMP2_PURE void __cdecl _Lock_shared_ptr_spin_lock() noexcept { // TRANSITION, ABI: "spin_lock" name is outdated
    AcquireSRWLockExclusive(&_Shared_ptr_lock);
}

_CRTIMP2_PURE void __cdecl _Unlock_shared_ptr_spin_lock() noexcept { // release previously obtained lock
    _Analysis_assume_lock_held_(_Shared_ptr_lock);
    ReleaseSRWLockExclusive(&_Shared_ptr_lock);
}

} // extern "C"
