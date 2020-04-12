// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement atomic wait / notify_one / notify_all

#include <atomic>
#include <cstdint>
#include <new>
#include <thread>

#include <Windows.h>

namespace {

    constexpr size_t _Wait_table_size_power = 8;
    constexpr size_t _Wait_table_size       = 1 << _Wait_table_size_power;
    constexpr size_t _Wait_table_index_mask = _Wait_table_size - 1;

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
    struct alignas(std::hardware_destructive_interference_size) _Wait_table_entry {
        // Arbitrary variable to wait/notify on if target variable is not proper atomic for that
        // Size is largest of lock-free to make aliasing problem into hypothetical
        std::atomic<unsigned long long> _Counter;

        CONDITION_VARIABLE _Condition = CONDITION_VARIABLE_INIT;
        SRWLOCK _Lock                 = SRWLOCK_INIT;
    };
#pragma warning(pop)

    _Wait_table_entry& _Atomic_wait_table_entry(const void* const _Storage) noexcept {
        static _Wait_table_entry wait_table[_Wait_table_size];
        auto index = reinterpret_cast<std::uintptr_t>(_Storage);
        index ^= index >> (_Wait_table_size_power * 2);
        index ^= index >> _Wait_table_size_power;
        return wait_table[index & _Wait_table_index_mask];
    }

    constexpr unsigned long _Uninitialized_spin_count = ULONG_MAX;
    std::atomic<unsigned long> _Atomic_spin_count{_Uninitialized_spin_count};

