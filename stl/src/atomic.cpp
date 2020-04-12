// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement shared_ptr spin lock

#include <yvals.h>

#include <xatomic_wait.h>
#include <intrin.h>
#pragma warning(disable : 4793)

_EXTERN_C

// SPIN LOCK FOR shared_ptr ATOMIC OPERATIONS
volatile long _Shared_ptr_flag;

_CRTIMP2_PURE void __cdecl _Lock_shared_ptr_spin_lock() { // spin until _Shared_ptr_flag successfully set
    _Atomic_wait_context_t _Wait_context;
#ifdef _M_ARM
    while (_InterlockedExchange_acq(&_Shared_ptr_flag, 1)) {
        _Atomic_wait_direct_timed_for_internal_spinlock(&_Shared_ptr_flag, 1L, _Context);
    }
#else // _M_ARM
    while (_interlockedbittestandset(&_Shared_ptr_flag, 0)) { // set bit 0
        _Atomic_wait_direct_for_internal_spinlock(&_Shared_ptr_flag, 1L, _Wait_context);
    }
#endif // _M_ARM
}

_CRTIMP2_PURE void __cdecl _Unlock_shared_ptr_spin_lock() { // release previously obtained lock
#ifdef _M_ARM
    __dmb(_ARM_BARRIER_ISH);
    __iso_volatile_store32((volatile int*) &_Shared_ptr_flag, 0);
#else // _M_ARM
    _interlockedbittestandreset(&_Shared_ptr_flag, 0); // reset bit 0
#endif // _M_ARM
    __std_atomic_notify_one_direct(const_cast<const long*>(&_Shared_ptr_flag));
}

_END_EXTERN_C
