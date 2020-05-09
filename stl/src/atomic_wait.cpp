// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement atomic wait / notify_one / notify_all

// clang-format off

#include <atomic>
#include <cstdint>
#include <new>
#include <thread>
#include <Windows.h>
#include <VersionHelpers.h>

// clang-format on

namespace {

    constexpr size_t _Wait_table_size_power = 8;
    constexpr size_t _Wait_table_size       = 1 << _Wait_table_size_power;
    constexpr size_t _Wait_table_index_mask = _Wait_table_size - 1;

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
    struct alignas(std::hardware_destructive_interference_size) _Wait_table_entry {
        // Arbitrary variable to wait/notify on if target variable is not proper atomic for that
        // Size is largest of lock-free to make aliasing problem into hypothetical
        std::atomic<unsigned long long> _Counter{};

        CONDITION_VARIABLE _Condition = CONDITION_VARIABLE_INIT;
        SRWLOCK _Lock                 = SRWLOCK_INIT;

        constexpr _Wait_table_entry() noexcept = default;
    };
#pragma warning(pop)

    _Wait_table_entry& _Atomic_wait_table_entry(const void* const _Storage) noexcept {
        static _Wait_table_entry wait_table[_Wait_table_size];
        auto index = reinterpret_cast<std::uintptr_t>(_Storage);
        index ^= index >> (_Wait_table_size_power * 2);
        index ^= index >> _Wait_table_size_power;
        return wait_table[index & _Wait_table_index_mask];
    }

    unsigned long _Get_remaining_waiting_time(_Atomic_wait_context_t& _Wait_context) {
        const unsigned long long deadline = _Wait_context._Deadline;
        if (deadline == _Atomic_wait_no_deadline) {
            return INFINITE;
        }

        const unsigned long long current_time = ::GetTickCount64();
        if (current_time >= deadline) {
            return 0;
        }

        unsigned long long remaining      = deadline - current_time;
        constexpr unsigned long _Ten_days = 864'000'000;
        if (remaining > _Ten_days) {
            return _Ten_days;
        }
        return static_cast<unsigned long>(remaining);
    }

    void _Assume_timeout() noexcept {
#ifdef _DEBUG
        if (::GetLastError() != ERROR_TIMEOUT) {
            abort(); // we are in noexcept, don't throw
        }
#endif
    }

#if _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8

#define _ATOMIC_WAIT_STATICALLY_AVAILABLE_TO_IMPL

#define __crtWaitOnAddress       WaitOnAddress
#define __crtWakeByAddressSingle WakeByAddressSingle
#define __crtWakeByAddressAll    WakeByAddressAll

#pragma comment(lib, "Synchronization.lib")

#else // ^^^ _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8 / _STL_WIN32_WINNT < _WIN32_WINNT_WIN8 vvv

    bool _Atomic_wait_fallback(const void* const _Storage, _Atomic_wait_context_t& _Wait_context) noexcept {
        DWORD remaining_waiting_time = _Get_remaining_waiting_time(_Wait_context);
        if (remaining_waiting_time == 0) {
            return false;
        }

        auto& _Entry = _Atomic_wait_table_entry(_Storage);
        switch (_Wait_context._Wait_phase_and_spin_count) {
        case _Atomic_wait_phase_wait_none:
            ::AcquireSRWLockExclusive(&_Entry._Lock);
            _Wait_context._Wait_phase_and_spin_count = _Atomic_wait_phase_wait_locked;
            // re-check, and go to _Atomic_wait_phase_wait_locked
            break;

        case _Atomic_wait_phase_wait_locked:
            if (!::SleepConditionVariableSRW(&_Entry._Condition, &_Entry._Lock, remaining_waiting_time, 0)) {
                _Assume_timeout();
                ::ReleaseSRWLockExclusive(&_Entry._Lock);
                _Wait_context._Wait_phase_and_spin_count = _Atomic_wait_phase_wait_none;
                return false;
            }
            // re-check, and still in _Atomic_wait_phase_wait_locked
            break;
        }

        return true;
    }

    void _Atomic_unwait_fallback(const void* const _Storage, _Atomic_wait_context_t& _Wait_context) noexcept {
        if (_Wait_context._Wait_phase_and_spin_count == _Atomic_wait_phase_wait_locked) {
            auto& _Entry = _Atomic_wait_table_entry(_Storage);
            ::ReleaseSRWLockExclusive(&_Entry._Lock);
            // Superflous currently, but let's have it for robustness
            _Wait_context._Wait_phase_and_spin_count = _Atomic_wait_phase_wait_none;
        }
    }

    void _Atomic_notify_fallback(const void* const _Storage) noexcept {
        auto& _Entry = _Atomic_wait_table_entry(_Storage);
        ::AcquireSRWLockExclusive(&_Entry._Lock);
        ::ReleaseSRWLockExclusive(&_Entry._Lock);
        ::WakeAllConditionVariable(&_Entry._Condition);
    }

