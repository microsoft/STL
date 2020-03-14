// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement atomic wait / notify_one / notify_all

#include <atomic>
#include <cstdint>
#include <new>
#include <thread>

#include <Windows.h>

namespace {

    inline long _Atomic_get_spin_count() noexcept {
        static unsigned long constexpr unilitialized_spin_count = (std::numeric_limits<unsigned long>::max)();
        std::atomic<unsigned long> atomic_spin_count            = unilitialized_spin_count;
        long result                                             = atomic_spin_count.load(std::memory_order_relaxed);
        if (result == unilitialized_spin_count) {
            result = (std::thread::hardware_concurrency() == 1 ? 0 : 10'000);
            atomic_spin_count.store(result, std::memory_order_relaxed);

            // Make sure other thread is likely to get this,
            // as we've done kernel call for that.
            std::atomic_thread_fence(std::memory_order_seq_cst);
        }
        return result;
    }

    constexpr std::size_t _Wait_table_size_power = 8;
    constexpr std::size_t _Wait_table_size       = 1 << _Wait_table_size_power;
    constexpr std::size_t _Wait_table_index_mask = _Wait_table_size - 1;

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
    struct alignas(std::hardware_destructive_interference_size) _Wait_table_entry {
        // Arbitraty variable to wait/notify on if target wariable is not proper atomic for that
        // Size is largest of lock-free to make aliasing problem into hypothetical
        std::atomic<std::uint64_t> _Counter;

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

    enum _Atomic_spin_phase : unsigned long {
        _Atomic_wait_phase_mask            = 0xF000'0000,
        _Atomic_spin_value_mask            = 0x0FFF'FFFF,
        _Atomic_wait_phase_init_spin_count = 0x0000'0000,
        _Atomic_wait_phase_spin            = 0x4000'0000,
        _Atomic_wait_phase_wait            = 0x8000'0000,
    };

    static_assert(_Atomic_unwait_needed == _Atomic_wait_phase_wait);

    void _Atomic_wait_fallback(const void* const _Storage, unsigned long& _Wait_context) noexcept {
        switch (_Wait_context & _Atomic_wait_phase_mask) {
        case _Atomic_wait_phase_init_spin_count: {
            _Wait_context = _Atomic_wait_phase_spin | _Atomic_get_spin_count();
            [[fallthrough]];
        }

        case _Atomic_wait_phase_spin: {
            if ((_Wait_context & _Atomic_spin_value_mask) > 0) {
                _Wait_context -= 1;
                YieldProcessor();
                return;
            }

            _Wait_context = _Atomic_wait_phase_wait;

            auto& entry = _Atomic_wait_table_entry(_Storage);
            ::AcquireSRWLockExclusive(&entry._Lock);
            [[fallthrough]];
        }

        case _Atomic_wait_phase_wait: {
            auto& entry = _Atomic_wait_table_entry(_Storage);
            ::SleepConditionVariableSRW(&entry._Condition, &entry._Lock, INFINITE, 0);
            return; // Return to recheck
        }
        }
    }

    void _Atomic_unwait_fallback(const void* const _Storage, const long& _Wait_context) noexcept {
        if ((_Wait_context & _Atomic_wait_phase_wait) != 0) {
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
            HMODULE sync_api_module      = ::GetModuleHandle(TEXT("API-MS-WIN-CORE-SYNCH-L1-2-0.DLL"));
            void* wait_on_address        = ::GetProcAddress(sync_api_module, "WaitOnAddress");
            void* wake_by_address_single = ::GetProcAddress(sync_api_module, "WakeByAddressSingle");
            void* wake_by_address_all    = ::GetProcAddress(sync_api_module, "WakeByAddressAll");

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

} // unnamed namespace

_EXTERN_C
void __stdcall __std_atomic_wait_direct(const void* _Storage, const void* const _Comparand, const std::size_t _Size,
    unsigned long& _Wait_context) noexcept {
    auto wait_on_address = _Get_wait_functions()._Pfn_WaitOnAddress.load(std::memory_order_relaxed);
    if (wait_on_address != nullptr) {
        wait_on_address(const_cast<volatile void*>(_Storage), const_cast<void*>(_Comparand), _Size, INFINITE);
    } else {
        _Atomic_wait_fallback(_Storage, _Wait_context);
    }
}


void __stdcall __std_atomic_notify_one_direct(const void* const _Storage) noexcept {
    auto wake_by_address_single = _Get_wait_functions()._Pfn_WakeByAddressSingle.load(std::memory_order_relaxed);
    if (wake_by_address_single != nullptr) {
        wake_by_address_single(const_cast<void*>(_Storage));
    } else {
        _Atomic_notify_fallback(_Storage);
    }
}


void __stdcall __std_atomic_notify_all_direct(const void* const _Storage) noexcept {
    const auto wake_by_address_all = _Get_wait_functions()._Pfn_WakeByAddressSingle.load(std::memory_order_relaxed);
    if (wake_by_address_all != nullptr) {
        wake_by_address_all(const_cast<void*>(_Storage));
    } else {
        _Atomic_notify_fallback(_Storage);
    }
}


void __stdcall __std_atomic_wait_indirect(const void* const _Storage, unsigned long& _Wait_context) noexcept {
    const auto wait_on_address = _Get_wait_functions()._Pfn_WaitOnAddress.load(std::memory_order_relaxed);
    if (wait_on_address != nullptr) {
        auto& entry = _Atomic_wait_table_entry(_Storage);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        auto counter = entry._Counter.load(std::memory_order_relaxed);
        wait_on_address((volatile VOID*) &entry._Counter._Storage._Value, &counter,
            sizeof(entry._Counter._Storage._Value), INFINITE);
    } else {
        _Atomic_wait_fallback(_Storage, _Wait_context);
    }
}


void __stdcall __std_atomic_notify_indirect(const void* const _Storage) noexcept {
    const auto wake_by_address_all = _Get_wait_functions()._Pfn_WakeByAddressSingle.load(std::memory_order_relaxed);
    if (wake_by_address_all != nullptr) {
        auto& entry = _Atomic_wait_table_entry(_Storage);
        entry._Counter.fetch_add(1, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        wake_by_address_all(&entry._Counter._Storage._Value);
    } else {
        _Atomic_notify_fallback(_Storage);
    }
}

void __stdcall __std_atomic_unwait_direct(const void* const _Storage, unsigned long& _Wait_context) noexcept {
    const auto wait_on_address = _Get_wait_functions()._Pfn_WaitOnAddress.load(std::memory_order_relaxed);
    if (wait_on_address == nullptr) {
        _Atomic_unwait_fallback(_Storage, _Wait_context);
    }
}

void __stdcall __std_atomic_unwait_indirect(const void* const _Storage, unsigned long& _Wait_context) noexcept {
    const auto wait_on_address = _Get_wait_functions()._Pfn_WaitOnAddress.load(std::memory_order_relaxed);
    if (wait_on_address == nullptr) {
        _Atomic_unwait_fallback(_Storage, _Wait_context);
    }
}
_END_EXTERN_C