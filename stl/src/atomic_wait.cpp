// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement atomic wait / notify_one / notify_all

// clang-format off

#include <atomic>
#include <cstdint>
#include <new>
#include <thread>
#include <Windows.h>

// clang-format on

namespace {

    constexpr size_t _Wait_table_size_power = 8;
    constexpr size_t _Wait_table_size       = 1 << _Wait_table_size_power;
    constexpr size_t _Wait_table_index_mask = _Wait_table_size - 1;

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
    struct alignas(_STD hardware_destructive_interference_size) _Wait_table_entry {
        SRWLOCK _Lock                 = SRWLOCK_INIT;
        _Wait_context _Wait_list_head = {false, nullptr, &_Wait_list_head, &_Wait_list_head, nullptr};

        constexpr _Wait_table_entry() noexcept = default;
    };
#pragma warning(pop)

    [[nodiscard]] _Wait_table_entry& _Atomic_wait_table_entry(const void* const _Storage) noexcept {
        static _Wait_table_entry wait_table[_Wait_table_size];
        auto index = reinterpret_cast<_STD uintptr_t>(_Storage);
        index ^= index >> (_Wait_table_size_power * 2);
        index ^= index >> _Wait_table_size_power;
        return wait_table[index & _Wait_table_index_mask];
    }

    [[nodiscard]] unsigned long _Get_remaining_wait_milliseconds(unsigned long long _Deadline) {
        if (_Deadline == _Atomic_wait_no_deadline) {
            return INFINITE;
        }

        const unsigned long long current_time = GetTickCount64();
        if (current_time >= _Deadline) {
            return 0;
        }

        unsigned long long remaining      = _Deadline - current_time;
        constexpr unsigned long _Ten_days = 864'000'000;
        if (remaining > _Ten_days) {
            return _Ten_days;
        }
        return static_cast<unsigned long>(remaining);
    }

    void _Assume_timeout() noexcept {
#ifdef _DEBUG
        if (GetLastError() != ERROR_TIMEOUT) {
            abort(); // we are in noexcept, don't throw
        }
#endif // _DEBUG
    }

#if _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE

#define __crtWaitOnAddress       WaitOnAddress
#define __crtWakeByAddressSingle WakeByAddressSingle
#define __crtWakeByAddressAll    WakeByAddressAll

#else // ^^^ _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE / !_ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE vvv


    struct _Wait_functions_table {
        _STD atomic<decltype(&::WaitOnAddress)> _Pfn_WaitOnAddress{nullptr};
        _STD atomic<decltype(&::WakeByAddressSingle)> _Pfn_WakeByAddressSingle{nullptr};
        _STD atomic<decltype(&::WakeByAddressAll)> _Pfn_WakeByAddressAll{nullptr};
        _STD atomic<__std_atomic_api_level> _Api_level{__std_atomic_api_level::__not_set};
    };

    _Wait_functions_table _Wait_functions;

    void _Force_wait_functions_srwlock_only() noexcept {
        auto _Local = _Wait_functions._Api_level.load(_STD memory_order_acquire);
        if (_Local <= __std_atomic_api_level::__detecting) {
            while (!_Wait_functions._Api_level.compare_exchange_weak(_Local, __std_atomic_api_level::__has_srwlock)) {
                if (_Local > __std_atomic_api_level::__detecting) {
                    return;
                }
            }
        }
    }

