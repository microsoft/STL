// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement shared_ptr spin lock

#include <yvals.h>

#include <intrin.h>
#pragma warning(disable : 4793)

#include "awint.h"
#include <atomic>
#include <cstdint>
#include <new>
#include <thread>

#include <Windows.h>

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

static std::atomic<long> _Atomic_spin_count = -1;

static inline long __std_atomic_spin_count_initialize() noexcept {
    long result = _Atomic_spin_count.load(std::memory_order_relaxed);
    if (result == -1) {
        result = (std::thread::hardware_concurrency() == 1 ? 0 : 10'000);
        _Atomic_spin_count.store(result, std::memory_order_relaxed);

        // Make sure other thread is likely to get this,
        // as we've done kernel call for that.
        std::atomic_thread_fence(std::memory_order_seq_cst);
    }
    return result;
}

static inline bool is_win8_wait_on_address_available() noexcept {
#if _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8
    return true;
#else
    // WaitOnAddress ONLY available on Windows 8+
    DYNAMICGETCACHEDFUNCTION(PFNWAITONADDRESS, WaitOnAddress, pfWaitOnAddress);
    return pfWaitOnAddress != nullptr;
#endif
}

static constexpr size_t TABLE_SIZE_POWER = 8;
static constexpr size_t TABLE_SIZE       = 1 << TABLE_SIZE_POWER;
static constexpr size_t TABLE_MASK       = TABLE_SIZE - 1;

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier

struct alignas(std::hardware_destructive_interference_size) _Contention_table_entry {
    // Arbitraty variable to wait/notify on if target wariable is not proper atomic for that
    // Size is largest of lock-free to make aliasing problem into hypothetical
    std::atomic<std::uint64_t> _Counter;

    CONDITION_VARIABLE _Condition = CONDITION_VARIABLE_INIT;
    SRWLOCK _Lock                 = SRWLOCK_INIT;
};

#pragma warning(pop)

static _Contention_table_entry _Contention_table[TABLE_SIZE];

_Contention_table_entry& _Atomic_contention_table(const void* _Storage) noexcept {
    auto index = reinterpret_cast<std::uintptr_t>(_Storage);
    index ^= index >> (TABLE_SIZE_POWER * 2);
    index ^= index >> TABLE_SIZE_POWER;
    return _Contention_table[index & TABLE_MASK];
}

enum _Atomic_spin_phase {
    _ATOMIC_SPIN_PHASE_MASK            = 0xF000'0000,
    _ATOMIC_SPIN_VALUE_MASK            = 0x0FFF'FFFF,
    _ATOMIC_SPIN_PHASE_INIT_SPIN_COUNT = 0x0000'0000,
    _ATOMIC_SPIN_PHASE_SPIN            = 0x1000'0000,
    _ATOMIC_WAIT_PHASE                 = 0x2000'0000,
};

static_assert(_ATOMIC_WAIT_PHASE == _ATOMIC_UNWAIT_NEEDED);

void __cdecl __std_atomic_wait_fallback(const void* _Storage, long& _Spin_context) noexcept {

    switch (_Spin_context & _ATOMIC_SPIN_PHASE_MASK) {
    case _ATOMIC_SPIN_PHASE_INIT_SPIN_COUNT: {
        _Spin_context = _ATOMIC_SPIN_PHASE_SPIN + __std_atomic_spin_count_initialize();
        [[fallthrough]];
    }

    case _ATOMIC_SPIN_PHASE_SPIN: {
        if ((_Spin_context & _ATOMIC_SPIN_VALUE_MASK) > 0) {
            _Spin_context -= 1;
            YieldProcessor();
            return;
        }

        _Spin_context = _ATOMIC_WAIT_PHASE;

        auto& entry = _Atomic_contention_table(_Storage);
        ::AcquireSRWLockExclusive(&entry._Lock);
        [[fallthrough]];
    }

    case _ATOMIC_WAIT_PHASE: {
        auto& entry = _Atomic_contention_table(_Storage);
        ::SleepConditionVariableSRW(&entry._Condition, &entry._Lock, INFINITE, 0);
        return; // Return to recheck
    }
    }
}

void __cdecl __std_atomic_unwait_fallback(const void* _Storage, long& _Spin_context) noexcept {
    if ((_Spin_context & _ATOMIC_WAIT_PHASE) != 0) {
        auto& entry = _Atomic_contention_table(_Storage);
        ::ReleaseSRWLockExclusive(&entry._Lock);
    }
}

void __cdecl __std_atomic_notify_fallback(void* _Storage) noexcept {
    auto& entry = _Atomic_contention_table(_Storage);
    ::AcquireSRWLockExclusive(&entry._Lock);
    ::ReleaseSRWLockExclusive(&entry._Lock);
    ::WakeAllConditionVariable(&entry._Condition);
}


void __cdecl __std_atomic_wait_direct(
    const void* _Storage, void* _Comparand, size_t _Size, long& _Spin_context) noexcept {
    if (is_win8_wait_on_address_available()) {
        __crtWaitOnAddress((volatile VOID*) _Storage, _Comparand, _Size, INFINITE);
    } else {
        __std_atomic_wait_fallback(_Storage, _Spin_context);
    }
}


void __cdecl __std_atomic_notify_one_direct(void* _Storage) noexcept {
    if (is_win8_wait_on_address_available()) {
        __crtWakeByAddressSingle(_Storage);
    } else {
        __std_atomic_notify_fallback(_Storage);
    }
}


void __cdecl __std_atomic_notify_all_direct(void* _Storage) noexcept {
    if (is_win8_wait_on_address_available()) {
        __crtWakeByAddressAll(_Storage);
    } else {
        __std_atomic_notify_fallback(_Storage);
    }
}


void __cdecl __std_atomic_wait_indirect(const void* _Storage, long& _Spin_context) noexcept {
    if (is_win8_wait_on_address_available()) {
        auto& _Table = _Atomic_contention_table(_Storage);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        auto _Counter = _Table._Counter.load(std::memory_order_relaxed);
        __crtWaitOnAddress((volatile VOID*) &_Table._Counter._Storage._Value, &_Counter,
            sizeof(_Table._Counter._Storage._Value), INFINITE);
    } else {
        __std_atomic_wait_fallback(_Storage, _Spin_context);
    }
}


void __cdecl __std_atomic_notify_indirect(void* _Storage) noexcept {
    if (is_win8_wait_on_address_available()) {
        auto& _Table = _Atomic_contention_table(_Storage);
        _Table._Counter.fetch_add(1, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        __crtWakeByAddressAll(&_Table._Counter._Storage._Value);
    } else {
        __std_atomic_notify_fallback(_Storage);
    }
}

void __cdecl __std_atomic_unwait_direct(const void* _Storage, long& _Spin_context) noexcept {
    if (!is_win8_wait_on_address_available()) {
        __std_atomic_unwait_fallback(_Storage, _Spin_context);
    }
}

void __cdecl __std_atomic_unwait_indirect(const void* _Storage, long& _Spin_context) noexcept {
    if (!is_win8_wait_on_address_available()) {
        __std_atomic_unwait_fallback(_Storage, _Spin_context);
    }
}

_END_EXTERN_C
