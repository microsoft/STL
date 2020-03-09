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
#include "new"
#include "thread"
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

enum _Atomic_spin_phase {
    _ATOMIC_SPIN_PHASE_MASK              = 0xF000'0000,
    _ATOMIC_WAIT_PHASE_MASK              = 0x0F00'0000,
    _ATOMIC_SPIN_VALUE_MASK              = 0x00FF'FFFF,
    _ATOMIC_WAIT_PHASE_SPIN              = 0x0000'0000,
    _ATOMIC_WAIT_PHASE_WAIT_SET          = 0x0100'0000,
    _ATOMIC_WAIT_PHASE_WAIT_CLEAR        = 0x0200'0000,
    _ATOMIC_WAIT_PHASE_WAIT_NO_SEMAPHORE = 0x0300'0000,
    _ATOMIC_SPIN_PHASE_INIT_SPIN_COUNT   = 0x0000'0000,
    _ATOMIC_SPIN_PHASE_SPIN              = 0x1000'0000,
    _ATOMIC_SPIN_PHASE_SWITCH_THD        = 0x2000'0000,
    _ATOMIC_SPIN_PHASE_SLEEP_ZERO        = 0x3000'0000,
    _ATOMIC_SPIN_PHASE_SLEEP             = 0x4000'0000,

    _ATOMIC_SPIN_MASK = _ATOMIC_SPIN_PHASE_MASK | _ATOMIC_SPIN_VALUE_MASK,
};

static_assert(_ATOMIC_WAIT_PHASE_WAIT_SET == _ATOMIC_UNWAIT_NEEDED);

static bool __cdecl __std_atomic_spin_active_only(long& _Spin_context) noexcept {
    switch (_Spin_context & _ATOMIC_SPIN_PHASE_MASK) {
    case _ATOMIC_SPIN_PHASE_INIT_SPIN_COUNT: {
        _Spin_context = _ATOMIC_SPIN_PHASE_SPIN + __std_atomic_spin_count_initialize();
        [[fallthrough]];
    }

    case _ATOMIC_SPIN_PHASE_SPIN: {
        if ((_Spin_context & _ATOMIC_SPIN_VALUE_MASK) > 0) {
            _Spin_context -= 1;
            YieldProcessor();
            return true;
        }
    }
    }
    return false;
}

static void __cdecl __std_atomic_spin(long& _Spin_context) noexcept {
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
        _Spin_context = _ATOMIC_SPIN_PHASE_SWITCH_THD | (_Spin_context & _ATOMIC_WAIT_PHASE_MASK);
        [[fallthrough]];
    }

    case _ATOMIC_SPIN_PHASE_SWITCH_THD: {
        if ((_Spin_context & _ATOMIC_SPIN_VALUE_MASK) < 4) {
            _Spin_context += 1;
            ::SwitchToThread();
            return;
        }
        _Spin_context = _ATOMIC_SPIN_PHASE_SLEEP_ZERO | (_Spin_context & _ATOMIC_WAIT_PHASE_MASK);
        [[fallthrough]];
    }

    case _ATOMIC_SPIN_PHASE_SLEEP_ZERO: {
        if ((_Spin_context & _ATOMIC_SPIN_VALUE_MASK) < 16) {
            _Spin_context += 1;
            ::Sleep(0);
            return;
        }
        _Spin_context = 10 | _ATOMIC_SPIN_PHASE_SLEEP | (_Spin_context & _ATOMIC_WAIT_PHASE_MASK);
        [[fallthrough]];
    }

    case _ATOMIC_SPIN_PHASE_SLEEP: {
        long sleep_count = _Spin_context & _ATOMIC_SPIN_VALUE_MASK;
        ::Sleep(sleep_count);
        sleep_count   = std::min<long>(sleep_count << 1, 5000L);
        _Spin_context = sleep_count | _ATOMIC_SPIN_PHASE_SLEEP | (_Spin_context & _ATOMIC_WAIT_PHASE_MASK);
        return;
    }
    }
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

// Flag for semaphore deletion
static std::atomic_flag _Semaphore_dereference_registered;

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier

struct alignas(std::hardware_destructive_interference_size) _Contention_table_entry {
    // Arbitraty variable to wait/notify on if target wariable is not proper atomic for that
    // Size is largest of lock-free to make aliasing problem into hypothetical
    std::atomic<std::uint64_t> _Counter;
    // Event to wait on in case of no atomic ops
    std::atomic<std::intptr_t> _Semaphore = -1;
    // Event use count, can delete event if drops to zero
    // Initialized to one to keep event used when progam runs, will drop to zero on program exit
    std::atomic<std::size_t> _Semaphore_use_count = 1;
    // Flag whether semaphore should be released
    std::atomic<LONG> _Semaphore_own_count = 0;
    // Flag to initialize semaphore
    std::once_flag _Flag_semaphore_initialized;

    static void _Dereference_all_semaphores() noexcept;