    [[nodiscard]] const _Wait_functions_table& _Get_wait_functions() noexcept {
        auto _Local = _Wait_functions._Api_level.load(_STD memory_order_acquire);
        if (_Local <= __std_atomic_api_level::__detecting) {
            while (!_Wait_functions._Api_level.compare_exchange_weak(_Local, __std_atomic_api_level::__detecting)) {
                if (_Local > __std_atomic_api_level::__detecting) {
                    return _Wait_functions;
                }
            }

            HMODULE _Sync_module = GetModuleHandleW(L"api-ms-win-core-synch-l1-2-0.dll");
            const auto _Wait_on_address =
                reinterpret_cast<decltype(&::WaitOnAddress)>(GetProcAddress(_Sync_module, "WaitOnAddress"));
            const auto _Wake_by_address_single =
                reinterpret_cast<decltype(&::WakeByAddressSingle)>(GetProcAddress(_Sync_module, "WakeByAddressSingle"));
            const auto _Wake_by_address_all =
                reinterpret_cast<decltype(&::WakeByAddressAll)>(GetProcAddress(_Sync_module, "WakeByAddressAll"));
            if (_Wait_on_address != nullptr && _Wake_by_address_single != nullptr && _Wake_by_address_all != nullptr) {
                _Wait_functions._Pfn_WaitOnAddress.store(_Wait_on_address, _STD memory_order_relaxed);
                _Wait_functions._Pfn_WakeByAddressSingle.store(_Wake_by_address_single, _STD memory_order_relaxed);
                _Wait_functions._Pfn_WakeByAddressAll.store(_Wake_by_address_all, _STD memory_order_relaxed);
                _Wait_functions._Api_level.store(
                    __std_atomic_api_level::__has_wait_on_address, _STD memory_order_release);
            } else {
                _Wait_functions._Api_level.store(__std_atomic_api_level::__has_srwlock, _STD memory_order_release);
            }
        }

        return _Wait_functions;
    }

    [[nodiscard]] bool _Have_wait_functions() noexcept {
        return _Get_wait_functions()._Api_level.load(_STD memory_order_relaxed)
               >= __std_atomic_api_level::__has_wait_on_address;
    }

    [[nodiscard]] BOOL __crtWaitOnAddress(
        volatile VOID* Address, PVOID CompareAddress, SIZE_T AddressSize, DWORD dwMilliseconds) {
        const auto _Wait_on_address = _Get_wait_functions()._Pfn_WaitOnAddress.load(_STD memory_order_relaxed);
        return _Wait_on_address(Address, CompareAddress, AddressSize, dwMilliseconds);
    }

    VOID __crtWakeByAddressSingle(PVOID Address) {
        const auto _Wake_by_address_single =
            _Get_wait_functions()._Pfn_WakeByAddressSingle.load(_STD memory_order_relaxed);
        _Wake_by_address_single(Address);
    }

    VOID __crtWakeByAddressAll(PVOID Address) {
        const auto _Wake_by_address_all = _Get_wait_functions()._Pfn_WakeByAddressAll.load(_STD memory_order_relaxed);
        _Wake_by_address_all(Address);
    }
#endif // _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE

} // unnamed namespace

_EXTERN_C
bool __stdcall __std_atomic_wait_direct(const void* _Comparand, const size_t _Size, _Wait_context& _Context) noexcept {
#if _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE == 0
    if (!_Have_wait_functions()) {
        return __std_atomic_wait_indirect(_Context);
    }
#endif // _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE == 0

    if (!__crtWaitOnAddress(const_cast<volatile void*>(_Context._Storage), const_cast<void*>(_Comparand), _Size,
            _Get_remaining_wait_milliseconds(_Context._Deadline))) {
        _Assume_timeout();
        return false;
    }

    return true;
}

void __stdcall __std_atomic_notify_one_direct(const void* const _Storage) noexcept {
#if _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE == 0
    if (!_Have_wait_functions()) {
        __std_atomic_notify_one_indirect(_Storage);
        return;
    }
#endif // _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE = 0

    __crtWakeByAddressSingle(const_cast<void*>(_Storage));
}

void __stdcall __std_atomic_notify_all_direct(const void* const _Storage) noexcept {
#if _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE == 0
    if (!_Have_wait_functions()) {
        __std_atomic_notify_all_indirect(_Storage);
        return;
    }
#endif // _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE == 0

    __crtWakeByAddressAll(const_cast<void*>(_Storage));
}


void __stdcall __std_atomic_unwait_direct(_Wait_context& _Context) noexcept {
    return __std_atomic_unwait_indirect(_Context);
}