    unsigned long _Atomic_init_spin_count() noexcept {
        unsigned long result = (std::thread::hardware_concurrency() == 1 ? 0 : 10'000) * _Atomic_spin_value_step;
        _Atomic_spin_count.store(result, std::memory_order_relaxed);
        // Make sure another thread is likely to get this,
        // as we've done a kernel call for that.
        std::atomic_thread_fence(std::memory_order_seq_cst);
        return result;
    }

    unsigned long _Get_remaining_waiting_time(_Atomic_wait_context_t& _Wait_context) {
        const unsigned long long deadline = _Wait_context._Deadline;
        if (deadline == _Atomic_wait_context_t::_No_deadline) {
            return INFINITE;
        }

        const unsigned long long current_time = __std_atomic_wait_get_current_time();
        if (current_time >= deadline) {
            return 0;
        }
        return static_cast<unsigned long>(deadline - current_time);
    }

    void _Assume_timeout() noexcept {
#ifndef _NDEBUG
        if (::GetLastError() != ERROR_TIMEOUT) {
            std::terminate(); // we are in noexcept, don't throw
        }
#endif // !_NDEBUG
    }

#if _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8
    constexpr bool _Have_wait_functions() {
        return true;
    }

#define __crtWaitOnAddress       WaitOnAddress
#define __crtWakeByAddressSingle WakeByAddressSingle
#define __crtWakeByAddressAll    WakeByAddressAll

#pragma comment(lib, "Synchronization.lib")

    [[noreturn]] bool _Atomic_wait_fallback(
        [[maybe_unused]] const void* const _Storage, [[maybe_unused]] _Atomic_wait_context_t& _Wait_context) noexcept {
        std::terminate();
    }

    [[noreturn]] void _Atomic_notify_fallback([[maybe_unused]] const void* const _Storage) noexcept {
        std::terminate();
    }

    void _Atomic_unwait_fallback(
        [[maybe_unused]] const void* const _Storage, [[maybe_unused]] _Atomic_wait_context_t& _Wait_context) noexcept {}

#else // ^^^ _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8 / _STL_WIN32_WINNT < _WIN32_WINNT_WIN8 vvv
    bool _Atomic_wait_fallback(const void* const _Storage, _Atomic_wait_context_t& _Wait_context) noexcept {
        auto& _Entry = _Atomic_wait_table_entry(_Storage);
        switch (_Wait_context._Wait_phase_and_spin_count) {
        case _Atomic_wait_phase_wait_none:
            ::AcquireSRWLockExclusive(&_Entry._Lock);
            _Wait_context._Wait_phase_and_spin_count = _Atomic_wait_phase_wait_locked;
            // re-check, and go to _Atomic_wait_phase_wait_locked
            break;

        case _Atomic_wait_phase_wait_locked:
            if (!::SleepConditionVariableSRW(
                    &_Entry._Condition, &_Entry._Lock, _Get_remaining_waiting_time(_Wait_context), 0)) {
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
        }
    }

    void _Atomic_notify_fallback(const void* const _Storage) noexcept {
        auto& _Entry = _Atomic_wait_table_entry(_Storage);
        ::AcquireSRWLockExclusive(&_Entry._Lock);
        ::ReleaseSRWLockExclusive(&_Entry._Lock);
        ::WakeAllConditionVariable(&_Entry._Condition);
    }

    struct _Wait_on_address_functions {
        std::atomic<decltype(&::WaitOnAddress)> _Pfn_WaitOnAddress{nullptr};
        std::atomic<decltype(&::WakeByAddressSingle)> _Pfn_WakeByAddressSingle{nullptr};
        std::atomic<decltype(&::WakeByAddressAll)> _Pfn_WakeByAddressAll{nullptr};
        std::atomic<bool> _Initialized{false};
    };

    const _Wait_on_address_functions& _Get_wait_functions() {
        static _Wait_on_address_functions functions;
        if (!functions._Initialized.load(std::memory_order_acquire)) {
            HMODULE sync_api_module        = ::GetModuleHandleW(L"api-ms-win-core-synch-l1-2-0.dll");
            FARPROC wait_on_address        = ::GetProcAddress(sync_api_module, "WaitOnAddress");
            FARPROC wake_by_address_single = ::GetProcAddress(sync_api_module, "WakeByAddressSingle");
            FARPROC wake_by_address_all    = ::GetProcAddress(sync_api_module, "WakeByAddressAll");

            if (wait_on_address != nullptr && wake_by_address_single != nullptr && wake_by_address_all != nullptr) {
                functions._Pfn_WaitOnAddress.store(
                    reinterpret_cast<decltype(&::WaitOnAddress)>(wait_on_address), std::memory_order_relaxed);
                functions._Pfn_WakeByAddressSingle.store(
                    reinterpret_cast<decltype(&::WakeByAddressSingle)>(wake_by_address_single),
                    std::memory_order_relaxed);
                functions._Pfn_WakeByAddressAll.store(
                    reinterpret_cast<decltype(&::WakeByAddressAll)>(wake_by_address_all), std::memory_order_relaxed);
            }
            functions._Initialized.store(true, std::memory_order_release);
        }
        return functions;
    }

    bool _Have_wait_functions() {
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
    if (_Have_wait_functions()) {
        if (!__crtWaitOnAddress(const_cast<volatile void*>(_Storage), const_cast<void*>(_Comparand), _Size,
                _Get_remaining_waiting_time(_Wait_context))) {
            _Assume_timeout();
            return false;
        }
        return true;
    }
    return _Atomic_wait_fallback(_Storage, _Wait_context);
}

void __stdcall __std_atomic_notify_one_direct(const void* const _Storage) noexcept {
    if (_Have_wait_functions()) {
        __crtWakeByAddressSingle(const_cast<void*>(_Storage));
    } else {
        _Atomic_notify_fallback(_Storage);
    }
}

void __stdcall __std_atomic_notify_all_direct(const void* const _Storage) noexcept {
    if (_Have_wait_functions()) {
        __crtWakeByAddressAll(const_cast<void*>(_Storage));
    } else {
        _Atomic_notify_fallback(_Storage);
    }
}

bool __stdcall __std_atomic_wait_indirect(const void* const _Storage, _Atomic_wait_context_t& _Wait_context) noexcept {
    if (_Have_wait_functions()) {
        auto& _Entry = _Atomic_wait_table_entry(_Storage);
        switch (_Wait_context._Wait_phase_and_spin_count) {
        case _Atomic_wait_phase_wait_none:
            std::atomic_thread_fence(std::memory_order_seq_cst);
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
    return _Atomic_wait_fallback(_Storage, _Wait_context);
}

void __stdcall __std_atomic_notify_one_indirect(const void* const _Storage) noexcept {
    return __std_atomic_notify_all_indirect(_Storage);
}

void __stdcall __std_atomic_notify_all_indirect(const void* const _Storage) noexcept {
    if (_Have_wait_functions()) {
        auto& _Entry = _Atomic_wait_table_entry(_Storage);
        _Entry._Counter.fetch_add(1, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        __crtWakeByAddressAll(&_Entry._Counter._Storage._Value);
    } else {
        _Atomic_notify_fallback(_Storage);
    }
}

void __stdcall __std_atomic_unwait_direct(const void* const _Storage, _Atomic_wait_context_t& _Wait_context) noexcept {
    _Atomic_unwait_fallback(_Storage, _Wait_context);
}

void __stdcall __std_atomic_unwait_indirect(
    const void* const _Storage, _Atomic_wait_context_t& _Wait_context) noexcept {
    _Atomic_unwait_fallback(_Storage, _Wait_context);
}

unsigned long __stdcall __std_atomic_get_spin_count(const bool _Is_direct) noexcept {
    if (_Is_direct && _Have_wait_functions()) {
        // WaitOnAddress spins by itself, but this is only helpful for direct waits,
        // since for indirect waits this will work only if notified.
        return 0;
    }
    const unsigned long result = _Atomic_spin_count.load(std::memory_order_relaxed);
    if (result != _Uninitialized_spin_count) {
        return result;
    }
    return _Atomic_init_spin_count();
}

_NODISCARD unsigned long long __cdecl __std_atomic_wait_get_current_time() noexcept {
    return ::GetTickCount64();
}
_END_EXTERN_C
