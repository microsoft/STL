// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement shared_ptr spin lock

#include <yvals.h>

#include <intrin.h>
#pragma warning(disable : 4793)

#include "awint.h"
#include <Winnt.h>

#include "atomic"
#include "cstdint"

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

inline bool is_win8_wait_on_address_available() {
#if _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8
    return true;
#else
    // TryAcquireSRWLockExclusive ONLY available on Windows 7+
    DYNAMICGETCACHEDFUNCTION(PFNWAITONADDRESS, WaitOnAddress, pfWaitOnAddress);
    return pfWaitOnAddress != nullptr;
#endif
}

void __cdecl _Atomic_wait_direct(const void* _Storage, void* _Comparand, size_t _Size, long& _Spin_context) {
    if (is_win8_wait_on_address_available())
        __crtAtomic_wait_direct(_Storage, _Comparand, _Size);
    else
        _AtomicSpin(_Spin_context);
}

void __cdecl _Atomic_notify_one_direct(void* _Storage) {
    if (is_win8_wait_on_address_available())
        __crtAtomic_notify_one_direct(_Storage);
}

void __cdecl _Atomic_notify_all_direct(void* _Storage) {
    if (is_win8_wait_on_address_available())
        __crtAtomic_notify_all_direct(_Storage);
}


constexpr size_t TABLE_SIZE_POWER = 8;
constexpr size_t TABLE_SIZE       = 1 << TABLE_SIZE_POWER;


#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier

struct alignas(64) _Contention_table_entry {
    std::atomic<std::uint64_t> _Counter;
};

#pragma warning(pop)

static _Contention_table_entry _Contention_table[TABLE_SIZE];

_Contention_table_entry& _Atomic_contention_table(const void* _Storage) {
    auto index = reinterpret_cast<std::uintptr_t>(_Storage);
    index >>= TABLE_SIZE_POWER * TABLE_SIZE_POWER;
    index >>= TABLE_SIZE_POWER;
    return _Contention_table[index & ((1 << TABLE_SIZE_POWER) - 1)];
}

void __cdecl _Atomic_wait_indirect(const void* _Storage, long& _Spin_context) noexcept {
    if (is_win8_wait_on_address_available()) {
        auto& _Table = _Atomic_contention_table(_Storage);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        auto _Counter = _Table._Counter.load(std::memory_order_relaxed);
        __crtAtomic_wait_direct(&_Table._Counter._Storage._Value, &_Counter, sizeof(_Table._Counter._Storage._Value));
    }
    else
        _AtomicSpin(_Spin_context);
}

void __cdecl _Atomic_notify_indirect(void* _Storage) noexcept {
    if (is_win8_wait_on_address_available()) {
        auto& _Table = _Atomic_contention_table(_Storage);
        _Table._Counter.fetch_add(1, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        __crtAtomic_notify_all_direct(&_Table._Counter._Storage._Value);
    }
}

_END_EXTERN_C