void __stdcall __std_atomic_notify_one_indirect(const void* const _Storage) noexcept {
    auto& _Entry = _Atomic_wait_table_entry(_Storage);
    AcquireSRWLockExclusive(&_Entry._Lock);
    _Wait_context* _Context = _Entry._Wait_list_head._Next;
    for (; _Context != &_Entry._Wait_list_head; _Context = _Context->_Next) {
        if (_Context->_Storage == _Storage) {
            WakeConditionVariable(&reinterpret_cast<CONDITION_VARIABLE&>(_Context->_Condition));
            break;
        }
    }
    ReleaseSRWLockExclusive(&_Entry._Lock);
}

void __stdcall __std_atomic_notify_all_indirect(const void* const _Storage) noexcept {
    auto& _Entry = _Atomic_wait_table_entry(_Storage);
    AcquireSRWLockExclusive(&_Entry._Lock);
    _Wait_context* _Context = _Entry._Wait_list_head._Next;
    for (; _Context != &_Entry._Wait_list_head; _Context = _Context->_Next) {
        if (_Context->_Storage == _Storage) {
            WakeAllConditionVariable(&reinterpret_cast<CONDITION_VARIABLE&>(_Context->_Condition));
            break;
        }
    }
    ReleaseSRWLockExclusive(&_Entry._Lock);
}


bool __stdcall __std_atomic_wait_indirect(_Wait_context& _Context) noexcept {
    auto& _Entry = _Atomic_wait_table_entry(_Context._Storage);
    if (_Context._Locked) {
        if (!SleepConditionVariableSRW(&reinterpret_cast<CONDITION_VARIABLE&>(_Context._Condition), &_Entry._Lock,
                _Get_remaining_wait_milliseconds(_Context._Deadline), 0)) {
            _Assume_timeout();
            return false;
        }
    } else {
        reinterpret_cast<CONDITION_VARIABLE&>(_Context._Condition) = CONDITION_VARIABLE_INIT;
        AcquireSRWLockExclusive(&_Entry._Lock);

        _Wait_context* const _Next = &_Entry._Wait_list_head;
        _Wait_context* const _Prev = _Next->_Prev;
        _Context._Prev             = _Prev;
        _Context._Next             = _Next;
        _Prev->_Next               = &_Context;
        _Next->_Prev               = &_Context;

        _Context._Locked = true;
    }
    return true;
}

void __stdcall __std_atomic_unwait_indirect(_Wait_context& _Context) noexcept {
    if (_Context._Locked) {

        _Wait_context* const _Prev = _Context._Prev;
        _Wait_context* const _Next = _Context._Next;
        _Context._Next->_Prev      = _Prev;
        _Context._Prev->_Next      = _Next;

        auto& _Entry = _Atomic_wait_table_entry(_Context._Storage);
        ReleaseSRWLockExclusive(&_Entry._Lock);
    }
}


unsigned long long __stdcall __std_atomic_wait_get_deadline(const unsigned long long _Timeout) noexcept {
    if (_Timeout == _Atomic_wait_no_deadline) {
        return _Atomic_wait_no_deadline;
    } else {
        return GetTickCount64() + _Timeout;
    }
}

__std_atomic_api_level __stdcall __std_atomic_set_api_level(__std_atomic_api_level _Requested_api_level) noexcept {
#if _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE
    (void) _Requested_api_level;
    return __std_atomic_api_level::__has_wait_on_address;
#else // ^^^ _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE // !_ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE vvv
    switch (_Requested_api_level) {
    case __std_atomic_api_level::__not_set:
    case __std_atomic_api_level::__detecting:
    case __std_atomic_api_level::__has_srwlock:
        _Force_wait_functions_srwlock_only();
        break;
    case __std_atomic_api_level::__has_wait_on_address:
    default: // future compat: new header using an old DLL will get the highest requested level supported
        break;
    }

    return _Get_wait_functions()._Api_level.load(_STD memory_order_relaxed);
#endif // !_ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE
}
_END_EXTERN_C
