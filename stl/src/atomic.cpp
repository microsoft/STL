// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement shared_ptr spin lock

#include <yvals.h>

#include <intrin.h>
#pragma warning(disable : 4793)

#include "awint.h"
#include <Winnt.h>

_EXTERN_C

// SPIN LOCK FOR shared_ptr ATOMIC OPERATIONS
volatile long _Shared_ptr_flag;

_CRTIMP2_PURE void __cdecl _Lock_shared_ptr_spin_lock() { // spin until _Shared_ptr_flag successfully set
#ifdef _M_ARM
    while (_InterlockedExchange_acq(&_Shared_ptr_flag, 1)) {
        __yield();
    }
#else // _M_ARM
    while (_interlockedbittestandset(&_Shared_ptr_flag, 0)) { // set bit 0
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
}


void __cdecl _AtomicSpin(long& _Spin_context) {
    switch (_Spin_context & 0xF000'0000) {
    case 0:
        if (_Spin_context < 10000) {
            _Spin_context += 1;
            YieldProcessor();
            return;
        }
        _Spin_context = 0x1000'0000;
        [[fallthrough]];

    case 0x1000'0000:
        if (_Spin_context < 0x1000'0004) {
            _Spin_context += 1;
            SwitchToThread();
            return;
        }
        _Spin_context = 0x2000'0000;
        [[fallthrough]];

    case 0x2000'0000:
        if (_Spin_context < 0x2000'0010) {
            _Spin_context += 1;
            Sleep(0);
            return;
        }
        _Spin_context = 0x3000'0000;
        [[fallthrough]];

    case 0x3000'0000:
        Sleep(10);
        return;
    }
}

void __cdecl _Atomic_wait_direct(const void* _Storage, void* _Comparand, size_t _Size, long& _Spin_context) {
    return __crtAtomic_wait_direct(_Storage, _Comparand, _Size, _Spin_context);
}

void __cdecl _Atomic_notify_one_direct(void* _Storage) {
    return __crtAtomic_notify_one_direct(_Storage);
}

void __cdecl _Atomic_notify_all_direct(void* _Storage) {
    return __crtAtomic_notify_all_direct(_Storage);
}

void __cdecl _Atomic_wait_indirect(const void* _Storage, long& _Spin_context) noexcept {
    (void) _Storage;
    _AtomicSpin(_Spin_context);
}

void __cdecl _Atomic_notify_indirect(void* _Storage) noexcept {
    (void) _Storage;
}


_END_EXTERN_C
