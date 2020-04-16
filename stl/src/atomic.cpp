
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement shared_ptr spin lock

#include <yvals.h>

#include <intrin.h>
#pragma warning(disable : 4793)

_EXTERN_C

// SPIN LOCK FOR shared_ptr ATOMIC OPERATIONS
volatile long _Shared_ptr_flag;

_CRTIMP2_PURE void __cdecl _Lock_shared_ptr_spin_lock() { // spin until _Shared_ptr_flag successfully set
#if defined(_M_ARM) || defined(_M_ARM64)
    while (_InterlockedExchange_acq(&_Shared_ptr_flag, 1)) {
        __yield();
    }
#else // defined(_M_ARM) || defined(_M_ARM64)
    while (_interlockedbittestandset(&_Shared_ptr_flag, 0)) { // set bit 0
    }
#endif // defined(_M_ARM) || defined(_M_ARM64)
}

_CRTIMP2_PURE void __cdecl _Unlock_shared_ptr_spin_lock() { // release previously obtained lock
#if defined(_M_ARM) || defined(_M_ARM)
    __dmb(_ARM_BARRIER_ISH);
#else // defined(_M_ARM) || defined(_M_ARM64)
    _ReadWriteBarrier();
#endif // defined(_M_ARM) || defined(_M_ARM64)
    __iso_volatile_store32(reinterpret_cast<volatile int*>(&_Shared_ptr_flag), 0);
}

_END_EXTERN_C
