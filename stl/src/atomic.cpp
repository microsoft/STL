// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement shared_ptr spin lock

#include <yvals.h>

#include <atomic>

_EXTERN_C

std::atomic_flag _Shared_ptr_flag{};

_CRTIMP2_PURE void __cdecl _Lock_shared_ptr_spin_lock() { // spin until _Shared_ptr_flag successfully set
    while (_Shared_ptr_flag.test_and_set(std::memory_order_acquire)) {
        _YIELD_PROCESSOR();
    }
}

_CRTIMP2_PURE void __cdecl _Unlock_shared_ptr_spin_lock() { // release previously obtained lock
    _Shared_ptr_flag.clear(std::memory_order_release);
}

_END_EXTERN_C
