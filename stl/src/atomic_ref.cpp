// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement atomic_ref spin lock

#include <cstdint>
#include <new>
#include <synchapi.h>

SRWLOCK* _Atomic_atomic_ref_get_mutex(const void* const _Key) noexcept {
    constexpr size_t _Table_size_power = 8;
    constexpr size_t _Table_size       = 1 << _Table_size_power;
    constexpr size_t _Table_index_mask = _Table_size - 1;

#pragma warning(push)
#pragma warning(disable : 4324) // '%s': structure was padded due to alignment specifier
    struct alignas(std::hardware_destructive_interference_size) _Table_entry {
        SRWLOCK _Mutex = SRWLOCK_INIT;
    };
    static _Table_entry _Table[_Table_size];
#pragma warning(pop)
    auto _Index = reinterpret_cast<const std::uintptr_t>(_Key);
    _Index ^= _Index >> (_Table_size_power * 2);
    _Index ^= _Index >> _Table_size_power;
    return &_Table[_Index & _Table_index_mask]._Mutex;
}

_EXTERN_C
void __stdcall __std_atomic_ref_lock(const void* const _Key) noexcept {
    ::AcquireSRWLockExclusive(_Atomic_atomic_ref_get_mutex(_Key));
}

void __stdcall __std_atomic_ref_unlock(const void* const _Key) noexcept {
    ::ReleaseSRWLockExclusive(_Atomic_atomic_ref_get_mutex(_Key));
}
_END_EXTERN_C
