// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement shared_ptr spin lock

#include <yvals.h>

#include <intrin.h>
#pragma warning(disable : 4793)

#include "atomic"
#include "awint.h"
#include "cstdint"
#include "mutex"
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

static std::once_flag _Atomic_spin_count_initialized;

static long _Atomic_spin_count;

static void _Atomic_spin_count_initialize() {
    _Atomic_spin_count = (std::thread::hardware_concurrency() == 1 ? 0 : 10'000);
}

enum _Atomic_spin_phase {
    _ATOMIC_SPIN_PHASE_MASK            = 0xF000'0000,
    _ATOMIC_SPIN_VALUE_MASK            = 0x0FFF'FFFF,
    _ATOMIC_SPIN_PHASE_INIT_SPIN_COUNT = 0x0000'0000,
    _ATOMIC_SPIN_PHASE_INIT_SPIN       = 0x1000'0000,
    _ATOMIC_SPIN_PHASE_INIT_SWITCH_THD = 0x2000'0000,
    _ATOMIC_SPIN_PHASE_INIT_SLEEP_ZERO = 0x3000'0000,
    _ATOMIC_SPIN_PHASE_INIT_SLEEP      = 0x4000'0000,
};

bool __cdecl _Atomic_spin_active_only(long& _Spin_context) {
    switch (_Spin_context & _ATOMIC_SPIN_PHASE_MASK) {
    case _ATOMIC_SPIN_PHASE_INIT_SPIN_COUNT:
        std::call_once(_Atomic_spin_count_initialized, _Atomic_spin_count_initialize);
        _Spin_context = _ATOMIC_SPIN_PHASE_INIT_SPIN + _Atomic_spin_count;
        [[fallthrough]];

    case _ATOMIC_SPIN_PHASE_INIT_SPIN:
        if ((_Spin_context & _ATOMIC_SPIN_VALUE_MASK) > 0) {
            _Spin_context -= 1;
            YieldProcessor();
            return true;
        }
    }
    return false;
}

void __cdecl _Atomic_spin(long& _Spin_context) {
    switch (_Spin_context & _ATOMIC_SPIN_PHASE_MASK) {
    case _ATOMIC_SPIN_PHASE_INIT_SPIN_COUNT:
        std::call_once(_Atomic_spin_count_initialized, _Atomic_spin_count_initialize);
        _Spin_context = _ATOMIC_SPIN_PHASE_INIT_SPIN + _Atomic_spin_count;
        [[fallthrough]];

    case _ATOMIC_SPIN_PHASE_INIT_SPIN:
        if ((_Spin_context & _ATOMIC_SPIN_VALUE_MASK) > 0) {
            _Spin_context -= 1;
            YieldProcessor();
            return;
        }
        _Spin_context = _ATOMIC_SPIN_PHASE_INIT_SWITCH_THD;
        [[fallthrough]];

    case _ATOMIC_SPIN_PHASE_INIT_SWITCH_THD:
        if (_Spin_context < (_ATOMIC_SPIN_PHASE_INIT_SWITCH_THD + 4)) {
            _Spin_context += 1;
            ::SwitchToThread();
            return;
        }
        _Spin_context = _ATOMIC_SPIN_PHASE_INIT_SLEEP_ZERO;
        [[fallthrough]];

    case _ATOMIC_SPIN_PHASE_INIT_SLEEP_ZERO:
        if (_Spin_context < (_ATOMIC_SPIN_PHASE_INIT_SLEEP_ZERO + 16)) {
            _Spin_context += 1;
            ::Sleep(0);
            return;
        }
        _Spin_context = _ATOMIC_SPIN_PHASE_INIT_SLEEP;
        [[fallthrough]];

    case _ATOMIC_SPIN_PHASE_INIT_SLEEP:
        ::Sleep(10);
        return;
    }
}

inline bool is_win8_wait_on_address_available() {
#if _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8
    return true;
#else
    // WaitOnAddress ONLY available on Windows 8+
    DYNAMICGETCACHEDFUNCTION(PFNWAITONADDRESS, WaitOnAddress, pfWaitOnAddress);
    return pfWaitOnAddress != nullptr;
#endif
}

constexpr size_t TABLE_SIZE_POWER = 8;
constexpr size_t TABLE_SIZE       = 1 << TABLE_SIZE_POWER;
constexpr size_t TABLE_MASK       = TABLE_SIZE - 1;


#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier

struct alignas(64) _Contention_table_entry {
    // Arbitraty variable to wait/notify on if target wariable is not proper atomic for that
    // Size is largest of lock-free to make aliasing problem into hypothetical
    std::atomic<std::uint64_t> _Counter;
    // Event to wait on in case of no atomic ops
    std::atomic<HANDLE> _Event;
    // Event use count, can delete event if drops to zero
    // Initialized to one to keep event used when progam runs, will drop to zero on program exit
    std::atomic<std::size_t> _Event_use_count = 1;
    // Flag whether event should be set
    std::atomic<std::uint32_t> _Event_should_set = 0;
    // Once flag for event creation
    std::once_flag _Event_created;
    // Once flag for event deletion
    static std::once_flag _Events_dereference_registered;

    static void _Dereference_all_events();

    HANDLE _Reference_event() {
        std::call_once(_Event_created, [this] {
            std::call_once(_Events_dereference_registered, [] { atexit(_Dereference_all_events); });

            // Try create just once, if low resources, use fall back permanently
            HANDLE event = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
            _Event.store(event, std::memory_order_relaxed);
        });
        _Event_use_count.fetch_add(1, std::memory_order_acquire);
        return _Event.load(std::memory_order_relaxed);
    }

    void _Dereference_event() {
        if (_Event_use_count.fetch_sub(1, std::memory_order_release) == 1) {
            HANDLE event = _Event.exchange(nullptr);
            if (event != nullptr)
                ::CloseHandle(event);
        }
    }
};

std::once_flag _Contention_table_entry::_Events_dereference_registered;

#pragma warning(pop)

static _Contention_table_entry _Contention_table[TABLE_SIZE];

_Contention_table_entry& _Atomic_contention_table(const void* _Storage) {
    auto index = reinterpret_cast<std::uintptr_t>(_Storage);
    index ^= index >> (TABLE_SIZE_POWER * 2);
    index ^= index >> TABLE_SIZE_POWER;
    return _Contention_table[index & TABLE_MASK];
}

void _Contention_table_entry::_Dereference_all_events() {
    for (_Contention_table_entry& entry : _Contention_table)
        entry._Dereference_event();
}


void __cdecl _Atomic_wait_fallback(const void* _Storage, long& _Spin_context) noexcept {
    if ((_Spin_context & _ATOMIC_SPIN_PHASE_MASK) >= _ATOMIC_SPIN_PHASE_INIT_SWITCH_THD) {
        // Wait phase
        auto& _Table = _Atomic_contention_table(_Storage);
        HANDLE event = _Table._Reference_event();
        if (event != nullptr)
            ::WaitForSingleObject(event, INFINITE);
        else
            _Atomic_spin(_Spin_context);
        _Table._Dereference_event();
    } else {
        // Spin phase
        if (_Atomic_spin_active_only(_Spin_context))
            return;
        // Spin is over, preparing to wait
        auto& _Table = _Atomic_contention_table(_Storage);
        HANDLE event = _Table._Reference_event();
        if (event != nullptr) {
            ::ResetEvent(event);
            // As to set event
            _Table._Event_should_set.fetch_add(1, std::memory_order_relaxed);
        }
        std::atomic_thread_fence(std::memory_order_seq_cst);
        _Table._Dereference_event();
        // Caller would check value once more
    }
}


void __cdecl _Atomic_notify_fallback(void* _Storage) noexcept {
    auto& _Table = _Atomic_contention_table(_Storage);
    std::atomic_thread_fence(std::memory_order_seq_cst);

    for (;;) {
        auto _Set_event = _Table._Event_should_set.load(std::memory_order_relaxed);
        if (_Set_event == 0)
            break;
        HANDLE event = _Table._Reference_event();
        ::SetEvent(event);
        _Table._Dereference_event();
        _Table._Event_should_set.fetch_sub(_Set_event, std::memory_order_relaxed);
    }
}


void __cdecl _Atomic_wait_direct(const void* _Storage, void* _Comparand, size_t _Size, long& _Spin_context) {
    if (is_win8_wait_on_address_available())
        __crtWaitOnAddress((volatile VOID*)_Storage, _Comparand, _Size, INFINITE);
    else
        _Atomic_wait_fallback(_Storage, _Spin_context);
}


void __cdecl _Atomic_notify_one_direct(void* _Storage) {
    if (is_win8_wait_on_address_available())
        __crtWakeByAddressSingle(_Storage);
    else
        _Atomic_notify_fallback(_Storage);
}


void __cdecl _Atomic_notify_all_direct(void* _Storage) {
    if (is_win8_wait_on_address_available())
        __crtWakeByAddressAll(_Storage);
    else
        _Atomic_notify_fallback(_Storage);
}


void __cdecl _Atomic_wait_indirect(const void* _Storage, long& _Spin_context) noexcept {
    if (is_win8_wait_on_address_available()) {
        auto& _Table = _Atomic_contention_table(_Storage);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        auto _Counter = _Table._Counter.load(std::memory_order_relaxed);
        __crtWaitOnAddress((volatile VOID*) &_Table._Counter._Storage._Value, &_Counter,
            sizeof(_Table._Counter._Storage._Value), INFINITE);
    } else {
        _Atomic_wait_fallback(_Storage, _Spin_context);
    }
}


void __cdecl _Atomic_notify_indirect(void* _Storage) noexcept {
    if (is_win8_wait_on_address_available()) {
        auto& _Table = _Atomic_contention_table(_Storage);
        _Table._Counter.fetch_add(1, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        __crtWakeByAddressAll(&_Table._Counter._Storage._Value);
    } else {
        _Atomic_notify_fallback(_Storage);
    }
}

_END_EXTERN_C
