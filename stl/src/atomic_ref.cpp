// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement atomic_ref spin lock

#include <cstdint>
#include <new>

_EXTERN_C
long* __stdcall __std_atomic_get_mutex(const void* const _Key) noexcept {
    constexpr size_t _Table_size_power = 8;
    constexpr size_t _Table_size       = 1 << _Table_size_power;
    constexpr size_t _Table_index_mask = _Table_size - 1;

    struct alignas(std::hardware_destructive_interference_size) _Table_entry {
        long _Mutex                                                           = 0;
        char _Pad[std::hardware_destructive_interference_size - sizeof(long)] = {};
    };
    static _Table_entry _Table[_Table_size];

    auto _Index = reinterpret_cast<const std::uintptr_t>(_Key);
    _Index ^= _Index >> (_Table_size_power * 2);
    _Index ^= _Index >> _Table_size_power;
    return &_Table[_Index & _Table_index_mask]._Mutex;
}
_END_EXTERN_C
