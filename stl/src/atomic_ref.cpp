// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement atomic_ref spin lock

#include <atomic>
#include <cstdint>
#include <intrin.h>
#include <new>
#include <synchapi.h>
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

namespace {

    _NODISCARD unsigned char __std_atomic_compare_exchange_128_fallback(_Inout_bytecount_(16) long long* _Destination,
        _In_ long long _ExchangeHigh, _In_ long long _ExchangeLow,
        _Inout_bytecount_(16) long long* _ComparandResult) noexcept {
        static SRWLOCK _Mtx = SRWLOCK_INIT;
        AcquireSRWLockExclusive(&_Mtx);
        if (_Destination[0] == _ComparandResult[0] && _Destination[1] == _ComparandResult[1]) {
            _ComparandResult[0] = _Destination[0];
            _ComparandResult[1] = _Destination[1];
            _Destination[0]     = _ExchangeLow;
            _Destination[1]     = _ExchangeHigh;
            ReleaseSRWLockExclusive(&_Mtx);
            return true;
        } else {
            _ComparandResult[0] = _Destination[0];
            _ComparandResult[1] = _Destination[1];
            ReleaseSRWLockExclusive(&_Mtx);
            return false;
        }
    }

} // unnamed namespace

_NODISCARD unsigned char __stdcall __std_atomic_compare_exchange_128(_Inout_bytecount_(16) long long* _Destination,
    _In_ long long _ExchangeHigh, _In_ long long _ExchangeLow,
    _Inout_bytecount_(16) long long* _ComparandResult) noexcept {
#if defined(_M_X64)
    if (__std_atomic_has_cmpxchg16b()) {
        return _InterlockedCompareExchange128(_Destination, _ExchangeHigh, _ExchangeLow, _ComparandResult);
    } else {
        return __std_atomic_compare_exchange_128_fallback(_Destination, _ExchangeHigh, _ExchangeLow, _ComparandResult);
    }
#elif defined(_M_ARM64)
    return _InterlockedCompareExchange128(_Destination, _ExchangeHigh, _ExchangeLow, _ComparandResult);
#else
    return __std_atomic_compare_exchange_128_fallback(_Destination, _ExchangeHigh, _ExchangeLow, _ComparandResult);
#endif
}

_NODISCARD bool __stdcall __std_atomic_has_cmpxchg16b() noexcept {
#if defined(_M_X64)
    enum class _Cmpxchg16_support : char {
        _Absent  = false,
        _Present = true,
        _Unknown,
    };
    static std::atomic<_Cmpxchg16_support> _Cached_value{_Cmpxchg16_support::_Unknown};

    _Cmpxchg16_support _Value = _Cached_value.load(std::memory_order_relaxed);

    if (_Value == _Cmpxchg16_support::_Unknown) {
        int regs[4];
        __cpuid(regs, 1); // assume leaf 1 exists
        if (regs[2] & (1 << 13)) {
            _Value = _Cmpxchg16_support::_Present;
        } else {
            _Value = _Cmpxchg16_support::_Absent;
        }
        _Cached_value.store(_Value, std::memory_order_relaxed);
    }
    return reinterpret_cast<bool&>(_Value);
#elif defined(_M_ARM64)
    return true;
#else
    return false;
#endif
}

_END_EXTERN_C
