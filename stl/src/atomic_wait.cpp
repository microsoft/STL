// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement atomic wait / notify_one / notify_all

#include <atomic>
#include <cstdint>
#include <new>
#include <thread>

#include <Windows.h>

namespace {

    constexpr std::size_t _Wait_table_size_power = 8;
    constexpr std::size_t _Wait_table_size       = 1 << _Wait_table_size_power;
    constexpr std::size_t _Wait_table_index_mask = _Wait_table_size - 1;

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
    struct alignas(std::hardware_destructive_interference_size) _Wait_table_entry {
        // Arbitraty variable to wait/notify on if target wariable is not proper atomic for that
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

    static constexpr std::size_t _Uninitialized_spin_count = (std::numeric_limits<std::size_t>::max)();
    static std::atomic<std::size_t> _Atomic_spin_count     = _Uninitialized_spin_count;

    std::size_t _Atomic_init_spin_count() noexcept {
        std::size_t result = (std::thread::hardware_concurrency() == 1 ? 0 : 10'000) * _Atomic_spin_value_step;
        _Atomic_spin_count.store(result, std::memory_order_relaxed);
        // Make sure other thread is likely to get this,
        // as we've done kernel call for that.
        std::atomic_thread_fence(std::memory_order_seq_cst);
        return result;
    }


#if _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8
    constexpr bool _Have_wait_functions() {
        return true;
    }
#define __crtWaitOnAddress       WaitOnAddress
#define __crtWakeByAddressSingle WakeByAddressSingle
#define __crtWakeByAddressAll    WakeByAddressAll

#pragma comment(lib, "Synchronization.lib")

    void _Atomic_wait_fallback(
        [[maybe_unused]] const void* const _Storage, [[maybe_unused]] unsigned long long& _Wait_context) noexcept {
        std::terminate();
    }

    void _Atomic_notify_fallback([[maybe_unused]] const void* const _Storage) noexcept {
        std::terminate();
    }

    void _Atomic_unwait_fallback(
        [[maybe_unused]] const void* const _Storage, [[maybe_unused]] unsigned long long& _Wait_context) noexcept {}

#else // ^^^ _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8 / _STL_WIN32_WINNT < _WIN32_WINNT_WIN8 vvv
    void _Atomic_wait_fallback(const void* const _Storage, unsigned long long& _Wait_context) noexcept {
        switch (_Wait_context & _Atomic_wait_phase_mask) {
        case _Atomic_wait_phase_wait_none: {
            _Wait_context = _Atomic_wait_phase_wait_locked;
            auto& entry   = _Atomic_wait_table_entry(_Storage);
            ::AcquireSRWLockExclusive(&entry._Lock);
            [[fallthrough]];
        }

        case _Atomic_wait_phase_wait_locked: {
            auto& entry = _Atomic_wait_table_entry(_Storage);
            ::SleepConditionVariableSRW(&entry._Condition, &entry._Lock, INFINITE, 0);
            // re-check, and still in _Atomic_wait_phase_wait_locked
            return;
        }
        }
    }

    void _Atomic_unwait_fallback(const void* const _Storage, unsigned long long& _Wait_context) noexcept {
        if (_Wait_context & _Atomic_wait_phase_wait_locked) {
            auto& entry = _Atomic_wait_table_entry(_Storage);
            ::ReleaseSRWLockExclusive(&entry._Lock);
        }
    }

    void _Atomic_notify_fallback(const void* const _Storage) noexcept {
        auto& entry = _Atomic_wait_table_entry(_Storage);
        ::AcquireSRWLockExclusive(&entry._Lock);
        ::ReleaseSRWLockExclusive(&entry._Lock);
        ::WakeAllConditionVariable(&entry._Condition);
    }

    struct _Wait_on_address_functions {
        std::atomic<decltype(&::WaitOnAddress)> _Pfn_WaitOnAddress;
        std::atomic<decltype(&::WakeByAddressSingle)> _Pfn_WakeByAddressSingle;
        std::atomic<decltype(&::WakeByAddressAll)> _Pfn_WakeByAddressAll;
        std::atomic<bool> _Initialized;
    };

    const _Wait_on_address_functions& _Get_wait_functions() {
        static _Wait_on_address_functions functions;
        if (!functions._Initialized.load(std::memory_order_relaxed)) {
            HMODULE sync_api_module        = ::GetModuleHandle(TEXT("API-MS-WIN-CORE-SYNCH-L1-2-0.DLL"));
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
            std::atomic_thread_fence(std::memory_order_seq_cst);
            functions._Initialized.store(true, std::memory_order_relaxed);
            std::atomic_thread_fence(std::memory_order_seq_cst);
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
void _CRT_SATELLITE_1 __stdcall __std_atomic_wait_direct(const void* _Storage, const void* const _Comparand,
    const std::size_t _Size, unsigned long long& _Wait_context) noexcept {
    if (_Have_wait_functions()) {
        __crtWaitOnAddress(const_cast<volatile void*>(_Storage), const_cast<void*>(_Comparand), _Size, INFINITE);
    } else {
        _Atomic_wait_fallback(_Storage, _Wait_context);
    }
}

void _CRT_SATELLITE_1 __stdcall __std_atomic_notify_one_direct(const void* const _Storage) noexcept {
    if (_Have_wait_functions()) {
        __crtWakeByAddressSingle(const_cast<void*>(_Storage));
    } else {
        _Atomic_notify_fallback(_Storage);
    }
}

void _CRT_SATELLITE_1 __stdcall __std_atomic_notify_all_direct(const void* const _Storage) noexcept {
    if (_Have_wait_functions()) {
        __crtWakeByAddressAll(const_cast<void*>(_Storage));
    } else {
        _Atomic_notify_fallback(_Storage);
    }
}

void _CRT_SATELLITE_1 __stdcall __std_atomic_wait_indirect(
    const void* const _Storage, unsigned long long& _Wait_context) noexcept {
    if (_Have_wait_functions()) {

        switch (_Wait_context & _Atomic_wait_phase_mask) {
        case _Atomic_wait_phase_wait_none: {
            auto& entry = _Atomic_wait_table_entry(_Storage);
            std::atomic_thread_fence(std::memory_order_seq_cst);
            unsigned long long counter = entry._Counter.load(std::memory_order_relaxed);
            // Save counter in context and check again
            _Wait_context = counter | _Atomic_wait_phase_wait_counter;
            break;
        }

        case _Atomic_wait_phase_wait_counter: {
            unsigned long long counter = _Wait_context & _Atomic_counter_value_mask;
            auto& entry                = _Atomic_wait_table_entry(_Storage);
            __crtWaitOnAddress(const_cast<volatile std::uint64_t*>(&entry._Counter._Storage._Value), &counter,
                sizeof(entry._Counter._Storage._Value), INFINITE);
            // Lock on new counter value if coming back
            _Wait_context = _Atomic_wait_phase_wait_none;
            break;
        }
        }

    } else {
        _Atomic_wait_fallback(_Storage, _Wait_context);
    }
}

void _CRT_SATELLITE_1 __stdcall __std_atomic_notify_one_indirect(const void* const _Storage) noexcept {
    return __std_atomic_notify_all_indirect(_Storage);
}

void _CRT_SATELLITE_1 __stdcall __std_atomic_notify_all_indirect(const void* const _Storage) noexcept {
    if (_Have_wait_functions()) {
        auto& entry = _Atomic_wait_table_entry(_Storage);
        entry._Counter.fetch_add(_Atomic_counter_value_step, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        __crtWakeByAddressAll(&entry._Counter._Storage._Value);
    } else {
        _Atomic_notify_fallback(_Storage);
    }
}

void _CRT_SATELLITE_1 __stdcall __std_atomic_unwait_direct(
    const void* const _Storage, unsigned long long& _Wait_context) noexcept {
    _Atomic_unwait_fallback(_Storage, _Wait_context);
}

void _CRT_SATELLITE_1 __stdcall __std_atomic_unwait_indirect(
    const void* const _Storage, unsigned long long& _Wait_context) noexcept {
    _Atomic_unwait_fallback(_Storage, _Wait_context);
}

std::size_t _CRT_SATELLITE_1 __stdcall __std_atomic_get_spin_count(const bool _Is_direct) noexcept {
    if (_Is_direct && _Have_wait_functions()) {
        // WaitOnAddress spins by itself, but this is only helpful for direct waits,
        // since for indirect waits this will work only if notified.
        return 0;
    }
    std::size_t result = _Atomic_spin_count.load(std::memory_order_relaxed);
    if (result == _Uninitialized_spin_count) {
        result = _Atomic_init_spin_count();
    }
    return result;
}
_END_EXTERN_C
