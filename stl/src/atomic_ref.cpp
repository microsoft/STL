// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement atomic_ref spin lock

#include <cstdint>
#include <intrin.h>
#include <new>
#include <xthreads.h>

_EXTERN_C
_Smtx_t* __stdcall __std_atomic_get_mutex(const void* const _Key) noexcept {
    constexpr size_t _Table_size_power = 8;
    constexpr size_t _Table_size       = 1 << _Table_size_power;
    constexpr size_t _Table_index_mask = _Table_size - 1;

    struct alignas(std::hardware_destructive_interference_size) _Table_entry {
        _Smtx_t _Mutex                                                           = 0;
        char _Pad[std::hardware_destructive_interference_size - sizeof(_Smtx_t)] = {};
    };
    static _Table_entry _Table[_Table_size];

    auto _Index = reinterpret_cast<const std::uintptr_t>(_Key);
    _Index ^= _Index >> (_Table_size_power * 2);
    _Index ^= _Index >> _Table_size_power;
    return &_Table[_Index & _Table_index_mask]._Mutex;
}



_NODISCARD unsigned char __stdcall __std_atomic_compare_exchange_128(_Inout_bytecount_(16) long long* _Destination,
    _In_ long long _ExchangeHigh, _In_ long long _ExchangeLow,
    _Inout_bytecount_(16) long long* _ComparandResult) noexcept {
#if defined(_M_X64) || defined(_M_ARM64)
    return _InterlockedCompareExchange128(_Destination, _ExchangeHigh, _ExchangeLow, _ComparandResult);
#else
    __debugbreak();
    return 0;
#endif
}

_NODISCARD bool __stdcall __std_atomic_has_cmpxchg16b() noexcept {
    return true;
}

_END_EXTERN_C