    void _Inititalize_semaphore(intptr_t& new_semaphore) noexcept {
        new_semaphore = reinterpret_cast<std::intptr_t>(::CreateSemaphore(nullptr, 0, MAXLONG, nullptr));
        _Semaphore.store(new_semaphore, std::memory_order_release);
        if (!_Semaphore_dereference_registered.test_and_set(std::memory_order_relaxed)) {
            atexit(_Dereference_all_semaphores);
        }
    }

    HANDLE _Reference_semaphore() noexcept {
        _Semaphore_use_count.fetch_add(1, std::memory_order_relaxed);
        intptr_t semaphore = _Semaphore.load(std::memory_order_acquire);
        if (semaphore == -1) {
            std::call_once(
                _Flag_semaphore_initialized, &_Contention_table_entry::_Inititalize_semaphore, this, semaphore);
        }
        return reinterpret_cast<HANDLE>(semaphore);
    }

    void _Dereference_semaphore() noexcept {
        if (_Semaphore_use_count.fetch_sub(1, std::memory_order_relaxed) == 1) {
            std::intptr_t semaphore = _Semaphore.exchange(0, std::memory_order_acq_rel);
            if (semaphore != 0) {
                ::CloseHandle(reinterpret_cast<HANDLE>(semaphore));
            }
        }
    }
};

#pragma warning(pop)

static _Contention_table_entry _Contention_table[TABLE_SIZE];

_Contention_table_entry& _Atomic_contention_table(const void* _Storage) noexcept {
    auto index = reinterpret_cast<std::uintptr_t>(_Storage);
    index ^= index >> (TABLE_SIZE_POWER * 2);
    index ^= index >> TABLE_SIZE_POWER;
    return _Contention_table[index & TABLE_MASK];
}

void _Contention_table_entry::_Dereference_all_semaphores() noexcept {
    for (_Contention_table_entry& entry : _Contention_table) {
        entry._Dereference_semaphore();
    }
}


void __cdecl __std_atomic_wait_fallback(const void* _Storage, long& _Spin_context) noexcept {
    switch (_Spin_context & _ATOMIC_WAIT_PHASE_MASK) {

    case _ATOMIC_WAIT_PHASE_SPIN: {
        if (__std_atomic_spin_active_only(_Spin_context)) {
            break;
        }
    }

        _Spin_context = _ATOMIC_WAIT_PHASE_WAIT_CLEAR | (_Spin_context & _ATOMIC_SPIN_MASK);
        [[fallthrough]];

    case _ATOMIC_WAIT_PHASE_WAIT_CLEAR: {
        auto& _Table = _Atomic_contention_table(_Storage);
        _Table._Semaphore_own_count.fetch_add(1);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        _Spin_context = _ATOMIC_WAIT_PHASE_WAIT_SET | (_Spin_context & _ATOMIC_SPIN_MASK);
        break; // query again directly before waiting
    }

    case _ATOMIC_WAIT_PHASE_WAIT_SET: {
        auto& _Table      = _Atomic_contention_table(_Storage);
        HANDLE _Semaphore = _Table._Reference_semaphore();
        if (_Semaphore != nullptr) {
            ::WaitForSingleObject(_Semaphore, INFINITE);
        }
        _Table._Dereference_semaphore();

        if (_Semaphore != nullptr) {
            _Spin_context = _ATOMIC_WAIT_PHASE_WAIT_CLEAR | (_Spin_context & _ATOMIC_SPIN_MASK);
            break;
        }

        _Spin_context = _ATOMIC_WAIT_PHASE_WAIT_NO_SEMAPHORE | (_Spin_context & _ATOMIC_SPIN_MASK);
        [[fallthrough]];
    }

    case _ATOMIC_WAIT_PHASE_WAIT_NO_SEMAPHORE: {
        __std_atomic_spin(_Spin_context);
        break;
    }
    }
}

void __cdecl __std_atomic_unwait_fallback(const void* _Storage, long& _Spin_context) noexcept {
    if ((_Spin_context & _ATOMIC_WAIT_PHASE_MASK) == _ATOMIC_WAIT_PHASE_WAIT_SET) {
        auto& _Table = _Atomic_contention_table(_Storage);
        _Table._Semaphore_own_count.fetch_sub(1);
    }
}

void __cdecl __std_atomic_notify_fallback(void* _Storage) noexcept {
    auto& _Table = _Atomic_contention_table(_Storage);
    std::atomic_thread_fence(std::memory_order_seq_cst);
    LONG _Semaphore_own_count = _Table._Semaphore_own_count.load();
    if (_Semaphore_own_count <= 0) {
        return;
    }
    HANDLE _Semaphore = _Table._Reference_semaphore();
    if (_Semaphore != nullptr) {
        ::ReleaseSemaphore(_Semaphore, _Semaphore_own_count, nullptr);
    }
    _Table._Dereference_semaphore();
    _Table._Semaphore_own_count.fetch_sub(_Semaphore_own_count);
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