    template <class _Function_pointer>
    inline void _Save_function_pointer_relaxed(std::atomic<_Function_pointer>& _Dest, FARPROC _Src) {
        _Dest.store(reinterpret_cast<_Function_pointer>(_Src), std::memory_order_relaxed);
    }

    enum _Api_initialized : int {
        _Not_initalized,
        _Initalized,
        _In_progress,
    };

    struct _Wait_on_address_functions {
        std::atomic<decltype(&::WaitOnAddress)> _Pfn_WaitOnAddress{nullptr};
        std::atomic<decltype(&::WakeByAddressSingle)> _Pfn_WakeByAddressSingle{nullptr};
        std::atomic<decltype(&::WakeByAddressAll)> _Pfn_WakeByAddressAll{nullptr};
        std::atomic<_Api_initialized> _Initialized{_Not_initalized};
    };

    _Wait_on_address_functions _Wait_fcns;

    const _Wait_on_address_functions& _Get_wait_functions() {
        if (_Wait_fcns._Initialized.load(std::memory_order_acquire) != _Initalized) {
            _Api_initialized expected = _Not_initalized;
            if (!_Wait_fcns._Initialized.compare_exchange_strong(expected, _In_progress, std::memory_order_acquire)) {
                if (expected == _Initalized) {
                    return _Wait_fcns;
                }
            }
            HMODULE sync_api_module        = ::GetModuleHandleW(L"api-ms-win-core-synch-l1-2-0.dll");
            FARPROC wait_on_address        = ::GetProcAddress(sync_api_module, "WaitOnAddress");
            FARPROC wake_by_address_single = ::GetProcAddress(sync_api_module, "WakeByAddressSingle");
            FARPROC wake_by_address_all    = ::GetProcAddress(sync_api_module, "WakeByAddressAll");
            if (wait_on_address != nullptr && wake_by_address_single != nullptr && wake_by_address_all != nullptr) {
                _Save_function_pointer_relaxed(_Wait_fcns._Pfn_WaitOnAddress, wait_on_address);
                _Save_function_pointer_relaxed(_Wait_fcns._Pfn_WakeByAddressSingle, wake_by_address_single);
                _Save_function_pointer_relaxed(_Wait_fcns._Pfn_WakeByAddressAll, wake_by_address_all);
            }

            expected = _In_progress;
            _Wait_fcns._Initialized.compare_exchange_strong(expected, _Initalized, std::memory_order_release);
        }
        return _Wait_fcns;
    }

    bool _Have_wait_functions() noexcept {
        return _Get_wait_functions()._Pfn_WaitOnAddress.load(std::memory_order_relaxed) != nullptr;
    }

    inline BOOL __crtWaitOnAddress(
        volatile VOID* Address, PVOID CompareAddress, SIZE_T AddressSize, DWORD dwMilliseconds) {
        const auto wait_on_address = _Get_wait_functions()._Pfn_WaitOnAddress.load(std::memory_order_relaxed);
        return wait_on_address(Address, CompareAddress, AddressSize, dwMilliseconds);
    }

    inline VOID __crtWakeByAddressSingle(PVOID Address) {
        const auto wake_by_address_single =
            _Get_wait_functions()._Pfn_WakeByAddressSingle.load(std::memory_order_relaxed);
        wake_by_address_single(Address);
    }

    inline VOID __crtWakeByAddressAll(PVOID Address) {
        const auto wake_by_address_all = _Get_wait_functions()._Pfn_WakeByAddressAll.load(std::memory_order_relaxed);
        wake_by_address_all(Address);
    }
#endif //  _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8

} // unnamed namespace

_EXTERN_C
bool __stdcall __std_atomic_wait_direct(const void* _Storage, const void* const _Comparand, const size_t _Size,
    _Atomic_wait_context_t& _Wait_context) noexcept {
#ifndef _ATOMIC_WAIT_STATICALLY_AVAILABLE_TO_IMPL
    if (!_Have_wait_functions()) {
        return _Atomic_wait_fallback(_Storage, _Wait_context);
    }
#endif
    if (!__crtWaitOnAddress(const_cast<volatile void*>(_Storage), const_cast<void*>(_Comparand), _Size,
            _Get_remaining_waiting_time(_Wait_context))) {
        _Assume_timeout();
        return false;
    }
    return true;
}

void __stdcall __std_atomic_notify_one_direct(const void* const _Storage) noexcept {
#ifndef _ATOMIC_WAIT_STATICALLY_AVAILABLE_TO_IMPL
    if (!_Have_wait_functions()) {
        _Atomic_notify_fallback(_Storage);
        return;
    }
#endif
    __crtWakeByAddressSingle(const_cast<void*>(_Storage));
}

void __stdcall __std_atomic_notify_all_direct(const void* const _Storage) noexcept {
#ifndef _ATOMIC_WAIT_STATICALLY_AVAILABLE_TO_IMPL
    if (!_Have_wait_functions()) {
        _Atomic_notify_fallback(_Storage);
        return;
    }
#endif
    __crtWakeByAddressAll(const_cast<void*>(_Storage));
}

bool __stdcall __std_atomic_wait_indirect(const void* const _Storage, _Atomic_wait_context_t& _Wait_context) noexcept {
#ifndef _ATOMIC_WAIT_STATICALLY_AVAILABLE_TO_IMPL
    if (!_Have_wait_functions()) {
        return _Atomic_wait_fallback(_Storage, _Wait_context);
    }
#endif
    auto& _Entry = _Atomic_wait_table_entry(_Storage);
    switch (_Wait_context._Wait_phase_and_spin_count) {
    case _Atomic_wait_phase_wait_none:
        _Wait_context._Counter = _Entry._Counter.load(std::memory_order_relaxed);
        // Save counter in context and check again
        _Wait_context._Wait_phase_and_spin_count = _Atomic_wait_phase_wait_counter;
        break;

    case _Atomic_wait_phase_wait_counter:
        if (!__crtWaitOnAddress(const_cast<volatile std::uint64_t*>(&_Entry._Counter._Storage._Value),
                &_Wait_context._Counter, sizeof(_Entry._Counter._Storage._Value),
                _Get_remaining_waiting_time(_Wait_context))) {
            _Assume_timeout();
            return false;
        }
        // Lock on new counter value if coming back
        _Wait_context._Wait_phase_and_spin_count = _Atomic_wait_phase_wait_none;
        break;
    }
    return true;
}

void __stdcall __std_atomic_notify_one_indirect(const void* const _Storage) noexcept {
    return __std_atomic_notify_all_indirect(_Storage);
}

void __stdcall __std_atomic_notify_all_indirect(const void* const _Storage) noexcept {
#ifndef _ATOMIC_WAIT_STATICALLY_AVAILABLE_TO_IMPL
    if (!_Have_wait_functions()) {
        _Atomic_notify_fallback(_Storage);
        return;
    }
#endif
    auto& _Entry = _Atomic_wait_table_entry(_Storage);
    _Entry._Counter.fetch_add(1, std::memory_order_relaxed);
    __crtWakeByAddressAll(&_Entry._Counter._Storage._Value);
}

void __stdcall __std_atomic_unwait_direct(const void* const _Storage, _Atomic_wait_context_t& _Wait_context) noexcept {
#ifdef _ATOMIC_WAIT_STATICALLY_AVAILABLE_TO_IMPL
    (void) _Storage, _Wait_context;
#else
    _Atomic_unwait_fallback(_Storage, _Wait_context);
#endif
}

void __stdcall __std_atomic_unwait_indirect(
    const void* const _Storage, _Atomic_wait_context_t& _Wait_context) noexcept {
#ifdef _ATOMIC_WAIT_STATICALLY_AVAILABLE_TO_IMPL
    (void) _Storage, _Wait_context;
#else
    _Atomic_unwait_fallback(_Storage, _Wait_context);
#endif
}

unsigned long __stdcall __std_atomic_get_spin_count(const bool _Is_direct) noexcept {
    if (_Is_direct) {
        // WaitOnAddress spins by itself, but this is only helpful for direct waits,
        // since for indirect waits this will work only if notified.
#ifdef _ATOMIC_WAIT_STATICALLY_AVAILABLE_TO_IMPL
        return 0;
#else
        if (_Have_wait_functions()) {
            return 0;
        }
#endif
    }
    constexpr unsigned long _Uninitialized_spin_count = ULONG_MAX;
    static std::atomic<unsigned long> _Atomic_spin_count{_Uninitialized_spin_count};
    const unsigned long spin_count_from_cache = _Atomic_spin_count.load(std::memory_order_relaxed);
    if (spin_count_from_cache != _Uninitialized_spin_count) {
        return spin_count_from_cache;
    }

    unsigned long spin_count = (std::thread::hardware_concurrency() == 1 ? 0 : 10'000) * _Atomic_spin_value_step;
    _Atomic_spin_count.store(spin_count, std::memory_order_relaxed);
    return spin_count;
}

void __stdcall __std_atomic_wait_get_deadline(
    _Atomic_wait_context_t& _Wait_context, const unsigned long long _Timeout, unsigned long timeout_pico) noexcept {
    if (_Timeout == _Atomic_wait_no_timeout) {
        _Wait_context._Deadline = _Atomic_wait_no_deadline;
    } else {
        _Wait_context._Deadline = ::GetTickCount64() + _Timeout + (timeout_pico ? 1 : 0);
    }
}

bool __stdcall __std_atomic_set_api_level(unsigned long _Api_level) noexcept {
    if (!IsWindowsVersionOrGreater(HIBYTE(LOWORD(_Api_level)), LOBYTE(LOWORD(_Api_level)), 0)) {
        return false;
    }
#if _STL_WIN32_WINNT < _WIN32_WINNT_WIN8
    if (_Api_level < _WIN32_WINNT_WIN8) {
        _Api_initialized expected = _Not_initalized;
        if (!_Wait_fcns._Initialized.compare_exchange_strong(expected, _Initalized, std::memory_order_relaxed)) {
            return false; // It is too late
        }
    }
#endif
    return true;
}
_END_EXTERN_C
